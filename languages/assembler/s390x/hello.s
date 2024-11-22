        .machinemode zarch
        .machine "z196"

        .section .rodata
        .align  2
.LC0:
        .string "Hello, World!"

        .text
        .align  8
        .globl  main
        .type   main, @function
main:
        stmg    %r14, %r15, 112(%r15)
        larl    %r2, .LC0
        lay     %r15, -160(%r15)
        brasl   %r14, puts@PLT
        lghi    %r2, 0
        lmg     %r14, %r15, 272(%r15)
        br      %r14
