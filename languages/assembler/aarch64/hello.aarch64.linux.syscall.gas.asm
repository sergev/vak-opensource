/*
Output "Hello world!" to standard output.
For Aarch64 (ARM64) architecture with GNU assembler, using Linux syscalls.

Assemble with :
as -o hello.o hello.aarch64.linux.syscall.gas.asm &&
ld -o hello hello.o
*/

.globl _start
.section .text

_start:

	mov x8, #64
	mov x0, #1
	adrp x1, message
	add x1, x1, :lo12:message
	mov x2, 13
	svc #0

	mov x8, #93
	mov x0, #42
	svc #0

	ret

.section .rodata
message:
	.asciz "Hello world!\n"
