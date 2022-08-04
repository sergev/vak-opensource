; Output "Hello world!" to standard output.
; For x86-64 architecture with NASM assembler, using Linux syscalls.
;
; Assemble with :
; nasm -felf64 -o hello.o hello.x86-64.linux.syscall.nasm.asm &&
; ld -melf_x86_64 -o hello hello.o

global _start
section .text

_start:
	mov rax, 1       ; syscall 1 - write
	mov rdi, 1       ; fd - stdout
	mov rsi, message ; pointer to message
	mov rdx, 13      ; message size
	syscall          ; do syscall

	mov rax, 60      ; syscall 60 - exit
	xor rdi, rdi     ; exit code - 0
	syscall          ; do syscall

section .rodata
message:
	db "Hello world!", 0xa
