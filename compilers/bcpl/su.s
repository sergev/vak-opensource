// Copyright (c) 2004 Robert Nordier.  All rights reserved.

.set	STACKBASE,54
.set	STACKEND,55
.set	STKSIZ,0x10000
.set	STRSIZ,256

.globl	_start
.globl	finish
.globl	stop

_start:
	cld
// Allocate a maximum length string on the stack.
	mov	%esp,%ecx
	sub	$STRSIZ,%esp
	mov	%esp,%edi
	inc	%edi
// Arg count must be 2 or more, else string empty.
	cmpl	$2,(%ecx)
	jb	4f
	lea	8(%ecx),%ebx
	mov	(%ebx),%esi
	jmp	3f
// Copy args, space-separated, into the new string.
1:
	mov	$' ',%al
2:
	cmp	%ecx,%edi
	je	4f
	stosb
3:
	lodsb
	test	%al,%al
	jnz	2b
	add	$4,%ebx
	mov	(%ebx),%esi
	test	%esi,%esi
	jnz	1b
// Fill in string length.
4:
	mov	%edi,%ebx
	sub	%esp,%ebx
	dec	%ebx
	mov	%bl,(%esp)
// Clear string tail to next word boundary.
	sub	%edi,%ecx
	and	$3,%ecx
	xor	%eax,%eax
	rep;	stosb
// Runtime startup; allocate stack space.
	call	rtinit
	push	$STKSIZ
	call	sbrk
	pop	%ecx
// Define STACKBASE, STACKEND.
	mov	$G,%edi
	mov	%eax,%ebp 	# XXX
	shr	$2,%eax
	mov	%eax,STACKBASE*4(%edi)
	add	$STKSIZ>>2,%eax
	mov	%eax,STACKEND*4(%edi)
// Invoke START.
	movl	$0,(%ebp)
	movl	$finish,4(%ebp)
	mov	%esp,%eax
	shr	$2,%eax
	mov	%eax,8(%ebp)
	mov	4(%edi),%eax
	jmp	*%eax
// Runtime cleanup, exit.
finish:
	xor	%eax,%eax
stop:
	push	%eax
	call	rtexit
	call	exit
