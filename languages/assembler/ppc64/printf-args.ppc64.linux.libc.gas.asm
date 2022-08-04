# Demonstation of passing integer and float arguments to libc from an
# assembly program for PowerPC 64-bit architecture with GNU assembler.
# 
# Assemble with :
# as -mpower5 -o printf-args.o printf-args.ppc64.linux.libc.gas.asm &&
# gcc -o printf-args printf-args.o
# 
# Run with :
# ./printf-args 42 3.141
#
# output is : [int=42 float=3.141000]

.section .toc, "aw"

.section .rodata
.balign 8
format: .asciz "[int=%d float=%f]\n"
error_message: .asciz "require 2 parameters\n"

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

	ld    3, 8(4)             # get argv[1]
	bl    atoi                # convert string to int
	nop
	std   3, 64(1)            # save

	ld    4, 56(1)            # get saved argv
	ld    3, 16(4)            # get argv[2]
	bl    atof                # convert string to double
	nop
	ld    4, 64(1)            # restore
	stfd  1, 64(1)            # copy float reg
	ld    5, 64(1)            # into fixed reg

	addis 3, 2, format@toc@ha # arg 0 : address of format (high)
	addi  3, 3, format@toc@l  # arg 0 : get address of format (low)
	bl    printf              # call libs printf
	nop                       # branch delay

	li    3, 0                # return value
	b     done
	nop

error:
	addis 3, 2, error_message@toc@ha # arg 0 : address of format (high)
	addi  3, 3, error_message@toc@l  # arg 0 : get address of format (low)
	bl    printf              # call libs printf
	nop                       # branch delay
	li    3, 1                # return value
	b     done
	nop
	
done:
	addi  1, 1, 128           # destroy stack frame
	ld    0, 16(1)            # load saved link register from stack
	mtlr  0                   # restore link register value
	blr                       # return
	nop                       # branch delay
