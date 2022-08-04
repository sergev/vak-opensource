/*
Calculate the Taylor series approximation of sin(x), using values specified
on the command line.
For SH4 architecture with GNU assembler, using libc functions.
Assemble with :
as -o taylor-sin.o taylor-sin.sh4.linux.libc.gas.asm &&
gcc -o taylor-sin taylor-sin.o
Run with :
./taylor-sin 0.1 5
Where '0.1' is the value to calculate the sine of, and '5' is the number of
terms in the approximation.
*/

.section .text
.balign 4
.globl main

main:
	sts.l pr, @-r15
	add   #-16, r15          /* create stack space for locals */

	mov   r4, r0             /* check if we have 2 command-line args */
	cmp/eq #3, r0
	bt    main$no_error      /* yes, carry on */
	bra   main$error         /* no, print error */
	nop                      /* branch delay slot */
main$no_error:

	mov.l r5, @(0,r15)       /* store copy of argv */
	add   #4, r5
	mov.l @r5, r4
	mov.l atof_ptr, r0
	jsr	  @r0
	nop                      /* branch delay slot */
	mov   r15, r0            /* save value on stack */
	add   #8, r0
	fmov.s fr1, @r0
	add   #4, r0
	fmov.s fr0, @r0

	mov.l @(0,r15), r5       /* get copy of argv */
	add   #8, r5
	mov.l @r5, r4
	mov.l atoi_ptr, r0
	jsr   @r0                /* call atoi */
	nop                      /* branch delay slot */

	mov   r0, r4             /* get atoi(argv[2]) */
	mov   r15, r0            /* get atof(argv[1]) */
	add   #8, r0
	fmov.s @r0, fr5
	add   #4, r0
	fmov.s @r0, fr4
	bsr   taylor_sin         /* call taylor_sin */
	nop                      /* branch delay slot */

	fmov  fr0, fr4           /* output result */
	fmov  fr1, fr5
	mov.l printf_ptr, r0
	mov.l format_ptr, r4
	jsr   @r0
	nop                      /* branch delay slot */

	mov   #0, r0             /* exit code 0 - success */
	bra   main$done
	nop                      /* branch delay slot */

main$error:
	mov.l puts_ptr, r0
	mov.l error_message_ptr, r4
	jsr   @r0
	nop                      /* branch delay slot */
	mov   #1, r0

main$done:
	add   #16, r15           /* restore stack */
	lds.l @r15+, pr	
	rts
	nop                      /* branch delay slot */

.balign 4
atof_ptr:          .long atof
atoi_ptr:          .long atoi
printf_ptr:        .long printf
format_ptr:        .long format
taylor_sin_ptr:    .long taylor_sin
puts_ptr:          .long puts
error_message_ptr: .long error_message

taylor_sin:
/*
inputs:
  dr4 : value to calculate sine of (x)
  r4  : number of terms
outputs:
  dr0 : sin(x)
used:
  dr0  s  = sum of all terms (final result)
  dr2  1  = constant 1
  dr4  x  = input value
  dr6  fi = factorial index (1, 3, 5, 7, ...)
  dr8  fc = factorial current (1, 6, 120, 5040, ...)
  dr10 n  = numerator of term (x, x^3, x^5, x^7, ...)
  dr12 t  = temporary term  
*/  
	mova f_const_0, r0
	fmov.s @r0+, fr1
	fmov.s @r0+, fr0

	mova f_const_1, r0
	fmov.s @r0+, fr3
	fmov.s @r0+, fr2

	fmov dr2, dr6
	fmov dr2, dr8
	fmov dr4, dr10

	/* first term */
	fmov dr10, dr12
	fdiv dr8, dr12
	fadd dr12, dr0

	mov #0, r0 /* zero add/sub counter */

taylor_sin_loop:
	fadd dr2, dr6            /* calculate next factorial */
	fmul dr6, dr8
	fadd dr2, dr6            /* calculate next factorial */
	fmul dr6, dr8

	fmul dr4, dr10           /* calculate next odd power of x */
	fmul dr4, dr10

	fmov dr10, dr12          /* divide power by factorial */
	fdiv dr8, dr12

	tst  #1, r0              /* check if odd term */
	bt   taylor_odd          /* no branch delay slot */
	fadd dr12, dr0           /* add even terms */
	bra  taylor_next
	nop
taylor_odd:
	fsub dr12, dr0           /* subtract odd terms */
taylor_next:	

	add #1, r0               /* increase term index */
	add #-1, r4              /* decrease loop counter */
	cmp/pl r4                /* more terms left? */
	bt taylor_sin_loop       /* no branch delay slot */

	rts
	nop                      /* branch delay slot */

.balign 4
f_const_0:
	.double 0.0	
f_const_1:
	.double 1.0
f_const_x:
	.double -1.8	

.section .rodata
.balign 4
error_message: .asciz "syntax: <x> <terms>"
format:        .asciz "%0.10f\n"
