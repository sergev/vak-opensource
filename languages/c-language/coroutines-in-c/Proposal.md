# Proposal: native coroutines for C, ABI-compatible with C++

**Status:** draft / discussion document
**Scope:** a language + runtime-library extension for C
**Companion demos:** [cadd.c](cadd.c), [compute.c](compute.c), [task.hpp](task.hpp), [Coroutines_In_C.md](Coroutines_In_C.md)

---

## 1. Motivation and summary

The rest of this project spends four demos proving, by hand, a single claim: a
C++20 coroutine is nothing but **a heap struct whose first two fields are a
`resume` and a `destroy` function pointer, followed by a promise object and the
saved local state.** We took that claim literally and re-implemented C++
coroutines in plain C:

- [cadd.c](cadd.c) re-implements the straight-line `add()` coroutine, and
- [compute.c](compute.c) re-implements the two-`co_await` `compute()` coroutine
  as an explicit re-entrant state machine,

and the **unchanged** C++ `Task<T>` ([task.hpp](task.hpp)) and `task_main`
driver ran them across the language boundary — with byte-identical output,
clean under AddressSanitizer.

It works, but the cost is telling. To make plain C speak the coroutine ABI we
had to write, by hand, all of this:

- a `struct` mirroring the compiler's coroutine frame, with the promise fields
  nailed to exact byte offsets and guarded by `_Static_assert`;
- an explicit `switch (state)` state machine to fake suspend/resume, plus manual
  bookkeeping of which child frame to destroy and *when* (the
  destroy-after-resume hazard, §10);
- arm64 assembly trampolines (`__Z7computev`, `_call_add`) to satisfy the C++
  **name mangling** and the **sret/`x8`** return convention that plain C cannot
  express.

None of that is fundamental. It is exactly the bookkeeping a C++ compiler
generates automatically. **This document proposes the language and runtime
additions that would let a C compiler generate it too** — so the C source
becomes ordinary straight-line code again, and the asm, the offset asserts, and
the hand-rolled state machine all disappear.

Three decisions frame the whole design:

1. **Compatibility is runtime ABI-level interop.** A C coroutine and a C++
   coroutine share the *same* Itanium coroutine frame layout and the *same*
   customization protocol, so either can `co_await`, resume, or destroy the
   other — precisely what the demos did manually.
2. **The full C++ customization protocol is exposed, translated to C
   conventions.** C has no member functions, templates, or overloading, so the
   promise/awaiter "methods" become free functions bound by a naming convention
   at compile time. Nothing is hidden; C can define its own `Task`/`Generator`.
3. **The keywords keep their C++ spelling:** `co_await`, `co_yield`,
   `co_return`.

The guiding principle throughout: **adopt the existing Itanium C++ ABI
coroutine lowering verbatim and change only the surface spelling.** Same frame,
same promise offsets, same resume/destroy contract, same return-value
classification ⇒ the two front-ends emit interchangeable coroutines.

---

## 2. Goals and non-goals

### Goals

- Native `co_await` / `co_yield` / `co_return` in C, with C++ semantics.
- **User-definable** promise and awaiter types in C — not a fixed built-in set —
  so a C programmer can write the equivalent of `Task<T>` or `Generator<T>`.
- Coroutine frames that are **byte-identical** to those emitted by the platform
  C++ compiler (Clang/GCC), so C and C++ coroutines interoperate at runtime.
- Zero hand-written assembly and zero `_Static_assert`-ed offsets for the common
  case: the compiler emits the frame, the sret entry, and the mangled symbol.

### Non-goals (for this first version)

- **Full C↔C++ exception propagation.** Plain C has no exceptions; §8 defines
  conservative boundary behavior and defers real propagation.
- **A new cross-vendor psABI.** We deliberately *reuse* the platform's existing
  C++ coroutine ABI (Itanium on the demo platform) rather than invent one. Where
  a target's C++ ABI differs, C follows the same target rules.
- **A rich standard coroutine library.** We specify a small reference header,
  `<stdcoro.h>` (§11); a `std::generator`-class convenience type is left to
  future library work.
- Changing how *existing* C code compiles: the feature is opt-in (§3).

---

## 3. Surface syntax

Three keywords are added, with the **same grammar productions as C++**:

```c
co_await  unary-expression         // pause until the awaitable is ready
co_yield  assignment-expression    // produce a value, then pause
co_return expression(opt) ;        // finish the coroutine (with a value)
```

As in C++, **a function is a coroutine if and only if its body uses any one of
these keywords.** There is no separate "coroutine" declarator; the return type's
associated promise (§4) supplies everything else. `main` may not be a coroutine,
matching C++.

### Opt-in and the identifier-clash hazard

`co_await`, `co_yield`, and `co_return` are ordinary identifiers in existing C —
a conforming program may already use them as names. They therefore **cannot be
unconditionally reserved** without breaking code. The feature is gated two ways,
either of which enables the keywords in a translation unit:

- a new standard mode (e.g. `-std=c2y` or later) that reserves the three tokens;
  and/or
- including `<stdcoro.h>`, which is the header a coroutine author needs anyway.

A predefined macro `__STDC_CORO__` (set to the version date) lets code detect
support. Inside a translation unit where the feature is off, the tokens remain
ordinary identifiers. This mirrors how C introduced `_Bool`/`bool`,
`_Static_assert`, and `_Atomic` without disturbing legacy spellings.

---

## 4. Associating a return type with its promise

In C++, the compiler finds a coroutine's promise type through the template
`std::coroutine_traits<ReturnType, Args...>::promise_type`. C has no templates,
so we need another way to answer the same question: *given a function that
returns `Task`, what struct is its promise?*

**Proposal: an attribute on the coroutine's return type that names the promise
struct tag.** Using C23 attribute syntax:

```c
typedef struct Task Task;

/* The handle wrapper the coroutine returns. Its promise is `struct Task_promise`. */
struct [[coro_promise(Task_promise)]] Task {
    coroutine_handle handle;   /* from <stdcoro.h>; just a pointer */
};
```

(An equivalent `__attribute__((coro_promise(Task_promise)))` spelling is
available for pre-C23 modes.)

This attribute does **two** jobs:

1. It tells the compiler which promise struct to instantiate inside the frame
   and which convention functions to call (§5).
2. It marks the return type as **"non-trivial for the purposes of calls"** —
   the same ABI classification C++ applies to `Task<T>` because it has a
   user-provided move-constructor/destructor. That classification is what forces
   the value to be returned **indirectly** (via the sret pointer, in register
   `x8` on arm64). By deriving it from the attribute, the C compiler emits the
   exact entry convention that [compute.c](compute.c)'s hand-written
   `__Z7computev` / `_call_add` trampolines had to fake.

The programmer never writes offsets or asserts: the compiler lays out
`resume`/`destroy`/promise/locals itself, identically to C++.

---

## 5. The customization protocol as C convention functions

C++ drives a coroutine by calling **methods on the promise object** at
well-defined points. Because that resolution is *static* (chosen at compile
time), it is exactly what makes the generated code — and therefore the ABI —
deterministic. We keep the static resolution and only change *how the functions
are named*: for a promise struct tagged `P`, the compiler binds free functions
named `P_<point>`, taking `struct P *` as their first argument.

| C++ (`promise.method()`)      | C convention function                              | When the compiler calls it                     |
|-------------------------------|----------------------------------------------------|-------------------------------------------------|
| `get_return_object()`         | `P_get_return_object(struct P *)`                  | once, to build the returned object              |
| `initial_suspend()`           | `P_initial_suspend(struct P *)`                    | before the body runs                            |
| `final_suspend()` *noexcept*  | `P_final_suspend(struct P *)`                      | after the body finishes                         |
| `yield_value(v)`              | `P_yield_value(struct P *, V v)`                   | at each `co_yield v`                            |
| `return_value(v)`             | `P_return_value(struct P *, V v)`                  | at `co_return v`                                |
| `return_void()`               | `P_return_void(struct P *)`                        | at `co_return;` / running off the end           |
| `unhandled_exception()`       | `P_unhandled_exception(struct P *)`                | on an escaping failure (see §8)                 |
| `await_transform(x)` *(opt.)* | `P_await_transform(struct P *, X x)`               | wraps each `co_await x`, if defined             |

Each returns the same thing its C++ counterpart does (an awaitable, a value,
`void`), and the compiler generates the identical calls at the identical points
in the frame. A promise that defines `P_return_value` is a "task-like"
coroutine; one that defines `P_yield_value` + `P_return_void` is a
"generator-like" coroutine — exactly the C++ distinction, expressed by which
convention functions exist.

### Frame allocation and elision

C++ allocates the frame with `operator new` (overridable per-promise) and
permits **HALO** — eliding the allocation entirely when the frame provably does
not outlive the caller. C has no `operator new`, so:

- the **default** allocator is `malloc` / `free`;
- a promise may override it with optional hooks
  `P_operator_new(size_t)` / `P_operator_delete(void *, size_t)`;
- the compiler is likewise **permitted to elide** the allocation (HALO) when it
  can prove the frame does not escape.

Crucially, allocation is *internal* to lowering — it happens before any handle
is handed out. So a C coroutine allocating with `malloc` and a C++ coroutine
allocating with `operator new` still interoperate perfectly: only the
`coroutine_handle` (a bare pointer to the frame) ever crosses the boundary, and
whoever created the frame is whoever destroys it via the frame's own `destroy`
pointer.

---

## 6. Awaiters and `coroutine_handle`

`co_await e` needs an **awaiter** — a small object with three methods. In C++
those are `await_ready` / `await_suspend` / `await_resume`; here they are
convention functions on the awaiter struct tag `A`:

| C++ awaiter method                    | C convention function                              |
|---------------------------------------|----------------------------------------------------|
| `await_ready()`                       | `A_await_ready(struct A *)` → `bool`               |
| `await_suspend(handle)`               | `A_await_suspend(struct A *, coroutine_handle)`    |
| `await_resume()`                      | `A_await_resume(struct A *)` → value of `co_await` |

`await_suspend` may return `void`, `bool`, or a `coroutine_handle`; returning a
handle performs **symmetric transfer** (resume that coroutine directly), just as
in C++. This is how [task.hpp](task.hpp)'s awaiter returns the awaited task's
handle to start it running, and how the final-awaiter hands control to the
continuation.

When the awaited expression is not itself an awaiter, C++ consults
`operator co_await`. The C equivalent is a convention function on the awaited
type `T` — `T_operator_co_await(struct T *)` returning an awaiter — so
`co_await someTask` finds it the same way.

### `coroutine_handle`

`<stdcoro.h>` provides a `coroutine_handle` type that is **just a pointer to the
frame**, layout-compatible with C++'s `std::coroutine_handle<>`. The operations
are the ones [compute.c](compute.c) open-coded by hand, now standard:

| Operation                    | Meaning (what the demo did by hand)                            |
|------------------------------|----------------------------------------------------------------|
| `coro_resume(h)`             | call the frame's first function pointer (`frame[0]`)           |
| `coro_destroy(h)`            | call the frame's second function pointer (`frame[1]`)          |
| `coro_done(h)`               | true iff the resume pointer has been nulled                    |
| `coro_promise(h, P)`         | typed pointer to the promise inside the frame                  |
| `coro_from_promise(p)`       | recover the handle from a promise pointer                      |
| `coro_address(h)` / `coro_from_address(v)` | round-trip to `void *`                            |
| `coro_noop()`                | the no-op coroutine (C++ `noop_coroutine()`)                   |

`<stdcoro.h>` also supplies the two trivial awaiters `suspend_always` and
`suspend_never`, layout-compatible with `std::suspend_always` /
`std::suspend_never` (both empty structs), with their `_await_*` convention
functions predefined. A promise's `P_initial_suspend` returning `suspend_always`
makes the coroutine **lazy**, exactly as in [task.hpp](task.hpp) and
[generator.hpp](generator.hpp).

---

## 7. Cross-language linkage

For a C coroutine to *be called by* C++ (or vice-versa), the two must agree on
the **symbol name** and the **calling convention**. In the demos we forced this
by hand: [cadd.c](cadd.c) exported the mangled name `_Z3addii` (that is
`add(int, int)`) and [compute.c](compute.c) exported `_Z7computev`, each via an
assembly stub that also managed the sret pointer in `x8`.

**Proposal: a linkage attribute that names the C++ signature**, letting the
compiler emit (or import) the exact Itanium-mangled symbol and the matching
return convention automatically:

```c
Task add(int a, int b) [[cxx_linkage("add(int, int)")]] {
    co_return a + b;
}
```

The compiler mangles `add(int, int)` to `_Z3addii`, and — because `Task`'s
return type is non-trivial-for-calls (§4) — uses the platform's indirect-return
convention. That single attribute replaces **both** hand-written asm stubs.
Symmetrically, a C caller that needs to invoke a C++ coroutine declares it with
the same attribute to import the mangled symbol.

For the reverse direction (C++ calling into C without any attribute), the C
coroutine can also be given plain C linkage and wrapped on the C++ side; but the
attribute is the clean path and the one this proposal recommends.

> Note: name mangling is inherently target-ABI-specific. The attribute takes a
> *source signature*, not a literal mangled string, so the same source is
> portable across targets while the compiler produces each target's correct
> symbol.

<!-- -->

> **Live evidence for the design.** The demos were later ported to wasm32
> ([Coroutines_In_C.md](Coroutines_In_C.md) §8), and they confirm exactly the
> split this attribute is built on. The "non-trivial-for-calls ⇒ return
> indirectly" *classification* is universal — it holds on wasm just as on arm64 —
> while the *mechanism* is a per-target detail the compiler already knows: arm64
> passes the sret pointer in register `x8`, whereas wasm (a register-less stack
> machine) passes it as an ordinary first parameter. On wasm the sret is thus
> already nameable in plain C, so the assembly shim vanishes even today; a
> `cxx_linkage` attribute would then only need to supply the mangled name, not a
> register dance. `cxx_linkage` naming a *source signature* rather than a mangled
> string is what lets one source line cover both.

---

## 8. Exceptions

C has no exceptions, which interacts with two customization points:

- **`P_unhandled_exception`** remains a required member of the protocol (so the
  frame layout and the set of calls match C++). In a plain-C coroutine it is
  reachable only through non-C++ failure paths; a reasonable default body simply
  calls `abort()`, mirroring the `std::terminate()` used in
  [task.hpp](task.hpp)/[generator.hpp](generator.hpp).
- **`final_suspend` is `noexcept`**, as in C++.

**Boundary rule (v1):** a C coroutine is treated as `noexcept` — **no exception
may propagate out of a C coroutine frame.** If C code is compiled in a mode that
can be traversed by a C++ exception (e.g. mixed unwinding), an exception that
would escape a C frame results in `unhandled_exception` / `abort`, never silent
corruption. Because the frame and cleanup structure still match the C++ layout,
the *handle* remains fully interoperable: a C++ coroutine can await a C one and
observe its result or its termination.

Full C↔C++ exception propagation (letting a C++ exception thrown inside an
awaited chain unwind through a C frame) is deferred to future work; it requires
agreeing on personality routines and cleanup landing pads, which is a larger ABI
undertaking than this proposal targets.

---

## 9. Worked example

Here is the entire Part-2 demo rewritten in the proposed C. Compare it with the
hand-built [cadd.c](cadd.c) / [compute.c](compute.c): there is **no frame
struct, no `_Static_assert`, no `switch (state)`, and no assembly.** The source
is ordinary straight-line code; the compiler generates everything the demos did
by hand.

### `task.h` — the awaitable, defined in C

```c
#include <stdcoro.h>

typedef struct Task Task;

struct Task_promise {
    int              result;         /* filled in by co_return       */
    coroutine_handle continuation;   /* who to resume when we finish */
};

struct [[coro_promise(Task_promise)]] Task {
    coroutine_handle handle;
};

/* The final-suspend awaiter: on finish, hand control to our continuation
   (symmetric transfer), or to the no-op coroutine if we are the top-level
   task. Declared before the promise, since final_suspend returns it. */
struct Task_final_awaiter { char _unused; };

static inline bool Task_final_awaiter_await_ready(struct Task_final_awaiter *a) {
    (void)a; return false;
}
static inline coroutine_handle
Task_final_awaiter_await_suspend(struct Task_final_awaiter *a, coroutine_handle me) {
    (void)a;
    struct Task_promise *p = coro_promise(me, Task_promise);
    return p->continuation ? p->continuation : coro_noop();
}
static inline void Task_final_awaiter_await_resume(struct Task_final_awaiter *a) {
    (void)a;
}

/* --- promise customization points ------------------------------------- */

static inline Task Task_promise_get_return_object(struct Task_promise *p) {
    return (Task){ coro_from_promise(p) };
}

/* Lazy: don't run the body until someone awaits (or drives) us. */
static inline suspend_always Task_promise_initial_suspend(struct Task_promise *p) {
    (void)p; return (suspend_always){0};
}

static inline struct Task_final_awaiter
Task_promise_final_suspend(struct Task_promise *p) {
    (void)p; return (struct Task_final_awaiter){0};
}

static inline void Task_promise_return_value(struct Task_promise *p, int v) {
    p->result = v;
}
static inline void Task_promise_unhandled_exception(struct Task_promise *p) {
    (void)p; abort();
}

/* Awaiting a Task: remember the awaiter as the awaited task's continuation,
   then symmetric-transfer into it. (C++: Task::operator co_await.) */
struct Task_awaiter { coroutine_handle coro; };

static inline struct Task_awaiter Task_operator_co_await(struct Task *t) {
    return (struct Task_awaiter){ t->handle };
}
static inline bool Task_awaiter_await_ready(struct Task_awaiter *a) {
    (void)a; return false;
}
static inline coroutine_handle
Task_awaiter_await_suspend(struct Task_awaiter *a, coroutine_handle awaiting) {
    coro_promise(a->coro, Task_promise)->continuation = awaiting;
    return a->coro;                       /* start the awaited task running */
}
static inline int Task_awaiter_await_resume(struct Task_awaiter *a) {
    return coro_promise(a->coro, Task_promise)->result;
}

/* Top-level driver for non-coroutine code. */
static inline int Task_get(Task t) {
    coro_resume(t.handle);
    return coro_promise(t.handle, Task_promise)->result;
}
```

### `add.c` and `compute.c` — the coroutines, as plain code

```c
#include "task.h"
#include <stdio.h>

Task add(int a, int b) [[cxx_linkage("add(int, int)")]] {
    printf("    add(%d, %d) is running\n", a, b);
    co_return a + b;
}

Task compute(void) [[cxx_linkage("compute()")]] {
    printf("compute: about to co_await add(1, 2)\n");
    int x = co_await add(1, 2);
    printf("compute: got x = %d\n", x);

    printf("compute: about to co_await add(%d, 10)\n", x);
    int y = co_await add(x, 10);
    printf("compute: got y = %d\n", y);

    co_return x + y;                       /* 16 */
}
```

That `compute` is the same nine lines as the C++ `compute()` — the compiler,
not the programmer, produces the frame, spills `x` across the suspend, emits the
`resume`/`destroy` functions, performs the destroy-after-resume cleanup (§10),
and provides the sret entry point under the mangled name `_Z7computev`. It links
against the **unchanged** C++ `task_main` and prints, byte-for-byte, the same
output as `make run-task` — `compute() = 16`.

---

## 10. Grammar and lowering notes

### Grammar additions

- *unary-expression*: add `co_await unary-expression`.
- *statement* (or a new *jump-statement* sibling): add
  `co_return expression(opt) ;`.
- *expression-context yield*: add `co_yield assignment-expression` as an
  expression (its value is what `yield_value` returns, usually discarded).

A *function-definition* whose compound statement contains any of these is a
**coroutine**; its declared return type must carry a `coro_promise` attribute
(§4), else it is a constraint violation (diagnosed, not silent).

### What the compiler must generate (all mirroring the Itanium C++ lowering)

1. **Frame:** a heap block with `resume` and `destroy` function pointers at
   offset `0` and `1`, the promise object next, then slots for every local and
   temporary that is live across a suspend point. Layout matches the target C++
   ABI so frames are interchangeable.
2. **Ramp function** (the function you call): allocate the frame (§5), construct
   the promise, call `P_get_return_object`, then evaluate
   `P_initial_suspend()` and suspend/continue accordingly; return the object.
3. **Resume/destroy bodies:** a resumable state machine keyed on a hidden
   suspend index (the `switch (f->state)` we wrote by hand). Each `co_await`
   lowers to the awaiter three-step — `await_ready`, and if not ready,
   `await_suspend` (honoring `void`/`bool`/handle returns for symmetric
   transfer), then on resumption `await_resume` produces the expression value.
4. **`co_yield v`** lowers to `co_await P_yield_value(&promise, v)`.
5. **`co_return`** stores via `P_return_value`/`P_return_void`, then runs
   `P_final_suspend()`.
6. **Cleanup ordering — the destroy-after-resume rule.** With non-tail
   symmetric transfer, a child frame is *still on the stack* (its resume is
   executing) when it transfers back into its awaiter. A child therefore must be
   destroyed by the invocation that **resumed** it, only *after* that resume has
   fully unwound — never by the re-entrant invocation that merely reads its
   result. This is the exact hazard [compute.c](compute.c) documents and handles
   by hand; the compiler must enforce it (as C++ compilers do), so the pattern
   is safe under AddressSanitizer.

---

## 11. `<stdcoro.h>` reference

The one new standard header. It contains no policy — only the primitives the
compiler and coroutine authors need:

- **Type** `coroutine_handle` — an opaque pointer to a frame, layout-compatible
  with `std::coroutine_handle<>`.
- **Handle operations** — `coro_resume`, `coro_destroy`, `coro_done`,
  `coro_promise(h, P)`, `coro_from_promise(p)`, `coro_address`,
  `coro_from_address`, `coro_noop`.
- **Trivial awaiters** — `suspend_always`, `suspend_never` (empty structs,
  layout-compatible with the C++ types) and their predefined `_await_ready` /
  `_await_suspend` / `_await_resume` convention functions.
- **Feature macro** — `__STDC_CORO__`.
- **Attribute spellings** — documentation of `coro_promise`,
  `cxx_linkage`, and the optional allocator/await-transform hooks, with
  `__attribute__` fallbacks for pre-C23 modes.

Everything richer — `Task`, `Generator`, executors — is ordinary user or
library code built on top, as `task.h` in §9 demonstrates.

---

## 12. Open questions and future work

- **psABI standardization.** This proposal *reuses* each target's existing C++
  coroutine ABI. Blessing a single documented layout in the platform psABI would
  let non-C++ toolchains interoperate without reverse-engineering (as we did).
- **Symmetric-transfer tail guarantee.** C++ relies on the "no unbounded stack
  growth" guarantee for chained symmetric transfers; C should adopt the same
  guarantee explicitly so deep `co_await` chains don't overflow.
- **Exception interop (§8).** Real C++-exception propagation through C frames
  needs personality-routine and unwind-table agreement.
- **A standard generator/task.** A convenience `<stdcoro.h>`-adjacent library
  (an ergonomic `generator`/`task`) would spare most users from writing the
  §9 boilerplate, analogous to C++23's `std::generator`.
- **Async/O/S integration.** Awaiters over timers, sockets, and thread pools —
  the payoff use case — are left to library design once the core lands.

---

### Appendix: what each proposed feature deletes from the demos

| Hand-written in the demos                                   | Replaced by                                    |
|-------------------------------------------------------------|------------------------------------------------|
| `struct frame { … }` + `_Static_assert` offsets ([compute.c](compute.c)) | compiler-generated frame (§4, §10)  |
| `switch (f->state)` state machine ([compute.c](compute.c))  | `co_await` lowering (§10)                      |
| manual `continuation` wiring + symmetric transfer           | awaiter protocol (§6)                          |
| destroy-after-resume bookkeeping by hand                    | enforced by the compiler (§10)                 |
| `__Z7computev` sret asm entry                               | `coro_promise` non-trivial-for-calls (§4)      |
| `_call_add` sret asm shim + `_Z3addii` mangling             | `cxx_linkage` attribute (§7)                    |

On wasm32 the two asm shims already vanish today (the sret pointer is a plain
first parameter, not a register), so there `cxx_linkage` would supply only the
mangled name — see [Coroutines_In_C.md](Coroutines_In_C.md) §8.
