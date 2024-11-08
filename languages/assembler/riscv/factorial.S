#include <sys/syscall.h>
#
# Print factorial of 20.
# Expect:
# 20! = 2432902008176640000
#
        .text
        .align  2
        .type   factorial, @function
factorial:
        mv      t0, a0      # Copy register a0 to t0. Use a0 as running total, t0 as counter
1:
        addi    t0, t0, -1  # Decrement counter in t0
        beqz    t0, 2f      # If result of comparison is zero, branch to return
        mul     a0, a0, t0  # Multiply running total and counter (a0*t0), then store in a0
        j       1b          # Jump to another iteration; keep going until the counter is 0
2:
        ret                 # Return the running total, which is already in a0

        .globl  _start
_start:
        ld      a0, input
        call    print_uns

        la      a0, text
        call    print_string

        ld      a0, input
        call    factorial
        call    print_uns

        call    print_newline

        li      a7, SYS_exit    # exit the program
        li      a0, 0           # status code
        ecall

        .align  3
input:  .dword  20
text:   .string "! = "
