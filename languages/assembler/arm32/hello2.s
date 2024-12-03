//
// Output "Hello world!" to standard output.
// For ARM architecture with GNU assembler, using Linux with libc functions.
//
// Assemble with :
// as -o hello.o hello.arm.linux.libc.gas.asm &&
// gcc -o hello hello.o
//
        .text
        .balign 4
        .globl  main
main:
	stmfd   sp!, {r3, lr}
	ldr     r0, message_ptr
	bl      puts
	mov     r0, #0
	ldmfd   sp!, {r3, pc}

message_ptr:
        .long   message

        .section .rodata
        .balign 4
message:
        .asciz "Hello world!"
