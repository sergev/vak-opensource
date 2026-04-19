; Output "Hello world!" to standard output.
; For x86-64 architecture with NASM assembler, using macOS syscalls.
;
; Assemble with :
; nasm -fmacho64 -o hello.o hello.x86-64.macos.syscall.nasm.asm
; clang -nostdlib -static -Wl,-e,_start -o hello hello.o

global _start
section .text

_start:
        mov rax, 0x2000004      ; syscall write()
        mov rdi, 1              ; fd - stdout
        lea rsi, [rel message]  ; pointer to message
        mov rdx, 13             ; message size
        syscall                 ; do syscall

        mov rax, 0x2000001      ; syscall exit()
        xor edi, edi            ; exit code - 0
        syscall                 ; do syscall

section .rodata
message:
        db "Hello world!", 10
