#include <sys/syscall.h>
//
// Calculate the length of a string.
//
// unsigned strlen(const char *str);
//
        .section .text
        .align  2
        .globl  strlen
        .type   strlen, @function
strlen:                         // a0 = const char *str
        addi    t1, a0, 1       // ptr + 1
1:
        lb      t0, 0(a0)       // get byte from string
        addi    a0, a0, 1       // increment pointer
        bnez    t0, 1b          // continue if not end

        sub     a0, a0, t1      // compute length - 1 for '\0' char
        ret
