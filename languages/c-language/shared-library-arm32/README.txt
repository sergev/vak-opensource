This demo shows how a dynamic library is linked at run time,
on FreeBSD with arm32 processor.

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
            "mov     r1, %0 \n" // message
            "mov     r0, #1 \n" // stdout
            "mov     r2, 4 \n"  // length
            "mov     r7, #4\n"  // SYS_write
            "svc     #0"
        : : "r"(message));
    }

Disassembled routine main() is present in file foobar.dis:

    0001055c <main>:
       1055c:   e92d4800    push    {fp, lr}
       10560:   e1a0b00d    mov     fp, sp
       10564:   e30005a4    movw    r0, #1444           @ 0x5a4
       10568:   e3400001    movt    r0, #1
       1056c:   ebffffac    bl      10424 <say@plt>
       10570:   e30005a9    movw    r0, #1449           @ 0x5a9
       10574:   e3400001    movt    r0, #1
       10578:   ebffffa9    bl      10424 <say@plt>
       1057c:   e3a00000    mov     r0, #0
       10580:   e8bd8800    pop     {fp, pc}

It calls stub:

    00010424 <say@plt>:
       10424:   e28fc600    add     ip, pc, #0, 12
       10428:   e28cca01    add     ip, ip, #4096       @ 0x1000
       1042c:   e5bcf3e8    ldr     pc, [ip, #1000]!    @ 0x3e8
