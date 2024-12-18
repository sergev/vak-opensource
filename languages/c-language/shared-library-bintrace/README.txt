This demo shows how a dynamic library is linked at run time.
All of this is for MacOS with arm64 processor.

File foobar.c contains a program which calls a routine say()
from dynamic library, twice:

    int main()
    {
        say("foo\n");
        say("bar\n");
    }

The library is created from file lib.c:

    void say(const char *message)
    {
        asm (
            "mov     x1, %0 \n" // message
            "mov     x0, #1 \n" // stdout
            "mov     x2, 4 \n"  // length
            "mov     x16, #4\n" // SYS_write
            "svc     0x80"
        : : "r"(message));
    }

Disassembled code is present in files:
 *  foobar.dis
 *  lib.so.dis

Execution trace was obtained by bintrace utility.
See it in file foobar-dynamic-lib.trace, with the startup code
of dynamic loader removed.
