; Division of two unsigned integers.
; Call:
;       xta     u1
;       xts     u2
;       vjm     u_div_u,14
; Result on accumulator, stack decreased by 1.
; Assumption is made that registers 10, 11 are scratch.

	.bss
ret:    .=.+1

	.text
	.globl  u_div_u ; unsigned u_div_u (u1, u2) unsigned u1,u2;
u_div_u:
			; cm = u2 , stack = u1
	uz      divbyzero ; u2=0
	atx     0,15
	acx     0
	aex     #1
	uz      divbytwo
			; entry of division cycle
	ztx     ,15     ; 0->rez

	ita     14
	atx     ret     ; save address for return

	xta     -2,15   ; u2
	anx
	xsa     #0
	ati     10      ; scratch register
loop:                   ; division loop         -s-t-a-c-k
	xta     -2,15   ; u2                    ;   u1   ;
	aex     -3,15   ; # u1                  ;---------
	anx                                     ;   u2   ;
	ati     11                              ;---------
	xta     #1                              ;   rez  ;
	asn     ,11                             ;---------
	yta                                     ;        ; <- free
	aax     -2,15   ; & u2
	ui      return  ; go if u2>u1 (rez is ready)
	xta     -3,15   ; u1
	anx
	ati     11
	jam     11,10   ; ir1 + ir2 -> ir1
	xta     -2,15   ; u2
	asn     ,11
	atx     ,15     ; -> rab
	aex     -4,15   ; # u1
	uz      gather
	anx
	ati     14
	xta     #1
	asn     ,14
	yta
	aax     -4,15   ; & u1
	ui      gather  ; go if u1>rab
	utm     1,11
	xta     ,15     ; rab
	asn     1
	atx     ,15     ; -> rab
gather:                 ; gather rez and make new u1 ( u1 = u1 - rab )
	xta     #1
	asn     ,11                             ;-c-t-e-k-
	aox     -2,15   ; | rez                 ;   u1   ;
	stx     -2,15   ; -> rez                ;---------
	aex     #-1     ; 0 - rab               ;   u2   ;
;       ntb     00060   ; bn bo                 ;---------
	arx     -3,15   ; + u1                  ;   rez  ;
	arx     #1                              ;---------
	atx     ,15     ; -> rab (new u1)       ;   rab  ;
	aex     -4,15   ; # u1                  ;---------
	anx                                     ;        ; <- free
	ati     14
	xta     #1
	asn     ,14
	yta
	aax     -4,15   ; & u1
	uz      ready   ; go if u1<new u1 (rez is ready)
	xta     ,15     ; new u1
	atx     -3,15   ; -> u1
	uj      loop    ; end of loop

ready:
	utm     -1,15   ; (rab)
return:
	xta     ,15     ; rez
	utm     -2,15   ; (u2,u1)
	uj      [ret]   ; return

divbytwo:               ; division by power of two
	xta     ,15     ; u2
	anx
	xsa     #64
	sti     11      ; scratch register
	asn     ,11
	uj      ,14

divbyzero:
	xta     ,15
	adx
	uj      ,14
