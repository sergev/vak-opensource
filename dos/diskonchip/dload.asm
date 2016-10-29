	ifndef	??version
?debug	macro
	endm
$comm	macro	name,dist,size,count
	comm	dist name:BYTE:count*size
	endm
	else
$comm	macro	name,dist,size,count
	comm	dist name[size]:BYTE:count
	endm
	endif
	?debug	S "dload.c"
	?debug	C E9C8A27C2F07646C6F61642E63
	?debug	C E90008A41415433A5C54435C494E434C5544455C737464696F2E68
	?debug	C E984930A2B13433A5C54435C494E434C5544455C646F732E68
_TEXT	segment byte public 'CODE'
_TEXT	ends
DGROUP	group	_DATA,_BSS
	assume	cs:_TEXT,ds:DGROUP
_DATA	segment word public 'DATA'
d@	label	byte
d@w	label	word
_DATA	ends
_BSS	segment word public 'BSS'
b@	label	byte
b@w	label	word
_BSS	ends
_TEXT	segment byte public 'CODE'
   ;	
   ;	unsigned char wait_ready ()
   ;	
	assume	cs:_TEXT
_wait_ready	proc	near
	push	bp
	mov	bp,sp
	push	si
   ;	
   ;	{
   ;		int count;
   ;	
   ;		ChipID++;
   ;	
	mov	ax,55296
	mov	es,ax
	inc	word ptr es:[4096]
   ;	
   ;		for (count=1000; count>0; --count)
   ;	
	mov	si,1000
	jmp	short @1@122
@1@50:
   ;	
   ;			if (CDSNControl & 0x80)	/* FR_B */
   ;	
	mov	ax,55296
	mov	es,ax
	test	byte ptr es:[4100],128
	je	short @1@98
	jmp	short @1@170
@1@98:
	dec	si
@1@122:
	or	si,si
	jg	short @1@50
   ;	
   ;				return CDSNControl;
   ;		printf ("Wait error, count = %d.\n", count);
   ;	
	push	si
	mov	ax,offset DGROUP:s@
	push	ax
	call	near ptr _printf
	pop	cx
	pop	cx
   ;	
   ;		return CDSNControl;
   ;	
@1@170:
	mov	ax,55296
	mov	es,ax
	mov	al,byte ptr es:[4100]
   ;	
   ;	}
   ;	
	pop	si
	pop	bp
	ret	
_wait_ready	endp
   ;	
   ;	void send_command (unsigned char cmd)
   ;	
	assume	cs:_TEXT
_send_command	proc	near
	push	bp
	mov	bp,sp
   ;	
   ;	{
   ;		CDSNControl = 0x1b;	/* FLASH_IO,WP,CLE,CE */
   ;	
	mov	ax,55296
	mov	es,ax
	mov	byte ptr es:[4100],27
   ;	
   ;		ChipID++;
   ;	
	mov	es,ax
	inc	word ptr es:[4096]
   ;	
   ;		CDSNSlowIO = cmd;
   ;	
	mov	dl,byte ptr [bp+4]
	mov	es,ax
	mov	byte ptr es:[4109],dl
   ;	
   ;		CDSN_IO = cmd;
   ;	
	mov	es,ax
	mov	byte ptr es:[6144],dl
   ;	
   ;	
   ;		wait_ready ();
   ;	
	call	near ptr _wait_ready
   ;	
   ;	}
   ;	
	pop	bp
	ret	
_send_command	endp
   ;	
   ;	void dump (char *filename, char far *data, int len)
   ;	
	assume	cs:_TEXT
_dump	proc	near
	push	bp
	mov	bp,sp
	push	si
	push	di
	mov	di,word ptr [bp+10]
   ;	
   ;	{
   ;		FILE *fd;
   ;	
   ;		fd = fopen (filename, "wb");
   ;	
	mov	ax,offset DGROUP:s@+25
	push	ax
	push	word ptr [bp+4]
	call	near ptr _fopen
	pop	cx
	pop	cx
	mov	si,ax
   ;	
   ;		if (! fd) {
   ;	
	or	ax,ax
	jne	short @3@74
   ;	
   ;			fprintf (stderr, "%s: cannot create\n", filename);
   ;	
	push	word ptr [bp+4]
	mov	ax,offset DGROUP:s@+28
	push	ax
	mov	ax,offset DGROUP:__streams+32
	push	ax
	call	near ptr _fprintf
	add	sp,6
   ;	
   ;			return;
   ;	
	jmp	short @3@170
@3@74:
   ;	
   ;		}
   ;		printf ("Dump %d bytes to %s.\n", len, filename);
   ;	
	push	word ptr [bp+4]
	push	di
	mov	ax,offset DGROUP:s@+47
	push	ax
	call	near ptr _printf
	add	sp,6
	jmp	short @3@122
@3@98:
   ;	
   ;		while (len-- > 0)
   ;			fputc (*data++, fd);
   ;	
	push	si
	les	bx,dword ptr [bp+6]
	inc	word ptr [bp+6]
	mov	al,byte ptr es:[bx]
	cbw	
	push	ax
	call	near ptr _fputc
	pop	cx
	pop	cx
@3@122:
	mov	ax,di
	dec	di
	or	ax,ax
	jg	short @3@98
   ;	
   ;	
   ;		fclose (fd);
   ;	
	push	si
	call	near ptr _fclose
	pop	cx
@3@170:
   ;	
   ;	}
   ;	
	pop	di
	pop	si
	pop	bp
	ret	
_dump	endp
   ;	
   ;	int main ()
   ;	
	assume	cs:_TEXT
_main	proc	near
	push	bp
	mov	bp,sp
	sub	sp,4
	push	si
   ;	
   ;	{
   ;		unsigned short dst, len;
   ;		unsigned char sum, c;
   ;	
   ;		printf ("DiskOnChip loader.\n");
   ;	
	mov	ax,offset DGROUP:s@+69
	push	ax
	call	near ptr _printf
	pop	cx
   ;	
   ;	
   ;		DOCControl = 0x85;
   ;	
	mov	ax,55296
	mov	es,ax
	mov	byte ptr es:[4098],133
   ;	
   ;		DOCControl = 0x85;
   ;	
	mov	es,ax
	mov	byte ptr es:[4098],133
   ;	
   ;		send_command (0xff);
   ;	
	mov	al,255
	push	ax
	call	near ptr _send_command
	pop	cx
   ;	
   ;		send_command (0);
   ;	
	mov	al,0
	push	ax
	call	near ptr _send_command
	pop	cx
   ;	
   ;	
   ;		printf ("Loading %d bytes to 0x%04x:0000.\n", NBYTES, TARGET);
   ;	
	mov	ax,32768
	push	ax
	mov	ax,8192
	push	ax
	mov	ax,offset DGROUP:s@+89
	push	ax
	call	near ptr _printf
	add	sp,6
   ;	
   ;		printf ("Disk initialized.\n");
   ;	
	mov	ax,offset DGROUP:s@+123
	push	ax
	call	near ptr _printf
	pop	cx
   ;	
   ;		dst = 0;
   ;	
	mov	word ptr [bp-2],0
   ;	
   ;		sum = 0;
   ;	
	mov	byte ptr [bp-3],0
   ;	
   ;		len = NBYTES;
   ;	
	mov	si,8192
   ;	
   ;		for (dst=0; dst<NBYTES; ++dst, --len) {
   ;	
	mov	word ptr [bp-2],0
	jmp	@4@146
@4@50:
   ;	
   ;			if ((len & 0x0f) == 0) {
   ;	
	test	si,15
	jne	short @4@98
   ;	
   ;				printf (".");
   ;	
	mov	ax,offset DGROUP:s@+142
	push	ax
	call	near ptr _printf
	pop	cx
   ;	
   ;				fflush (stdout);
   ;	
	mov	ax,offset DGROUP:__streams+16
	push	ax
	call	near ptr _fflush
	pop	cx
   ;	
   ;	
   ;				CDSNControl = 0x1d;	/* FLASH_IO,WP,ALE,CE */
   ;	
	mov	ax,55296
	mov	es,ax
	mov	byte ptr es:[4100],29
   ;	
   ;				ChipID++;
   ;	
	mov	es,ax
	inc	word ptr es:[4096]
   ;	
   ;				CDSNSlowIO = (unsigned char) dst;
   ;	
	mov	dl,byte ptr [bp-2]
	mov	es,ax
	mov	byte ptr es:[4109],dl
   ;	
   ;				CDSN_IO = (unsigned char) dst;
   ;	
	mov	es,ax
	mov	byte ptr es:[6144],dl
   ;	
   ;				CDSNSlowIO = (unsigned char) (dst >> 8);
   ;	
	mov	ax,word ptr [bp-2]
	mov	cl,8
	shr	ax,cl
	mov	dx,55296
	mov	es,dx
	mov	byte ptr es:[4109],al
   ;	
   ;				CDSN_IO = (unsigned char) (dst >> 8);
   ;	
	mov	ax,word ptr [bp-2]
	shr	ax,cl
	mov	es,dx
	mov	byte ptr es:[6144],al
   ;	
   ;				CDSNSlowIO = 0;
   ;	
	mov	ax,55296
	mov	es,ax
	mov	byte ptr es:[4109],0
   ;	
   ;				CDSN_IO = 0;
   ;	
	mov	es,ax
	mov	byte ptr es:[6144],0
   ;	
   ;				CDSNControl = 0x19;	/* FLASH_IO,WP,CE */
   ;	
	mov	es,ax
	mov	byte ptr es:[4100],25
   ;	
   ;	
   ;				wait_ready ();
   ;	
	call	near ptr _wait_ready
@4@98:
   ;	
   ;			}
   ;			c = CDSNSlowIO;
   ;	
	mov	ax,55296
	mov	es,ax
	mov	al,byte ptr es:[4109]
	mov	byte ptr [bp-4],al
   ;	
   ;			c = CDSN_IO;
   ;	
	mov	ax,55296
	mov	es,ax
	mov	al,byte ptr es:[6144]
	mov	byte ptr [bp-4],al
   ;	
   ;			sum += c;
   ;	
	add	byte ptr [bp-3],al
   ;	
   ;			*(char far*) MK_FP (TARGET, dst) = c;
   ;	
	mov	ax,32768
	mov	bx,word ptr [bp-2]
	mov	dl,byte ptr [bp-4]
	mov	es,ax
	mov	byte ptr es:[bx],dl
	mov	ax,word ptr [bp-2]
	inc	ax
	mov	word ptr [bp-2],ax
	mov	ax,si
	dec	ax
	mov	si,ax
@4@146:
	cmp	word ptr [bp-2],8192
	jae	@@0
	jmp	@4@50
@@0:
   ;	
   ;		}
   ;		printf (" done\n", dst);
   ;	
	push	word ptr [bp-2]
	mov	ax,offset DGROUP:s@+144
	push	ax
	call	near ptr _printf
	pop	cx
	pop	cx
   ;	
   ;		if (sum != 0x55) {
   ;	
	cmp	byte ptr [bp-3],85
	je	short @4@218
   ;	
   ;			printf ("Checksum error: 0x%x (must be 0x55).\n", sum);
   ;	
	mov	al,byte ptr [bp-3]
	mov	ah,0
	push	ax
	mov	ax,offset DGROUP:s@+151
	push	ax
	call	near ptr _printf
	pop	cx
	pop	cx
   ;	
   ;			exit (1);
   ;	
	mov	ax,1
	push	ax
	call	near ptr _exit
	pop	cx
@4@218:
   ;	
   ;		}
   ;	/*	dump ("boot2.bin", MK_FP (TARGET, 0), NBYTES); */
   ;	
   ;		printf ("Calling secondary boot at 0x%04x:0000.\n", TARGET);
   ;	
	mov	ax,32768
	push	ax
	mov	ax,offset DGROUP:s@+189
	push	ax
	call	near ptr _printf
	pop	cx
	pop	cx
   ;	
   ;	
   ;		asm {
   ;			push	CS
   ;	
	push		CS
   ;	
   ;			mov	CX, offset back_label
   ;	
	mov		CX, offset back_label
   ;	
   ;			push	CX
   ;	
	push		CX
   ;	
   ;			mov	CX, BIOS
   ;	
	mov		CX, 0d800H
   ;	
   ;			push	CX
   ;	
	push		CX
   ;	
   ;			mov	CX, 83Eh
   ;	
	mov		CX, 83Eh
   ;	
   ;			push	CX
   ;	
	push		CX
   ;	
   ;			mov	CX, TARGET
   ;	
	mov		CX, 08000H
   ;	
   ;			push	CX
   ;	
	push		CX
   ;	
   ;			mov	CX, 0
   ;	
	mov		CX, 0
   ;	
   ;			push	CX
   ;	
	push		CX
   ;	
   ;			db	0CBh	/* retf */
   ;	
	db		0CBh	
   ;	
   ;	back_label:
   ;	
	back_label	:
   ;	
   ;		}
   ;	
   ;		printf ("Done.\n");
   ;	
	mov	ax,offset DGROUP:s@+229
	push	ax
	call	near ptr _printf
	pop	cx
   ;	
   ;		return 0;
   ;	
	xor	ax,ax
   ;	
   ;	}
   ;	
	pop	si
	mov	sp,bp
	pop	bp
	ret	
_main	endp
	?debug	C E9
_TEXT	ends
_DATA	segment word public 'DATA'
s@	label	byte
	db	'Wait error, count = %d.'
	db	10
	db	0
	db	'wb'
	db	0
	db	'%s: cannot create'
	db	10
	db	0
	db	'Dump %d bytes to %s.'
	db	10
	db	0
	db	'DiskOnChip loader.'
	db	10
	db	0
	db	'Loading %d bytes to 0x%04x:0000.'
	db	10
	db	0
	db	'Disk initialized.'
	db	10
	db	0
	db	'.'
	db	0
	db	' done'
	db	10
	db	0
	db	'Checksum error: 0x%x (must be 0x55).'
	db	10
	db	0
	db	'Calling secondary boot at 0x%04x:0000.'
	db	10
	db	0
	db	'Done.'
	db	10
	db	0
_DATA	ends
_TEXT	segment byte public 'CODE'
_TEXT	ends
	public	_send_command
	extrn	_fopen:near
	extrn	_printf:near
	extrn	_fflush:near
	public	_wait_ready
	public	_dump
	extrn	_fprintf:near
	public	_main
	extrn	_fputc:near
	extrn	__streams:word
	extrn	_exit:near
	extrn	_fclose:near
	end
