	.section	__TEXT,__text,regular,pure_instructions
	.build_version macos, 15, 0	sdk_version 15, 1
	.globl	_main                           ## -- Begin function main
_main:                                  ## @main
	.cfi_startproc
## %bb.0:
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset %rbp, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register %rbp
	movq	_a(%rip), %rsi
	movq	_b(%rip), %rdx
	movq	%rsi, %rcx
	imulq	%rdx, %rcx
	jno	LBB0_2
## %bb.1:
	leaq	L_.str(%rip), %rdi
	xorl	%eax, %eax
	callq	_printf
	jmp	LBB0_3
LBB0_2:
	leaq	L_.str.1(%rip), %rdi
	xorl	%eax, %eax
	callq	_printf
LBB0_3:
	xorl	%eax, %eax
	popq	%rbp
	retq
	.cfi_endproc
                                        ## -- End function
	.section	__DATA,__data
	.globl	_a                              ## @a
	.p2align	3, 0x0
_a:
	.quad	5000000000                      ## 0x12a05f200

	.globl	_b                              ## @b
	.p2align	3, 0x0
_b:
	.quad	6000000000                      ## 0x165a0bc00

	.section	__TEXT,__cstring,cstring_literals
L_.str:                                 ## @.str
	.asciz	"%ld * %ld = overflow\n"

L_.str.1:                               ## @.str.1
	.asciz	"%ld * %ld = %ld\n"

.subsections_via_symbols
