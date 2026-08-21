# C++20 Coroutines — a gentle Fibonacci demo

This little project shows off one of the headline features of C++20:
**coroutines**. It builds a `fib(n)` function that produces Fibonacci numbers
one at a time, and a `main` program that prints them.

## What is a coroutine?

An ordinary function runs from top to bottom and then returns *once*. Every
time you call it, it starts over from the beginning.

A **coroutine** is a function that can **pause in the middle, hand a value back
to whoever called it, and later resume exactly where it left off** — with all
of its local variables still intact.

> **Analogy.** A normal function is like reading a book cover to cover in one
> sitting. A coroutine is like reading with a bookmark: you stop, put the book
> down, and when you come back you continue from the bookmark, remembering
> everything that happened so far.

That "remember where I was" ability is perfect for generating a *sequence*: the
coroutine computes one number, pauses, and only computes the next number when
you actually ask for it. This is called **lazy** evaluation — values are
produced on demand, not all up front.

## The three new keywords

C++20 adds three keywords. Using any one of them turns a function into a
coroutine:

- **`co_yield value;`** — "here is the next value; pause me until you want
  another." This is the one we use for our generator.
- **`co_await something;`** — "pause me until `something` is ready" (used for
  asynchronous code, e.g. waiting on I/O). Not used in this demo.
- **`co_return value;`** — the coroutine's version of `return`; it finishes the
  coroutine. Our `fib` simply runs off the end instead.

## Why the `Generator` header?

Here is the catch that surprises newcomers: C++20 gives you the coroutine
*keywords* and the low-level machinery in `<coroutine>`, but it does **not**
give you a ready-to-use type to return from a generator. (That type,
`std::generator`, only arrives in **C++23**.)

So in [generator.hpp](generator.hpp) we write our own small `Generator<T>`. It
is about 100 lines and every piece is commented. The key part is the nested
`promise_type` — think of it as the coroutine's "control panel." The compiler
calls its methods automatically:

| When… | the compiler calls… | and we say… |
|---|---|---|
| the coroutine is created | `get_return_object()` | "return this `Generator`" |
| right after creation | `initial_suspend()` | "pause immediately (be lazy)" |
| you write `co_yield x` | `yield_value(x)` | "save `x`, then pause" |
| the coroutine ends | `final_suspend()` / `return_void()` | "pause, then it's done" |

Writing this by hand once is actually the best way to *see* how coroutines
work.

## The demo itself

[fib.cpp](fib.cpp) is tiny:

```cpp
Generator<unsigned long long> fib(unsigned long long n) {
    unsigned long long a = 0, b = 1;
    while (a <= n) {
        co_yield a;                 // give back `a`, then pause
        unsigned long long next = a + b;
        a = b;
        b = next;
    }
}
```

It yields every Fibonacci number that is **less than or equal to `n`**.

### What happens when you loop over it

[main.cpp](main.cpp) drives it with a normal range-`for`:

```cpp
for (unsigned long long value : fib(limit))
    std::cout << value << ' ';
```

Step by step:

1. `fib(limit)` is called. Because of `initial_suspend()`, **none** of the body
   runs yet — you just get back a paused `Generator`.
2. The loop asks for the first value. The coroutine resumes, runs until
   `co_yield a` with `a == 0`, and pauses. The loop prints `0`.
3. The loop asks again. Execution continues *right after* the `co_yield`,
   updates `a` and `b`, loops around, and yields the next number.
4. This repeats until `a > n`, at which point the coroutine finishes and the
   loop ends.

Notice that at no point does `fib` build a list or array. Each number exists
only for the moment it is needed.

## Build and run

You need a C++20 compiler (recent Clang, GCC, or Apple Clang).

```sh
make run              # builds, then prints Fibonacci numbers up to 1000
make run N=100000     # choose a different upper bound
make clean            # remove build artifacts
```

Expected output for the default limit:

```
Fibonacci numbers up to 1000:
0 1 1 2 3 5 8 13 21 34 55 89 144 233 377 610 987
```

## Files

- [generator.hpp](generator.hpp) — the reusable `Generator<T>` coroutine type.
- [fib.hpp](fib.hpp) / [fib.cpp](fib.cpp) — the Fibonacci coroutine.
- [main.cpp](main.cpp) — drives the coroutine and prints results.
- [Makefile](Makefile) — `all`, `run`, `run-task`, `clean` targets.

---

# Part 2: `co_await` — one coroutine waiting on another

The first demo used `co_yield` to stream *many* values out of a single
coroutine. This second demo uses **`co_await`**, the keyword for the other big
use of coroutines: **pausing one coroutine until another has produced a
result.** This is the mechanism that async/await frameworks are built on.

Here, everything runs on a single thread with no real waiting — that keeps the
spotlight on the plumbing: how `co_await` suspends a coroutine, runs the one it
is waiting on, and then feeds the returned value back.

## `Task<T>`: a coroutine that returns one value

Where `co_yield` needs a `Generator`, `co_await` needs a return type that is
**awaitable**. C++20 (again) doesn't ship one, so [task.hpp](task.hpp) defines a
small `Task<T>`. A `Task<int>` is "a coroutine that will eventually `co_return`
an `int`."

The demo coroutines ([add.cpp](add.cpp) and [compute.cpp](compute.cpp)) are
tiny:

```cpp
Task<int> add(int a, int b) {
    co_return a + b;                 // finishes the task with a value
}

Task<int> compute() {
    int x = co_await add(1, 2);      // pause, run add, resume with 3
    int y = co_await add(x, 10);     // pause, run add, resume with 13
    co_return x + y;                 // 16
}
```

Read `int x = co_await add(1, 2);` as: *"start `add(1, 2)`, pause `compute`
until it's done, then let `x` be its result."*

## How `co_await` works, in plain terms

When you write `co_await someTask`, the compiler talks to a little helper called
an **awaiter** (defined inside `Task`). It has three methods:

- **`await_ready()`** — "is the answer already available?" We return `false`, so
  we always pause.
- **`await_suspend(awaiting)`** — runs while paused. `awaiting` is the coroutine
  that is waiting (here, `compute`). We tuck it away as the awaited task's
  **continuation** — a note that says *"when you finish, wake this one up"* —
  and then start the awaited task (`add`) running.
- **`await_resume()`** — runs after the awaited task finishes; its return value
  becomes the value of the whole `co_await` expression (the `3`, the `13`).

When `add` hits `co_return`, the task's **`final_suspend`** looks up that
continuation and resumes `compute` right where it paused. (Handing control
straight from one coroutine to another like this is called *symmetric
transfer*.)

> **Bookmark analogy, part 2.** `compute` is reading along and reaches "I need
> the result of `add`." It sticks a bookmark in its own page, hands the work to
> `add`, and waits. When `add` finishes, it sees the bookmark, taps `compute` on
> the shoulder, and says "here's your answer" — and `compute` picks up exactly
> where it left off.

## What happens when you run it

`main` ([task_main.cpp](task_main.cpp)) does:

```cpp
Task<int> job = compute();   // nothing runs yet — Tasks are lazy
int answer = job.get();      // NOW it runs to completion
```

Step by step:

1. Calling `compute()` builds a paused `Task`. **No code inside runs yet.**
2. `job.get()` resumes `compute`. It reaches `co_await add(1, 2)`, pauses, and
   starts `add`.
3. `add(1, 2)` runs, `co_return`s `3`, and — following the continuation — wakes
   `compute` back up with `x = 3`.
4. The same dance happens for `add(3, 10)`, giving `y = 13`.
5. `compute` does `co_return 3 + 13`, and `get()` hands `16` back to `main`.

Run it:

```sh
make run-task
```

Expected output:

```
Calling compute() (nothing runs yet — Tasks are lazy)
Running the task...
compute: about to co_await add(1, 2)
    add(1, 2) is running
compute: got x = 3
compute: about to co_await add(3, 10)
    add(3, 10) is running
compute: got y = 13
compute() = 16
```

Notice `add(1, 2) is running` prints *after* `compute` says it is about to await
it — proof that the task was lazy and only ran once awaited.

## Files (Part 2)

- [task.hpp](task.hpp) — the awaitable `Task<T>` coroutine type.
- [add.hpp](add.hpp) / [add.cpp](add.cpp) — the `add` building-block coroutine.
- [compute.hpp](compute.hpp) / [compute.cpp](compute.cpp) — the `compute`
  coroutine that composes `add` calls with `co_await`.
- [task_main.cpp](task_main.cpp) — drives the task and prints the result.

---

# Part 3: `add()` re-implemented in plain C

Here is the punchline. A C++20 coroutine looks like compiler magic, but at the
machine level **a coroutine is just a struct plus some functions.** To prove it,
[cadd.c](cadd.c) re-implements `add()` in plain C11 so that the *unchanged* C++
`compute()` calls it with no idea it isn't C++. You literally swap one object
file: link `cadd.o` instead of `add.o`.

For the full, gentle walk-through see **[Coroutines_In_C.md](Coroutines_In_C.md)**.
The short version:

- A coroutine's state lives in a heap-allocated **coroutine frame**. Its first
  two fields are function pointers: *"resume me"* and *"destroy me."* After that
  comes the **promise** (here, the `int result` and the `continuation` handle).
- `handle.resume()` just calls the frame's first function pointer;
  `handle.destroy()` calls the second. `co_await` stashes "who to wake up next"
  (the continuation) and jumps to the awaited frame. That's the whole trick.

So [cadd.c](cadd.c) `malloc`s a struct laid out exactly like that frame, points
the two function pointers at ordinary C functions, and fills in `a` and `b`. Its
resume function prints, computes `a + b`, and wakes the continuation.

The one unavoidable piece of glue is the **calling convention**: the C++ caller
returns `Task<int>` through a hidden pointer in register `x8` (arm64), which
plain C can't name — so `cadd.c` includes a ~8-instruction assembly trampoline
to receive it. The offsets and that trampoline are specific to Clang on arm64
macOS; the *idea* is universal, the exact numbers are an ABI detail.

How universal? Both C files also carry a `#elif defined(__wasm32__)` branch that
ports them to WebAssembly — and there even the trampoline vanishes: wasm is a
stack machine with no registers, so the hidden return-slot pointer becomes an
ordinary first parameter that plain C *can* name. wasm32 also has 4-byte
pointers, which shifts the promise offsets — which is exactly why the frames
assert their offsets in units of `sizeof(void*)` rather than hard-coded `16`/`24`.
See [Coroutines_In_C.md §8](Coroutines_In_C.md) for the full walk-through.

Build and run — output is byte-for-byte identical to `make run-task`:

```sh
make run-taskc
```

## Going further: `compute()` in C too

`add()` was the easy case: it runs straight through, no pause. The real test is
`compute()`, which has **two `co_await` points** — so it must *pause twice*,
handing control to a child `add` each time and resuming afterward. That is a
genuine **state machine**, and it is exactly what a compiler emits for a
coroutine: the locals that must survive a pause live in the frame, and a small
`state` index records "which `co_await` am I resuming after."

[compute.c](compute.c) hand-builds that machine. Its `resume` function is a
`switch (state)`: case 0 runs up to the first `co_await` and starts `add(1, 2)`;
case 1 (re-entered when that `add` finishes) reads the result into the saved
local `x` and starts `add(x, 10)`; case 2 finishes with `co_return x + y`. Each
`co_await` writes `compute`'s own handle into the child's `continuation`, so the
child resumes `compute` back — the same symmetric-transfer handoff described in
Part 2, now spelled out by hand. (The one subtlety: a child is destroyed only
*after* its `resume()` fully returns, since it is still on the stack when it
transfers back in.)

With `compute.c` you can now build the demo with **both** coroutines in C — only
the driver (`task_main`) and the `Task<T>` plumbing remain C++:

```sh
make run-taskcc   # both compute() and add() are C — still prints 16
```

The [Coroutines_In_C.md](Coroutines_In_C.md) walk-through has a dedicated
section on this state machine.

## Files (Part 3)

- [cadd.c](cadd.c) — `add()` as a hand-built C coroutine frame + ABI shim
  (builds for both arm64 macOS and wasm32).
- [compute.c](compute.c) — `compute()` as a hand-built C coroutine **state
  machine** (two `co_await`s) + ABI shim (arm64 macOS and wasm32).
- [Coroutines_In_C.md](Coroutines_In_C.md) — full explanation for newcomers.

---

# Part 4: what if C did this natively?

The C versions work, but every one of them is hand-labor: a frame struct with
`_Static_assert`ed offsets, a hand-written `switch (state)` machine, and arm64
assembly to satisfy C++ name mangling and the `x8` return convention. All of it
is exactly what a C++ compiler emits automatically.

**[Proposal.md](Proposal.md)** turns that observation into a language proposal:
what a C compiler and runtime library would have to add so the compiler
*generates* the coroutine machinery — native `co_await` / `co_yield` /
`co_return`, user-definable promise and awaiter types, and frames that are
byte-identical to C++'s, so C and C++ coroutines interoperate at runtime. It
ends with the same demo rewritten as ordinary straight-line C, with no asm and
no offset asserts.
