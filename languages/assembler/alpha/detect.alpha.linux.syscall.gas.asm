/*
Detect and output processor number.
*/

.arch ev4
.set noat
.set nomacro
.globl _start
.balign 8

.section .data
message: .ascii "21x64\n"

.section .text
_start:
	bsr $27,0
	ldah $29,0($27)!gpdisp!1
	lda $29,0($29)!gpdisp!1
	
	ldah $17,message($29)!gprelhigh
	lda $17,message($17)!gprellow

	ldq $0,0($17)
	mskbl $0,2,$1
	implver $0
	addq $0,48,$0
	insbl $0,2,$0
	bis $0,$1,$2
	stq $2,0($17)

	lda $0,4($31)
	lda $16,1($31)
	lda $18,6($31)
	callsys

	lda $0, 1($31)
	lda $16, 0($31)
	callsys

