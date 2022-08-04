/*
Output "Hello world!" to standard output.
For DEC Alpha EV4 architecture with GNU assembler, using libc functions.

Assemble with :
as -o hello.o hello.alpha.linux.libc.gas.asm &&
gcc -o hello hello.o
*/

.arch ev4
.set noat
.set nomacro
.set volatile

.section .data
$message:
	.ascii "Hello world!\0"

.section .text
.balign 4
.globl main

main:
	/* load gp from pv, relocate at link time */
	ldah $29,0($27)!gpdisp!1
	lda  $29,0($29)!gpdisp!1

	lda  $sp,-8($sp)
	stq  $26,0($30)                    /* push return address */

	ldah $16,$message($29)!gprelhigh   /* message address */
	lda  $16,$message($16)!gprellow
	ldq  $27,puts($29)!literal!2       /* get gp offset of puts() */
	jsr  $26,($27),puts!lituse_jsr!2   /* call puts() */

	/* we'd have to restore gp here if we wanted to load any more gp-offset
	   immediates or reference the GOT */

	ldq  $26,0($sp)                    /* pop return address */
	ret
