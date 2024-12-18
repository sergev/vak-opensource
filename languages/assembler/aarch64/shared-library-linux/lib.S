#include <sys/syscall.h>

        .text
        .globl  say
say:
        mov     x1, x0                  // message
        mov     x0, #1                  // file descriptor 1 - stdout
        mov     x2, 3                   // size
        mov     x8, #SYS_write          // syscall write(fd, message, size)
        svc     #0                      // do system call

        adrp    x1, newline             // high 20 bits of message address
        add     x1, x1, :lo12:newline   // low 12 bits of message address
        mov     x0, #1                  // file descriptor 1 - stdout
        mov     x2, 1                   // size
        mov     x8, #SYS_write          // syscall write(fd, message, size)
        svc     #0                      // do system call

        ret

newline:
        .ascii  "\n"
