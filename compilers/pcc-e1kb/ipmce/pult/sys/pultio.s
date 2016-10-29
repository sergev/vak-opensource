	title   pultio
	.286p

B       =       378h            ; parallel port 1 base address

PTIO_TEXT segment byte public 'CODE'
	assume  cs: PTIO_TEXT

GETBIT  macro   bit
	local   zero
	mov     dx, B+1
	in      al, dx
	test    al, 20h
	jne     zero
	or      si, bit
zero    label   near
	endm

STROBE  macro
	mov     dx, B+2
	mov     al, 5
	out     dx, al
	mov     al, 4
	out     dx, al
	endm

DATA    macro
	mov     dx, B+0
	out     dx, al
	STROBE
	endm

	public __ptwr
__ptwr  proc far                ; _ptwr (index, cmd)
	enter   word ptr 2, 0
	push    dx

	xor     al, al
	mov     dx, B+2
	out     dx, al          ; outb (0x378+2, 0);
	mov     al, 4
	out     dx, al          ; outb (0x378+2, 4);

	mov     al, [bp+6]      ; index
	not     al
	DATA

	mov     al, 10h
	DATA

	mov     al, [bp+8]      ; cmd
	not     al
	DATA

	pop     dx
	leave
	ret	
__ptwr  endp

	public __ptdn
__ptdn  proc far                ; _ptdn (index, cmd)
	enter   word ptr 2, 0
	push    dx

	xor     al, al
	mov     dx, B+2
	out     dx, al          ; outb (0x378+2, 0);
	mov     al, 4
	out     dx, al          ; outb (0x378+2, 4);

	mov     al, [bp+6]      ; index
	not     al
	DATA

	mov     al, [bp+8]      ; cmd
	not     al
	DATA

	pop     dx
	leave
	ret	
__ptdn  endp

	public  __ptrd
__ptrd  proc far
	enter   word ptr 4, 0
	push    dx
	push	si

	xor     si,si
	GETBIT  01h
	STROBE
	GETBIT  02h
	STROBE
	GETBIT  04h
	STROBE
	GETBIT  08h
	STROBE
	GETBIT  10h
	STROBE
	GETBIT  20h
	STROBE
	GETBIT  40h
	STROBE
	GETBIT  80h
	mov	ax,si

	pop	si
	pop     dx
	leave	
	ret	
__ptrd  endp

	public  __ptevn
__ptevn proc far
	enter   word ptr 2,0
	push    bx

	xor     ax,ax
	mov     bx,[bp+8]
	xor     bx,[bp+6]
	xor	bl,bh
	jpo     odd
	mov     al,1
odd:
	pop     bx
	leave
	ret	
__ptevn endp

PTIO_TEXT ends
	end
