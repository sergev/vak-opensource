.include "2313def.inc"


.def	DEL25		=R1		;(GL)DELAY 0.25 ms


;======================== *** SRAM SEGMENT ***==============================

	.DSEG  				;SRAM memory segment

NUMBER:		.byte	1
POSITION:	.byte	1
ADRES:		.byte	1

EFF:		.byte	1

BUT_FL:		.byte	1

PIC_BUF1:	.byte	1
PIC_BUF2:	.byte	1
PIC_BUF3:	.byte	1
PIC_BUF4:	.byte	1
PIC_BUF5:	.byte	1
PIC_BUF6:	.byte	1	
PIC_BUF7:	.byte	1

;======================== *** CODE SEGMENT ***==============================

	.CSEG     	 		;begin of program memory
	.org 0

;-----------------SETUP INTERRUPTS HANDLERS---------------------------------

reset:	rjmp	start			; 00 Reset Handler
	reti				; 01 IRQ0 Handler
	reti				; 02 IRQ1 Handler
	reti				; 03 Timer1 Capture Handler
	reti				; 04 Timer1 Compare Handler
	reti				; 05 Timer1 Overflow Handler
	rjmp	TimeKey			; 06 Timer0 Overflow Handler
	reti				; 07 UART RX Complete Handler
	reti				; 08 UDR Empty Handler
	reti				; 09 UART TX Complete Handler
	reti				; 0A Analog Comparator Handler
	
;--------------------- Messages & Constants --------------------------------

PIC1_1a:	.db	0b00110110
PIC1_1b:	.db	0b01111111
PIC1_1c:	.db	0b01111111
PIC1_1d:	.db	0b01111111
PIC1_1e:	.db	0b00111110
PIC1_1f:	.db	0b00011100
PIC1_1g:	.db	0b00001000

PIC1_2a:	.db	0b00000000
PIC1_2b:	.db	0b00011100
PIC1_2c:	.db	0b00111110
PIC1_2d:	.db	0b00111110
PIC1_2e:	.db	0b00011100
PIC1_2f:	.db	0b00001000
PIC1_2g:	.db	0b00000000

PIC1_3a:	.db	0b00000000
PIC1_3b:	.db	0b00000000
PIC1_3c:	.db	0b00011100
PIC1_3d:	.db	0b00011100
PIC1_3e:	.db	0b00001000
PIC1_3f:	.db	0b00000000
PIC1_3g:	.db	0b00000000

PIC1_4a:	.db	0b00000000
PIC1_4b:	.db	0b00000000
PIC1_4c:	.db	0b00000000
PIC1_4d:	.db	0b00001000
PIC1_4e:	.db	0b00000000
PIC1_4f:	.db	0b00000000
PIC1_4g:	.db	0b00000000


PIC1_5a:	.db	0b00000000
PIC1_5b:	.db	0b00000000
PIC1_5c:	.db	0b00000000
PIC1_5d:	.db	0b00000000
PIC1_5e:	.db	0b00000000
PIC1_5f:	.db	0b00000000
PIC1_5g:	.db	0b00000000

;-------------------------------

PIC2_1a:	.db	0b00001000
PIC2_1b:	.db	0b00001000
PIC2_1c:	.db	0b00001000
PIC2_1d:	.db	0b00001000
PIC2_1e:	.db	0b00001000
PIC2_1f:	.db	0b00001000
PIC2_1g:	.db	0b00001000

PIC2_2a:	.db	0b00000001
PIC2_2b:	.db	0b00000010
PIC2_2c:	.db	0b00000100
PIC2_2d:	.db	0b00001000
PIC2_2e:	.db	0b00010000
PIC2_2f:	.db	0b00100000
PIC2_2g:	.db	0b01000000

PIC2_3a:	.db	0b00000000
PIC2_3b:	.db	0b00000000
PIC2_3c:	.db	0b00000000
PIC2_3d:	.db	0b01111111
PIC2_3e:	.db	0b00000000
PIC2_3f:	.db	0b00000000
PIC2_3g:	.db	0b00000000

PIC2_4a:	.db	0b01000000
PIC2_4b:	.db	0b00100000
PIC2_4c:	.db	0b00010000
PIC2_4d:	.db	0b00001000
PIC2_4e:	.db	0b00000100
PIC2_4f:	.db	0b00000010
PIC2_4g:	.db	0b00000001

;-------------------------------

PIC3_1a:	.db	0b00011100
PIC3_1b:	.db	0b00101010
PIC3_1c:	.db	0b01001001
PIC3_1d:	.db	0b01001001
PIC3_1e:	.db	0b01010101
PIC3_1f:	.db	0b00100010
PIC3_1g:	.db	0b00011100

PIC3_2a:	.db	0b00011100
PIC3_2b:	.db	0b00101010
PIC3_2c:	.db	0b00101010
PIC3_2d:	.db	0b00101010
PIC3_2e:	.db	0b00110110
PIC3_2f:	.db	0b00100010
PIC3_2g:	.db	0b00011100

PIC3_3a:	.db	0b00001000
PIC3_3b:	.db	0b00011100
PIC3_3c:	.db	0b00011100
PIC3_3d:	.db	0b00011100
PIC3_3e:	.db	0b00010100
PIC3_3f:	.db	0b00010100
PIC3_3g:	.db	0b00001000

PIC3_4a:	.db	0b00001000
PIC3_4b:	.db	0b00001000
PIC3_4c:	.db	0b00001000
PIC3_4d:	.db	0b00001000
PIC3_4e:	.db	0b00001000
PIC3_4f:	.db	0b00001000
PIC3_4g:	.db	0b00001000


;-------------------------------

PIC4_1a:	.db	0b00001000
PIC4_1b:	.db	0b00001000
PIC4_1c:	.db	0b00001000
PIC4_1d:	.db	0b01111111
PIC4_1e:	.db	0b00001000
PIC4_1f:	.db	0b00001000
PIC4_1g:	.db	0b00001000

PIC4_2a:	.db	0b01000001
PIC4_2b:	.db	0b00100010
PIC4_2c:	.db	0b00010100
PIC4_2d:	.db	0b00001000
PIC4_2e:	.db	0b00010100
PIC4_2f:	.db	0b00100010
PIC4_2g:	.db	0b01000001


;-------------------------------

PIC5_1a:	.db	0b00000000
PIC5_1b:	.db	0b01000001
PIC5_1c:	.db	0b00100010
PIC5_1d:	.db	0b00010100
PIC5_1e:	.db	0b00001000
PIC5_1f:	.db	0b00000000
PIC5_1g:	.db	0b00000000

PIC5_2a:	.db	0b00000000
PIC5_2b:	.db	0b00000000
PIC5_2c:	.db	0b01000001
PIC5_2d:	.db	0b00110110
PIC5_2e:	.db	0b00001000
PIC5_2f:	.db	0b00000000
PIC5_2g:	.db	0b00000000

PIC5_3a:	.db	0b00000000
PIC5_3b:	.db	0b00000000
PIC5_3c:	.db	0b00000000
PIC5_3d:	.db	0b01110111
PIC5_3e:	.db	0b00001000
PIC5_3f:	.db	0b00000000
PIC5_3g:	.db	0b00000000

PIC5_4a:	.db	0b00000000
PIC5_4b:	.db	0b00000000
PIC5_4c:	.db	0b00000000
PIC5_4d:	.db	0b00110110
PIC5_4e:	.db	0b01001001
PIC5_4f:	.db	0b00000000
PIC5_4g:	.db	0b00000000

;-------------------------------

PIC6_1a:	.db	0b00000000
PIC6_1b:	.db	0b00000000
PIC6_1c:	.db	0b00000000
PIC6_1d:	.db	0b00001000
PIC6_1e:	.db	0b00000000
PIC6_1f:	.db	0b00000000
PIC6_1g:	.db	0b00000000

PIC6_2a:	.db	0b00000000
PIC6_2b:	.db	0b00000000
PIC6_2c:	.db	0b00001000
PIC6_2d:	.db	0b00011100
PIC6_2e:	.db	0b00001000
PIC6_2f:	.db	0b00000000
PIC6_2g:	.db	0b00000000

PIC6_3a:	.db	0b00000000
PIC6_3b:	.db	0b00000000
PIC6_3c:	.db	0b00011100
PIC6_3d:	.db	0b00011100
PIC6_3e:	.db	0b00011100
PIC6_3f:	.db	0b00000000
PIC6_3g:	.db	0b00000000

PIC6_4a:	.db	0b00000000
PIC6_4b:	.db	0b00101010
PIC6_4c:	.db	0b00011100
PIC6_4d:	.db	0b00111110
PIC6_4e:	.db	0b00011100
PIC6_4f:	.db	0b00101010
PIC6_4g:	.db	0b00000000

PIC6_5a:	.db	0b01001001
PIC6_5b:	.db	0b00101010
PIC6_5c:	.db	0b00011100
PIC6_5d:	.db	0b01111111
PIC6_5e:	.db	0b00011100
PIC6_5f:	.db	0b00101010
PIC6_5g:	.db	0b01001001

PIC6_6a:	.db	0b00000000
PIC6_6b:	.db	0b00000000
PIC6_6c:	.db	0b00000000
PIC6_6d:	.db	0b00000000
PIC6_6e:	.db	0b00000000
PIC6_6f:	.db	0b00000000
PIC6_6g:	.db	0b00000000

;-------------------------------

PIC7_1a:	.db	0b00001000
PIC7_1b:	.db	0b00001000
PIC7_1c:	.db	0b00001000
PIC7_1d:	.db	0b01111111
PIC7_1e:	.db	0b00001000
PIC7_1f:	.db	0b00001000
PIC7_1g:	.db	0b00001000

PIC7_2a:	.db	0b00001000
PIC7_2b:	.db	0b00001000
PIC7_2c:	.db	0b00011100
PIC7_2d:	.db	0b01110111
PIC7_2e:	.db	0b00011100
PIC7_2f:	.db	0b00001000
PIC7_2g:	.db	0b00001000

PIC7_3a:	.db	0b00001000
PIC7_3b:	.db	0b00111110
PIC7_3c:	.db	0b00100010
PIC7_3d:	.db	0b01100011
PIC7_3e:	.db	0b00100000
PIC7_3f:	.db	0b00111110
PIC7_3g:	.db	0b00001000

PIC7_4a:	.db	0b01111111
PIC7_4b:	.db	0b01000001
PIC7_4c:	.db	0b01000001
PIC7_4d:	.db	0b01000001
PIC7_4e:	.db	0b01000001
PIC7_4f:	.db	0b01000001
PIC7_4g:	.db	0b01111111

;-------------------------------

PIC8_1a:	.db	0b00000000
PIC8_1b:	.db	0b00000000
PIC8_1c:	.db	0b00100000
PIC8_1d:	.db	0b00100100
PIC8_1e:	.db	0b00101101
PIC8_1f:	.db	0b01111111
PIC8_1g:	.db	0b01111111

PIC8_2a:	.db	0b00000000
PIC8_2b:	.db	0b01000000
PIC8_2c:	.db	0b01000000
PIC8_2d:	.db	0b01010100
PIC8_2e:	.db	0b01111110
PIC8_2f:	.db	0b01111111
PIC8_2g:	.db	0b01111111

PIC8_3a:	.db	0b00000000
PIC8_3b:	.db	0b00000000
PIC8_3c:	.db	0b00001001
PIC8_3d:	.db	0b00001011
PIC8_3e:	.db	0b00101111
PIC8_3f:	.db	0b01111111
PIC8_3g:	.db	0b01111111

PIC8_4a:	.db	0b00000000
PIC8_4b:	.db	0b00001000
PIC8_4c:	.db	0b00001000
PIC8_4d:	.db	0b00001001
PIC8_4e:	.db	0b00101011
PIC8_4f:	.db	0b01111111
PIC8_4g:	.db	0b01111111

;-------------------------------

PIC9_1a:	.db	0b01111111
PIC9_1b:	.db	0b01000001
PIC9_1c:	.db	0b01000001
PIC9_1d:	.db	0b01000001
PIC9_1e:	.db	0b01000001
PIC9_1f:	.db	0b01000001
PIC9_1g:	.db	0b01111111

PIC9_2a:	.db	0b00000000
PIC9_2b:	.db	0b00111110
PIC9_2c:	.db	0b00100010
PIC9_2d:	.db	0b00100010
PIC9_2e:	.db	0b00100010
PIC9_2f:	.db	0b00111110
PIC9_2g:	.db	0b00000000

PIC9_3a:	.db	0b00000000
PIC9_3b:	.db	0b00000000
PIC9_3c:	.db	0b00011100
PIC9_3d:	.db	0b00010100
PIC9_3e:	.db	0b00011100
PIC9_3f:	.db	0b00000000
PIC9_3g:	.db	0b00000000

PIC9_4a:	.db	0b00000000
PIC9_4b:	.db	0b00000000
PIC9_4c:	.db	0b00000000
PIC9_4d:	.db	0b00001000
PIC9_4e:	.db	0b00000000
PIC9_4f:	.db	0b00000000
PIC9_4g:	.db	0b00000000

;-------------------------------

PIC10_1a:	.db	0b00011100
PIC10_1b:	.db	0b00100010
PIC10_1c:	.db	0b01000001
PIC10_1d:	.db	0b01000001
PIC10_1e:	.db	0b01000001
PIC10_1f:	.db	0b00100010
PIC10_1g:	.db	0b00011100

PIC10_2a:	.db	0b00000000
PIC10_2b:	.db	0b00011100
PIC10_2c:	.db	0b00100010
PIC10_2d:	.db	0b00100010
PIC10_2e:	.db	0b00100010
PIC10_2f:	.db	0b00011100
PIC10_2g:	.db	0b00000000

PIC10_3a:	.db	0b00000000
PIC10_3b:	.db	0b00000000
PIC10_3c:	.db	0b00001000
PIC10_3d:	.db	0b00010100
PIC10_3e:	.db	0b00001000
PIC10_3f:	.db	0b00000000
PIC10_3g:	.db	0b00000000

PIC10_4a:	.db	0b00000000
PIC10_4b:	.db	0b00000000
PIC10_4c:	.db	0b00000000
PIC10_4d:	.db	0b00001000
PIC10_4e:	.db	0b00000000
PIC10_4f:	.db	0b00000000
PIC10_4g:	.db	0b00000000

;-------------------------------

PIC11_1a:	.db	0b01111111
PIC11_1b:	.db	0b01000001
PIC11_1c:	.db	0b01000001
PIC11_1d:	.db	0b01000001
PIC11_1e:	.db	0b01000001
PIC11_1f:	.db	0b01000001
PIC11_1g:	.db	0b01111111

PIC11_2a:	.db	0b00000000
PIC11_2b:	.db	0b00000000
PIC11_2c:	.db	0b00000000
PIC11_2d:	.db	0b00000000
PIC11_2e:	.db	0b00000000
PIC11_2f:	.db	0b00000000
PIC11_2g:	.db	0b00000000

PIC11_3a:	.db	0b00001000
PIC11_3b:	.db	0b00010100
PIC11_3c:	.db	0b00100010
PIC11_3d:	.db	0b01000001
PIC11_3e:	.db	0b00100010
PIC11_3f:	.db	0b00010100
PIC11_3g:	.db	0b00001000

PIC11_4a:	.db	0b00000000
PIC11_4b:	.db	0b00000000
PIC11_4c:	.db	0b00000000
PIC11_4d:	.db	0b00000000
PIC11_4e:	.db	0b00000000
PIC11_4f:	.db	0b00000000
PIC11_4g:	.db	0b00000000

;-------------------------------

PIC12_1a:	.db	0b01111111
PIC12_1b:	.db	0b01000001
PIC12_1c:	.db	0b01000001
PIC12_1d:	.db	0b01000001
PIC12_1e:	.db	0b01000001
PIC12_1f:	.db	0b01000001
PIC12_1g:	.db	0b01111111

PIC12_2a:	.db	0b00111110
PIC12_2b:	.db	0b01000001
PIC12_2c:	.db	0b01000001
PIC12_2d:	.db	0b01000001
PIC12_2e:	.db	0b01000001
PIC12_2f:	.db	0b01000001
PIC12_2g:	.db	0b00111110

PIC12_3a:	.db	0b00011100
PIC12_3b:	.db	0b00100010
PIC12_3c:	.db	0b01000001
PIC12_3d:	.db	0b01000001
PIC12_3e:	.db	0b01000001
PIC12_3f:	.db	0b00100010
PIC12_3g:	.db	0b00011100

PIC12_4a:	.db	0b00001000
PIC12_4b:	.db	0b00010100
PIC12_4c:	.db	0b00110110
PIC12_4d:	.db	0b01000001
PIC12_4e:	.db	0b00110110
PIC12_4f:	.db	0b00010100
PIC12_4g:	.db	0b00001000

PIC12_5a:	.db	0b00000000
PIC12_5b:	.db	0b00010100
PIC12_5c:	.db	0b00110110
PIC12_5d:	.db	0b00001010
PIC12_5e:	.db	0b00110110
PIC12_5f:	.db	0b00010100
PIC12_5g:	.db	0b00000000

PIC12_6a:	.db	0b00000000
PIC12_6b:	.db	0b00000000
PIC12_6c:	.db	0b00011100
PIC12_6d:	.db	0b00011100
PIC12_6e:	.db	0b00011100
PIC12_6f:	.db	0b00000000
PIC12_6g:	.db	0b00000000

PIC12_7a:	.db	0b00000000
PIC12_7b:	.db	0b00000000
PIC12_7c:	.db	0b00000000
PIC12_7d:	.db	0b00001000
PIC12_7e:	.db	0b00000000
PIC12_7f:	.db	0b00000000
PIC12_7g:	.db	0b00000000

PIC12_8a:	.db	0b00000000
PIC12_8b:	.db	0b00000000
PIC12_8c:	.db	0b00000000
PIC12_8d:	.db	0b00000000
PIC12_8e:	.db	0b00000000
PIC12_8f:	.db	0b00000000
PIC12_8g:	.db	0b00000000

;----------------------- BEGIN OF PROGRAMM ---------------------------------

;---------------------- *** setup part *** ---------------------------------

start:	ldi	r16,0xdf		;stack init SP=0xdf
	out	spl,r16			;

	sbi	ACSR,ACD		;Analog comparator off

	ldi	r16,0xFF		;Set Ini Ports func (DIRECTION)
	out	ddrb,r16		;	DDRB
	ldi	r16,0b11111110		;Set Ini Ports value (VALUE)
	out	portb,r16		;	PORTB

	ldi	r16,0xFF		;Set Ini Ports func (DIRECTIO) (76543210)
	out	ddrd,r16		;	DDRD
	ldi	r16,0b01010101		;Set Ini Ports value (VALUE)   (76543210)
	out	portd,r16		;	PORTD

	ldi	r16,0b00000011
	out	TCCR0,r16		;T/C0 CK/64
	ldi	r16,0b00000010
	out	TIMSK,r16		;T/C0 Overflow INT enable
	
	
;--------------------- Registers clearing ----------------------------------

	ldi	r31,0x00		;Load 0=>MSB (Z)
	ldi	r30,0x1d		;Load 29=>LSB (Z)
RgClr:	st	Z,R31			;0=>R(Z)
	dec	R30			;Counter=counter-1
	brne	RgClr			;Do until counter=0

;====================== Main program =======================================
		
	sei			;global INT enable	


;--------------------------- SETUP -----------------------------------------

		ldi	r16,7
		ldi	r17,0b111111110		
		sts	NUMBER,r16
		sts	POSITION,r17

		ldi	r16,1		
		sts	EFF,r16	

		clr	r16		
		sts	BUT_FL,r16	

;---------------------------------------------------------------------------


EFF1:		lds	r16,EFF
		cpi	r16,0x01
		brne	EFF2		

		rcall	DL25		

		ldi	ZL,low(PIC1_1a*2)	
		ldi	ZH,high(PIC1_1a*2)	
		rcall	PIC_PICB

		rcall	DL25
		rcall	DL25

		ldi	ZL,low(PIC1_2a*2)	
		ldi	ZH,high(PIC1_2a*2)	
		rcall	PIC_PICB	

		rcall	DL25


		ldi	ZL,low(PIC1_3a*2)	
		ldi	ZH,high(PIC1_3a*2)	
		rcall	PIC_PICB	

		rcall	DL25

		ldi	ZL,low(PIC1_4a*2)	
		ldi	ZH,high(PIC1_4a*2)	
		rcall	PIC_PICB	

		rcall	DL25
		
		ldi	ZL,low(PIC1_5a*2)	
		ldi	ZH,high(PIC1_5a*2)	
		rcall	PIC_PICB	

		rcall	DL25

		ldi	ZL,low(PIC1_4a*2)	
		ldi	ZH,high(PIC1_4a*2)	
		rcall	PIC_PICB	

		rcall	DL25

		ldi	ZL,low(PIC1_3a*2)	
		ldi	ZH,high(PIC1_3a*2)	
		rcall	PIC_PICB	

		rcall	DL25

		ldi	ZL,low(PIC1_2a*2)	
		ldi	ZH,high(PIC1_2a*2)	
		rcall	PIC_PICB	

		
		clr	r16
		sts	BUT_FL,r16
		rjmp 	EFF1


EFF2:		lds	r16,EFF
		cpi	r16,0x02
		brne	EFF3		

		rcall	DL25
		
		ldi	ZL,low(PIC2_1a*2)	
		ldi	ZH,high(PIC2_1a*2)	
		rcall	PIC_PICB

		rcall	DL25
		

		ldi	ZL,low(PIC2_2a*2)	
		ldi	ZH,high(PIC2_2a*2)	
		rcall	PIC_PICB	

		rcall	DL25


		ldi	ZL,low(PIC2_3a*2)	
		ldi	ZH,high(PIC2_3a*2)	
		rcall	PIC_PICB	

		rcall	DL25

		ldi	ZL,low(PIC2_4a*2)	
		ldi	ZH,high(PIC2_4a*2)	
		rcall	PIC_PICB	

		clr	r16
		sts	BUT_FL,r16		
		rjmp 	EFF2


EFF3:		lds	r16,EFF
		cpi	r16,0x03
		brne	EFF4			

		rcall	DL25
		
		ldi	ZL,low(PIC3_1a*2)	
		ldi	ZH,high(PIC3_1a*2)	
		rcall	PIC_PICB

		rcall	DL25
		
		ldi	ZL,low(PIC3_2a*2)	
		ldi	ZH,high(PIC3_2a*2)	
		rcall	PIC_PICB	

		rcall	DL25

		ldi	ZL,low(PIC3_3a*2)	
		ldi	ZH,high(PIC3_3a*2)	
		rcall	PIC_PICB	

		rcall	DL25

		ldi	ZL,low(PIC3_4a*2)	
		ldi	ZH,high(PIC3_4a*2)	
		rcall	PIC_PICB	

		rcall	DL25

		ldi	ZL,low(PIC3_3a*2)	
		ldi	ZH,high(PIC3_3a*2)	
		rcall	PIC_PICB

		rcall	DL25
		
		ldi	ZL,low(PIC3_2a*2)	
		ldi	ZH,high(PIC3_2a*2)	
		rcall	PIC_PICB

		clr	r16
		sts	BUT_FL,r16		
		rjmp 	EFF3


EFF4:		lds	r16,EFF
		cpi	r16,0x04
		brne	EFF5			

		rcall	DL25
		
		ldi	ZL,low(PIC4_1a*2)	
		ldi	ZH,high(PIC4_1a*2)	
		rcall	PIC_PICB

		rcall	DL25
		
		ldi	ZL,low(PIC4_2a*2)	
		ldi	ZH,high(PIC4_2a*2)	
		rcall	PIC_PICB	
		
		
		clr	r16
		sts	BUT_FL,r16
		rjmp 	EFF4


EFF5:		lds	r16,EFF
		cpi	r16,0x05
		brne	EFF6			

		rcall	DL25
		
		ldi	ZL,low(PIC5_1a*2)	
		ldi	ZH,high(PIC5_1a*2)	
		rcall	PIC_PICB

		rcall	DL25
		
		ldi	ZL,low(PIC5_2a*2)	
		ldi	ZH,high(PIC5_2a*2)	
		rcall	PIC_PICB	

		rcall	DL25

		ldi	ZL,low(PIC5_3a*2)	
		ldi	ZH,high(PIC5_3a*2)	
		rcall	PIC_PICB	

		rcall	DL25

		ldi	ZL,low(PIC5_4a*2)	
		ldi	ZH,high(PIC5_4a*2)	
		rcall	PIC_PICB	

		rcall	DL25

		ldi	ZL,low(PIC5_3a*2)	
		ldi	ZH,high(PIC5_3a*2)	
		rcall	PIC_PICB

		rcall	DL25
		
		ldi	ZL,low(PIC5_2a*2)	
		ldi	ZH,high(PIC5_2a*2)	
		rcall	PIC_PICB

		clr	r16
		sts	BUT_FL,r16		
		rjmp 	EFF5

EFF6:		lds	r16,EFF
		cpi	r16,0x06
		brne	EFF7			

		rcall	DL25
		
		ldi	ZL,low(PIC6_1a*2)	
		ldi	ZH,high(PIC6_1a*2)	
		rcall	PIC_PICB

		rcall	DL25
				
		ldi	ZL,low(PIC6_2a*2)	
		ldi	ZH,high(PIC6_2a*2)	
		rcall	PIC_PICB	

		rcall	DL25
		
		ldi	ZL,low(PIC6_3a*2)	
		ldi	ZH,high(PIC6_3a*2)	
		rcall	PIC_PICB	

		rcall	DL25
		
		ldi	ZL,low(PIC6_4a*2)	
		ldi	ZH,high(PIC6_4a*2)	
		rcall	PIC_PICB	

		rcall	DL25
		
		ldi	ZL,low(PIC6_5a*2)	
		ldi	ZH,high(PIC6_5a*2)	
		rcall	PIC_PICB	
		
		rcall	DL25
		
		ldi	ZL,low(PIC6_6a*2)	
		ldi	ZH,high(PIC6_6a*2)	
		rcall	PIC_PICB	
		
		rcall	DL25
		
		clr	r16
		sts	BUT_FL,r16		
		rjmp 	EFF6

EFF7:		lds	r16,EFF
		cpi	r16,0x07
		brne	EFF8		

		rcall	DL25
		
		ldi	ZL,low(PIC7_1a*2)	
		ldi	ZH,high(PIC7_1a*2)	
		rcall	PIC_PICB

		rcall	DL25
				
		ldi	ZL,low(PIC7_2a*2)	
		ldi	ZH,high(PIC7_2a*2)	
		rcall	PIC_PICB	

		rcall	DL25
		
		ldi	ZL,low(PIC7_3a*2)	
		ldi	ZH,high(PIC7_3a*2)	
		rcall	PIC_PICB	

		rcall	DL25
		
		ldi	ZL,low(PIC7_4a*2)	
		ldi	ZH,high(PIC7_4a*2)	
		rcall	PIC_PICB	
				
		
		clr	r16
		sts	BUT_FL,r16
		rjmp 	EFF7


EFF8:		lds	r16,EFF
		cpi	r16,0x08
		brne	EFF9	

		rcall	DL25
		
		ldi	ZL,low(PIC8_1a*2)	
		ldi	ZH,high(PIC8_1a*2)	
		rcall	PIC_PICB

		rcall	DL25
				
		ldi	ZL,low(PIC8_2a*2)	
		ldi	ZH,high(PIC8_2a*2)	
		rcall	PIC_PICB	

		rcall	DL25
		
		ldi	ZL,low(PIC8_3a*2)	
		ldi	ZH,high(PIC8_3a*2)	
		rcall	PIC_PICB	

		rcall	DL25
		
		ldi	ZL,low(PIC8_4a*2)	
		ldi	ZH,high(PIC8_4a*2)	
		rcall	PIC_PICB	
				
		
		clr	r16
		sts	BUT_FL,r16
		rjmp 	EFF8

EFF9:		lds	r16,EFF
		cpi	r16,9
		brne	EFF10	

		rcall	DL25
		
		ldi	ZL,low(PIC9_1a*2)	
		ldi	ZH,high(PIC9_1a*2)	
		rcall	PIC_PICB

		rcall	DL25
				
		ldi	ZL,low(PIC9_2a*2)	
		ldi	ZH,high(PIC9_2a*2)	
		rcall	PIC_PICB	

		rcall	DL25
		
		ldi	ZL,low(PIC9_3a*2)	
		ldi	ZH,high(PIC9_3a*2)	
		rcall	PIC_PICB	

		rcall	DL25
		
		ldi	ZL,low(PIC9_4a*2)	
		ldi	ZH,high(PIC9_4a*2)	
		rcall	PIC_PICB	
				
		
		clr	r16
		sts	BUT_FL,r16
		rjmp 	EFF9

EFF10:		lds	r16,EFF
		cpi	r16,10
		brne	EFF11	

		rcall	DL25
		
		ldi	ZL,low(PIC10_1a*2)	
		ldi	ZH,high(PIC10_1a*2)	
		rcall	PIC_PICB

		rcall	DL25
				
		ldi	ZL,low(PIC10_2a*2)	
		ldi	ZH,high(PIC10_2a*2)	
		rcall	PIC_PICB	

		rcall	DL25
		
		ldi	ZL,low(PIC10_3a*2)	
		ldi	ZH,high(PIC10_3a*2)	
		rcall	PIC_PICB	

		rcall	DL25
		
		ldi	ZL,low(PIC10_4a*2)	
		ldi	ZH,high(PIC10_4a*2)	
		rcall	PIC_PICB	
				
		
		clr	r16
		sts	BUT_FL,r16
		rjmp 	EFF10

EFF11:		lds	r16,EFF
		cpi	r16,11
		brne	EFF12

		rcall	DL25
		
		ldi	ZL,low(PIC11_1a*2)	
		ldi	ZH,high(PIC11_1a*2)	
		rcall	PIC_PICB

		rcall	DL25
				
		ldi	ZL,low(PIC11_2a*2)	
		ldi	ZH,high(PIC11_2a*2)	
		rcall	PIC_PICB	

		rcall	DL25
		
		ldi	ZL,low(PIC11_3a*2)	
		ldi	ZH,high(PIC11_3a*2)	
		rcall	PIC_PICB	

		rcall	DL25
		
		ldi	ZL,low(PIC11_4a*2)	
		ldi	ZH,high(PIC11_4a*2)	
		rcall	PIC_PICB	
				

		clr	r16
		sts	BUT_FL,r16
		rjmp 	EFF10


EFF12:		lds	r16,EFF
		cpi	r16,12
		brne	EFF13

		rcall	DL25
		
		ldi	ZL,low(PIC12_1a*2)	
		ldi	ZH,high(PIC12_1a*2)	
		rcall	PIC_PICB

		rcall	DL25
				
		ldi	ZL,low(PIC12_2a*2)	
		ldi	ZH,high(PIC12_2a*2)	
		rcall	PIC_PICB	

		rcall	DL25
		
		ldi	ZL,low(PIC12_3a*2)	
		ldi	ZH,high(PIC12_3a*2)	
		rcall	PIC_PICB	

		rcall	DL25
		
		ldi	ZL,low(PIC12_4a*2)	
		ldi	ZH,high(PIC12_4a*2)	
		rcall	PIC_PICB	
			
		rcall	DL25
		
		ldi	ZL,low(PIC12_5a*2)	
		ldi	ZH,high(PIC12_5a*2)	
		rcall	PIC_PICB	
					
		rcall	DL25
		
		ldi	ZL,low(PIC12_6a*2)	
		ldi	ZH,high(PIC12_6a*2)	
		rcall	PIC_PICB	
					
		rcall	DL25
		
		ldi	ZL,low(PIC12_7a*2)	
		ldi	ZH,high(PIC12_7a*2)	
		rcall	PIC_PICB	
				
		rcall	DL25
		
		ldi	ZL,low(PIC12_8a*2)	
		ldi	ZH,high(PIC12_8a*2)	
		rcall	PIC_PICB		

		clr	r16
		sts	BUT_FL,r16
		rjmp 	EFF10

EFF13:		rjmp	EFF1

;========================== END of MAIN ====================================

.include "timekey.asm"




;==================  DELAY 025s ============================================

DL25:		cli
		push	r16
		ldi	r16,50
		mov	DEL25,r16
		pop	r16
		sei
DEL1:		tst	DEL25
		brne	DEL1
		
		RET	

;==================  PICTURE TO PICTURE BUFFER =============================


PIC_PICB:	push	r16
		push	XL		
		push	XH		

		ldi	XH,high(PIC_BUF1)	
		ldi	XL,low(PIC_BUF1)	
		ldi	r16,7	


PP1:		LPM				  
		adiw	r30,2			                    
		st	X+,r0	
		dec	r16
		brne	PP1

		pop	XH		
		pop	XL		
		pop	r16

		RET


;===========================================================================
