// compute.c — the compute() coroutine, re-implemented in plain C11.
//
// This is the harder sibling of cadd.c. Where add() runs straight through,
// compute() has TWO `co_await` points, so it is a genuine state machine: it
// must be able to pause, hand control to a child coroutine, and later resume
// exactly where it left off — with its local variables intact.
//
// A compiler implements a coroutine's suspend/resume by (a) storing every local
// that must survive a pause inside the heap frame, and (b) keeping a small
// "where am I?" state index that the resume function switches on. We do exactly
// that, by hand, below. The result links against the unchanged C++ task_main /
// Task<T>, and drives real add() coroutines (C or C++) via co_await.
//
// Frame/ABI layout is the same Task<int> contract reverse-engineered in cadd.c.
// The exact byte offsets depend on the pointer width (P = sizeof(void*)):
//
//                        LP64 (arm64)   ILP32 (wasm32)
//   frame + 0*P  resume    (fn ptr)   0              0
//   frame + 1*P  destroy   (fn ptr)   8              4
//   frame + 2*P  promise.result       16             8
//   frame + 3*P  promise.continuation 24             12
//   after        our private state (state, x, c1, c2)
//
// NOT portable standard C: the offsets follow Clang's Itanium coroutine ABI and
// the entry shims at the bottom are per-target (arm64 macOS asm, or plain C on
// wasm32). We let the struct lay itself out naturally and assert the offsets in
// units of sizeof(void*) so the same source is correct on 64- and 32-bit.

#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>

// One struct serves for compute's own frame AND for the child add() frames —
// the first 4 words are the shared Task<int> layout; compute only ever touches
// a child's first four fields.
struct frame {
    void (*resume)(void *);   // + 0*sizeof(void*)
    void (*destroy)(void *);  // + 1*sizeof(void*)
    int   result;             // + 2*sizeof(void*)  promise.result
    void *continuation;       // + 3*sizeof(void*)  promise.continuation
    int   state;              // our state-machine index
    int   x;                  // saved local: result of the first co_await
    void *c1;                 // first  child (add) handle
    void *c2;                 // second child (add) handle
};

_Static_assert(offsetof(struct frame, result) == 2 * sizeof(void *),
               "promise.result must sit right after the two frame fn pointers");
_Static_assert(offsetof(struct frame, continuation) == 3 * sizeof(void *),
               "promise.continuation must follow promise.result (ptr-aligned)");

// "Resume this coroutine" / "destroy this coroutine" are literally: call the
// function pointer at the front of its frame.
static inline void coro_resume(void *h)  { (*((struct frame *)h)->resume)(h); }
static inline void coro_destroy(void *h) { (*((struct frame *)h)->destroy)(h); }

// Provided by the entry shim at the bottom: calls the real add(int,int)
// (symbol _Z3addii, C or C++) using add's sret calling convention and returns
// the resulting coroutine-frame pointer (i.e. the Task<int>'s handle).
extern void *call_add(int a, int b);

// The coroutine body, as a state machine. It is re-entered once per co_await:
// when a child add() finishes, it resumes *us* (its continuation), which lands
// back here at the next case.
static void compute_resume(void *p)
{
    struct frame *f = p;
    switch (f->state) {
    case 0:  // start of the coroutine, up to the first co_await
        printf("compute: about to co_await add(1, 2)\n");
        f->c1 = call_add(1, 2);
        ((struct frame *)f->c1)->continuation = f;  // "add, wake me when done"
        f->state = 1;
        coro_resume(f->c1);   // symmetric transfer: run add -> it re-enters us
        // We only get here after the child's resume has FULLY returned, so it is
        // now safe to destroy the child frame (its resume is off the stack).
        coro_destroy(f->c1);
        return;

    case 1:  // resumed by the first add(); its result is ready
        f->x = ((struct frame *)f->c1)->result;     // the value of `co_await`
        printf("compute: got x = %d\n", f->x);

        printf("compute: about to co_await add(%d, 10)\n", f->x);
        f->c2 = call_add(f->x, 10);
        ((struct frame *)f->c2)->continuation = f;
        f->state = 2;
        coro_resume(f->c2);
        coro_destroy(f->c2);
        return;

    case 2: {  // resumed by the second add(); finish up
        int y = ((struct frame *)f->c2)->result;
        printf("compute: got y = %d\n", y);

        f->result = f->x + y;   // this is `co_return x + y;`  (16)

        // final_suspend: mark done, then resume whoever awaited us (if anyone).
        f->resume = NULL;
        if (f->continuation)
            coro_resume(f->continuation);
        return;
    }
    }
}

static void compute_destroy(void *p) { free(p); }

// The C equivalent of *calling* the coroutine: allocate a fresh frame, suspended
// at the very start (state 0), and hand it back. calloc zeroes result/
// continuation/x/c1/c2 for us. Called by the assembly entry point below.
void *compute_make(void)
{
    struct frame *f = calloc(1, sizeof *f);
    f->resume = compute_resume;
    f->destroy = compute_destroy;
    return f;
}

// --- ABI shims ---------------------------------------------------------------
//
// Two things need the platform ABI, and both concern how Task<int> is returned
// by value. Because Task has a user-declared move ctor + dtor it is "non-trivial
// for the purposes of calls", so the Itanium C++ ABI returns it INDIRECTLY: the
// caller allocates the return slot and passes its address to the callee.
//
//   compute() [_Z7computev] : WE are the callee. Fill the caller's slot with a
//                             fresh compute frame.
//   call_add()              : WE are the caller of add() [_Z3addii]. Provide a
//                             slot, call add, and hand back the frame it wrote.
//
// HOW the slot address is passed is the per-target part:
//   * arm64/AAPCS64 puts it in the dedicated register x8, which plain C cannot
//     name — hence the assembly trampolines below.
//   * wasm32 has no registers; the slot pointer is just an ordinary first
//     parameter, so both shims are plain C (see the #elif).

#if defined(__aarch64__) && defined(__APPLE__)
__asm__(
    "    .globl __Z7computev\n"
    "    .p2align 2\n"
    "__Z7computev:\n"
    "    stp x29, x30, [sp, #-32]!\n"
    "    mov x29, sp\n"
    "    str x8, [sp, #16]\n"       // save sret slot pointer
    "    bl  _compute_make\n"       // x0 = frame
    "    ldr x8, [sp, #16]\n"
    "    str x0, [x8]\n"            // *sret = frame  => Task{ handle }
    "    mov x0, x8\n"
    "    ldp x29, x30, [sp], #32\n"
    "    ret\n"
    "\n"
    "    .globl _call_add\n"
    "    .p2align 2\n"
    "_call_add:\n"                  // void *call_add(int a, int b)  (a=w0, b=w1)
    "    sub sp, sp, #32\n"
    "    stp x29, x30, [sp, #16]\n"
    "    add x29, sp, #16\n"
    "    add x8, sp, #8\n"          // x8 = &slot for the returned Task<int>
    "    bl  __Z3addii\n"           // a,b already in w0,w1; writes frame ptr to [x8]
    "    ldr x0, [sp, #8]\n"        // return that frame pointer
    "    ldp x29, x30, [sp, #16]\n"
    "    add sp, sp, #32\n"
    "    ret\n");

#elif defined(__wasm32__)
// On wasm32 the sret pointer is a plain first parameter, so no assembly is
// needed. Task<int> is layout-compatible with a single pointer (the handle).
struct compute_task { void *handle; };

// compute(): Task<int> compute(void)  ==ABI==>  void _Z7computev(Task *ret)
void compute_entry(struct compute_task *ret) __asm__("_Z7computev");
void compute_entry(struct compute_task *ret)
{
    ret->handle = compute_make();   // *sret = frame  => Task{ handle }
}

// add(): Task<int> add(int,int)  ==ABI==>  void _Z3addii(Task *ret, int, int).
// We import it with that wasm signature and call it with a local return slot.
void add_entry(struct compute_task *ret, int a, int b) __asm__("_Z3addii");

void *call_add(int a, int b)
{
    struct compute_task slot;
    add_entry(&slot, a, b);         // add writes the new frame into slot.handle
    return slot.handle;
}

#else
#error "compute.c ABI shims are only implemented for arm64 macOS and wasm32"
#endif
