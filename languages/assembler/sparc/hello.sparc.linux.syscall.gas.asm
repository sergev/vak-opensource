/*
SPARC (32-bit):
as --32 -o hello.o hello.sparc.linux.syscall.gas.asm &&
ld -melf32_sparc -o hello hello.o

SPARC (64-bit):
as --64 -o hello.o hello.sparc.linux.syscall.gas.asm &&
ld -melf64_sparc -o hello hello.o
*/

.section .rodata
.balign 8

message:
.asciz	"hello world\n"

.section .text
.balign 8
.global _start
_start:

mov 1,%o0
sethi %hi(message), %g1
or %g1, %lo(message), %o1
mov 4,%g1
mov 13,%o2
t 0x6d
nop

mov 1,%g1
mov 42,%o0
t 0x6d
nop

