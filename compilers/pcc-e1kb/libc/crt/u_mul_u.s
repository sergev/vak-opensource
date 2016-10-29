; Multiplication of two unsigned integers.
; Call:
;       xta     u1
;       xts     u2
;       vjm     u_mul_u,14
; Result on accumulator, stack decreased by 1.

	.text
	.globl  u_mul_u ; unsigned u_mul_u (u1, u2) unsigned u1, u2;
u_mul_u:
			; cm = u2 , stack = u1
	atx     ,15     ; -> u2
	aox     -2,15   ; | u1
	asn     52
	ui      longs
			; short way
	xta     -2,15   ; u1
	amx     ,15     ; * u2
	yts             ; rez.st -> u2 ; rez.ml
	stx     -2,15   ; rez.ml -> u1 ; rez.st
	aln     52
	aox     ,15     ; | rez.ml
	uj      ,14
longs:                  ; long  way
	xta     ,15     ; u2            s-t-a-c-k
	asn     52      ; u2l           ;  u1r  ;
	yts                             ;-------;
	asn     12      ; u2r           ;  u2l  ;
	xts     -3,15   ; u1            ;-------;
	asn     52      ; u1l           ;  u2r  ;
	yts                             ;-------;
	asn     12      ; u1r           ;  u1l  ;
	atx     -4,15   ; u1r -> u1     ;-------;
	amx     -3,15   ; u1r * u2l     ; (rez) ; <- free
	yta
	aln     12
	xts     -4,15   ; -> rez; u2l
	amx     -2,15   ; u2l * u1l
	yta
	aln     24
	arx     ,15     ; + rez
	stx     -3,15   ; rez -> u2l; u1l
	amx     -1,15   ; u1l * u2r
	yta
	aln     12
	arx     -2,15   ; + rez
	stx     -2,15   ; rez -> rez; u2r
	amx     -2,15   ; u2r * u1r
	yts             ; rez.st -> u2r; rez.ml
	stx     -3,15   ; rez.ml -> u1r; rez.st
	aln     52
	arx     ,15
	arx     ,15
	uj      ,14
