#include <sys/syscall.h>

        .section .text
        .globl _start
_start:
        li  a7, SYS_write   # system call to print a string
        li  a0, 1           # stdout
        la  a1, message     # text to print
        li  a2, 14          # how many bytes
        ecall

        li  a7, SYS_exit    # exit the program
        li  a0, 0           # status code
        ecall
message:
        .string "Hello, world!\n"
