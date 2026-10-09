// The five dining philosophers with coroutines.
//
// Each philosopher is a coroutine that yields the number of ticks it wants
// to sleep.  A scheduler keeps a clock and resumes whoever is due.  Taking a
// fork is a sub-coroutine, `await take(f)`, which waits a tick at a time while
// the fork is busy.  Each fork is given back by a `defer`, so it goes back on
// every way out of a meal, the frame's destruction included.
//
// Everyone picks up the left fork first.  If the right one is taken, the
// philosopher waits one tick for it, holding the left; if it is still taken,
// the left one goes back on the table and the philosopher backs off before
// trying again.  No one holds a fork for long while waiting for the other,
// so the table cannot deadlock.
//
// With threads this scheme can livelock: five philosophers in step take,
// wait, put back and retry forever.  Here it cannot, as the scheduler runs
// one philosopher at a time, in order, so the five never act in step.  The
// back-off is random anyway, so that no two philosophers retry together.
#include <coro.h>
#include <stdio.h>

#define N     5
#define TICKS 40
#define FREE  (-1)

static int fork_owner[N];          // who holds each fork, or FREE
static char state[N];              // 'T' thinking, 'H' hungry, 'E' eating
static int meals[N];

static unsigned seed = 12345;
static int rnd(int lo, int hi)     // deterministic, so runs repeat
{
    seed = seed * 1103515245 + 12345;
    return lo + (int)((seed >> 16) % (unsigned)(hi - lo + 1));
}

// Waits until fork f is free, then takes it.
coro(int) void take(int f, int who)
{
    while (fork_owner[f] != FREE)
        yield 1;                   // try again on the next tick
    fork_owner[f] = who;
}

// Takes fork f, waiting one tick for it if it is busy.  Returns 1 if it got
// the fork, 0 if the fork was still taken.
coro(int) int try_take(int f, int who)
{
    if (fork_owner[f] != FREE) {
        yield 1;                   // wait a tick for the fork
        if (fork_owner[f] != FREE)
            return 0;
    }
    fork_owner[f] = who;
    return 1;
}

static void put(int f)
{
    fork_owner[f] = FREE;
}

// One attempt at a meal: returns 1 if philosopher i ate, 0 if the right fork
// stayed taken.  Either way it leaves holding no fork.
coro(int) int try_to_eat(int i)
{
    int left = i, right = (i + 1) % N;

    await take(left, i);
    defer put(left);
    if (!await try_take(right, i))
        return 0;                  // put(left) runs here
    defer put(right);

    state[i] = 'E';
    meals[i]++;
    yield rnd(1, 3);               // eat
    return 1;                      // put(right), then put(left)
}

coro(int) void philosopher(int i)
{
    int think = 1;                 // everyone gets hungry at once

    for (;;) {
        state[i] = 'T';
        yield think;
        state[i] = 'H';
        while (!await try_to_eat(i))
            yield rnd(1, 3);       // back off, then try again
        think = rnd(1, 4);
    }
}

// Runs the table for TICKS ticks.  The frames live on this function's stack:
// co_alloca gives each one, past the frame itself, room for the `try_to_eat`
// it awaits and the larger of `take` and `try_take`, which that one awaits
// one after the other.  (Not in a loop: a co_alloca in a loop body is freed
// at the end of every iteration.)  On return, every philosopher not finished,
// which here is all of them, is destroyed: its pending defers run and give
// back whatever forks it holds.
static void run_table(void)
{
    int inner = co_sizeof(take) > co_sizeof(try_take) ? co_sizeof(take)
                                                       : co_sizeof(try_take);
    int room = co_sizeof(try_to_eat) + inner;
    co_frame(int, void) *ph[N] = {
        co_alloca(philosopher, room, 0),
        co_alloca(philosopher, room, 1),
        co_alloca(philosopher, room, 2),
        co_alloca(philosopher, room, 3),
        co_alloca(philosopher, room, 4),
    };
    int wake[N] = { 0 };

    printf("tick  0 1 2 3 4   forks\n");
    for (int now = 0; now < TICKS; now++) {
        for (int i = 0; i < N; i++)
            if (wake[i] <= now) {
                co_resume(ph[i]);
                wake[i] = now + co_value(ph[i]);
            }

        printf("%4d ", now);
        for (int i = 0; i < N; i++)
            printf(" %c", state[i]);
        printf("   ");
        for (int f = 0; f < N; f++)
            printf("%c", fork_owner[f] == FREE ? '.' : '0' + fork_owner[f]);
        printf("\n");
    }
}

int main(void)
{
    for (int f = 0; f < N; f++)
        fork_owner[f] = FREE;

    run_table();

    printf("meals:");
    for (int i = 0; i < N; i++)
        printf(" %d", meals[i]);
    int held = 0;
    for (int f = 0; f < N; f++)
        held += fork_owner[f] != FREE;
    printf("\nforks still held after the table is cleared: %d\n", held);
    return 0;
}
