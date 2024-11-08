#include <sys/syscall.h>

        .section .text
        .align  2
strlen:                         // a0 = const char *str
        li      t0, 0           // i = 0
1:                              // Start loop
        add     t1, t0, a0      // Add the byte offset for str[i]
        lb      t1, 0(t1)       // Dereference str[i]
        beqz    t1, 2f          // if str[i] == 0, break for loop
        addi    t0, t0, 1       // Add 1 to our iterator
        j       1b              // Jump back to condition (1 backwards)
2:                              // End loop
        mv      a0, t0          // Move t0 into a0 to return
        ret                     // Return back via the return address register

        .globl  _start
_start:
        la      a0, text
        call    strlen
        call    print_uns
        call    print_newline

        li      a7, SYS_exit    // exit the program
        li      a0, 0           // status code
        ecall

        .section .data
        .align  3
text:   .string "Hello, world\n"
