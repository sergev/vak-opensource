//
// Output "Hello world!" to standard output.
// For ARM32 architecture with GNU assembler, using Linux EABI syscalls.
//
// Assemble with:
// cpp hello.S | as -o hello.o -
// ld hello.o -o hello
//
#include <sys/syscall.h>

        .text
        .balign 4
        .globl  _start
_start:
	mov     r7, #SYS_write          // syscall 4 - write
	mov     r0, #1                  // file descriptor 1 - stdout
	movw    r1, #:lower16:message   // low 16-bits of message address
	movt    r1, #:upper16:message   // high 16-bits of message address
	mov     r2, #13                 // character count
	svc     #0                      // do system call

	mov     r7, #SYS_exit           // syscall 1 - exit
	mov     r0, #0                  // exit code 0 - success
	svc     #0                      // do system call

        .section .rodata
message:
        .ascii "Hello world!\n"
