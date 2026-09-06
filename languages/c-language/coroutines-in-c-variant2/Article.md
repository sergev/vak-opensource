# An RTOS with no stacks: what if every blocking call were a `yield`?

I've been sketching a small real-time operating system for Arduino-class chips, and it
ended up with a shape I did not expect. There are no per-task stacks. There is no heap.
There is exactly one system call, and it isn't a function.

Fair warning before you get excited: this rests on a language feature that no C compiler
has today. I wrote up a proposal for native coroutines in C ([Proposal2.md](Proposal2.md))
and then an OS design on top of it ([RTOS.md](RTOS.md)). Both are drafts. What follows is
the tour, in plain language.

## The problem, in one sketch

Everybody's first Arduino program blinks an LED with `digitalWrite` and `delay(500)`.
Now add a second job: read a sensor every 100 ms. And a third: answer commands on the
serial port. `delay()` is a wall. While the LED is sleeping, the sensor is not sampled
and the serial buffer overflows.

The usual fix is to throw away `delay()` and hand-roll a state machine:

```c
void loop() {
    u32 now = millis();
    if (now - led_last >= 500)     { led_last = now;    toggle_led(); }
    if (now - sensor_last >= 100)  { sensor_last = now; read_sensor(); }
    if (Serial.available())        { handle_byte(Serial.read()); }
}
```

This works, and it is the standard Arduino idiom, and it stops being readable at about
the third job. Every task's control flow has been turned inside out into a pile of
globals. A protocol handler that wants to say "read three bytes, then wait for an ACK,
then time out after a second" becomes a `switch` over states with names like
`WAITING_FOR_ACK_2`.

What you actually want is to write each job as a straight-line loop that says
`sleep 500 ms` in the middle, and have something else make the three of them share the
chip.

## Why a normal RTOS doesn't fit

That something else is an RTOS, and FreeRTOS will happily do this. But look at what it
costs on an ATmega328P, the chip in a classic Arduino Uno: **2 KB of RAM. Total.**

FreeRTOS gives every task its own stack, because that's how a task gets paused
mid-function: you save the CPU registers and switch the stack pointer somewhere else.
A do-nothing task needs on the order of 100 to 200 bytes of stack, and you have to guess
the number in advance, at compile time, for each task:

```c
xTaskCreate(blink_task, "blink", 128, NULL, 1, NULL);  // 128 words? bytes? enough?
```

Two things go wrong. First, four or five tasks and your 2 KB is gone before your
application has a single variable. Second, if you guess too low, the task doesn't fail
politely. It runs off the end of its stack into whatever is next in RAM and the device
starts behaving strangely on Tuesdays.

So the question is: can a task pause in the middle of a function *without* owning a
stack?

## A coroutine is a struct plus a switch

Yes, and the trick is old. A **stackless coroutine** is a function that can pause and
later resume, where its paused state lives in a small struct instead of on a stack.

The analogy I like: a stack is renting the task its own desk, permanently, so it can walk
away and leave its papers spread out. A coroutine frame is a bookmark. When the task
pauses, it writes down only what it needs to pick up again, and hands the desk back.

In the proposed syntax it looks like this:

```c
coro(int) void counter(int limit) {
    for (int i = 0; i < limit; i++)
        yield i;                     // pause here, hand `i` to whoever resumed us
}
```

`coro(int) void` means "a coroutine that yields `int`s and returns nothing". The `yield`
statement pauses the function. The caller drives it with `co_init` and `co_resume`, and
reads each yielded value with `co_value`:

```c
coro_frame(counter) f;               // the frame, an ordinary local variable
co_init(&f, counter, 10);            // set it up, run nothing yet
while (co_resume(&f) == CO_SUSPENDED)
    print_int(co_value(&f));
```

Note where the frame lives: it's just a variable. No allocator was called. The compiler
knows its size, because the compiler knows exactly which locals need to survive the
pause.

## What the compiler actually emits

This is the part that convinces people it isn't magic. Take a blink task:

```c
coro(sys_req *) void blink(u8 pin, u16 period) {
    gpio_mode(pin, GPIO_OUT);
    for (;;) {
        gpio_toggle(pin);
        os_sleep_ms(period);         // this is where we pause
    }
}
```

The compiler turns that into a struct and a function. Roughly, in ordinary C:

```c
struct blink__frame {
    u16 state;                       // where to resume: 0 = start, 1 = after the sleep
    u8  pin;                         // parameter, lives across the pause
    u16 period;                      // parameter, lives across the pause
    sys_req req;                     // the local we yield the address of
};

co_status blink__resume(struct blink__frame *f, co_signal sig) {
    switch (f->state) {
    case 0:
        gpio_mode(f->pin, GPIO_OUT);
        for (;;) {
            gpio_toggle(f->pin);
            f->req = (sys_req){ .op = SYS_SLEEP,
                                .u.sleep.deadline = os_now() + f->period };
            f->state = 1;
            return CO_SUSPENDED;     // <-- pause: return to the kernel
    case 1: ;                        // <-- resume: jump back inside the loop
            if (sig == CO_CANCEL) return CO_DONE;
        }
    }
}
```

Yes, the `case 1:` label really is inside the `for` loop. That's the old Duff's device
trick that Adam Dunkels' protothreads used; the difference is that a compiler doing it
can also put your locals in the frame, which protothreads famously could not.

In RAM, the whole task is this:

```
    blink__frame  (about 16 bytes on AVR)
  +----------------------------------+
  |  state      2 bytes              |   which line to resume at
  |  pin        1 byte               |   parameter
  |  period     2 bytes  (+padding)  |   parameter
  |  req       10 bytes              |   the pending system call
  +----------------------------------+
```

Sixteen bytes, not two hundred. And the number came out of the compiler, not out of your
guess. The one rule that makes this work: **locals that don't survive a pause stay on the
machine stack and never enter the frame.** A loop counter used between two yields costs
you nothing.

## The one idea: blocking is yielding a request

Here's the design decision the whole OS hangs on.

A task is a coroutine that yields `sys_req *`. And *every* blocking operation in the
system is the same act: fill in a request struct, yield its address, get resumed when
it's done.

```c
struct sys_req {
    sys_req *next;                   // queue link, the kernel owns this while we're blocked
    u8       op;                     // SYS_SLEEP, SYS_RECV, SYS_READ, ...
    u8       flags;
    i16      out;                    // result, written by the kernel before it resumes us
    union {                          // one arm per kind of request
        struct { u32 deadline; }             sleep;
        struct { os_chan *ch; void *msg; }   chan;
        struct { u8 dev; u8 *buf; u16 len; } io;
    } u;
};
```

Ten or twelve bytes on AVR. Sleeping is a request. Receiving a message is a request.
Waiting for a UART byte, an ADC conversion, a button press: all requests. There is no
second mechanism, no separate "driver API", nothing to learn twice.

Written out longhand, a sleep is:

```c
sys_req r = { .op = SYS_SLEEP, .u.sleep.deadline = os_now() + 100 };
yield &r;
```

and `os_sleep_ms(100)` is just a macro that expands to those two lines.

The flip side of the rule is equally mechanical: **operations that can't block stay
ordinary functions.** `gpio_write()` is a register store. Making it a coroutine would cost
bytes and cycles and buy nothing. So the API splits itself: if it can block it's a
syscall, otherwise it's a function, and you never have to wonder which.

## The kernel allocates nothing. Ever.

Look again at where that `sys_req` lives: it's a local in the task's own frame. And the
frame doesn't move while the task is suspended.

That one fact removes the kernel's memory allocator. When a task blocks on a channel, the
kernel doesn't allocate a "waiter" record. It links the task's own `sys_req` into a list
through the `next` field that's already sitting in the task's frame. A channel's waiter
queue costs one pointer in the channel. A timer queue is the same list, sorted by
deadline.

So the kernel's RAM is one small array of task control blocks (8 bytes each) plus about
twenty bytes of globals. Nothing else scales with anything. Eight tasks, three channels
and a couple of event groups should land near 400 bytes, leaving the application most of
the chip. The same eight tasks with a stack each don't fit at all.

## The scheduler is a twenty-line loop

```c
void os_run(void) {
    os_init_all();
    for (;;) {
        os_tcb *t = highest_priority_ready();
        if (!t) { os_idle(); continue; }        // nothing to do: sleep the chip

        if (t->resume(t->frame, CO_CONTINUE) == CO_DONE) {
            t->state = OS_DONE;
            continue;
        }
        t->pending = co_value_of(t);            // the sys_req * it just yielded
        os_service(t);                          // satisfy it, or park it on a queue
    }
}
```

That's it. Pick the highest-priority ready task, resume it, look at what it yielded.
If the request can be satisfied right now (a sleep whose deadline has already passed, a
receive on a non-empty channel) the task goes straight back to ready. Otherwise its
`sys_req` gets linked onto a queue and the task is blocked.

`os_idle()` is where the power savings live. When nothing is ready, the kernel looks at
the nearest deadline in the timer queue, programs the hardware timer for exactly that
moment, and puts the MCU to sleep. There is no periodic tick interrupt at all, so a
device whose tasks are all sleeping for 500 ms wakes up twice a second instead of a
thousand times.

## Interrupts stay interrupts

Interrupt handlers are **not** coroutines. They're ordinary functions, they can't yield,
and they must not call any of the `os_*` blocking macros. That rule enforces itself: the
coroutine spec already makes `yield` outside a coroutine body a compile error.

An ISR gets exactly three operations, all constant-time and allocation-free:

```c
void os_isr_post(os_events *ev, os_events bits);   // set event bits, wake waiters
bool os_isr_send(os_chan *ch, const void *msg);    // non-blocking send
void os_isr_wake(os_task *t);                      // mark a task ready
```

The principle to hold onto: **an ISR makes a task ready, it never does work on the task's
behalf.** Your UART driver isn't an ISR; it's a task, woken by a three-line ISR.

## Bonus: most of your locks vanish

Here's a consequence people miss. Task code runs to its next suspend point without any
other task interrupting it. Therefore:

**Any critical section that contains no `yield` needs no lock.**

Read-modify-write of a shared counter, updating a struct, walking a list: all atomic with
respect to other tasks, for free, no mutex, no disabled interrupts. Only ISRs can
interleave with you, and there's a separate `OS_ATOMIC { ... }` block for that.

This deletes most of the mutex traffic of a conventional RTOS. You still get real
communication primitives when you want them: `OS_CHAN(readings, i16, 4)` declares a
statically sized ring buffer with blocking `os_send` / `os_recv`, and `OS_EVENTS(uart_ev)`
declares a bitmask that an ISR can set and a task can wait on. And a mutex exists for the
one case the free lunch doesn't cover: holding a resource *across* a suspend point, like
an I2C transaction with yields inside it.

## The whole thing, in one program

```c
#include <braam/rt.h>

OS_CHAN(samples, i16, 4);
OS_EVENTS(button_ev);
#define BTN_PRESSED (1u << 0)

// ISR: makes a task ready, does no work
ISR(PCINT0_vect) {
    os_isr_post(&button_ev, BTN_PRESSED);
}

// periodic sensor, drift-free
coro(sys_req *) void sensor(u8 ch, u16 period) {
    u32 next = os_now();
    for (;;) {
        i16 v = os_adc_read(ch);              // yields; the ADC ISR wakes us
        os_try_send(&samples, &v);            // drop on overrun rather than block
        next += period;
        os_sleep_until(next);                 // absolute deadline: no drift
    }
}

// consumer with a timeout
coro(sys_req *) void reporter(void) {
    for (;;) {
        i16 v;
        i16 r = os_deadline(2000, os_recv(&samples, &v));
        if (r == OS_ETIMEDOUT) { os_uart_write(0, "stall\n", 6); continue; }
        u8 line[8];
        os_uart_write(0, line, fmt_i16(line, v));
    }
}

// event-driven task
coro(sys_req *) void on_button(void) {
    for (;;) {
        os_wait_any(&button_ev, BTN_PRESSED);
        gpio_toggle(13);
        os_sleep_ms(50);                      // debounce, costs nothing while asleep
    }
}

OS_TASK(t_sensor,   sensor,    1, 0, 100);
OS_TASK(t_reporter, reporter,  1);
OS_TASK(t_button,   on_button, 3);
OS_TASKS(&t_sensor, &t_reporter, &t_button);

int main(void) { os_run(); }
```

Three concurrent activities, each written as a straight-line loop that says what it means.
No stacks, no heap, no tick interrupt, and the chip sleeps between events. The tasks
themselves cost a few dozen bytes each, and the total RAM figure is a link-time constant:
if it doesn't fit, the link fails instead of the device.

Notice `os_deadline(2000, os_recv(...))`. A timeout isn't a parameter on every call,
which would double the API. It sets a flag on the request before it's yielded, and the
kernel parks that one request on both the channel queue and the timer queue, resolving
whichever fires first. One mechanism.

## What it costs

Cooperative scheduling has a price and it's worth saying out loud. A task that computes
for 10 ms with no yield delays every other task by up to 10 ms, and this design cannot
preempt it. There's a watchdog that catches a runaway task and reboots with a report, but
detecting is not preventing: if you have a deadline shorter than your longest
run-to-yield interval, serve it from an ISR.

The other cost is that a plain helper function can't block. Anything that might yield has
to be a coroutine itself and be reached with `await`, so the async/sync split propagates
up your call chains exactly as it does in Rust or C#. That's the real tax on
stacklessness, and it buys the RAM.

And of course, none of this compiles today. `coro`, `yield` and `await` aren't C. This is
a proposal looking for arguments, and the closest thing to a proof that the architecture
works is Embassy, which is essentially this design in Rust and is shipping in production
right now.

## Read the drafts

[Proposal2.md](Proposal2.md) is the language half: two keywords, one type constructor,
seven operations, no hidden allocation. [RTOS.md](RTOS.md) is the OS half: syscall
protocol, scheduler, drivers, cancellation, the memory budget, and a full list of what it
can't do. Objections welcome, especially about the parts marked "open question".
