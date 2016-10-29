; -------------------------------
; Boot block (IPL) for DiskOnChip 2000.
;
DISKSEGM	equ	0xe000

ChipID		equ	0x1000
DOCStatus	equ	0x1001
DOCControl	equ	0x1002
FloorSelect	equ	0x1003
CDSNControl	equ	0x1004
CDSNDevSelect	equ	0x1005
ECCConf 	equ	0x1006
ECCStatus	equ	0x1007

CDSNSlowIO	equ	0x100d
ECCSyndrome0	equ	0x1010
ECCSyndrome1	equ	0x1011
ECCSyndrome2	equ	0x1012
ECCSyndrome3	equ	0x1013
ECCSyndrome4	equ	0x1014
ECCSyndrome5	equ	0x1015
AliasResolution equ	0x101b
ConfigInput	equ	0x101c
ReadPipeInit 	equ	0x101d
WritePipeTerm 	equ	0x101e
LastDataRead 	equ	0x101f
NoOper 		equ	0x1020

CDSN_IO 	equ	0x1800

; BIOS header
;
%ifdef COM
		org	100h
		section .text
%else
		db	0x55,0xAA,0x20	; 16 kilobytes size
		jmp	short start
		db	0		; place for checksum
%endif

not_detected:
		mov si, disk_not_detected
		call output_message
		jmp exit

; -------------------------------
; Setup the stack and segment registers
;
start:
		retf
%ifndef COM
		mov word [bp-4],0xF000	; stop scanning for ROM-BIOS
%endif
		mov bx, ss		; get caller's FAR stack pointer
		mov cx, sp
		mov ax, 0x1000		; set stack at 1000:ffff
		mov ss, ax
		mov sp, 0xFFFF
		push cx
		push bx

		push ds
		push es

; -------------------------------
; Enable Ampro socket S0.
;
		mov si, DISKSEGM
		push si
		pop ds
		xor si,si
		test byte [si+4],0x60		; e000:4 == pusha ?
		jnz not_detected
		test byte [si+0x083C],0x61	; e000:83c == popa ?
		jnz not_detected
;%ifdef COM
		mov si, disk_detected
		call output_message
;%endif
		mov bx, CDSNControl
		mov byte [bx-2],0x85	; DOCControl
		mov byte [bx-2],0x85	; DOCControl
		mov si, CDSN_IO

		mov cl, 0xff
		call send_command

		mov cl, 0x00
		call send_command
;%ifdef COM
		push si
		mov si, disk_init
		call output_message
		pop si
;%endif
		cld
		mov cx, 0x2000		; target 2000:0000
		mov es,cx
		xor di,di
		xor ah,ah		; compute chechsum here

next_byte:	test cl,0xf
		jnz skip_address

		mov byte [bx],0x1d	; CDSNControl := FLASH_IO,WP,ALE,CE
		inc word [bx-4]		; ChipID
		mov dx,di
		mov [bx+9],dl		; [CDSNSlowIO] := low di
		mov [si],dl		; [CDSN_IO] := low di
		mov [bx+9],dh		; [CDSNSlowIO] := high di
		mov [si],dh		; [CDSN_IO] := high di
		xor dx,dx
		mov [bx+9],dl		; [CDSNSlowIO] := 0
		mov [si],dl		; [CDSN_IO] := 0
		mov byte [bx],0x19	; CDSNControl := FLASH_IO,WP,CE
		call wait_ready
skip_address:
		test [bx+9],al		; read CDSNSlowIO
		lodsb			; al :=	[CDSN_IO]
		add ah,al		; checksum
		stosb			; [2000:di++] := al
		dec si
		loop next_byte		; until (--cx == 0) - 8 kbytes

		cmp ah,0x55		; checksum == 0x55 ?
		jnz exit
;%ifdef COM
		mov si, second_boot
		call output_message
;%endif
		push	CS		; our return address
		mov	CX, exit
		push	CX
		mov	CX, DISKSEGM	; return at e000:83e, on retf command
		push	CX
		mov	CX, 0x083E
		push	CX
		mov	CX, 0x2000	; call 0x2000:0
		push	CX
		mov	CX, 0
		push	CX
		retf			; call secondary boot
exit:
;%ifdef COM
		mov si, done_msg
		call output_message
;%endif
		pop di
		pop si
		pop bp
		pop dx

		pop bx
		pop cx
		mov ss, bx
		mov sp, cx
%ifdef COM
		mov ax, 0x4c00		; exit (0)
		int 0x21
%else
		retf
%endif

; -------------------------------
; Send a flash command to the flash chip
;
send_command:
		mov byte [bx], 0x1b	; CDSNControl := FLASH_IO,WP,CLE,CE
		inc word [bx-4]		; ChipID
		mov [bx+9],cl		; send command to CDSNSlowIO
		mov [si],cl		; send command to CDSN_IO
wait_ready:
		inc word [bx-4]		; ChipID
wait_loop:	test byte [bx], 0x80	; CDSNControl & FR_B ?
		jz wait_loop
		test [bx], dx
		ret
;%ifdef COM
; -------------------------------
; Output a message
;
output_message:
		push	AX
		push	BX
		push	DS

		push	CS
		pop	DS
		xor	BX, BX
out_loop:
		cld			; reset direction flag
		lodsb			; load argument from string
		or	AL,AL		; check for zero
		jz	output_done
		mov     AH,0xE		; write tty
		int     10h		; bios video service
		jmp	short out_loop	; do until end of string
output_done:
		pop	DS
		pop	BX
		pop	AX
		ret			; return to caller

; -------------------------------
		section .data
disk_detected	db	'DiskOnChip 2000 detected.',13,10,0
disk_not_detected db	'No DiskOnChip detected.',13,10,0
disk_init	db	'DiskOnChip initialized.',13,10,0
second_boot	db	'Calling secondary boot.',13,10,0
done_msg	db	'Done.',13,10,0
;%endif
