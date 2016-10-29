; Remainder of division of 64-bit unsigned integers.
; Result is unsigned.
; Call:
;       xta     u1
;       xts     u2
;       vjm     u_mod_u,14
; Result on accumulator, stack decreased by 1.

; if u1 has less than 51 bits then
;       if u2 has more than 51 bits
;               return u1
;       return (int) u1 % (int) u2
; else
;       /* u1 has more than 51 bits */
;       if u2 has less than 38 bits
;               return u_mod_i (u1, (int) u2)
;       /* u2 has more than 38 bits */
;       q = u1>>13 / u2>>13
;       if q==0
;               return u1
;       mod = u1-q*u2
;       if u1-q*u2 < 0
;               return mod+u2
;       return mod
; fi

; ---------------------
; |        u1         |
; |-------------------|
; |        u2         |
; |-------------------|
; |        q          |
; |-------------------|

	.globl  u_mod_u
u_mod_u:
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
:       uiv     .+1             ; if unsigned<0
	arx     -2,15           ; + u2

	ntr     200h

	utm     -3,15
	uj      [ret]           ; return mod

ret0:
	xta     -2,15           ; u1
	utm     -2,15
	uj      ,14             ; return u1

u1short:
	xta     -1,15
	asn     51+1024
	ui      ret0

	xta     -1,15

; integer division
	ecn     400h-34h
	xts     -3,15
	adx     ,15             ; u1/u2
	int     68h
	amux    ,15             ; u1/u2*u2
	xsua    ,15             ; u1 - u1/u2*u2 == u1%u2

	uj      ,14             ; return (int) u1 % (int) u2

u2short:
	xta     ,15

; Remainder of division of 64-bit unsigned by 38-bit integer.
; Result is 38-bit unsigned.
; Call:
;       xta     u
;       xts     i
;       vjm     u_mod_i,14
; Result on accumulator, stack decreased by 1.

	.globl  u_mod_i
u_mod_i:

; -------------------------
; |       a       |   b   |  = u
; -------------------------
;  64           14 13    1

; -------------------------
; |   0   |       i       |  = i
; -------------------------
;  64   39 38            1

; return (a%i << 13 | b) % i

; ---------------------
; |      u -> b       |
; |-------------------|
; |        i          |
; |-------------------|
; |        a          |
; |-------------------|

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

	amux    -2,15           ; a % i * i

	xsua    ,15             ; a - a/i*i
	aln     13+1024         ; << 13
	aox     -2,15           ; | b
	atx     -2,15           ; -> b

	xts     -2,15           ; i

; integer division
	ecn     400h-34h
	stx     ,15
	adx     1,15
	int     68h

	amux    ,15             ; * i

	xsua    ,15
	uj      ,14             ; return (a%i << 13 | b) % i

	.bss
ret:    .=.+1
