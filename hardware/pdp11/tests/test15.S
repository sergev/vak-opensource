	.TITLE test15
	.ASECT

	.=4
	rts	r0
	mfpt

	.=200
	mov	#200, r5
	nop
	rti

	.=500
	mov	#400, sp
	jsr	pc, r0
	nop
	halt
