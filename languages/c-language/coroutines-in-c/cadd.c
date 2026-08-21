// cadd.c — the add() coroutine, re-implemented in plain C11.
//
// This is the punchline of the whole demo: a C++20 coroutine frame is nothing
// magical. It is just a heap-allocated struct whose first two fields are
// function pointers — "resume me" and "destroy me" — followed by the promise
// object. Suspending/resuming a coroutine is literally "call the function
// pointer at the front of the frame."
//
// So we can build that struct by hand in C, fill in our own resume/destroy
// functions, and export the exact symbol the C++ caller expects. The C++
// compute() coroutine then co_awaits us without any idea that add() is C.
//
// The layout below is NOT portable standard C — the offsets are dictated by
// Clang's Itanium coroutine ABI, reverse-engineered from the compiler's own
// assembly output for the original C++ add(). The *idea* is portable; the exact
// numbers are an ABI detail that depends on the pointer width:
//
//                       LP64 (arm64)      ILP32 (wasm32)
//   frame + 0*P   resume    (fn ptr)   0                 0
//   frame + 1*P   destroy   (fn ptr)   8                 4
//   frame + 2*P   promise.result (int) 16                8
//   frame + 3*P   promise.continuation 24                12   (P = sizeof(void*))
//   after         our saved state (a, b)
//
// So instead of hard-coding 16/24 we let the struct lay itself out naturally
// (no manual padding) and assert the offsets in units of sizeof(void*) — which
// is correct on both 64-bit and 32-bit targets.

#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>

struct cadd_frame {
    void (*resume)(void *);   // + 0*sizeof(void*)
    void (*destroy)(void *);  // + 1*sizeof(void*)
    int   result;             // + 2*sizeof(void*)  promise.result
    void *continuation;       // + 3*sizeof(void*)  promise.continuation
    int   a, b;               // saved parameters (natural alignment)
};

// Guard the parts of the layout the C++ side reads directly. If a future
// compiler/ABI changes them, this fails to compile instead of misbehaving.
// (continuation lands at 3*P because the int `result` is padded up to pointer
// alignment before it — true for both LP64 and ILP32.)
_Static_assert(offsetof(struct cadd_frame, result) == 2 * sizeof(void *),
               "promise.result must sit right after the two frame fn pointers");
_Static_assert(offsetof(struct cadd_frame, continuation) == 3 * sizeof(void *),
               "promise.continuation must follow promise.result (ptr-aligned)");

// The coroutine body. Called the first (and only) time the frame is resumed —
// which happens when compute() does `co_await add(...)`.
static void cadd_resume(void *p)
{
    struct cadd_frame *f = p;

    printf("    add(%d, %d) is running\n", f->a, f->b);  // matches the C++ text
    f->result = f->a + f->b;   // this is the `co_return a + b;`

    // Reaching the end of a coroutine runs its final_suspend. In the C++ Task,
    // that marks the coroutine done and hands control to whoever awaited us
    // (the "continuation"). We do exactly that:
    f->resume = NULL;                 // mark done (handle.done() <=> resume==NULL)
    void *cont = f->continuation;
    if (cont) {
        // A continuation is just another coroutine frame: resume it by calling
        // the function pointer at its front. (Real symmetric transfer would be
        // a tail call; a plain call is fine here — the chain is only 2 deep.)
        void (*cont_resume)(void *) = *(void (**)(void *))cont;
        cont_resume(cont);
    }
}

static void cadd_destroy(void *p)
{
    // handle.destroy() ends up here. add() has no non-trivial locals and always
    // runs to completion before being destroyed, so freeing the frame is all
    // that is needed. (We malloc'd it, so we free it — a matched pair.)
    free(p);
}

// Builds a fresh, suspended coroutine frame and returns a pointer to it. The
// returned pointer IS what a std::coroutine_handle wraps. Called by the tiny
// assembly entry point below.
void *cadd_make(int a, int b)
{
    struct cadd_frame *f = malloc(sizeof *f);
    f->resume = cadd_resume;
    f->destroy = cadd_destroy;
    f->result = 0;
    f->continuation = NULL;   // set later by the awaiter, if we are awaited
    f->a = a;
    f->b = b;
    return f;                 // lazy: we hand back a frame that hasn't run yet
}

// --- ABI entry point ---------------------------------------------------------
//
// The C++ caller was compiled expecting the symbol `add(int, int)` returning a
// Task<int> BY VALUE. Because Task has a user-declared move constructor and
// destructor, it is "non-trivial for the purposes of calls" and the AAPCS64 ABI
// returns it INDIRECTLY: the caller allocates the return slot and passes its
// address in register x8 (the "indirect result location register"), with the
// int arguments in w0 and w1.
//
// Plain C has no way to name x8, so we provide the entry as a minimal assembly
// trampoline that: saves x8, calls cadd_make(a, b), stores the returned frame
// pointer into *x8 (constructing the Task in place), and returns.
//
// The exported symbol must be the C++ mangled name of add(int,int): `_Z3addii`
// (written `__Z3addii` in Mach-O assembly, which adds a leading underscore).

#if defined(__aarch64__) && defined(__APPLE__)
__asm__(
    "    .globl __Z3addii\n"
    "    .p2align 2\n"
    "__Z3addii:\n"
    "    stp x29, x30, [sp, #-32]!\n"  // save frame ptr / return address
    "    mov x29, sp\n"
    "    str x8, [sp, #16]\n"          // save sret ptr (x8 is call-clobbered)
    "    bl  _cadd_make\n"             // x0 = frame; a,b already in w0,w1
    "    ldr x8, [sp, #16]\n"          // restore sret ptr
    "    str x0, [x8]\n"               // *sret = frame  => Task{ handle }
    "    mov x0, x8\n"                 // also return the sret ptr in x0
    "    ldp x29, x30, [sp], #32\n"
    "    ret\n");

#elif defined(__wasm32__)
// On WebAssembly there are no CPU registers, so there is no "x8" to reach for —
// and that is exactly what makes this the easy case. The Itanium C++ ABI still
// classifies Task<int> as "non-trivial for the purposes of calls", so add() is
// still returned INDIRECTLY. But an indirect result on wasm is passed the only
// way anything is passed on a stack machine: as an ordinary function argument.
// The hidden return-slot pointer simply becomes the *first* parameter, shifting
// the ints to the second and third:
//
//     Task<int> add(int a, int b)   ==ABI==>   void _Z3addii(Task *ret,
//                                                             int a, int b);
//
// A pointer parameter is something plain C can name, so no assembly is needed:
// we write the entry as a normal C function, store the frame into *ret (this is
// the in-place construction of the returned Task), and return void — the caller
// already owns the slot it passed us. The only trick left is spelling: an asm()
// label gives the function the C++ mangled name `_Z3addii` (wasm has no Mach-O
// leading-underscore, so it is written exactly as the mangler emits it).

// Task<int> is layout-compatible with a single pointer: the coroutine handle.
struct cadd_task { void *handle; };

void cadd_entry(struct cadd_task *ret, int a, int b) __asm__("_Z3addii");

void cadd_entry(struct cadd_task *ret, int a, int b)
{
    ret->handle = cadd_make(a, b);   // *sret = frame  => Task{ handle }
}

#else
#error "cadd.c ABI entry shim is only implemented for arm64 macOS and wasm32"
#endif
