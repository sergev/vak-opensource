; Remainder of division of two unsigned integers.
; Call:
;       xta     u1
;       xts     u2
;       vjm     u_mod_u,14
; Result on accumulator, stack decreased by 1.
; Assumption is made that registers 10, 11 are scratch.

	.bss
ret:    .=.+1

	.text
	.globl  u_mod_u ; unsigned u_mod_u (u1, u2) unsigned u1,u2;
u_mod_u:
			; cm = u2 , ctek = u1
	uz      divbyzero ; u2=0
	atx     ,15
	acx
	aex     #1
	uz      divbytwo

	ita     14
	atx     ret     ; save address for return

			; entry of division cycle
	xta     -1,15   ; u2
	anx
	xsa     #0
	ati     10
loop:                   ; division loop         -s-t-a-c-k
	xta     -1,15   ; u2                    ;   u1   ;
	aex     -2,15   ; # u1                  ;---------
	anx                                     ;   u2   ;
	ati     11                              ;---------
	xta     #1                              ;        ; <- free
	asn     ,11
	yta
	aax     -1,15   ; & u2
	ui      return  ; go if u2>u1 (rez is ready)
	xta     -2,15   ; u1
	anx
	ati     11
	jam     11,10   ; ir1 + ir2 -> ir1
	xta     -1,15   ; u2
	asn     ,11
	atx     ,15     ; -> rab
	aex     -3,15   ; # u1
	uz      gather
	anx
	ati     14
	xta     #1
	asn     ,14
	yta
	aax     -3,15   ; & u1
	ui      gather  ; go if u1>rab
	utm     1,11
	xta     ,15     ; rab
	asn     1
	atx     ,15     ; -> rab
gather:                 ; gather rez and make new u1 ( u1 = u1 - rab )
	xta     ,15     ; rab                   -s-t-a-c-k
	aex     #-1     ; 0 - rab               ;   u1   ;
;       ntb     00060   ; bn bo                 ;---------
	arx     -2,15   ; + u1                  ;   u2   ;
	arx     #1                              ;---------
	atx     ,15     ; -> rab (new u1)       ;   rab  ;
	aex     -3,15   ; # u1                  ;---------
	anx                                     ;        ; <- free
	ati     14
	xta     #1
	asn     ,14
	yta
	aax     -3,15   ; & u1
	uz      ready   ; go if u1<new u1 ( rez is ready )
	xta     ,15     ; new u1
	atx     -2,15   ; -> u1
	uj      loop    ; end of loop
			; exits : 4 , 5 , 6
ready:
	utm     -1,15   ; (rab)
return:
	utm     -1,15   ; (u2)
	xta     ,15     ; rez (u1)
	uj      [ret]   ; return

divbytwo:               ; division by power of two
	xta     ,15     ; u2
	anx
	xsa     #64
	sti     11
	aln     ,11
	asn     ,11
	uj      ,14

divbyzero:
	xta     ,15     ; u1
	adx
	uj      ,14
