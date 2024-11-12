	.file	"mul-overflow.c"
	.option pic
	.attribute arch, "rv64i2p1_m2p0_a2p1_f2p2_d2p2_c2p0_zicsr2p0_zifencei2p0"
	.attribute unaligned_access, 0
	.attribute stack_align, 16
	.text
	.section	.rodata.str1.8,"aMS",@progbits,1
	.align	3
.LC0:
	.string	"%ld * %ld = overflow\n"
	.align	3
.LC1:
	.string	"%ld * %ld = %ld\n"
	.section	.text.startup,"ax",@progbits
	.align	1
	.globl	main
	.type	main, @function
main:
.LFB13:
	.cfi_startproc
	lla	a5,.LANCHOR0
	ld	a2,0(a5)
	ld	a3,8(a5)
	addi	sp,sp,-16
	.cfi_def_cfa_offset 16
	sd	ra,8(sp)
	.cfi_offset 1, -8

	mul	a4, a2, a3          # a4 = (a * b) bits 63:0
	mulh	a1, a2, a3          # a1 = (a * b) >> 64
	srai	a5, a4, 63          # a5 = (int64_t) (a * b) >> 63
	bne	a1, a5, overflow    # (a * b) >> 64 != (int64_t) (a * b) >> 63 ? overflow

	lla	a1,.LC0
	li	a0,2
	call	__printf_chk@plt
.L5:
	ld	ra,8(sp)
	.cfi_remember_state
	.cfi_restore 1
	li	a0,0
	addi	sp,sp,16
	.cfi_def_cfa_offset 0
	jr	ra
.L7:
	.cfi_restore_state
	lla	a1,.LC1
	li	a0,2
	call	__printf_chk@plt
	j	.L5
	.cfi_endproc
.LFE13:
	.size	main, .-main
	.globl	b
	.globl	a
	.data
	.align	3
	.set	.LANCHOR0,. + 0
	.type	a, @object
	.size	a, 8
a:
	.dword	5000000000
	.type	b, @object
	.size	b, 8
b:
	.dword	6000000000
	.ident	"GCC: (Ubuntu 14.2.0-4ubuntu2) 14.2.0"
	.section	.note.GNU-stack,"",@progbits
