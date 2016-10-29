	.TITLE test17
	.ASECT

PSW=177776	;processor status word

	.=34
	.word	200
	.word 	0007

	.=200
	mov	#200, r5
   	rti

	.=500

	clr	@#PSW		;kernel mode	
	mov	#500,sp

	mov	#140000,@#PSW	;user mode	
	MOV	#700,sp

	trap	377

	clr	@#PSW		;back to kernel mode	
	halt

