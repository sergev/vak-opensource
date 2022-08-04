/*
Output "Hello world!" to standard output.
For x86-64 architecture with GNU assembler, using Linux syscalls.

Assemble with :
as --64 -o hello.o hello.x86-64.linux.syscall.gas.asm &&
ld -melf_x86_64 -o hello hello.o
*/

.globl _start
.section .text

_start:
	mov $1, %rax      /* syscall 1 - write */
	mov $1, %rdi      /* fd - stdout */
	lea message, %rsi /* pointer to message */
	mov $13, %rdx     /* size */
	syscall           /* do syscall */

	mov $60, %rax     /* syscall 60 - exit */
	xor %rdi, %rdi    /* exit code - 0 */
	syscall           /* do syscall */

.section .rodata
message:
	.ascii "Hello world!\x0a"
