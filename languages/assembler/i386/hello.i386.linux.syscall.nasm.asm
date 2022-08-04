; Output "Hello world!" to standard output.
; For Intel i386 architecture with NASM assembler, using Linux syscalls.
; 
; Assemble with :
; nasm -felf -o hello.o hello.i386.linux.syscall.nasm.asm &&
; ld -melf_i386 -o hello hello.o

global _start
section .text

_start:
	mov eax, 4        ; syscall 4 - write
	mov ebx, 1        ; fd - stdout
	mov ecx, message  ; pointer to message
	mov edx, 13       ; size
	int 0x80          ; do syscall

	mov eax, 1        ; syscall 1 - exit
	xor ebx, ebx      ; exit code - 0
	int 0x80          ; do syscall

section .rodata
message:
	db "Hello world!", 0xa
