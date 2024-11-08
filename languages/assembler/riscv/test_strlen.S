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
