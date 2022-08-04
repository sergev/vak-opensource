/*
Output "Hello world!" to standard output.
For PowerPC 64-bit architecture with GNU assembler, using Linux syscalls.

Assemble with :
as -o hello.o hello.ppc64.linux.syscall.gas.asm &&
ld -o hello hello.o
*/

.section .rodata
message:
	.ascii "Hello world!\n"

message_length = . - message

.section .text
.global _start
.section ".opd", "aw"
.balign 8
_start:
.quad ._start, .TOC.@tocbase, 0
.previous

.global ._start
._start:
	li     0, 4 # syscall 4 - write
	li     3, 1 # arg 0 - fd = 1 (stdout)
	lis    4, message@highest # arg 1 - buffer
	ori    4, 4, message@higher
	rldicr 4, 4, 32, 31
	oris   4, 4, message@h
	ori    4, 4, message@l
	li     5, message_length # arg 2 - size
	sc     # do syscall

	li     0, 1 # syscall 1 - exit
	li     3, 0 # arg 0 - exit code = 0
	sc     # do syscall

