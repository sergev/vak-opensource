;
; Copyright (C) 2005 Serge Vakulenko, <vak@cronyx.ru>
;
; This file is distributed in the hope that it will be useful, but WITHOUT
; ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
; FITNESS FOR A PARTICULAR PURPOSE.
;
; You can redistribute this file and/or modify it under the terms of the GNU
; General Public License (GPL) as published by the Free Software Foundation;
; either version 2 of the License, or (at your discretion) any later version.
; See the accompanying file "COPYING" for more details.
;
#define _(x) x
#define JMP(label) rjmp _(label)

	.section	.init
	.func	.start_of_init
.start_of_init:
	JMP(_init_)
	JMP(_interrupt0_)
	JMP(_interrupt1_)
	JMP(_input_capture1_)
	JMP(_output_compare1_)
	JMP(_overflow1_)
	JMP(_overflow0_)
	JMP(_uart_recv_)
	JMP(_uart_data_)
	JMP(_uart_trans_)
	JMP(_comparator_)

_(_init_):
	;
	; Copy the .data image from flash to ram.
	; Linker places it at the end of .text segment.
	; Register usage:
	; X (r26:27) - destination pointer (ram)
	; Y (r28:29) - byte counter (size of .data segment)
	; Z (r30:31:RAMPZ) - source pointer (flash)
	;
	ldi	r28, lo8 (_edata-0x60)	; load size of .data image
	ldi	r29, hi8 (_edata-0x60)
	sbiw	r28,0			; if (counter == 0)...
	breq	.clear_bss		; ...skip loop

	ldi	r26, 0x60		; load start of ram
	ldi	r27, 0

	ldi	r30, lo8 (_etext)	; load the end of .text segment
	ldi	r31, hi8 (_etext)	; this is a start of .data image

	;
	; Loop #1: source in lower flash memory.
	;
.copy_data:
	lpm				; load byte by Z pointer into r0
	st	X+, r0			; store byte by X++
	sbiw	r28, 1			; decrement counter
	breq	.clear_bss		; if (counter == 0) done
	adiw	r30, 1			; increment Z pointer
	rjmp	.copy_data		; continue

.clear_bss:
	;; avr-ld always put .bss after .data
	clr	r1
	ldi	r26, lo8(__bss_start)
	ldi	r27, hi8(__bss_start)
	ldi	r16, hi8(__bss_end)
	rjmp	1f
2:
	st	X+,r1
1:	cpi	r26, lo8(__bss_end)
	cpc	r27, r16
	brne	2b

	JMP(main)
.end_of_init:
	.endfunc

.section .text
	.func	.weaks		; stay avr-as happy with -gstabs
.weaks:
	.weak	_(_interrupt0_)
	.weak	_(_interrupt1_)
	.weak	_(_input_capture1_)
	.weak	_(_output_compare1_)
	.weak	_(_overflow1_)
	.weak	_(_overflow0_)
	.weak	_(_uart_recv_)
	.weak	_(_uart_data_)
	.weak	_(_uart_trans_)
	.weak	_(_comparator_)

_(_interrupt0_):
_(_interrupt1_):
_(_input_capture1_):
_(_output_compare1_):
_(_overflow1_):
_(_overflow0_):
_(_uart_recv_):
_(_uart_data_):
_(_uart_trans_):
_(_comparator_):
	reti
