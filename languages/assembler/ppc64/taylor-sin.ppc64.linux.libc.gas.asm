# Calculate the Taylor series approximation of sin(x), using values specified
# on the command line.
# 
# For PowerPC 64-bit architecture with GNU assembler, using libc functions.
# 
# Assemble with :
# as -mpower5 -o taylor-sin.o taylor-sin.ppc64.linux.libc.gas.asm &&
# gcc -o taylor-sin taylor-sin.o
# 
# Run with :
# ./taylor-sin 0.1 5
# 
# Where '0.1' is the value to calculate the sine of, and '5' is the number of
# terms in the approximation.

.section .toc, "aw"

.section .rodata
.balign 8
format: .asciz "%0.10f\n"
error_message: .asciz "syntax: <x> <terms>"
float_0: .double 0.0
float_1: .double 1.0

.section .opd, "aw"
.balign 8
.globl main
main: .quad .main, .TOC.@tocbase, 0

.section .text
.balign 4
.main:
	mflr  0                   # get link register value
	std   0, 16(1)            # save link register to stack
	stdu  1, -128(1)          # create stack frame
	std   4, 56(1)            # save argv

	cmpwi 3, 3                # 3 arguments (including program name) ?
	bne   0, error            # no, print error and exit

	ld    3, 16(4)            # get argv[1]
	bl    atoi                # convert string to int
	nop                       # toc fixup space
	std   3, 64(1)            # save

	ld    4, 56(1)            # get saved argv
	ld    3, 8(4)             # get argv[2]
	bl    atof                # convert string to double
	nop
	ld    3, 64(1)            # restore

	bl    taylor_sin

	stfd  1, 64(1)            # copy float reg
	ld    4, 64(1)            # into fixed reg
	addis 3, 2, format@toc@ha # arg 0 : address of format (high)
	addi  3, 3, format@toc@l  # arg 0 : get address of format (low)
	bl    printf              # call libc printf
	nop                       # toc fixup space

	li    3, 0                # return value
	b     done

error:
	addis 3, 2, error_message@toc@ha # arg 0 : address of format (high)
	addi  3, 3, error_message@toc@l  # arg 0 : get address of format (low)
	bl    puts                # call libc puts
	nop                       # toc fixup space
	li    3, 1                # return value
	b     done
	
done:
	addi  1, 1, 128           # destroy stack frame
	ld    0, 16(1)            # load saved link register from stack
	mtlr  0                   # restore link register value
	blr                       # return

# input:
# r3 = terms
# f1 = sum

# output:
# f1 = sum

# work:
# f1 = sum
# f2 = x
# f3 = 1.0
# f4 = factorial product
# f5 = factorial index
# f6 = x power

taylor_sin:
	mflr  0                   # get link register value
	std   0, 16(1)            # save link register to stack
	stdu  1, -128(1)          # create stack frame

	fmr   2, 1                # f2 = x

	addis 4, 2, float_0@toc@ha 
	addi  4, 4, float_0@toc@l
	lfd   1, 0(4)             # f1 = sum

	addis 4, 2, float_1@toc@ha 
	addi  4, 4, float_1@toc@l
	lfd   3, 0(4)             # const 1.0

	fmr   4, 3
	fmr   5, 3
	fmr   6, 2
	xor   4, 4, 4             # term counter

	# first term
	fadd  1, 1, 2
	subi  3, 3, 1             # done first term
	mtctr 3                   # setup loop counter

taylor_sin_loop:
	fmul  6, 6, 2
	fmul  6, 6, 2
	fadd  5, 5, 3
	fmul  4, 4, 5
	fadd  5, 5, 3
	fmul  4, 4, 5
	fdiv  7, 6, 4
	rldicl. 5, 4, 0, 63       # term counter last bit odd / even ?
	bne   1f
	fsub  1, 1, 7             # subtract term from sum
	b     2f
1:
	fadd  1, 1, 7             # add term to sum
2:
	addi  4, 4, 1
	bdnz  taylor_sin_loop     # loop until counter is 0

	addi  1, 1, 128           # destroy stack frame
	ld    0, 16(1)            # load saved link register from stack
	mtlr  0                   # restore link register value
	blr                       # return
