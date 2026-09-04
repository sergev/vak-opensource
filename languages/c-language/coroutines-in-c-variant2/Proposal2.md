# Proposal: native coroutines for C

Status: draft for discussion. Nothing here is settled.

---

## 0. Design rules

Five constraints drive every decision below.

1. **Stackless.** A coroutine lowers to a struct plus a switch. No separate stack, no
   stack switching, no Asyncify, no JSPI. The output runs on wasm32, on a Cortex-M0,
   and on a host with equal semantics.
2. **No hidden allocation.** The compiler never calls an allocator. Frame storage is
   always supplied by the caller, either as an object with automatic or static storage
   duration, or as bytes from an explicit allocator.
3. **No customization points.** There is no `promise_type`, no awaiter protocol, no
   library type the compiler must know about. The language provides suspension and
   delegation. Everything else — schedulers, I/O, timers, cancellation policy — is
   ordinary C code with no privileged status.
4. **No undefined behavior.** Every misuse listed in §7 is either a compile-time error
   or a defined trap.
5. **Fixed surface.** Two keywords, one type constructor, seven library-level
   operations. That is the whole feature.

The consequence of rule 3 is worth stating plainly, because it is the load-bearing idea:
**`await` is defined entirely in terms of `yield`.** Awaiting a sub-coroutine means
running it and forwarding its suspensions up to your own caller. The language therefore
needs no notion of "an I/O operation," "a scheduler," or "readiness." A yielded value is
whatever your runtime says it is.

---

## 1. Declaring a coroutine

```
coro ( yield-type ) return-type identifier ( parameter-list ) ;
```

`coro(Y) T f(...)` declares a coroutine that suspends yielding values of type `Y` and,
when it finishes, produces a value of type `T`.

```c
coro(int)      void  counter(int limit);        // yields ints, returns nothing
coro(io_req *) isize read_line(fd f, buf *b);   // yields I/O requests, returns a count
coro(void)     int   tick(void);                // suspends, but yields no value
```

`coro(void)` permits bare `yield;` and forbids `yield expr;`.

A coroutine function may not be `inline`, may not be variadic, and may not have its
address taken as an ordinary function pointer. §5 covers indirect calls.

---

## 2. Frames

The frame is the coroutine's state: its parameters, its locals that live across a
suspend point, and its resume index. Its size and alignment are known at compile time.

```c
coro_frame(counter) f;                    // automatic storage
static coro_frame(read_line) g;           // static storage
void *p = alloc(a, co_sizeof(read_line),  // explicit allocator
                   co_alignof(read_line));
```

`coro_frame(f)` is an incomplete-until-defined object type, unique per coroutine.

**Frames are address-stable and non-movable.** A frame may hold pointers into itself.
Assigning, `memcpy`ing, or returning a frame by value is a constraint violation
diagnosed at compile time. This replaces the role `Pin` plays in Rust; because frames
are always named objects rather than values, the restriction is checkable syntactically.

A frame's size is a property of one function alone. Frames are never nested inside other
frames, even for `await`. This costs one indirection per delegation level and buys three
things: frame size never requires whole-program analysis, recursion is legal, and
separate compilation works without any cross-module frame-layout protocol.

---

## 3. Operations on frames

Seven operations. All are compiler intrinsics with the syntax of function calls; none
require a header, and none can be shadowed.

| Operation | Meaning |
|---|---|
| `co_init(&f, fn, args...)` | Initialize `f` for `fn`, copying `args` into the frame. Runs no body code. |
| `co_resume(&f)` | Run until the next suspend or until return. Yields a `co_status`. |
| `co_cancel(&f)` | Resume, delivering `CO_CANCEL` at the suspend point. |
| `co_done(&f)` | `true` once the body has returned. |
| `co_value(&f)` | The most recently yielded value. Type `Y`. |
| `co_result(&f)` | The return value. Type `T`. |
| `co_sizeof(fn)` / `co_alignof(fn)` | Frame size and alignment. Integer constant expressions. |

```c
typedef enum { CO_SUSPENDED, CO_DONE } co_status;
```

**Coroutines start suspended.** `co_init` copies arguments and sets the resume index to
zero. No user code runs until the first `co_resume`. Eager start is rejected: it makes
the point at which side effects happen depend on the call site rather than the source.

`co_value` is valid only after a `co_resume` that returned `CO_SUSPENDED`. `co_result`
is valid only after `CO_DONE`. Both trap otherwise (§7).

---

## 4. Suspending

### 4.1 `yield`

```
yield expr ;      // when Y is not void
yield ;           // when Y is void
```

`yield` is an expression of type `co_signal`:

```c
typedef enum { CO_CONTINUE, CO_CANCEL } co_signal;
```

It evaluates to `CO_CONTINUE` when the coroutine was resumed by `co_resume`, and to
`CO_CANCEL` when resumed by `co_cancel`. There is no unwinding — C has no
exceptions, and adding an unwind path for cancellation would smuggle in a second
control-flow mechanism. Cancellation is cooperative and visible in the source:

```c
coro(int) void counter(int limit) {
    for (int i = 0; i < limit; i++)
        if (yield i == CO_CANCEL)
            return;
}
```

Ignoring the result of `yield` is legal; a coroutine that never checks simply cannot be
cancelled at that point and will run to its next check. Compilers should warn when a
coroutine contains no cancellation check on any path.

> **Open question.** If extended C adopts `defer`, cancellation cleanup should run deferred
> blocks on the early-return path automatically, and this section gets simpler. If it
> does not, cancellation cleanup is entirely manual. This is the single largest
> unresolved dependency in the design.

### 4.2 `await`

```
await frame-pointer
```

`await &sub` runs `sub` to completion and evaluates to `co_result(&sub)`. Whenever `sub`
suspends, the awaiting coroutine also suspends, forwarding `sub`'s yielded value to its
own caller unchanged. Cancellation delivered to the awaiter is forwarded into `sub`.

`await &sub` is exactly equivalent to:

```c
while (co_resume(&sub) == CO_SUSPENDED)
    if (yield co_value(&sub) == CO_CANCEL)
        /* forward: next iteration uses co_cancel */ ;
co_result(&sub)
```

That equivalence is the specification, not an illustration of it. It follows that:

- The yield type of the awaited coroutine must be identical to the awaiter's `Y`.
  Two coroutines in one `await` chain speak the same protocol to the runtime.
- `await` on a `coro(void)` chain is legal and forwards bare suspensions.
- Delegation depth is a runtime cost, not a semantic one. An `await` chain of depth *n*
  costs *n* frames of storage and *n* resume calls per suspension. Deep chains should be
  flattened by the programmer, or elided by the optimizer under the rule in §6.

```c
coro(io_req *) isize read_line(fd f, buf *b) {
    isize total = 0;
    for (;;) {
        isize n = await &(coro_frame(read_chunk)){0};   // see §8 on this form
        if (n <= 0) return total;
        total += n;
        if (memchr(b->data, '\n', (usize)n)) return total;
    }
}
```

---

## 5. Indirect calls

A coroutine's frame size is not knowable from a function pointer, so `coro` functions do
not convert to ordinary function pointers. Instead:

```c
coro_ptr(io_req *, isize) p = &read_line;
```

`coro_ptr(Y, T)` is a fat pointer: a resume-function address plus a descriptor holding
frame size, alignment, and the init thunk. `co_sizeof(p)` on a `coro_ptr` is a runtime
value rather than a constant expression; everything else in §3 works unchanged.

This is the only place in the design where a runtime value replaces a constant, and it
exists so that a scheduler can hold a heterogeneous list of tasks.

---

## 6. Lowering

A coroutine `coro(Y) T f(A a, B b)` lowers to:

```c
struct f__frame {
    u32 state;          // 0 = initial, 1..n = suspend points, ~0u = done
    u32 flags;          // bit 0: running (reentrancy guard)
    A a; B b;           // parameters
    /* locals live across a suspend point */
    Y yielded;
    T result;
};

co_status f__resume(struct f__frame *, co_signal);
```

`f__resume` is a single function with a `switch (frame->state)` prologue dispatching to
the instruction after each suspend point. Locals that do not live across any suspend
point stay on the machine stack and are absent from the frame; this is a required
optimization, not an optional one, because it determines whether frame sizes are
tolerable on small targets.

**Frame layout is part of the ABI** and must be documented per target: field order
follows declaration order after the header, with standard struct padding rules.

**Elision.** When a coroutine is awaited exactly once, from a known call site, and its
frame does not escape, the implementation may allocate the callee's frame inside the
caller's frame and inline the resume function. This is observable only through
`co_sizeof` on the caller, so `co_sizeof` inhibits elision for that coroutine. Elision
is never required for a program to be correct; a debug build with elision fully disabled
must behave identically apart from memory use.

**WebAssembly.** The lowering above compiles to a wasm function plus a struct in linear
memory. No stack switching proposal, no JSPI, no Binaryen post-processing, no
host-provided event loop. A Braam scheduler is a wasm function that calls
`f__resume` in a loop.

---

## 7. Defined behavior for misuse

We assume our enhanced C has no undefined behavior, so every one of these has an answer.

**Compile-time errors:**

| Condition | Rationale |
|---|---|
| `yield` or `await` outside a coroutine body | |
| `yield expr` where `Y` is `void`, or bare `yield` where it is not | |
| `await` of a coroutine whose yield type differs from the awaiter's | §4.2 |
| VLA or `alloca` whose lifetime crosses a suspend point | frame must be fixed size |
| Copying, assigning, or returning a `coro_frame` by value | §2 |
| Taking a plain function pointer to a `coro` function | §5 |
| `co_resume` on a frame the compiler can prove is uninitialized | |

**Traps** (an abort with a diagnostic, in every build configuration — these are not
assertions to be compiled out):

| Condition | Trap |
|---|---|
| `co_resume` or `co_cancel` on an uninitialized frame | `CO_TRAP_UNINIT` |
| `co_resume` or `co_cancel` on a finished frame | `CO_TRAP_FINISHED` |
| Reentrant resume — resuming a frame already running | `CO_TRAP_REENTRANT` |
| `co_value` when the last status was not `CO_SUSPENDED` | `CO_TRAP_NO_VALUE` |
| `co_result` before `CO_DONE` | `CO_TRAP_NOT_DONE` |
| Falling off the end of a non-`void` coroutine | `CO_TRAP_NO_RETURN` |

The reentrancy guard is one bit in the frame header and one branch per resume. It also
covers the common threading mistake: two threads resuming one frame concurrently will
reliably trap rather than corrupt the frame, provided the flag is read and written
atomically on targets with threads.

**Not covered.** Destroying a suspended frame — letting an automatic-storage frame go
out of scope mid-suspension — leaks anything the coroutine owned. The language cannot
detect this without ownership tracking, which is out of scope per the tier-one decision.
The idiom is `co_cancel` then let it fall out of scope, and a lint should flag suspended
frames at end of scope.

---

## 8. Worked examples

### 8.1 Generator

```c
coro(int) void range(int lo, int hi) {
    for (int i = lo; i < hi; i++)
        if (yield i == CO_CANCEL) return;
}

void print_range(void) {
    coro_frame(range) f;
    co_init(&f, range, 0, 10);
    while (co_resume(&f) == CO_SUSPENDED)
        print_int(co_value(&f));
}
```

No allocation, no runtime, no library. The frame is a stack object.

### 8.2 A scheduler, in user code

The runtime is not privileged. Here `io_req *` is kernel's own type; the compiler has
never heard of it.

```c
typedef struct { fd f; u8 op; void *buf; usize len; isize out; } io_req;

void run(coro_ptr(io_req *, void) *tasks, usize n, arena *a) {
    void **frames = arena_alloc(a, n * sizeof(void *), alignof(void *));
    for (usize i = 0; i < n; i++) {
        frames[i] = arena_alloc(a, co_sizeof(tasks[i]), co_alignof(tasks[i]));
        co_init(frames[i], tasks[i]);
    }
    usize live = n;
    while (live) {
        for (usize i = 0; i < n; i++) {
            if (!frames[i]) continue;
            if (co_resume(frames[i]) == CO_DONE) { frames[i] = 0; live--; continue; }
            io_req *r = co_value(frames[i]);
            submit(r);                    // your I/O layer, whatever it is
        }
        drain_completions();              // fills in r->out
    }
}
```

Swap `submit`/`drain_completions` for io_uring, for a wasm host import, or for a bare
polling loop on an MCU. Nothing in the language changes.

### 8.3 Delegation

```c
coro(io_req *) isize read_exact(fd f, u8 *p, usize n) {
    usize got = 0;
    while (got < n) {
        io_req r = { .f = f, .op = OP_READ, .buf = p + got, .len = n - got };
        if (yield &r == CO_CANCEL) return -1;
        if (r.out <= 0) return (isize)got;
        got += (usize)r.out;
    }
    return (isize)got;
}

coro(io_req *) int read_header(fd f, header *h) {
    coro_frame(read_exact) sub;
    co_init(&sub, read_exact, f, (u8 *)h, sizeof *h);
    isize n = await &sub;
    return n == (isize)sizeof *h ? 0 : -1;
}
```

Note that `r` is a local whose address is yielded. Because the frame is address-stable,
this is well defined for exactly as long as the coroutine is suspended at that point —
which is precisely the window in which the scheduler uses it.

---

## 9. Deliberately excluded

- **Awaiting arbitrary "awaitables."** Only coroutine frames are awaitable. Everything
  else is `yield`.
- **`co_select` / joining / task groups.** Library concerns. A `select` is a scheduler
  that resumes several frames and stops at the first `CO_DONE`.
- **Symmetric transfer.** Asymmetric only: a coroutine returns to its resumer. Symmetric
  transfer saves a frame of dispatch overhead in deep chains and costs a large amount of
  specification complexity.
- **Eager start, implicit allocation, allocator customization hooks, `noexcept`-style
  annotations, return-object conversion.** These are the parts of C++20 coroutines that
  make it a framework rather than a feature.
- **Coroutines as thread-safe objects.** A frame is resumed by one thread at a time. The
  reentrancy trap enforces this rather than a lock.

---

## 10. Open questions

1. **`defer` interaction** (§4.1). The biggest one. Cancellation without automatic
   cleanup puts real burden on the programmer.
2. **Yield-type uniformity in `await` chains** (§4.2) is a strong constraint. An adapter
   form, `await &sub mapping expr`, would relax it at the cost of a second delegation
   rule. Probably not worth it, but it should be argued rather than assumed.
3. **Is `co_signal` on `yield` the right shape,** or should cancellation be checked with
   an explicit `co_cancelled()` and `yield` return nothing? The current form makes the
   check hard to forget, which is the point, but it reads oddly in `if (yield i == ...)`
   and will need parenthesization rules.
4. **Frame layout stability across compiler versions.** Committing frame layout to the
   ABI (§6) enables separate compilation and debuggers, and freezes optimization
   choices. An alternative is to make layout opaque and require `co_sizeof` from the
   defining translation unit.
5. **Naming.** `coro` / `yield` / `await` are not reserved in C. If we want a strict
   C compatibility mode, these become `_Coro` / `_Yield` / `_Await` with macros in a
   `<coro.h>` header.
