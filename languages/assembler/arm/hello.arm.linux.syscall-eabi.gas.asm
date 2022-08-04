/*
Output "Hello world!" to standard output.
For ARM architecture with GNU assembler, using Linux EABI syscalls.

Assemble with :
as -o hello.o hello.arm.linux.syscall-eabi.gas.asm &&
ld -o hello hello.o
*/

.section .text
.balign 4
.globl _start

_start:
	mov r7, #4                 /* syscall 4 - write */
	mov r0, #1                 /* file descriptor 1 - stdout */
	movw r1, #:lower16:message /* low 16-bits of message address */
	movt r1, #:upper16:message /* high 16-bits of message address */
	mov r2, #13                /* character count */
	svc #0                     /* do system call */

	mov r7, #1                 /* syscall 1 - exit */
	mov r0, #0                 /* exit code 0 - success */
	svc #0                     /* do system call */

.section .rodata
.balign 4
message: .ascii "Hello world!\n"
