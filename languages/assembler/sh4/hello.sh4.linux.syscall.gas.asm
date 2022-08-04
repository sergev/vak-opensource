/*
Output "Hello world!" to standard output.
For SH4 architecture with GNU assembler, using Linux syscalls.
Assemble with :
as -o hello.o hello.sh4.linux.syscall.gas.asm &&
ld -o hello hello.o
*/

.section .text
.balign 4
.globl _start

_start:
	mov #4, r3
	mov #1, r4
	mov.l message_ptr, r5
	mov #14, r6
	trapa #17

	mov #1, r3
	xor r4, r4
	trapa #17

.balign 4
message_ptr:
	.long message

.section .rodata
.balign 4
message:
	.ascii "Hello world!\n"
