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

        .globl  main
main:
        ld      a0, input
        call    factorial

        mv      a3, a0
        ld      a2, input
        lla     a1, format
        li      a0, 2
        call    __printf_chk

        li      a7, SYS_exit    # exit the program
        li      a0, 0           # status code
        ecall

input:  .dword  20
format: .string "%lu! = %lu\n"
