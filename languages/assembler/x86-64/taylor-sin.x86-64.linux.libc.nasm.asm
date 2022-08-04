; Calculate the Taylor series approximation of sin(x), using values specified
; on the command line.
; 
; For x86-64 architecture with NASM assembler, using libc functions.
; 
; Assemble with :
; nasm -f elf64 -o taylor-sin.o taylor-sin.x86-64.linux.libc.nasm.asm &&
; gcc -m64 -o taylor-sin taylor-sin.o
; 
; Run with :
; ./taylor-sin 0.1 5
; 
; Where '0.1' is the value to calculate the sine of, and '5' is the number of
; terms in the approximation.

bits 64

section .text

extern printf
extern atof
extern atoi
extern puts
global main

main:
	push  rbp                  ; setup stack frame
	mov   rbp, rsp
	sub   rsp, 16              ; create space for locals

	; check if we have 2 command-line args
	cmp   rdi, 3
	jnz   error

	mov   [rsp], rsi           ; save copy of argv
	
	mov   rdi, [rsi+8]         ; get argv[1] - value to calc sine of
	call  atof                 ; convert string to double precision float
	movsd [rsp+8], xmm0        ; save parsed value on stack
	
	mov   rsi, [rsp]           ; restore argv from stack
	mov   rdi, [rsi+16]        ; get argv[2] - number of taylor series terms
	call  atoi                 ; convert string to integer
	
	movsx rdi, eax             ; get result into first parameter
	movsd xmm0, [rsp+8]        ; get previously parsed value from stack
	call  taylor_sin           ; do the taylor series approximation
	                           ; result returned in xmm0

	; output result
	mov   rdi, format
	mov   eax, 1               ; number of vector args (1 - xmm0)
	call  printf

	; return to libc
	xor   eax, eax             ; exit code 0 - success
	jmp   done

error:
	lea   rdi, [error_message]
	call  puts
	mov   eax, 1               ; exit code 1 - failure

done:
	add   rsp, 16              ; restore stack frame
	pop   rbp
	ret                        ; return to libc


taylor_sin:
; inputs:
;   xmm0(scalar double) : value to calculate sine of (x)
;   rdi                 : number of terms
; 
; outputs:
;   xmm0(scalar double) : sin(x)
; 
; used:
;   xmm0 s  = sum of all terms (final result)
;   xmm1 1  = constant 1
;   xmm2 fi = factorial index (1, 3, 5, 7, ...)
;   xmm3 fc = factorial current (1, 6, 120, 5040, ...)
;   xmm4 n  = numerator of term (x, x^3, x^5, x^7, ...)
;   xmm5 t  = temporary term
;   xmm6 x  = input value

	movsd xmm6, xmm0           ; copy x
	xorpd xmm0, xmm0           ; start with zero sum
	movsd xmm1, [d_const_1]    ; constant 1.0

	movsd xmm2, xmm1           ; start with factorial index 1
	movsd xmm3, xmm1           ; start with factorial 1
	movsd xmm4, xmm6           ; start with term x

; first term
	movsd xmm5, xmm4           ; x^n / n! -> xmm5
	divsd xmm5, xmm3
	addsd xmm0, xmm5           ; accumulate result into f0

	mov   rcx, rdi             ; number of terms
	xor   rax, rax             ; zero add/sub counter

taylor_loop:                 ; loop through each term
	addsd xmm2, xmm1           ; calculate next odd factorial
	mulsd xmm3, xmm2
	addsd xmm2, xmm1
	mulsd xmm3, xmm2

	mulsd xmm4, xmm6           ; calculate next odd power of x
	mulsd xmm4, xmm6

	movsd xmm5, xmm4           ; divide power by factorial
	divsd xmm5, xmm3

	test  rax, 1              
	jz    taylor_odd
	addsd xmm0, xmm5           ; add even terms
	jmp   taylor_next
taylor_odd:
	subsd xmm0, xmm5           ; subtract odd terms
taylor_next:

	inc   rax                  ; increase term index
	dec   rcx                  ; decrease loop counter
	jnz   taylor_loop          ; more terms left?

	ret                        ; resturn with result in xmm0


section .rodata

error_message: db "syntax: <x> <terms>", 0
format:        db "%0.10f", 10, 0
test_message:  db "[%g]", 10, 0

align 8
d_const_1:     dq 1.0
