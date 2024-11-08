#include <sys/syscall.h>
//
// Print given string.
//
        .text
        .align  2
        .globl  print_string
        .type   print_string, @function
print_string:
        addi    sp, sp, -16     // allocate space in stack
        sd      ra, 0(sp)       // save return address
        sd      a0, 8(sp)       // save string pointer

        call    strlen
        mv      a2, a0          // byte count
        ld      a1, 8(sp)       // restore string pointer
        li      a0, 1           // stdout
        li      a7, SYS_write   // write() system call
        ecall

        ld      ra, 0(sp)       // restore return address
        addi    sp, sp, 16
        ret
