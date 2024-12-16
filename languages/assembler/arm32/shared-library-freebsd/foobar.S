#include <sys/syscall.h>

        .text
        .globl  _start
_start:
        movw    r0, #:lower16:foo
        movt    r0, #:upper16:foo
        bl      say                     // say("foo")

        movw    r0, #:lower16:bar
        movt    r0, #:upper16:bar
        bl      say                     // say("bar")

        mov     r7, #SYS_exit           // syscall 1 - exit
        mov     r0, #0                  // exit code 0 - success
        svc     #0                      // do system call

foo:    .ascii  "foo\n"
bar:    .ascii  "bar\n"
