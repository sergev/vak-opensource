	.TITLE test8
	.ASECT
	.=500
	mov	#500, sp
	mov	#200, r1
	mov	#300, r2
	mov	newpc, pc
spot:
	mov	34, (pc)
	mov	2, #4
	mov	2, @(pc)+
	mov	2, -(pc)
	mov	2, @-(pc)
	mov	2, 4(pc)
	mov	4, @0(pc)
	mov	0, #0
newpc:
	.word	spot
