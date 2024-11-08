#include <sys/syscall.h>
#
# Print newline.
#
# void print_newline(void);
#
        .text
        .align  2
        .globl  print_newline
        .type   print_newline, @function
print_newline:
        li      a7, SYS_write   # write() system call
        li      a0, 1           # stdout
        la      a1, newline     # string
        li      a2, 1           # one character
        ecall
        ret
newline:
        .string "\n"
