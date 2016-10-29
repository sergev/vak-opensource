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

	.text
	.globl _setjmp  ; setjmp (env) jmp_buf env;
_setjmp:
	ati     11
	utcs    ,11
	utm     -1,15   ; pop arguments from stack

	xta     1,15
	ati     11      ; address of env

	utcs    ,15
	xta
	atx     0,11    ; save accumulator

	ita     1
	atx     1,11    ; save register 1
	ita     2
	atx     2,11
	ita     3
	atx     3,11
	ita     4
	atx     4,11
	ita     5
	atx     5,11
	ita     6
	atx     6,11
	ita     7
	atx     7,11
	ita     8
	atx     8,11
	ita     9
	atx     9,11
	ita     10
	atx     10,11
	ita     11
	atx     11,11
	ita     12
	atx     12,11
	ita     13
	atx     13,11
	ita     14
	atx     14,11
	ita     15
	atx     15,11   ; save register 15

	xta
	uj      ,14     ; return (0)

	.globl _longjmp ; longjmp (env, val) jmp_buf env;
_longjmp:
	ati     11
	utcs    ,11
	utm     -1,15   ; pop arguments from stack

	xta     1,15
	ati     11      ; address of env

	xta     2,15    ; val
	ui      nonzero ; if val is zero
	xta     #1      ; let it be 1
nonzero:
	atx     0,11    ; store in the place of old accumulator

	xta     1,11    ; restore register 1
	ati     1
	xta     2,11
	ati     2
	xta     3,11
	ati     3
	xta     4,11
	ati     4
	xta     5,11
	ati     5
	xta     6,11
	ati     6
	xta     7,11
	ati     7
	xta     8,11
	ati     8
	xta     9,11
	ati     9
	xta     10,11
	ati     10
			; don't restore register 11 !
	xta     12,11
	ati     12
	xta     13,11
	ati     13
	xta     14,11
	ati     14
	xta     15,11    ; restore register 15
	ati     15

	xta     0,11
	uj      ,14     ; return (val ? val : 1)
