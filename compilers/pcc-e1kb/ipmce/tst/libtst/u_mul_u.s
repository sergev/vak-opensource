; Multiplication of two 64-bit unsigned integers.
; Call:
;       xta     u1
;       xts     u2
;       vjm     u_mul_u,14
; Result on accumulator, stack decreased by 1.

	.text
	.globl  u_mul_u
u_mul_u:

; -------------------------
; | u1l |       u1r       |  = u1
; -------------------------
;  64 52 51              1

; -------------------------
; | u2l |       u2r       |  = u2
; -------------------------
;  64 52 51              1

; result = u2l*u1r<<51 + u2r*u1l<<51 + u2r*u1r

; ---------------------
; |     u1 -> u1r     |
; |-------------------|
; |        u2l        |
; |-------------------|
; |        u2r        |
; |-------------------|
; |        u1l        |
; ---------------------

	asn     51+1024
	uz      u2short

	yts                     ; u2l
	asn     13+1024         ; u2r

	xts     -3,15           ; u1
	asn     51+1024
	uz      u1short

	yts                     ; u1l
	asn     13+1024         ; u1r
	atx     -4,15           ; -> u1

	aax     #01fffh         ; 13 bits
	amux    -3,15           ; u2l
	atx     rez             ; u1r * u2l

	xta     -2,15           ; u2r
	aax     #01fffh         ; 13 bits
	amux    ,15             ; u1l

	arx     rez
	aln     51+1024
	stx     rez             ; u2r
	utm     -1,15

; multiply 52-bit integers with 64-bit rezult
mul52:
	ntr     230h
	een     400h
	amux    ,15
	yts
	stx     ,15
	aln     52+1024
	aox     1,15

	ntr
	arx     rez
	ntr     200h
	uj      ,14

; ---------------------
; |     u1 -> u1r     |
; |-------------------|
; |        u2         |
; |-------------------|
; |        u1l        |
; ---------------------

u2short:                        ; u2 has only 52 bits
	yta
	asn     13+1024         ; u2

	xts     -2,15           ; u1
	asn     51+1024
	uz      u12short

	yts                     ; u1l
	asn     13+1024         ; u1r
	atx     -3,15           ; -> u1

	xta     -2,15           ; u2
	aax     #01fffh         ; 13 bits
	amux    ,15             ; u1l

	aln     51+1024
u12short:
	stx     rez             ; u2
	uj      mul52

; ---------------------
; |        u1         |
; |-------------------|
; |        u2l        |
; |-------------------|
; |        u2r        |
; ---------------------

u1short:                        ; u1 has only 52 bits
	xta     -3,15           ; u1
	aax     #01fffh         ; 13 bits
	amux    -2,15           ; u2l

	aln     51+1024
	stx     rez             ; u2r
	utm     -1,15
	uj      mul52

	.bss
rez:    .=.+1
