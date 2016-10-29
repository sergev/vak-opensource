	.TITLE test1
	.ASECT
	.=500
	mov	#500, sp
	mov	#700, r1
	mov	#712, r2
	mov	#1000, r3
	mov	#1012, r4
	clr	r0
	clr	r5
loop1:	
	add	(r1)+, r5
	cmp	r1, r2
	bne	loop1
loop2:	
	add	(r3)+, r0
	cmp	r3, r4
	bne	loop2
	sub	r5, r0
	halt

	.=700
	0001
	0002
	0003
	0004
	0005

	.=1000
	0004
	0005
	0006
	0007
	0010

