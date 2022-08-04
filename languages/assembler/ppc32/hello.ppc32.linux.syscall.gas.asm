/*
Output "Hello world!" to standard output.
For PowerPC 32-bit architecture with GNU assembler, using Linux syscalls.

Assemble with :
as -a32 -o hello.o hello.ppc32.linux.syscall.gas.asm
ld -melf32ppc -o hello hello.o

*/

.section .data
msg:
	.ascii "Hello world!\n"
	len = . - msg

.section .text
.global _start
_start:

	li 0, 4
	li 3, 1
	lis 4, msg@ha
	addi 4, 4, msg@l
	li 5,len
	sc

	li 0, 1
	li 3, 1
	sc

