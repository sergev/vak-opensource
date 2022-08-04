/*
Calculate the Taylor series approximation of sin(x), using values specified
on the command line.

For DEC Alpha EV4 architecture with GNU assembler, using libc functions.

Assemble with :
as -o taylor-sin.o taylor-sin.alpha.linux.libc.gas.asm &&
gcc -o taylor-sin taylor-sin.o

Run with :
./taylor-sin 0.1 5

Where '0.1' is the value to calculate the sine of, and '5' is the number of
terms in the approximation.
*/

.arch ev4
.set nomacro
.set noat
.set volatile
.set noreorder

.section .text
.balign 4

.extern printf
.extern atof
.extern atoi
.extern puts
.globl main

main:
/* make stack frame */
lda   $30, -64($30)

stq   $26, 0($30)  /* save return address */
stq   $16, 8($30)  /* save argc */
stq   $17, 16($30) /* save argv */

bsr   $27, 0       /* load our gp */
ldah  $29, 0($27)!gpdisp!1
lda   $29, 0($29)!gpdisp!1

stq   $29, 24($30) /* make a copy of our gp */

/* if we don't have 2 command-line args, then output an error message */
lda   $1, 0($16)
cmpeq $1, 3, $1
beq   $1, error

/* convert argv[1] (value to calc sin of) to float */
ldq   $1, 16($30)
ldq   $1, 8($1)
lda   $16, 0($1)
ldq   $27, atof($29)!literal!3        /* get gp offset of puts() */
jsr   $26, ($27), atof!lituse_jsr!3   /* call puts() */
ldq   $29, 24($30)                    /* restore gp after libc call */
stt   $f0, 32($30)                    /* save for later */

/* convert argv[2] (iteration count) to integer */
ldq   $1, 16($30)                     /* get argv */
ldq   $1, 16($1)                      /* get argv[2] */
lda   $16, 0($1)                      /* setup arg 0 for atoi */
ldq   $27, atoi($29)!literal!4        /* get gp offset of puts() */
jsr   $26, ($27), atoi!lituse_jsr!4   /* call puts() */
ldq   $29, 24($30)                    /* restore gp after libc call */
stq   $0, 40($30)                     /* save for later */

/* calculate taylor sine approximation */
ldt   $f16, 32($30)                   /* get saved float(argv[1]) */
ldq   $17, 40($30)                    /* get saved int(argv[2]) */
bsr   $26, taylor_sin

/* output result */
ldah  $16, format($29)!gprelhigh
lda   $16, format($16)!gprellow
addt  $f0, $f31, $f17                 /* copy taylor_sin result in v0 to a1 */
ldq   $27, printf($29)!literal!5      /* get gp offset of printf */
jsr   $26, ($27), printf!lituse_jsr!5 /* call printf */
ldq   $29, 24($30)                    /* restore gp after libc call */

clr   $0                              /* return 0 (success) */
br    done

error:
ldah  $16, error_message($29)!gprelhigh
lda   $16, error_message($16)!gprellow
ldq   $27, puts($29)!literal!4        /* get gp offset of puts() */
jsr   $26, ($27), puts!lituse_jsr!4   /* call puts() */
lda   $0, 1($31)

done:
ldq   $26, 0($30)                     /* restore return address */
lda   $30, 8($30)

ret

taylor_sin:
/*
inputs:
  f16 (fa0): value to calculate sine of (x)
  r17 (a1) : number of terms

outputs:
  r0 (v0) : sin(x)

used:
  f0  s  = sum of all terms (final result)
  f1  1  = constant 1
  f2  fi = factorial index (1, 3, 5, 7, ...)
  f3  fc = factorial current (1, 6, 120, 5040, ...)
  f4  n  = numerator of term (x, x^3, x^5, x^7, ...)
  f5  t  = temporary term
  f16 x  = input value
*/  

fclr  $f0                /* start with zero sum */

ldah  $1, f_const_1($29)!gprelhigh /* f1 = 1.0 */
lda   $1, f_const_1($1)!gprellow
ldt   $f1, 0($1)

addt  $f1, $f31, $f2     /* start with factorial index 1 */
addt  $f1, $f31, $f3     /* start with factorial 1 */
addt  $f16, $f31, $f4    /* start with term x */

/* first term */
divt  $f4, $f3, $f5      /* x^n / n! -> f5 */
addt  $f0, $f5, $f0      /* accumulate result into f0 */

bic   $17, $31, $1       /* number of terms */
bic   $31, $31, $2       /* zero add/sub counter */

taylor_loop:             /* loop through each term */

addt  $f2, $f1, $f2      /* calculate next odd factorial */
mult  $f2, $f3, $f3
addt  $f2, $f1, $f2
mult  $f2, $f3, $f3

mult  $f4, $f16, $f4     /* calculate next odd power of x */
mult  $f4, $f16, $f4

divt  $f4, $f3, $f5      /* divide power by factorial */

blbc  $2, taylor_odd
addt  $f0, $f5, $f0      /* add even terms */
br    taylor_next
taylor_odd:
subt  $f0, $f5, $f0      /* subtract odd terms */
taylor_next:

addl  $2, 1, $2          /* increment term index */
subl  $1, 1, $1          /* decrement loop counter */
bne   $1, taylor_loop    /* loop if more terms */

ret

.section .rodata
.balign 8
error_message: .asciz "syntax: <x> <terms>"
format:        .asciz "%0.10f\n"
f_const_1:     .double 1.0

