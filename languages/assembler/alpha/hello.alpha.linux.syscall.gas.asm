/*
Output "Hello world!" to standard output.
For DEC Alpha EV4 architecture with GNU assembler, using Linux syscalls.

Assemble with :
as -o hello.o hello.alpha.linux.syscall.gas.asm &&
ld -o hello hello.o
*/

.arch ev4
.set noreorder
.set nomacro
.set noat
.set volatile

/* read-only data section */
.section .rodata
$message:
	.ascii "Hello world!\0"

/* code section, align to 4 bytes (size of instruction) */
.section .text
.balign 4
.globl _start

_start:	
	bsr  $27,0                        /* setup pv from current pc */
	ldah $29,0($27)!gpdisp!1          /* get high offset to data section */
	lda  $29,0($29)!gpdisp!1          /* get low offset to data section */

	lda  $0, 4($31)                   /* v0 = syscall 4 (write) */
	lda  $16,1($31)                   /* a0 = fd 1 (stdout) */
	ldah $17,$message($29)!gprelhigh  /* a1 = message (relative to gp) */
	lda  $17,$message($17)!gprellow
	lda  $18,6($31)                   /* a2 = 6 bytes (write length) */
	callsys                           /* do syscall specified in v0 (write) */

	lda  $0, 1($31)                   /* v0 = syscall 1 (exit) */
	lda  $16,0($31)                   /* a0 = exitcode 0 */
	callsys                           /* do syscall specified in v0 (exit) */

