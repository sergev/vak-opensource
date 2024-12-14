//
// Output "Hello world!" to standard output.
// For x86-64 architecture with GNU assembler, using MacOS syscalls.
//
// Assemble with :
// cpp hello.S | as -o hello.o -
// ld -o hello hello.o
//
#include <sys/syscall.h>

        .text
        .globl _main
_main:
        mov $0x2000000|SYS_write, %rax  // syscall write(fd, message, size)
        mov $1, %rdi                    // fd - stdout
        lea message(%rip), %rsi         // pointer to message
        mov $13, %rdx                   // size
        syscall                         // do syscall

        mov $0x2000000|SYS_exit, %rax   // syscall exit(status)
        xor %rdi, %rdi                  // status - 0
        syscall                         // do syscall

message:
        .ascii "Hello world!\n"
