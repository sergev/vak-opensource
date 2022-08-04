/*
Output "Hello world!" to standard output.
For Intel i386 architecture with GNU assembler, using Linux syscalls.

Assemble with :
as --32 -o hello.o hello.i386.linux.syscall.gas.asm &&
ld -melf_i386 -o hello hello.o
*/

.globl _start
.section .text

_start:
	mov $4, %eax      /* syscall 4 - write */
	mov $1, %ebx      /* fd - stdout */
	lea message, %ecx /* pointer to message */
	mov $13, %edx     /* size */
	int $0x80         /* do syscall */

	mov $1, %eax      /* syscall 1 - exit */
	xor %ebx, %ebx    /* exit code - 0 */
	int $0x80         /* do syscall */

.section .rodata
message:
	.ascii "Hello world!\x0a"
