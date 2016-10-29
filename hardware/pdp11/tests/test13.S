	.TITLE test13
	.ASECT
	.=500
	mov	#2, r1
	mov	#2, r2
	mul	r1, r2
	mov	#12, r1
	mov	#12, r2
	mul	r1, r2
	halt
