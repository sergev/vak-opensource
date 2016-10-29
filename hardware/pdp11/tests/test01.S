#define halt .word 0

	mov	$0500, sp
	mov	$tab1, r1
	mov	$tab1+012, r2
	mov	$tab2, r3
	mov	$tab2+012, r4
	clr	r0
	clr	r5
loop1:
	add	(r1)+, r5
	cmp	r1, r2
//	bne	loop1
//jmp loop1           // 00167 177770
//jmp $loop1          // 00127 000530
//jmp *ploop1          // 000177 177754
jmp *$ploop1         // 000137 000530
//jmp *(r0)+
loop2:
	add	(r3)+, r0
	cmp	r3, r4
	bne	loop2
	sub	r5, r0
	halt

tab1:	0001
	0002
	0003
	0004
	0005

tab2:	0004
	0005
	0006
	0007
	0010
ploop1:
        .word   loop1
