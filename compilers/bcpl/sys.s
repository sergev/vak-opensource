# Copyright (c) 2004 Robert Nordier.  All rights reserved.

# BCPL V7/x86 system interface.

.globl	exit
exit:
	mov	$1,%eax
	int	$0x30

.globl	read
read:
	mov	$3,%eax
	int	$0x30
	jc	error
	ret

.globl	write
write:
	mov	$4,%eax
	int	$0x30
	jc	error
      ret

.globl	open
open:
	mov	$5,%eax
	int	$0x30
	jc	error
	ret

.globl	close
close:
	mov	$6,%eax
	int	$0x30
	jc	error
	ret

.globl	creat
creat:
	mov	$8,%eax
	int	$0x30
	jc	error
	ret

.globl	lseek
lseek:
	mov	$0x13,%eax
	int	$0x30
	jc	error
	ret

.globl	sbrk
sbrk:
	mov	4(%esp),%ecx
	mov	curbrk,%eax
	add	%eax,4(%esp)
	mov	$17,%eax
	int	$0x30
	jc	error
	mov	curbrk,%eax
	add	%ecx,curbrk
	ret

.globl	ioctl
ioctl:
	mov	$0x36,%eax
	int	$0x30
	jc	error
	ret

error:
	mov	%eax,errno
	mov	$-1,%eax
	ret

.set	TIOCGETP,'t'<<8|8

.globl	isatty
isatty:
	mov	4(%esp),%eax
	sub	$8,%esp
	push	%esp
	push	$TIOCGETP
	push	%eax
	call	ioctl
	mov	$0,%eax
	jc	1f
	inc	%eax
1:
	add	$8+12,%esp
	ret

        .data
curbrk: .long _end
errno:	.long 0
