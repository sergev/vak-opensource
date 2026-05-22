# Non-Local Jumps in C: Understanding `setjmp` and `longjmp`

> **Audience:** C programmers who are comfortable with functions, pointers, and the call stack, but
> have not yet encountered non-local control flow.  
> **Goal:** Build a solid mental model of how `setjmp`/`longjmp` work, when to use them, and —
> just as importantly — when to avoid them.

---

## Table of Contents

1. [The Problem: Jumping Out of Deep Call Chains](#1-the-problem)
2. [A Quick Review: The Call Stack](#2-the-call-stack)
3. [What Is a Non-Local Jump?](#3-what-is-a-non-local-jump)
4. [The Two Key Functions](#4-the-two-key-functions)
5. [A Minimal Working Example](#5-a-minimal-working-example)
6. [How It Works Internally](#6-how-it-works-internally)
7. [The `jmp_buf` Type](#7-the-jmp_buf-type)
8. [Return Values and Error Codes](#8-return-values-and-error-codes)
9. [Practical Pattern: Error Handling in a Compiler](#9-practical-pattern-error-handling-in-a-compiler)
10. [Volatile Variables and the Optimiser](#10-volatile-variables-and-the-optimiser)
11. [Nested `setjmp` Contexts](#11-nested-setjmp-contexts)
12. [Rules and Restrictions](#12-rules-and-restrictions)
13. [Common Mistakes](#13-common-mistakes)
14. [Best Practices](#14-best-practices)
15. [Comparison with Alternatives](#15-comparison-with-alternatives)
16. [Summary Cheat-Sheet](#16-summary-cheat-sheet)

---

## 1. The Problem

Imagine you are writing a C compiler.  Your pipeline looks roughly like this:

```
main()
  └─ parse_translation_unit()
       └─ parse_function_definition()
            └─ parse_statement()
                 └─ parse_expression()
                      └─ parse_primary()
                           └─ ... (many more levels)
```

Deep inside `parse_primary`, you encounter an unexpected token — a syntax error.  You need to
**abort the current compilation** and report the error.  How do you get control back to `main`?

The naive approach is to return an error code from every function and check it at every call site:

```c
int parse_primary(Parser *p) {
    if (/* error */) return ERROR;
    ...
}

int parse_expression(Parser *p) {
    int r = parse_primary(p);
    if (r == ERROR) return ERROR;  // propagate upward
    ...
}

// ... and so on, all the way to main()
```

This works but it is **tedious, error-prone, and clutters every function** with boilerplate that has
nothing to do with parsing.  In a deep call chain of 15 functions, forgetting one `if (r == ERROR)
return ERROR;` causes a silent bug.

`setjmp`/`longjmp` solves this elegantly by letting you jump directly back to a previously marked
point in the call chain, skipping all the intermediate frames in one shot.

---

## 2. A Quick Review: The Call Stack

When a C function is called, the runtime pushes a **stack frame** onto the call stack.  This frame
holds the function's local variables, its saved registers, and the **return address** — the
instruction to jump back to when the function returns.

```
High addresses
┌──────────────────┐
│  main frame      │  ← bottom of chain
├──────────────────┤
│  parse_unit      │
├──────────────────┤
│  parse_expr      │
├──────────────────┤
│  parse_primary   │  ← currently executing
└──────────────────┘
Low addresses (stack grows downward)
```

A normal `return` pops the top frame, restores the saved registers, and jumps to the return
address.  `longjmp` does something far more dramatic: it throws away *multiple* frames at once and
restores an older snapshot of the machine state.

---

## 3. What Is a Non-Local Jump?

A **local jump** is a `goto` — it moves the instruction pointer within the same function.  
A **non-local jump** moves the instruction pointer *and* the stack pointer to a point in a
**different function** (one that is still active on the call stack).  This is what `longjmp` does.

The word "non-local" emphasises that the destination of the jump is outside the current function's
scope.

---

## 4. The Two Key Functions

Both are declared in `<setjmp.h>`.

### `setjmp` — Mark a Recovery Point

```c
#include <setjmp.h>

int setjmp(jmp_buf env);
```

`setjmp` **saves a snapshot** of the current execution state into `env` and returns **0**.  Think
of it as placing a bookmark: "if anything goes wrong further down the call chain, come back here."

### `longjmp` — Jump Back to the Bookmark

```c
void longjmp(jmp_buf env, int val);
```

`longjmp` **restores the snapshot** stored in `env` and makes `setjmp` appear to return a *second
time*, this time returning `val`.  If `val` is 0, `setjmp` will return 1 instead (because 0 is
reserved to mean "this is the first call").

`longjmp` **never returns** to its own call site.  Control goes directly to the saved point.

---

## 5. A Minimal Working Example

```c
#include <stdio.h>
#include <setjmp.h>

jmp_buf recovery_point;   /* the "bookmark" */

void deep_function(int level) {
    if (level == 0) {
        printf("Triggering error!\n");
        longjmp(recovery_point, 42);  /* jump back, return value 42 */
        printf("This line is NEVER reached.\n");
    }
    printf("Level %d — going deeper...\n", level);
    deep_function(level - 1);
    printf("Level %d — returning normally.\n", level);  /* also never reached */
}

int main(void) {
    int result = setjmp(recovery_point);   /* first call: result == 0 */

    if (result == 0) {
        printf("Starting deep call chain.\n");
        deep_function(3);
        printf("This is also never reached.\n");
    } else {
        printf("Recovered! longjmp returned value: %d\n", result);
    }

    return 0;
}
```

**Expected output:**

```
Starting deep call chain.
Level 3 — going deeper...
Level 2 — going deeper...
Level 1 — going deeper...
Triggering error!
Recovered! longjmp returned value: 42
```

Notice that "Level 1 — returning normally", "Level 2 — returning normally", etc. are **never
printed**.  The call frames for `deep_function` were simply discarded by `longjmp`.

---

## 6. How It Works Internally

The C standard is intentionally vague about the implementation, but in practice `setjmp` saves:

| What is saved | Why |
|---|---|
| **Stack pointer** (`SP`) | So the stack can be unwound to the right depth |
| **Frame pointer** (`FP` / `BP`) | So the callee's frame can be reconstructed |
| **Instruction pointer** (`IP` / `PC`) | The address inside `setjmp`'s caller to return to |
| **Callee-saved registers** | e.g., `rbx`, `r12`–`r15` on x86-64 |
| **Signal mask** (in `sigsetjmp`) | Optionally saves the signal blocking mask |

When `longjmp` is called, it:

1. Loads the saved stack pointer — effectively throwing away every frame above the `setjmp` frame.
2. Loads the saved instruction pointer — jumping back into the `setjmp` caller.
3. Restores the saved registers.
4. Places `val` (or 1 if `val == 0`) in the return-value register, making it look like `setjmp`
   just returned that value.

The CPU does not know anything special happened; it simply sees its registers change.

---

## 7. The `jmp_buf` Type

`jmp_buf` is an array type (or struct) defined in `<setjmp.h>` large enough to hold the machine
state described above.  On x86-64 Linux with glibc it is roughly 200 bytes; the exact size is
implementation-defined.

**Important rules about `jmp_buf`:**

- Declare it at a scope that is **alive for the entire lifetime** of the setjmp/longjmp pair.
  A global variable or a local variable in `main` (or your top-level error handler) is typical.
- Do **not** copy a `jmp_buf` with `memcpy` and then `longjmp` into the copy — the result is
  undefined behaviour on some platforms.
- Do **not** pass a `jmp_buf` by value (it is an array; passing it decays to a pointer anyway, but
  be explicit: always pass `jmp_buf *` when you need to share it across functions).

```c
/* Correct: pass by pointer */
void parser_init(Parser *p, jmp_buf *error_jump) {
    p->error_jump = error_jump;
}
```

---

## 8. Return Values and Error Codes

The return value of `setjmp` is your communication channel from the `longjmp` site back to the
recovery point.  Common conventions:

```c
/* Convention 1: single error flag */
if (setjmp(env) != 0) {
    /* an error occurred */
}

/* Convention 2: error code */
int code = setjmp(env);
switch (code) {
    case 0:   /* no error — normal entry */   break;
    case ERR_SYNTAX:    handle_syntax();      break;
    case ERR_TYPE:      handle_type();        break;
    case ERR_INTERNAL:  handle_internal();    break;
}

/* Convention 3: phase identifier */
/* Use the value to identify which subsystem threw the error */
```

Keep error codes positive and non-zero (reserve 0 for "no error").

---

## 9. Practical Pattern: Error Handling in a Compiler

Here is a realistic, idiomatic pattern for a recursive-descent compiler or interpreter.

### 9.1 Define Your Error Context

```c
/* error.h */
#ifndef ERROR_H
#define ERROR_H

#include <setjmp.h>
#include <stdnoreturn.h>

#define ERR_SYNTAX   1
#define ERR_TYPE     2
#define ERR_MEMORY   3
#define ERR_INTERNAL 4

typedef struct {
    jmp_buf  jump;        /* recovery point */
    int      code;        /* error code */
    char     message[256];/* human-readable message */
    int      line;        /* source line where error occurred */
} ErrorContext;

/* Call this to throw an error from anywhere in the parser */
noreturn void error_throw(ErrorContext *ctx, int code, int line,
                          const char *fmt, ...);

#endif
```

### 9.2 Implement the Throw Function

```c
/* error.c */
#include "error.h"
#include <stdio.h>
#include <stdarg.h>

noreturn void error_throw(ErrorContext *ctx, int code, int line,
                          const char *fmt, ...)
{
    ctx->code = code;
    ctx->line = line;

    va_list ap;
    va_start(ap, fmt);
    vsnprintf(ctx->message, sizeof(ctx->message), fmt, ap);
    va_end(ap);

    longjmp(ctx->jump, code);   /* never returns */
}
```

The `noreturn` attribute (C11) tells the compiler this function never returns, which enables better
warnings and optimisation.

### 9.3 Set Up the Recovery Point in main

```c
/* main.c */
#include "error.h"
#include "parser.h"
#include <stdio.h>

int main(int argc, char **argv) {
    ErrorContext err = {0};

    int code = setjmp(err.jump);
    if (code != 0) {
        /* We arrive here after any longjmp */
        fprintf(stderr, "Error (line %d): %s\n", err.line, err.message);
        return 1;
    }

    /* Normal compilation path */
    Parser *p = parser_create(argv[1], &err);
    compile(p);
    parser_destroy(p);

    printf("Compilation successful.\n");
    return 0;
}
```

### 9.4 Throw an Error Deep in the Parser

```c
/* parser.c */
static Token *expect(Parser *p, TokenKind kind) {
    Token *t = next_token(p);
    if (t->kind != kind) {
        error_throw(p->err, ERR_SYNTAX, t->line,
                    "expected %s, got '%s'",
                    token_kind_name(kind), t->text);
        /* longjmp fires — this function never returns */
    }
    return t;
}
```

The call chain can be 20 functions deep; none of them need to pass error codes around.  The moment
`error_throw` is called, control snaps back to `main`.

---

## 10. Volatile Variables and the Optimiser

This is the most common source of subtle bugs with `setjmp`/`longjmp`.

The C standard says: **local variables whose values are modified between the `setjmp` call and the
`longjmp` call, and that are not declared `volatile`, have indeterminate values after `longjmp`
returns.**

In other words, the compiler may have cached such a variable in a register.  When `longjmp`
restores the saved register state, the register-cached value is overwritten with whatever was there
at the time of `setjmp`, silently discarding any changes made after that point.

### Bad: variable modified after setjmp, not volatile

```c
int main(void) {
    int count = 0;              /* NOT volatile */
    if (setjmp(env) == 0) {
        count = 10;             /* may be in a register */
        do_something();         /* may call longjmp */
    }
    /* count may be 0 or 10 here — UNDEFINED */
    printf("%d\n", count);
}
```

### Good: mark the variable volatile

```c
int main(void) {
    volatile int count = 0;     /* always read from memory */
    if (setjmp(env) == 0) {
        count = 10;             /* written to memory */
        do_something();
    }
    /* count is reliably 10 here */
    printf("%d\n", count);
}
```

**Rule of thumb:** any local variable in the function that calls `setjmp` that is written to *after*
the `setjmp` call and read *after* a potential `longjmp` must be declared `volatile`.

Variables in *other* functions are not affected — once those stack frames are discarded, their
locals cease to exist entirely.

---

## 11. Nested `setjmp` Contexts

You can have multiple active `setjmp` points at the same time, as long as only the *innermost* one
is still valid.

```c
jmp_buf outer, inner;

void try_risky(void) {
    if (setjmp(inner) == 0) {
        do_risky_thing();           /* may longjmp to inner */
    } else {
        printf("Inner error caught, continuing.\n");
        /* longjmp to outer is still possible */
    }
}

int main(void) {
    if (setjmp(outer) != 0) {
        printf("Outer error caught.\n");
        return 1;
    }
    try_risky();
    return 0;
}
```

**Important constraint:** you must **never** `longjmp` to a `jmp_buf` whose `setjmp` call's
containing function has already returned.  The stack frame no longer exists, and the jump would
corrupt the stack.

```c
/* WRONG — catastrophic undefined behaviour */
jmp_buf saved;

void register_jump(void) {
    setjmp(saved);
}   /* <-- function returns; saved is now invalid */

void later(void) {
    longjmp(saved, 1);   /* stack frame is gone — crash or worse */
}
```

---

## 12. Rules and Restrictions

The C standard imposes several restrictions.  Violating them causes **undefined behaviour**.

### R1 — The `setjmp` expression must be simple

`setjmp` is technically a macro, not a function.  It may only appear in these contexts:

```c
setjmp(env);                      /* standalone expression statement */
if (setjmp(env)) { ... }          /* controlling expression of if/while/for */
int v = setjmp(env);              /* assignment to a simple variable */
if (setjmp(env) == 0) { ... }     /* comparison with an integer constant */
```

It may **not** appear in a more complex expression:

```c
int x = setjmp(env) + 1;          /* UNDEFINED — do not do this */
result = foo() || setjmp(env);    /* UNDEFINED */
```

### R2 — The `setjmp` frame must still be on the stack

As shown above: never `longjmp` into a function that has already returned.

### R3 — Do not invoke `longjmp` from a nested signal handler (use `siglongjmp` instead)

If you are inside a signal handler and wish to do a non-local jump, use `sigsetjmp` /
`siglongjmp`, which also save and restore the signal mask.  Using plain `longjmp` from a signal
handler is undefined behaviour.

### R4 — Do not modify the `jmp_buf` between `setjmp` and `longjmp`

Treat `jmp_buf` as write-once.  Overwriting it (e.g., by calling `setjmp` again on the same
buffer) invalidates any `longjmp` targeting the old snapshot.

---

## 13. Common Mistakes

### Mistake 1 — Forgetting to free resources

`longjmp` does **not** call destructors, `free`, `fclose`, or anything else.  If you allocate
memory or open a file after `setjmp`, a `longjmp` will leak those resources.

```c
/* Resource leak */
if (setjmp(env) == 0) {
    char *buf = malloc(1024);      /* allocated */
    parse(buf);                    /* may longjmp — buf is leaked! */
    free(buf);                     /* never reached on error */
}
```

**Solutions:**

- Preallocate before `setjmp` and free after the `if` block.
- Use an arena allocator that is reset in the error handler.
- Track resources in the `ErrorContext` and clean them up in the error handler.

### Mistake 2 — Jumping across initialisers

```c
if (setjmp(env) == 0) {
    int x = compute();            /* this initialiser may be skipped on re-entry */
    use(x);
}
```

On the second "return" of `setjmp` (after `longjmp`), execution resumes at the `setjmp` line, not
at `int x = compute()`.  The variable `x` is in scope but uninitialised.

### Mistake 3 — Using `longjmp` to exit a signal handler on non-POSIX systems

Stick to `siglongjmp` in signal handlers, or better yet, set a flag and handle errors in the main
loop.

### Mistake 4 — Assuming `longjmp` is like `throw` in C++

It is not.  There is no stack unwinding, no destructor calls, no RAII.  You are solely
responsible for all cleanup.

---

## 14. Best Practices

### BP1 — Centralise `jmp_buf` in a context struct

Never use bare globals if you can avoid it.  Embedding `jmp_buf` in a context struct makes the
code reentrant and testable.

```c
typedef struct { jmp_buf jump; /* ... other state ... */ } CompilerCtx;
```

### BP2 — Limit `setjmp` to the top level

Have exactly one `setjmp` per logical phase (parsing, type-checking, code generation).  Deep
nesting of `setjmp` points is confusing and hard to reason about.

### BP3 — Use `noreturn` on your throw helper

```c
#include <stdnoreturn.h>
noreturn void fatal(CompilerCtx *ctx, int code, const char *msg);
```

This gives the compiler the information it needs to warn about unreachable code after a `fatal`
call, just as it would for `exit` or `abort`.

### BP4 — Always declare setjmp-local variables `volatile` if modified after the call

When in doubt, mark them `volatile`.  The performance cost is negligible in error-handling paths.

### BP5 — Pair `setjmp` with cleanup logic immediately

Write the cleanup (free, close, reset) in the `else` branch right next to `setjmp`, so it is
never forgotten:

```c
Arena *arena = arena_create();          /* created BEFORE setjmp */

if (setjmp(ctx.jump) != 0) {
    arena_destroy(arena);               /* cleanup right here, impossible to forget */
    return 1;
}

/* ... do work using arena ... */
arena_destroy(arena);
return 0;
```

### BP6 — Prefer `sigsetjmp` in signal-aware code

```c
#include <setjmp.h>
sigjmp_buf env;

/* savemask = 1: save and restore signal mask */
if (sigsetjmp(env, 1) == 0) {
    ...
}
```

### BP7 — Document every `longjmp` site

A comment like `/* ERROR: jumps to main's setjmp on line 42 */` prevents future maintainers from
being surprised.

---

## 15. Comparison with Alternatives

| Technique | Pros | Cons |
|---|---|---|
| **Return codes** | Simple, portable, no surprises | Boilerplate at every call site; easy to forget |
| **`setjmp`/`longjmp`** | Clean separation of error-path from normal-path; works in C89 | No automatic cleanup; volatile rules; non-obvious control flow |
| **`errno` + `goto`** | Structured cleanup with `goto cleanup` | Only works within a single function |
| **C++ exceptions** | RAII, automatic destructors, structured | Requires C++; ABI overhead |
| **`abort`/`exit`** | Simple | Terminates the whole process; unusable in libraries |

For a hand-written recursive-descent compiler written in C, `setjmp`/`longjmp` combined with an
arena allocator is a well-established and highly practical pattern — used by production compilers
such as early versions of GCC, TCC (Tiny C Compiler), and many others.

---

## 16. Summary Cheat-Sheet

```
#include <setjmp.h>

/* 1. Declare a jmp_buf in a long-lived scope */
jmp_buf env;

/* 2. Set the recovery point */
int code = setjmp(env);
if (code == 0) {
    /* normal execution path */
} else {
    /* error recovery path — code is the value passed to longjmp */
}

/* 3. Throw an error from anywhere below */
longjmp(env, NON_ZERO_ERROR_CODE);   /* never returns */

/* Key rules at a glance */
// ✓  jmp_buf must outlive all longjmp calls targeting it
// ✓  Declare post-setjmp locals as volatile if modified before longjmp
// ✓  Use noreturn on your throw helper
// ✓  Free resources before or after the setjmp block, not between
// ✗  Never longjmp into a function that has returned
// ✗  Never use setjmp in a complex sub-expression
// ✗  Never longjmp from a signal handler (use siglongjmp instead)
```

---

## Further Reading

- ISO/IEC 9899:2018 (C17), §7.13 — *Nonlocal jumps*
- POSIX.1-2017, `sigsetjmp(3)` — signal-safe variant
- *The C Programming Language*, Kernighan & Ritchie, Appendix B
- Nils M Holm, *Scheme 9 from Empty Space* — a real interpreter that uses `setjmp` for error handling
- TCC (Tiny C Compiler) source — `tccgen.c` — a production example of this pattern

---

*Happy compiling!*
