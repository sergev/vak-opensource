#include <sys/syscall.h>
#
# Print integer value as unsigned decimal number.
#
# void print_uns(unsigned value)
# {
#      char buf[32];
#      char *end = buf + sizeof(buf);
#      char *ptr = end;
#      unsigned base = 10;
#
#      do {
#          *--ptr = (value % base) + '0';
#          value /= base;
#      } while (value != 0);
#
#      write(1, ptr, end - ptr);
# }
        .text
        .align  2
        .globl  print_uns
        .type   print_uns, @function
print_uns:
        mv      a2, sp          # end
        addi    sp, sp, -32     # allocate buf
        mv      a1, a2          # ptr
        li      a3, 10          # base
1:
        remu    a5, a0, a3      # value % base
        addi    a1, a1, -1      # --ptr
        addi    a5, a5, 48      # + '0'
        sb      a5, 0(a1)       # *ptr = character
        mv      a5, a0          # old value
        divu    a0, a0, a3      # value /= base
        bgeu    a5, a3, 1b      # if (old value >= base) continue

        sub     a2, a2, a1      # end - ptr
        li      a0, 1           # stdout
        li      a7, SYS_write   # write() system call
        ecall

        addi    sp, sp, 32
        ret
