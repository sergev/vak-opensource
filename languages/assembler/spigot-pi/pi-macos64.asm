; macOS x86_64 — NASM macho64
; assemble: nasm -f macho64 -o pi-macos64.o pi-macos64.asm
; link: clang -nostdlib -static -Wl,-e,_start -o pi-macos64 pi-macos64.o

%define D 9360  ; digits, 9360 is the limit for 64 KB ra-array
%define N (D*7/2)

default rel

global _start

section .text
_start:
        xor esi, esi
        mov ecx, N/4                ; fill r-array
        mov rax, 2000*0x1000100010001
        lea r8, [rel ra]
        lea rdi, [r8+2]
        rep stosq

        mov word [rel cv], cx
        mov word [rel kv], strict word N
.l0:    xor edi, edi                ; d <- 0
        mov si, word [rel kv]
        add esi, esi                ; i <- k*2
        mov cx, 10000               ; r[i]*10000, mul16x16
        jmp .l2

.l4:    sub edi, edx
        sub edi, eax
        shr edi, 1
.l2:    movzx eax, word [r8+rsi]    ; r[i]
        mul ecx
        add eax, edi
        mov edi, eax

        dec esi                     ; b <- 2*i-1
        div esi
        mov word [r8+rsi+1], dx     ; r[i] <- d%b
        dec esi                     ; i <- i - 1
        jne .l4

        mov eax, edi
        xor edx, edx
        div ecx
        add ax, word [rel cv]       ; c + d/10000
        mov word [rel cv], dx       ; c <- d%10000
        mov cx, ax
        call PR0000
        mov edx, 4
        lea rsi, [rel wb]
        mov rax, 0x2000004          ; SYS_write
        mov rdi, 1                  ; STDOUT
        syscall
        xor rsi, rsi
        xor ecx, ecx

        sub word [rel kv], 14       ; k <- k - 14
        jne .l0

.l5:    xor edi, edi                ; exit code 0
        mov rax, 0x2000001          ; SYS_exit
        syscall

PR0000:                             ; prints cx
        lea rdi, [rel wb]
        mov bx, 1000
        call .l0
        mov bx, 100
        call .l0
        mov bx, 10
        call .l0
        mov al, cl
.l2:    add al, '0'
        stosb
        ret

.l0:    mov al, 0ffh
.l4:    inc al
        mov bp, cx
        sub cx, bx
        jnc .l4

        mov cx, bp
        jmp .l2

section .bss
        align 8
cv:     resw 1
kv:     resw 1
wb:     resw 1                      ; 2 bytes; PR0000 writes 4 bytes into wb..wb+3 (spills into start of ra)
ra:     resw (N+1)
