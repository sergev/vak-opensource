; Division of 64-bit unsigned by 64-bit unsogned.
; Result is unsigned.
; Call:
;       xta     u1
;       xts     u2
;       vjm     u_div_u,14
; Result on accumulator, stack decreased by 1.

; if u1 has less than 51 bits then
;       if u2 has more than 51 bits
;               return 0
;       return (int) u1 / (int) u2
; else
;       /* u1 has more than 51 bits */
;       if u2 has less than 38 bits
;               return u_div_i (u1, (int) u2)
;       /* u2 has more than 38 bits */
;       q = u1>>13 / u2>>13
;       if q==0
;               return q
;       if u1-q*u2 < 0
;               return q-1
;       return q
; fi

; ---------------------
; |        u1         |
; |-------------------|
; |        u2         |
; |-------------------|
; |        q          |
; |-------------------|

	.globl  u_div_u
u_div_u:
	xts     -2,15           ; u1
	asn     51+1024
	uz      u1short

; u1 has more than 51 bits

	xta     -1,15           ; u2
	asn     38+1024
	uz      u2short

; u2 has more than 38 bits

	xta     -2,15           ; u1
	asn     13+1024
	xts     -2,15           ; u2
	asn     13+1024

; integer division
	ecn     400h-34h
	stx     ,15
	adx     1,15
	int     68h

	uz      ret0

	its     14
	atx     ret

	xta     -3,15           ; u1
	xts     -2,15           ; q
	xts     -4,15           ; u2
	vjm     u_mul_u,14
	aex     #-1
	ntr
	arx     #1
	arx     ,15
	uzv     sub1            ; if unsigned<0

	ntr     200h
	xta     ,15             ; q

	utm     -2,15
	uj      [ret]           ; return q

sub1:
	ntr     200h
	xta     ,15             ; q
	asux    #1

	utm     -2,15
	uj      [ret]           ; return q-1

ret0:
	xta
	utm     -2,15
	uj      ,14             ; return 0

u1short:
	xta     -1,15
	asn     51+1024
	ui      ret0

	xta     ,15

; integer division
	ecn     400h-34h
	stx     ,15
	adx     1,15
	int     68h

	uj      ,14             ; return (int) u1 / (int) u2

u2short:
	xta     ,15

; Division of 64-bit unsigned by 38-bit integer.
; Result is 24-bit unsigned.
; Call:
;       xta     u
;       xts     i
;       vjm     u_div_i,14
; Result on accumulator, stack decreased by 1.

	.globl  u_div_i
u_div_i:

; -------------------------
; |       a       |   b   |  = u
; -------------------------
;  64           14 13    1

; -------------------------
; |   0   |       i       |  = i
; -------------------------
;  64   39 38            1

; x = a/i
; y = ((a - i*x) << 13 | b) / i
; result = x<<13 | y

; ---------------------
; |      u -> b       |
; |-------------------|
; |        i          |
; |-------------------|
; |        a          |
; |-------------------|
; |        x          |
; ---------------------

	xts     -2,15           ; u
	asn     13+1024         ; a
	yts
	asn     51+1024         ; b
	atx     -3,15           ; -> u

	xta     -1,15           ; a
	xts     -3,15           ; i

; integer division
	ecn     400h-34h
	stx     ,15
	adx     1,15
	int     68h

	atx     ,15             ; a/i
	amux    -3,15           ; * i

	xsua    -2,15           ; a - i*x
	aln     13+1024         ; << 13
	aox     -4,15           ; | b

	xts     -4,15           ; i

; integer division
	ecn     400h-34h
	stx     ,15
	adx     1,15
	int     68h             ; y

	stx     ,15             ; x
	aln     13+1024         ; << 13
	aox     1,15            ; | y
	utm     -3,15
	uj      ,14             ; return x<<13 | y

	.bss
ret:    .=.+1
