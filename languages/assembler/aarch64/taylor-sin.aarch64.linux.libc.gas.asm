/*
Calculate the Taylor series approximation of sin(x), using values specified
on the command line.

For Aarch64 (ARM64) architecture with GNU assembler, using libc functions.

Assemble with :
as -o taylor-sin.o taylor-sin.aarch64.linux.libc.gas.asm &&
gcc -o taylor-sin taylor-sin.o

Run with :
./taylor-sin 0.1 5

Where '0.1' is the value to calculate the sine of, and '5' is the number of
terms in the approximation.
*/

.global main
.section .text
.balign 4

main:
/* setup stack frame, 16 bytes for x29/x30 + 2 locals of 8 bytes = 32 total */
	stp   x29, x30, [sp, -32]!
	mov   x29, sp

/* if we don't have 2 command-line args, then output an error message */
	cmp   x0, 3
	bne   error

/* save argv (x1 to [x29, 16] because x1 will be clobbered by libc calls */
	str   x1, [x29, 16]

/* convert argv[1] (value to calc sin of) to float */
	ldr   x1, [x29, 16]
	ldr   x0, [x1, 8]
	bl    atof
	str   d0, [x29, 24]      /* save for later */

/* convert argv[2] (iteration count) to integer */
	ldr   x1, [x29, 16]
	ldr   x0, [x1, 16]
	bl    atoi

/* calculate taylor sine approximation */
	ldr   d0, [x29, 24]      /* get saved float(argv[1]) */
	mov   x1, x0
	bl    taylor_sin

/* output result */
	adrp  x0, format
	add   x0, x0, :lo12:format
	bl    printf	

/* set return value */
	mov   x0, 0
	b     done

error:
/* output error message */
	adrp  x0, error_message	
	add   x0, x0, :lo12:error_message
	bl    puts
	mov   x0, 1

/* return to libc */
done:
	ldp   x29, x30, [sp], 32
	ret

taylor_sin:
/*
inputs:
  d0 : value to calculate sine of (x)
  x0 : number of terms

outputs:
  d0 : sin(x)

used:
  d0  s  = sum of all terms (final result)
  d1  1  = constant 1
  d2  fi = factorial index (1, 3, 5, 7, ...)
  d3  fc = factorial current (1, 6, 120, 5040, ...)
  d4  n  = numerator of term (x, x^3, x^5, x^7, ...)
  d5  t  = temporary term
  d6  x  = input value
*/  
	stp   x29, x30, [sp, -16]! /* setup stack frame */
	mov   x29, sp

	fadd  d6, d0, d31        /* copy x */
	fadd  d0, d31, d31       /* start with zero sum */

	adrp  x1, d_const_1	
	add   x1, x1, :lo12:d_const_1
	ldr   d1, [x1]
	fadd  d2, d1, d31        /* start with factorial index 1 */
	fadd  d3, d1, d31        /* start with factorial 1 */
	fadd  d4, d6, d31        /* start with term x */

	/* first term */
	fdiv  d5, d4, d3         /* x^n / n! -> f5 */
	fadd  d0, d5, d0         /* accumulate result into f0 */

	mov   x1, 0              /* zero term index */

taylor_loop:                 /* loop through each term */
	fadd  d2, d2, d1         /* calculate next odd factorial */
	fmul  d3, d2, d3
	fadd  d2, d2, d1
	fmul  d3, d2, d3

	fmul  d4, d4, d6         /* calculate next odd power of x */
	fmul  d4, d4, d6 

	fdiv  d5, d4, d3         /* divide power by factorial */

	tbz   x1, 0, taylor_odd
	fadd  d0, d0, d5         /* add even terms */
	b     taylor_next

taylor_odd:
	fsub  d0, d0, d5         /* subtract odd terms */

taylor_next:
	add   x1, x1, 1          /* increment term index */
	sub   x0, x0, 1          /* decrement loop counter */
	cbnz  x0, taylor_loop    /* loop if more terms */

	ldp   x29, x30, [sp], 16 /* restore stack frame */
	ret		

.section .rodata
.balign 8
error_message: .asciz "syntax: <x> <terms>"
format:        .asciz "%0.10f\n"
d_const_1:     .double 1.0
