# Coroutines, and how to build one in plain C

This document is for the curious reader who has seen C++20 coroutines and
wondered: *what is actually going on under the hood? Is this real, or compiler
sorcery?* We'll answer it the most convincing way possible — by re-building one
of our coroutines, `add()`, in ordinary C, and having the C++ program call it
without noticing the difference.

No prior coroutine expertise is assumed. We build up from the idea.

---

## 1. A one-minute refresher: what a coroutine is

A normal function runs top to bottom and returns **once**. Call it again and it
starts over from scratch, remembering nothing.

A **coroutine** is a function that can **pause in the middle, hand control back
to its caller, and later resume exactly where it paused** — with all its local
variables still intact.

> **Analogy.** A normal function is reading a book in one sitting. A coroutine is
> reading with a bookmark: you can stop, do something else, and later continue
> from the bookmark, remembering the whole story so far.

C++20 adds three keywords; using any one turns a function into a coroutine:

- `co_yield` — "here is a value; pause me until you want the next one."
- `co_return` — "I'm finished; here is my final result."
- `co_await` — "pause me until *this other thing* is ready, then continue."

Our demo has a coroutine `compute()` that uses `co_await` to call two smaller
coroutines, `add(1, 2)` and `add(3, 10)`, and adds up their results.

---

## 2. The big secret: a coroutine is a struct

When you call a coroutine, the compiler does something an ordinary function
never does: it allocates a little block of memory on the heap called the
**coroutine frame**. Everything the coroutine needs to be paused and resumed
lives in that frame:

- the coroutine's local variables (so they survive across a pause),
- a note of *where* in the function it is paused (which line to resume at),
- and — the key part — **two function pointers at the very front**:

```text
        coroutine frame (a heap block)
      +--------------------------------+
  +0  |  resume  ->  function pointer  |   "run me forward from where I paused"
  +8  |  destroy ->  function pointer  |   "clean me up and free me"
      +--------------------------------+
 +16  |  the promise object            |   coroutine-specific bookkeeping
      |     ...                        |
      +--------------------------------+
 +32  |  saved local variables         |
      +--------------------------------+
```

(The byte offsets above are for a 64-bit target, where a pointer is 8 bytes. On
a 32-bit target such as wasm32 they shrink to +4, +8, +16 — the *shape* is
identical, only the pointer size differs. We come back to this in section 8.)

The thing you pass around in C++ — a `std::coroutine_handle` — is nothing more
than **a pointer to this frame.** And the operations on it are shockingly plain:

- `handle.resume()` → *call the function pointer at the front of the frame.*
- `handle.destroy()` → *call the second function pointer.*
- `handle.done()` → *check whether the resume pointer has been set to null.*

That's it. "Resuming a coroutine" is literally an indirect function call. Once
you see this, the magic evaporates: a coroutine is a struct that carries its own
"continue" and "clean up" buttons.

---

## 3. The promise, and what `co_await` really does

The middle section of the frame is the **promise**. It is where the coroutine
stores things specific to *this* kind of coroutine. For our `Task<int>`, the
promise holds exactly two things:

- `int result;` — where `co_return a + b;` puts its answer.
- a **continuation** — a handle to "whoever is waiting for me."

That continuation is the heart of `co_await`. When `compute()` runs

```cpp
int x = co_await add(1, 2);
```

three things happen:

1. `add(1, 2)` is called, producing a fresh (paused) `add` frame.
2. `compute` writes *its own* handle into `add`'s promise as the continuation —
   a sticky note that says *"add, when you finish, wake me up."*
3. Control jumps to `add`'s frame and runs it.

When `add` finishes, it looks at that sticky note and resumes `compute`, which
reads `add`'s `result` out of the promise — that becomes the value of `x`.

> **Bookmark analogy again.** `compute` reaches "I need `add`'s answer," sticks a
> bookmark in its own page, and hands the book to `add`. When `add` is done it
> sees the bookmark and taps `compute`: "here's your number." `compute` resumes
> right where it left off.

So the entire `co_await` machinery is: *store who to resume next, jump to the
callee, and on finish jump back.* Plain data, plain function calls.

---

## 4. Re-building `add()` in C

If a coroutine is just that struct plus a couple of functions, then nothing
stops us from building the struct **by hand in C**. That is what
[cadd.c](cadd.c) does. In C we describe the frame as an ordinary struct:

```c
struct cadd_frame {
    void (*resume)(void *);   // + 0*sizeof(void*)   the "continue" button
    void (*destroy)(void *);  // + 1*sizeof(void*)   the "clean up" button
    int   result;             // + 2*sizeof(void*)   promise.result
    void *continuation;       // + 3*sizeof(void*)   promise.continuation
    int   a, b;               // our saved parameters (natural alignment)
};
```

The offsets are not arbitrary — they must match what the C++ side reads. We
didn't guess them; we asked the compiler by inspecting the assembly it generated
for the original C++ `add()`, and we lock them in with compile-time checks.
Rather than hard-code `16` and `24`, we let the struct lay itself out naturally
and assert the offsets *in units of `sizeof(void*)`* — the `int result` is padded
up to pointer alignment before `continuation`, so this is correct whether a
pointer is 8 bytes (64-bit) or 4 bytes (wasm32 — see section 8):

```c
_Static_assert(offsetof(struct cadd_frame, result) == 2 * sizeof(void *), "...");
_Static_assert(offsetof(struct cadd_frame, continuation) == 3 * sizeof(void *), "...");
```

Then we write the two "buttons" as normal C functions. The resume function is
the body of the coroutine:

```c
static void cadd_resume(void *p) {
    struct cadd_frame *f = p;
    printf("    add(%d, %d) is running\n", f->a, f->b);
    f->result = f->a + f->b;      // this is `co_return a + b;`
    f->resume = NULL;             // mark ourselves "done"
    void *cont = f->continuation; // and wake up whoever awaited us
    if (cont)
        (*(void (**)(void *))cont)(cont);  // call the continuation's resume button
}

static void cadd_destroy(void *p) { free(p); }
```

And a maker that allocates a paused frame — the C equivalent of *calling* the
coroutine:

```c
void *cadd_make(int a, int b) {
    struct cadd_frame *f = malloc(sizeof *f);
    f->resume = cadd_resume;
    f->destroy = cadd_destroy;
    f->result = 0;
    f->continuation = NULL;
    f->a = a; f->b = b;
    return f;    // hand back a frame that hasn't run yet (lazy)
}
```

Look at `cadd_resume`: waking the continuation is just
`(*(void (**)(void *))cont)(cont)` — "read the function pointer at the front of
the continuation's frame and call it." Exactly the definition of `resume()` from
section 2. The C code and the C++ compiler are speaking the same low-level
language.

---

## 5. The one hard part: the calling convention

There is a single wrinkle that keeps this from being *pure* portable C.

`compute()` calls `add(1, 2)` expecting it to **return a `Task<int>` by value**.
Because `Task` has a custom move-constructor and destructor, the platform's ABI
(the arm64 "AAPCS64" rulebook) says such a return value is passed back
**indirectly**: the caller sets aside space for the result and passes its
*address* to the callee in a specific CPU register — `x8` on arm64. Plain C has
no syntax for "the value in register x8," so we can't write this entry point in C
alone.

The fix is small and honest: a handful of assembly instructions that grab `x8`,
call our C `cadd_make(a, b)`, store the returned frame pointer where `x8` points,
and return. It also has to export the exact name the C++ caller is looking for —
the "mangled" name `_Z3addii` (C++'s encoding of `add(int, int)`):

```asm
__Z3addii:
    stp x29, x30, [sp, #-32]!
    mov x29, sp
    str x8, [sp, #16]      // save the return-slot pointer
    bl  _cadd_make         // frame = cadd_make(a, b); (a,b already in place)
    ldr x8, [sp, #16]
    str x0, [x8]           // *return_slot = frame   -> builds the Task<int>
    mov x0, x8
    ldp x29, x30, [sp], #32
    ret
```

This trampoline is the only architecture-specific code. The *concept* — build a
frame, point its buttons at your functions, respect how values are returned — is
universal; these exact register names and offsets are details of one compiler on
one CPU. Section 8 makes that concrete by taking the *same* C to WebAssembly,
where — with no registers to name — the trampoline disappears entirely.

---

## 6. The harder case: `compute()` as a state machine

`add()` was the gentle introduction — it runs straight through and never pauses.
`compute()` is the real thing, because it **pauses twice**:

```cpp
Task<int> compute() {
    int x = co_await add(1, 2);      // pause #1
    int y = co_await add(x, 10);     // pause #2
    co_return x + y;
}
```

You cannot write that as one straight-line C function, because after each
`co_await` control *leaves* `compute` and comes back later. So how does the
compiler do it? It rewrites the coroutine into a **state machine**:

- Every local that must survive a pause (here, `x`) is stored **in the frame**,
  not on the stack — the stack is long gone by the time we resume.
- A small integer **`state`** records *where* we paused ("after which
  `co_await`"). The resume function is a `switch (state)`: each pause point is a
  `case` label. Resuming = call resume again; it jumps to the right `case`.

[compute.c](compute.c) builds this by hand. Its frame carries the state and the
saved local, plus handles to the two children:

```c
struct frame {
    void (*resume)(void *);   // + 0*sizeof(void*)
    void (*destroy)(void *);  // + 1*sizeof(void*)
    int   result;             // + 2*sizeof(void*)  promise.result
    void *continuation;       // + 3*sizeof(void*)  promise.continuation
    int   state;              // which co_await are we resuming after?
    int   x;                  // saved local (the first co_await's value)
    void *c1, *c2;            // the two child add() frames
};
```

(Same first four fields, same `sizeof(void*)`-based asserts as `cadd_frame` —
that shared prefix is precisely the `Task<int>` layout both sides agree on.)

And the resume function is literally the coroutine chopped at its pause points:

```c
static void compute_resume(void *p) {
    struct frame *f = p;
    switch (f->state) {
    case 0:  // from the top, up to the first co_await
        printf("compute: about to co_await add(1, 2)\n");
        f->c1 = call_add(1, 2);
        ((struct frame *)f->c1)->continuation = f;  // "wake me when done"
        f->state = 1;
        coro_resume(f->c1);          // run add -> it resumes us at case 1
        coro_destroy(f->c1);         // safe now: add's resume has returned
        return;
    case 1:  // resumed by the first add()
        f->x = ((struct frame *)f->c1)->result;     // this is the value of x
        printf("compute: got x = %d\n", f->x);
        printf("compute: about to co_await add(%d, 10)\n", f->x);
        f->c2 = call_add(f->x, 10);
        ((struct frame *)f->c2)->continuation = f;
        f->state = 2;
        coro_resume(f->c2);
        coro_destroy(f->c2);
        return;
    case 2: {  // resumed by the second add()
        int y = ((struct frame *)f->c2)->result;
        printf("compute: got y = %d\n", y);
        f->result = f->x + y;        // co_return x + y  (16)
        f->resume = NULL;            // mark done
        if (f->continuation) coro_resume(f->continuation);
        return;
    }}
}
```

Each `co_await` is the same three-step dance from section 3 — set the child's
continuation to *us*, resume the child, and let it resume us back. The only new
idea is the `state` index that lets a single function re-enter at the right spot.

> **One sharp edge worth naming.** Notice that each child is destroyed *after*
> `coro_resume` returns, not inside the `case` that reads its result. When
> `add`'s resume calls back into `compute`, `add`'s resume is **still on the
> stack**. Freeing `add` from within that nested call would pull the rug out
> from under code that is still running. So the rule is: whoever *resumed* a
> child destroys it, once that resume has fully unwound. The compiler is careful
> about exactly this; so are we.

## 7. Seeing it work

Programs are built from the **same** C++ `main` and `Task<T>`, swapping in the C
implementations one object file at a time:

- `task` links the original C++ `add()` ([add.cpp](add.cpp)) and `compute()`
  ([compute.cpp](compute.cpp)).
- `taskc` swaps in the C `add()` ([cadd.c](cadd.c)) — `add.o` → `cadd.o`.
- `taskcc` swaps in **both** C coroutines ([compute.c](compute.c) +
  [cadd.c](cadd.c)) — now only the driver and `Task<T>` are C++.

```sh
make run-task     # add() and compute() are C++
make run-taskc    # add() is C, compute() is C++ — identical output
make run-taskcc   # BOTH are C — still identical output
```

All print:

```text
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

Neither side can tell the difference, because there *is* no meaningful
difference. Whether `add` and `compute` come from the C++ front-end or from our
hand-written C, they are the same struct-with-two-function-pointers, laid out
the same way, resumed by the same indirect call. The C++ `Task<T>` drives them
identically; a C `compute` drives a C++ `add` and vice-versa. They all agree on
the layout, so they all interoperate.

---

## 8. The same code on WebAssembly

Everything so far ran on arm64 macOS. If a coroutine frame is really "just a
struct, plus a calling convention," then the *same* [cadd.c](cadd.c) and
[compute.c](compute.c) should port to a wildly different target with only the
target-specific bits changing. WebAssembly (wasm32) is a good stress test: it is
a **stack machine with no CPU registers at all**, and its pointers are **4 bytes,
not 8**. Exactly two things from the earlier sections were ever target-specific —
and each one changes in an instructive way.

### The trampoline disappears

Section 5's whole difficulty was that arm64 passes the hidden return-slot pointer
in register `x8`, which plain C cannot name. WebAssembly has *no registers*, so
there is nothing to name: on a stack machine everything is passed as an ordinary
argument, and the hidden sret pointer simply becomes the **first parameter**,
pushing the real arguments over by one:

```text
    Task<int> add(int a, int b)   ==ABI==>   void _Z3addii(Task *ret, int a, int b)
    Task<int> compute()           ==ABI==>   void _Z7computev(Task *ret)
```

A pointer parameter is something C can express directly, so the assembly vanishes
and the entry points become plain C functions. An `__asm__` label still supplies
the exact C++ mangled name — but note there is **no Mach-O leading underscore** on
wasm, so the label is written exactly as the mangler emits it (`"_Z3addii"`, not
`"__Z3addii"`):

```c
// add(): fill the caller's return slot with a fresh frame, then return.
struct cadd_task { void *handle; };
void cadd_entry(struct cadd_task *ret, int a, int b) __asm__("_Z3addii");
void cadd_entry(struct cadd_task *ret, int a, int b) {
    ret->handle = cadd_make(a, b);   // *sret = frame  => Task{ handle }
}

// compute(): same idea, no int args.
void compute_entry(struct compute_task *ret) __asm__("_Z7computev");
void compute_entry(struct compute_task *ret) {
    ret->handle = compute_make();
}

// call_add(): now WE are the caller — just pass a local slot as the first arg.
void *call_add(int a, int b) {
    struct compute_task slot;
    add_entry(&slot, a, b);          // add writes the frame into slot.handle
    return slot.handle;
}
```

That is the whole ABI shim: no saved registers, no stack frame arithmetic, no
`x8`. The concept from section 5 — "respect how the platform returns a
non-trivial value by hand" — is unchanged; only the mechanism (a parameter
instead of a register) is simpler.

### Pointers shrink, so the offsets shrink

wasm32 is an ILP32 target: a pointer is 4 bytes. That is exactly why the real
code asserts offsets in units of `sizeof(void*)` instead of hard-coding `16` and
`24`. The *shape* of the frame is identical; the byte offsets just scale with the
pointer:

| field                  | offset (64-bit) | offset (wasm32) |
|------------------------|-----------------|-----------------|
| `resume`               | 0               | 0               |
| `destroy`              | 8               | 4               |
| `result` (promise)     | 16              | 8               |
| `continuation`         | 24              | 12              |

And the whole frame shrinks with it: `cadd`'s frame goes from **40 bytes to 24**,
`compute`'s from **56 bytes to 32**. The `_Static_assert(... == 2 * sizeof(void
*))` lines pass unchanged on both targets — which is the entire point of writing
them that way. A version that had hard-coded `== 16` would have failed to compile
here, correctly flagging that the layout had moved.

### How this was verified

The port was checked by compiling both files to WebAssembly objects with a recent
Clang and a wasm sysroot (Clang's own resource headers plus a bundled wasi-musl):

```sh
clang-22 -target wasm32-wasip1 -nostdinc \
    -isystem "$(clang-22 -print-resource-dir)/include" \
    -isystem <wasi-musl-include> -isystem <generic-musl-include> \
    -std=c11 -Wall -Wextra -O2 -c compute.c cadd.c
```

Disassembling the result confirms the ABI story: `_Z3addii` stores the new frame
pointer into `[param0]` (the caller's slot) and returns nothing; `_Z7computev`
does the same with no int args; and `call_add` calls `_Z3addii` with
`(&slot, a, b)` and returns `slot.handle`. The `_Static_assert`s compiling clean
is itself the proof that the offsets are right for a 4-byte pointer.

> **Honest scope.** This verifies **compilation** — that the frame layout, ABI
> shims, and mangled symbols are correct for wasm32. It is not a runnable module:
> that would additionally require building the C++ side (`task_main` and
> `Task<T>`, with libc++ coroutine support) for wasm32 and linking it all under a
> WebAssembly runtime. The point here is narrower and exact: the *same* hand-built
> C frames are ABI-correct on a second, very different target.

The lesson reinforces the whole document: a coroutine is a portable *idea* — a
struct with two function pointers and a promise. What differs between arm64 and
wasm32 is only how a non-trivial return value is handed over (a register vs. a
parameter) and how wide a pointer is. Neither touches the concept.

---

## 9. Takeaways

- A coroutine is **a heap struct** whose first two fields are a *resume* and a
  *destroy* function pointer, followed by a promise and saved locals.
- `resume` / `destroy` / `co_await` boil down to **reading a function pointer and
  calling it**, plus storing a "continuation" so the awaited coroutine knows who
  to wake.
- Because it's just data and functions, another language (C) can produce a
  coroutine that C++ happily drives — as long as both sides agree on the memory
  layout and the calling convention.
- What C++20 buys you is not new hardware capability; it's the compiler writing
  all of this bookkeeping *correctly and automatically* so you don't hand-roll
  frames and trampolines. This demo hand-rolls one only to show that you *could*.

> **Caution.** The hand-built version depends on Clang's coroutine ABI — the frame
> layout, the mangled names, and the way a non-trivial value is returned. Those
> details are target-specific (section 8 showed how they shift between arm64
> macOS and wasm32), and a future compiler is free to change them. It is a
> teaching device, not something to ship. Real code should always let the
> compiler generate coroutine frames.
