# double cubic_root(double n)
# {
#   double x = 1.0;
#   for (;;) {
#       double nx = (n/x/x + x + x) / 3.0;
#       if (nx == x) {
#           return x;
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
loop:
        fdiv.d  fa5, fa3, fa0       # n / x
        fmv.d   fa4, fa0            # old x in register fa4
        fdiv.d  fa5, fa5, fa0       # n / x / x
        fadd.d  fa5, fa5, fa0       # + x
        fadd.d  fa5, fa5, fa0       # + x
        fdiv.d  fa0, fa5, fa2       # divide by 3.0
        feq.d   a5, fa4, fa0        # nx == x?
        beqz    a5, loop            # otherwise to next iteration
        ret

one:    .double 1.0
three:  .double 3.0

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
        call    _exit               # terminate

input:  .double 2.0
format: .string "cubic root of %.15g is %.15g\n"
