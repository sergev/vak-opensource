MHZ     .equ    16384000
BAUD    .equ    9600

RCSTA   .equ    0x13
TXSTA   .equ    0x15
TXREG   .equ    0x16
SPBRG   .equ    0x17
SPEN    .equ    7
TXEN    .equ    5
TRMT    .equ    1
BRG     .equ    ((MHZ / BAUD) + 32) / 64 - 1

	.config protected, crystal

	.org    0
	goto    main

	.org    0x28
main:   xz      TXSTA           ; clear transmitter
	xz      RCSTA           ; clear transmitter

	cta     BRG >> 8
	atx     SPBRG + 1       ; set baud rate (upper)
	cta     BRG & 0xff
	atx     SPBRG           ; set baud rate (lower)

	bs      RCSTA,SPEN      ; enable serial port
	bs      TXSTA,TXEN      ; enable transmitter

	cta     'H'
	call    output
	cta     'e'
	call    output
	cta     'l'
	call    output
	cta     'l'
	call    output
	cta     'o'
	call    output
	cta     '!'
	call    output
	cta     '\r'
	call    output
	cta     '\n'
	call    output
again:
	cta     '\\'
	call    output
	cta     '\b'
	call    output
	cta     '|'
	call    output
	cta     '\b'
	call    output
	cta     '/'
	call    output
	cta     '\b'
	call    output
	cta     '-'
	call    output
	cta     '\b'
	call    output
	goto    again           ; loop forever

output: atx     TXREG           ; output the symbol to async port
	bz?     TXSTA,TRMT
	goto    .-1
	ret
