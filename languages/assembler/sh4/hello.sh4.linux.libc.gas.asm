/*
Output "Hello world!" to standard output.
For SH4 architecture with GNU assembler, using libc functions.
Assemble with :
as -o hello.o hello.sh4.linux.libc.gas.asm &&
gcc -o hello hello.o
*/

.section .text
.balign 4
.globl main

main:

	mov.l puts_ptr, r0
	sts.l pr, @-r15
	mov.l message_ptr, r4
	jsr	@r0
	nop
	mov	#0, r0
	lds.l @r15+, pr	
	rts
	nop

.balign 4
message_ptr:
	.long message
puts_ptr:
	.long puts

.section .rodata
.balign 4
message:
	.asciz "Hello world!"
