	.text
	.globl	main
	.type	main, @function
main:
	lla	a5, .LANCHOR0
	ld	a2, 0(a5)       # a2 = a
	ld	a3, 8(a5)       # a3 = b

	add	a4, a2, a3          # a4 = a + b
	slti	a1, a3, 0           # a1 = (b < 0)
	slt	a5, a4, a2          # a5 = (a + b < a)
	bne	a1, a5, overflow    # (b < 0) != (a + b < a) ? overflow

	lla	a1, .LC1        # valid result
	li	a0, 2
	call	__printf_chk
.L5:
	li	a0, 0           # finish
	call	_exit
overflow:
	lla	a1, .LC0        # overflow
	li	a0, 2
	call	__printf_chk
	j	.L5

	.align	3
.LC0:
	.string	"%ld + %ld = overflow\n"
	.align	3
.LC1:
	.string	"%ld + %ld = %ld\n"

	.data
	.globl	b
	.globl	a
	.align	3
	.set	.LANCHOR0, .
	.type	a, @object
	.size	a, 8
a:
	.dword	5000000000000000000
	.type	b, @object
	.size	b, 8
b:
	.dword	6000000000000000000
