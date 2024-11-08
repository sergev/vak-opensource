#include <sys/syscall.h>

        .section .text
        .globl _start
_start:
        la      a0, message     # text to print
        call    print_string

        li      a7, SYS_exit    # exit the program
        li      a0, 0           # status code
        ecall
message:
        .string "Hello, world!\n"
