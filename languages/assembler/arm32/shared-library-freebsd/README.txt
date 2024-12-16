This demo shows how a dynamic library is linked at run time.
All of this is for FreeBSD with arm32 processor.

File foobar.S contains a program which calls a routine say()
from dynamic library, twice:

    _start:
            movw    r0, #:lower16:foo
            movt    r0, #:upper16:foo
            bl      say                     // say("foo")

            movw    r0, #:lower16:bar
            movt    r0, #:upper16:bar
            bl      say                     // say("bar")

            mov     r7, #SYS_exit
            mov     r0, #0
            svc     #0                      // _exit(0)

    foo:    .ascii  "foo\n"
    bar:    .ascii  "bar\n"

The library is created from file lib.S:

    say:
            stmfd   sp!, {r7, lr}           // save registers

            mov     r1, r0                  // message
            mov     r0, #1                  // file descriptor 1 - stdout
            mov     r2, #4                  // character count
            mov     r7, #SYS_write          // syscall 4 - write
            svc     #0                      // do system call

            ldmfd   sp!, {r7, pc}           // return

Disassembled code is present in files:
 *  foobar.dis
 *  lib.so.dis

Execution trace was obtained by bintrace utility.
See it in file foobar-dynamic-lib.trace, with the startup code
of dynamic loader removed.
