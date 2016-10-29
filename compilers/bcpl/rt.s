// Copyright (c) 2004 Robert Nordier.  All rights reserved.

.set	IFLAGS,0		# Input flags (open)
.set	OFLAGS,0666		# Output flags (creat)
.set	BUFSIZ,512		# Buffer size
.set	STRSIZ,256		# String size (maximum)
.set	FCBCNT,8		# Number of FCBs
.set	FCBSIZ,18		# FCB size

# File control block
.set	FCB.BF,0		# Buffer pointer
.set	FCB.XP,4		# Extra pointer
.set	FCB.CP,8		# Char pointer
.set	FCB.FD,12		# File descriptor
.set	FCB.FL,16		# Flags

# Flags
.set	FL.RD,1 		# Read
.set	FL.WR,2 		# Write
.set	FL.BF,4 		# XXX
.set	FL.EOF,64		# End of file
.set	FL.ERR,128		# I/O error

# File descriptors
.set	FD.STDIN,0
.set	FD.STDOUT,1

# Runtime startup.
.globl	rtinit
rtinit:
# Allocate buffer space.
	push	$FCBCNT*BUFSIZ
	call	sbrk
	pop	%ecx
# Set the buffer pointer of each FCB.
	mov	$ft,%ebx
	mov	$FCBCNT,%ecx
1:
	mov	%eax,(%ebx)
	add	$BUFSIZ,%eax
	add	$FCBSIZ,%ebx
	loop	1b
# Set up FCB for stdin.
	mov	$ft,%ebx
	mov	$FD.STDIN,%eax
	mov	$FL.RD,%dl
	call	fopen
# Set up FCB for stdout.
	mov	$ft+FCBSIZ,%ebx
	mov	$FD.STDOUT,%eax
	mov	$FL.WR,%dl
	call	fopen
	ret

# Runtime cleanup.
.globl	rtexit
rtexit:
	mov	$ft,%ebx
	mov	$FCBCNT,%ecx
# Close every open stream.
1:
	cmpb	$0,FCB.FL(%ebx)
	je	2f
	push	%ecx
	call	fclose
	pop	%ecx
2:
	add	$FCBSIZ,%ebx
	loop	1b
	ret

.globl	_findinput
_findinput:
	mov	$IFLAGS,%edx
	jmp	findio

.globl	_findoutput
_findoutput:
	mov	$OFLAGS,%edx

findio:
	push	%ebx
	push	%esi
	push	%edi
# Find a free FCB.
	mov	$ft,%ebx
	mov	$FCBCNT,%ecx
1:
	cmpb	$0,FCB.FL(%ebx)
	je	2f
	add	$FCBSIZ,%ebx
	loop	1b
1:
	xor	%eax,%eax
	jmp	1f
2:
# Set up for syscall (need null-terminated path)
	shl	$2,%eax		# XXX
	mov	%eax,%esi
	sub	$STRSIZ,%esp
	mov	%esp,%edi
	push	%edx
	push	%edi
	lodsb
	xor	%ecx,%ecx
	mov	%al,%cl
	rep;	movsb
	mov	%cl,(%edi)
# Create or open file.
	test	%edx,%edx
	jnz	2f
	call	open
	jmp	3f
2:
	call	creat
3:
	lea	8+STRSIZ(%esp),%esp
	jc	1b
# Set up FCB.
	test	%edx,%edx
	mov	$FL.RD,%dl
	jz	2f
	mov	$FL.WR,%dl
2:
	call	fopen

	mov	%ebx,%eax
1:
	pop	%edi
	pop	%esi
	pop	%ebx
	ret

# Set up FCB (file already open).
fopen:
	mov	%eax,FCB.FD(%ebx)
	mov	FCB.BF(%ebx),%eax
	mov	%eax,FCB.CP(%ebx)
	mov	%eax,FCB.XP(%ebx)
	mov	%dl,FCB.FL(%ebx)
	cmp	$FL.RD,%dl
	je	1f
	push	FCB.FD(%ebx)
	call	isatty
	pop	%ecx
	test	%eax,%eax
	jnz	1f
	addl	$BUFSIZ,FCB.XP(%ebx)
1:
	ret

.globl	_wrch
_wrch:
	push	%ebx
	mov	cos,%ebx
# Write character to buffer.
	mov	FCB.CP(%ebx),%ecx
	mov	%al,(%ecx)
	inc	%ecx
	mov	%ecx,FCB.CP(%ebx)
# Don't flush if fully buffered and still space.
	cmp	FCB.XP(%ebx),%ecx
	jb	2f
	testb	$FL.BF,FCB.FL(%ebx)	# XXX
	jne	1f
# Flush if buffer full or newline.
	sub	FCB.BF(%ebx),%ecx
	cmp	$BUFSIZ,%ecx
	je	1f
	cmp	$0xa,%al
	jne	2f
1:
	call	flush
2:
	pop	%ebx
	ret

# Flush buffer and reset char pointer.
flush:
	mov	FCB.CP(%ebx),%ecx
	mov	FCB.BF(%ebx),%edx
	mov	%edx,FCB.CP(%ebx)
	sub	%edx,%ecx
	jz	2f
1:
	push	%ecx
	push	%edx
	push	FCB.FD(%ebx)
	call	write
	lea	12(%esp),%esp
	jb	ferr
	add	%eax,%edx
	sub	%eax,%ecx
	jnz	1b
2:
	xor	%eax,%eax
	ret

# Set error flag.
ferr:
	orb	$FL.ERR,FCB.FL(%ebx)
	ret

# Flush and close a stream.
fclose:
	testb	$FL.WR,FCB.FL(%ebx)
	jz	1f
	call	flush
1:
	push	FCB.FD(%ebx)
	call	close
	pop	%ecx
	movb	$0,FCB.FL(%ebx)
	ret

.globl	_rdch
_rdch:
	mov	cis,%ebx
# Branch if unread chars.
	mov	FCB.CP(%ebx),%ecx
	cmp	FCB.XP(%ebx),%ecx
	jb	1f
# Flush output stream.
	push	%ebx
	mov	cos,%ebx
	call	flush
	pop	%ebx
# Initialise pointers.
	mov	FCB.BF(%ebx),%ecx
	mov	%ecx,FCB.CP(%ebx)
	mov	%ecx,FCB.XP(%ebx)
# Read data and set up extra pointer.
	push	$BUFSIZ
	push	%ecx
	push	FCB.FD(%ebx)
	call	read
	lea	12(%esp),%esp
	jc	ferr
	test	%eax,%eax
	jz	feof
	add	%eax,FCB.XP(%ebx)
1:
# Get already buffered character.
	xor	%eax,%eax
	mov	(%ecx),%al
	inc	%ecx
	mov	%ecx,FCB.CP(%ebx)
	ret

# Set EOF flag and return -1.
feof:
	orb	$FL.EOF,FCB.FL(%ebx)
	dec	%eax
	ret

.globl	_unrdch
_unrdch:
	mov	cis,%edx
	mov	FCB.CP(%edx),%ecx
	cmp	FCB.BF(%edx),%ecx
	je	1f
	dec	%ecx
	mov	%al,(%ecx)
	mov	%ecx,FCB.CP(%edx)
1:
	ret

.globl	_selectinput
_selectinput:
	mov	%eax,cis
	ret

.globl	_selectoutput
_selectoutput:
	mov	%eax,cos
	ret

.globl	_input
_input:
	mov	cis,%eax
	ret

.globl	_output
_output:
	mov	cos,%eax
	ret

.globl	_endread
_endread:
	mov	cis,%ebx
	call	fclose
	xor	%eax,%eax
	mov	%eax,cis
	ret

.globl	_endwrite
_endwrite:
	mov	cos,%ebx
	call	fclose
	xor	%eax,%eax
	mov	%eax,cos
	ret

.globl	_rewind
_rewind:
	mov	cis,%ebx
	xor	%ecx,%ecx
	push	%ecx
	push	%ecx
	push	FCB.FD(%ebx)
	call	lseek
	pop	%eax
	pop	%ecx
	pop	%ecx
	mov	$FL.RD,%dl
	jmp	fopen

.data
cis:	.long ft
cos:	.long ft+FCBSIZ

.comm	ft,FCBCNT*FCBSIZ
