	.text
	.file	"mul-overflow.c"
	.globl	main                            # -- Begin function main
	.type	main,@function
main:                                   # @main
	.cfi_startproc
# %bb.0:
	pushl	%ebp
	.cfi_def_cfa_offset 8
	.cfi_offset %ebp, -8
	movl	%esp, %ebp
	.cfi_def_cfa_register %ebp
	movl	a, %eax
	movl	b, %ecx
	movl	%eax, %edx
	imull	%ecx, %edx
	jno	.LBB0_2
# %bb.1:
	pushl	%ecx
	pushl	%eax
	pushl	$.L.str
	calll	printf
	addl	$12, %esp
	jmp	.LBB0_3
.LBB0_2:
	pushl	%edx
	pushl	%ecx
	pushl	%eax
	pushl	$.L.str.1
	calll	printf
	addl	$16, %esp
.LBB0_3:
	xorl	%eax, %eax
	popl	%ebp
	.cfi_def_cfa %esp, 4
	retl
.Lfunc_end0:
	.size	main, .Lfunc_end0-main
	.cfi_endproc
                                        # -- End function
	.type	a,@object                       # @a
	.data
	.globl	a
	.p2align	2, 0x0
a:
	.long	500000                          # 0x7a120
	.size	a, 4

	.type	b,@object                       # @b
	.globl	b
	.p2align	2, 0x0
b:
	.long	600000                          # 0x927c0
	.size	b, 4

	.type	.L.str,@object                  # @.str
	.section	.rodata.str1.1,"aMS",@progbits,1
.L.str:
	.asciz	"%ld * %ld = overflow\n"
	.size	.L.str, 22

	.type	.L.str.1,@object                # @.str.1
.L.str.1:
	.asciz	"%ld * %ld = %ld\n"
	.size	.L.str.1, 17

	.ident	"Homebrew clang version 19.1.3"
	.section	".note.GNU-stack","",@progbits
	.addrsig
