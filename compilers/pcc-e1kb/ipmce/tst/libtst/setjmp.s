; setjmp, longjmp - nonlocal goto

; # include <setjmp.h>
; setjmp (env)
; jmp_buf env;
; longjmp (env, val)
; jmp_buf env;

; The setjmp subroutine saves its stack environment in env
; for later use by longjmp. It returns value 0.
; The longjmp subroutine restores the environment saved
; by the last call of setjmp. It then returns in such a
; way that execution continues as if the call of setjmp
; had just returned the value val to the function that
; invoked setjmp, which must not itself have returned in
; the interim. However, longjmp cannot cause setjmp to
; return the value 0. If longjmp is invoked with a val of
; 0, setjmp will return 1. All accessible data have
; values as of the time longjmp was called.

; Implemented for Elbrus-B by Vakulenko S.V., KIAE, Moscow.
; Assume jmp_buf is int [16].
; Register 11 (b) is not restored by longjmp,
; because it is scratch in C.

fp      =       14

	.text
	.globl _setjmp  ; setjmp (env) jmp_buf env;
_setjmp:
	atc
	utm     -1,15   ; pop arguments from stack

	ita     14
	atx     ret

	xta     1,15
	ati     fp      ; address of env

	utcs    ,15
	xta
	atx     0,fp    ; save accumulator

	ita     1
	atx     1,fp    ; save register 1
	ita     2
	atx     2,fp
	ita     3
	atx     3,fp
	ita     4
	atx     4,fp
	ita     5
	atx     5,fp
	ita     6
	atx     6,fp
	ita     7
	atx     7,fp
	ita     8
	atx     8,fp
	ita     9
	atx     9,fp
	ita     10
	atx     10,fp
	ita     11
	atx     11,fp
	ita     12
	atx     12,fp
	ita     13
	atx     13,fp
	ita     14
	atx     14,fp
	ita     15
	atx     15,fp   ; save register 15

	xta
	uj      [ret]   ; return (0)

	.globl _longjmp ; longjmp (env, val) jmp_buf env;
_longjmp:
	atc
	utm     -1,15   ; pop arguments from stack

	xta     1,15
	ati     fp      ; address of env

	xta     2,15    ; val
	ui      nonzero ; if val is zero
	xta     #1      ; let it be 1
nonzero:
	atx     0,fp    ; store in the place of old acc

; restore registers

	xta     1,fp
	ati     1
	xta     2,fp
	ati     2
	xta     3,fp
	ati     3
	xta     4,fp
	ati     4
	xta     5,fp
	ati     5
	xta     6,fp
	ati     6
	xta     7,fp
	ati     7
	xta     8,fp
	ati     8
	xta     9,fp
	ati     9
	xta     10,fp
	ati     10
	xta     11,fp
	ati     11
	xta     12,fp
	ati     12
	xta     13,fp
	ati     13
	xta     15,fp           ; restore register 15
	ati     15

	xta     0,fp
	uj      [14,fp]         ; return (val ? val : 1)

	.bss
ret:    .=.+1
