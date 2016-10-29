	.text	0
	addi	r2, r0, 0		; r2 := 0
loop:
	sw	counter(r0), r2		; counter := r2
	addi	r2, r2, 1		; increment r2
	snei	r1, r2, 10		; if r2 = 10 then
	bnez	r1, loop		;   next loop
	trap	0
counter:
	.word	0
