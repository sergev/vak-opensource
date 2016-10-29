	.TITLE test10
	.ASECT
	.=500
	mov	#500, sp
	mov	#700, r1
	clr	100(r1)
	jsr	r0, 2
	halt
subroutine:
	mov	r1, -(sp)
	mov	r2, -(sp)
	mov	(sp)+, r2
	mov	(sp)+, r1
	rts	r0
