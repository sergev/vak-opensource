; Addition of two unsigned integers.
; Call:
;       xta     u1
;       xts     u2
;       vjm     u_add_u,14
; Result on accumulator, stack decreased by 1.

	.text
	.globl  u_add_u ; unsigned u_add_u (u1, u2) unsigned u1, u2;
u_add_u:
	xts     -2,15
	aax     #0x8000000000000000
	ui      hibit1
	xta     -1,15
	aax     #0x8000000000000000
	ui      hibit01
	xta     ,15             ; 64 bits are zeros
	arx     ,15
	uj      ,14
hibit1:
	xta     -1,15
	aax     #0x8000000000000000
	ui      hibit11
	xta     -2,15           ; nonzero - zero
	aex     #0x8000000000000000
	arx     ,15
	aex     #0x8000000000000000
	utm     -1,15
	uj      ,14
hibit01:
	xta     ,15             ; zero - nonzero
	aex     #0x8000000000000000
	arx     ,15
	aex     #0x8000000000000000
	uj      ,14
hibit11:
	xta     -2,15           ; nonzero - nonzero
	aex     #0x8000000000000000
	xts     -2,15           ; nonzero - nonzero
	aex     #0x8000000000000000
	arx     ,15
	utm     -2,15
	uj      ,14
