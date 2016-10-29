	.TITLE test14
	.ASECT

	.=10
	rts	r0
	mfpt

	.=200
	mov	#200, r5
	nop
	rti

	.=500
	mov	#500, sp
	cfcc
	nop
	halt

