# double cubic_root(double n)
# {
#   double x = 1.0; # Initial guess
#   for (;;) {
#       double nx = ((n / x / x) + x + x) / 3.0;
#       if (nx == x) {
#           return nx;
#       }
#       x = nx;
#   }
# }
        .text
        .type   cubic_root, @function
cubic_root:
        fmv.d   fa3, fa0            # n in register fa3
        fld     fa2, three, a5      # constant 3.0
        fld     fa0, one, a5        # x = 1.0, initial guess
1:
        fdiv.d  fa5, fa3, fa0       # n / x
        fmv.d   fa4, fa0            # old x in registe fa4
        fdiv.d  fa5, fa5, fa0       # n / x / x
        fadd.d  fa5, fa5, fa0       # + x
        fadd.d  fa5, fa5, fa0       # + x
        fdiv.d  fa0, fa5, fa2       # divide by 3.0
        feq.d   a5, fa4, fa0        # nx == x?
        beq     a5, zero, 1b        # otherwise to next iteration
        ret

        .align  3
one:    .quad   0x3ff0000000000000  # 1.0
three:  .quad   0x4008000000000000  # 3.0

        .text
        .globl  main
        .type   main, @function
main:
        fld     fs0, input, a5      # input value
        fmv.d   fa0, fs0
        call    cubic_root

        li      a0, 2
        lla     a1, format
        fmv.x.d a2, fs0             # input
        fmv.x.d a3, fa0             # square root
        call    __printf_chk

        li      a0, 0               # exit status
        call    _exit

        .align  3
input:  .quad   0x4000000000000000  # 2.0
format: .string "cubic root of %.15g is %.15g\n"
