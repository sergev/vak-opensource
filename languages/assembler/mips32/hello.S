//
// Output "Hello world!" to standard output.
// For MIPS32 architecture with GNU assembler, using Linux syscalls.
//
// Assemble with:
// cpp hello.S | as -o hello.o -
// ld hello.o -o hello
//
#include <sys/syscall.h>

        .text
        .balign 4
        .globl  __start
__start:
	li      $v0, SYS_write   // syscall 4 - write
	li      $a0, 1           // file descriptor 1 - stdout
	la      $a1, message     // message address
	li      $a2, 13          // character count
	syscall

	li      $v0, SYS_exit    // syscall 1 - exit
	move    $a0, $0          // exit code 0 - success
	syscall

message:
        .ascii "Hello world!\n"
