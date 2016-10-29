	.TITLE test5
	.ASECT
	.=500
	mov	#500, sp
	mov	#200, r1
	mov	#300, r2
	mov	@2(r1), r2
	mov	@2(r1), (r2)
	mov	@2(r1), (r2)+
	mov	@2(r1), @(r2)+
	mov	@2(r1), -(r2)
	mov	@2(r1), @-(r2)
	mov	@2(r1), 4(r2)
	mov	@6(r1), @0(r2)
	halt