	.TITLE test7
	.ASECT
	.=500
	mov	#500, sp
	mov	#200, r1
	mov	#300, r2
	mov	2, r2
	mov	2, (r2)
	mov	2, (r2)+
	mov	2, @(r2)+
	mov	2, -(r2)
	mov	2, @-(r2)
	mov	2, 4(r2)
	mov	4, @0(r2)
	halt
