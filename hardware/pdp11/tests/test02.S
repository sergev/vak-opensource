	.TITLE test2
	.ASECT
	.=500
	mov	#500, sp
	mov	#700, r1
	mov	#712, r2
	inc	r1
	inc	r1
	inc	(r1)
	inc	(r1)+
	inc	@(r1)+
	inc	-(r1)
	inc	@-(r1)
	inc	2(r1)
	inc	@4(r1)
	halt
