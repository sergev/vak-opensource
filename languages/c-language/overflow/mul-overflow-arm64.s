	.section	__TEXT,__text,regular,pure_instructions
	.build_version macos, 15, 0	sdk_version 15, 1
	.globl	_main                           ; -- Begin function main
	.p2align	2
_main:                                  ; @main
	.cfi_startproc
; %bb.0:
	sub	sp, sp, #48
	stp	x29, x30, [sp, #32]             ; 16-byte Folded Spill
	add	x29, sp, #32
	.cfi_def_cfa w29, 16
	.cfi_offset w30, -8
	.cfi_offset w29, -16
Lloh0:
	adrp	x8, _a@PAGE
Lloh1:
	ldr	x8, [x8, _a@PAGEOFF]
Lloh2:
	adrp	x9, _b@PAGE
Lloh3:
	ldr	x9, [x9, _b@PAGEOFF]
	mul	x10, x8, x9
	smulh	x11, x8, x9
	cmp	x11, x10, asr #63
	b.eq	LBB0_2
; %bb.1:
	stp	x8, x9, [sp]
Lloh4:
	adrp	x0, l_.str@PAGE
Lloh5:
	add	x0, x0, l_.str@PAGEOFF
	b	LBB0_3
LBB0_2:
	stp	x9, x10, [sp, #8]
	str	x8, [sp]
Lloh6:
	adrp	x0, l_.str.1@PAGE
Lloh7:
	add	x0, x0, l_.str.1@PAGEOFF
LBB0_3:
	bl	_printf
	mov	w0, #0                          ; =0x0
	ldp	x29, x30, [sp, #32]             ; 16-byte Folded Reload
	add	sp, sp, #48
	ret
	.loh AdrpLdr	Lloh2, Lloh3
	.loh AdrpLdr	Lloh0, Lloh1
	.loh AdrpAdd	Lloh4, Lloh5
	.loh AdrpAdd	Lloh6, Lloh7
	.cfi_endproc
                                        ; -- End function
	.section	__DATA,__data
	.globl	_a                              ; @a
	.p2align	3, 0x0
_a:
	.quad	5000000000                      ; 0x12a05f200

	.globl	_b                              ; @b
	.p2align	3, 0x0
_b:
	.quad	6000000000                      ; 0x165a0bc00

	.section	__TEXT,__cstring,cstring_literals
l_.str:                                 ; @.str
	.asciz	"%ld * %ld = overflow\n"

l_.str.1:                               ; @.str.1
	.asciz	"%ld * %ld = %ld\n"

.subsections_via_symbols
