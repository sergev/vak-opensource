;----------------------------------------------------------------------------
;                    I N T E R R U P T     P R O G R A M M                   
;----------------------------------------------------------------------------


;============================== Entrance ====================================

TimeKey:	;sei			;global INT enable	

		push	r16		;push R16
	
		ldi	r16,99
		out	TCNT0,r16	;T/C0 start from 99
	
		in	r16,sreg	;push sreg
		push	r16		;push sreg

		push	XL		;
		push	XH		;
		push	YL		;
		push	YH		;
		push	R17		;
		push	R18		;
		push	R19		;
		push	R20		;
		push	R21		;
		push	R22		;
		push	R23		;
		push	R24		;
		push	R25		;
		push	R0		;Push Rx in STACK

;============================ BUTTONS CAPTURE ===============================

		lds	r16,BUT_FL
		tst	r16
		brne	HD2

		in	r16,PINB		;GET PORTB State
		andi	r16,0b10000000
		tst	r16
		brne	HD2

		in	r16,PINB		;GET PORTB State
		andi	r16,0b10000000
		tst	r16
		brne	HD2

		in	r16,PINB		;GET PORTB State
		andi	r16,0b10000000
		tst	r16
		brne	HD2

		ser	r16		
		sts	BUT_FL,r16	
		
		lds	r16,EFF
		inc	r16
		cpi	r16,13
		brne	BC1

		ldi	r16,1		

BC1:		sts	EFF,r16	

		;cpi	r16,1
		;brne	BC2
		;rjmp	EFF1
;
BC2:		;cpi	r16,2
		;brne	BC3
		;rjmp	EFF2

BC3:


;===================== HardWare DIGIT DISPLAY ===============================
				

HD2:		lds	r16,NUMBER
		lds	r17,POSITION
		lds	XL,ADRES


		out	PORTB,r17
		ld	r18,X+
		out	PORTD,r18

		sec
		rol	r17
		dec	r16
		tst	r16
		brne	HD1

		ldi	r16,7
		ldi	r17,0b111111110	
		ldi	XL,PIC_BUF1	

HD1:		sts	NUMBER,r16
		sts	POSITION,r17
		sts	ADRES,XL



;============================= DELAY 025s ==================================

		tst	DEL25
		breq	EXIT
		dec	DEL25
		rjmp	EXIT
		
;============================== EXIT =======================================
	
EXIT:		pop	r0		;
		pop	r25		;
		pop	R24	
		pop	r23		;
		pop	r22		;
		pop	r21		;
		pop	r20		;
		pop	R19		;
		pop	R18		;
		pop	R17		;
		pop	YH		;
		pop	YL		;
		pop	XH		;
		pop	XL		;
		pop	r16		;Pop Rx from STACK

		out	sreg,r16	;Pop sreg
		pop	r16		;pop R16

		RETI
