	.TITLE test1
	.ASECT
	.=500
	
start:
	mov	msg,r0
loop:
	movb	(r0)+,r1
	beq	done
	jsr	pc,tpchr
	br	loop

done:
	halt
	
tpchr:
	tstb	@tps
	bpl	tpchr
	movb	r1,@tpb
	rts	pc

tpb:
	.word 177566
tps:
	.word 177564

msg:
	.byte	15,12
	.ascii "Hello world!"
	.byte	15,12
	.byte	0


