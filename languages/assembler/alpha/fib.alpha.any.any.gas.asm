
.arch ev4
.set nomacro
.set noat
.set noreorder
.set volatile

.section .text
.valign 4
.globl fib

fib:
	lda   $0, 0($31)
	lda   $1, 1($31)
fib$again:
	cmpeq $16, $31, $7
	bne   $7, fib$done
	subq  $16, 1, $16
	addq  $0, $1, $2
	lda   $0, 0($1)
	lda   $1, 0($2)
	bsr   $31, fib$again
fib$done:
	ret
