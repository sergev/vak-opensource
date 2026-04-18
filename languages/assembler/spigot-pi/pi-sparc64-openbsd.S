//
// Spigot π — OpenBSD sparc64, GNU assembler.
//
#include <sys/syscall.h>

        .set    D, 9360
        .set    N, ((D*7)/2)

        .section .bss
        .align 8
cv:     .skip 2
kv:     .skip 2
wb:     .skip 2
ra:     .skip (2*(N+1))

        .section .text
        .globl _start

_start:
        setx    ra, %g1, %g2
        set     10000, %g3

        // Fill r[1..N] with 2000: (N/4) steps of four halfwords at ra+2
        add     %g2, 2, %l0
        set     (N/4), %l1
1:
        sethi   %hi(2000), %l2
        or      %l2, %lo(2000), %l2
        sth     %l2, [%l0+0]
        sth     %l2, [%l0+2]
        sth     %l2, [%l0+4]
        sth     %l2, [%l0+6]
        add     %l0, 8, %l0
        subcc   %l1, 1, %l1
        bne     1b
        nop

        setx    cv, %g1, %l6
        sth     %g0, [%l6]
        sethi   %hi(N), %l0
        or      %l0, %lo(N), %l0
        setx    kv, %g1, %l6
        sth     %l0, [%l6]

outer:
        mov     %g0, %l1                // d = edi

        setx    kv, %g1, %l6
        lduh    [%l6], %l0
        sll     %l0, 1, %l0            // esi = byte offset 2*k

        ba      inner_body
        nop

inner_corr:
        sub     %l1, %l5, %l1
        sub     %l1, %l4, %l1
        srl     %l1, 1, %l1

inner_body:
        add     %g2, %l0, %l3
        lduh    [%l3], %l3              // eax = r[i]

        umul    %l3, %g3, %l7
        rd      %y, %l6
        add     %l7, %l1, %l7
        clruw   %l7, %l7
        // Like x86 "mov edi,eax" after add: edi is pre-div low for .l4 / div remainder chain.
        mov     %l7, %l1

        sub     %l0, 1, %l0             // esi = b = 2*i-1

        mov     %l6, %g4
        mov     %l7, %g5
        wr      %g0, %g4, %y
        udiv    %g5, %l0, %l4           // eax = quotient

        umul    %l4, %l0, %l7
        rd      %y, %l6
        subcc   %g5, %l7, %l5
        subc    %g4, %l6, %l6

        add     %g2, %l0, %l3
        add     %l3, 1, %l3
        sth     %l5, [%l3]

        sub     %l0, 1, %l0

        subcc   %l0, %g0, %g0
        bne     inner_corr
        nop

        wr      %g0, %g0, %y
        udiv    %l1, %g3, %l4
        umul    %l4, %g3, %l7
        rd      %y, %l6
        subcc   %l1, %l7, %l5
        subc    %g0, %l6, %l6

        setx    cv, %g1, %l6
        lduh    [%l6], %l3
        add     %l4, %l3, %l4
        sth     %l5, [%l6]
        clruw   %l4, %l4

        mov     %l4, %o0
        call    pr0000
        nop

        call    write4
        nop

        setx    kv, %g1, %l6
        lduh    [%l6], %l0
        sub     %l0, 14, %l0
        sth     %l0, [%l6]

        subcc   %l0, %g0, %g0
        bne     outer
        nop

        ba      exit_prog
        nop

write4:
        // setx uses %g1 as temp; must not load SYS_write until after (see demo order).
        setx    wb, %g4, %o1
        mov     1, %o0
        mov     4, %o2
        mov     SYS_write, %g1
2:      ta      0
        retl
        nop

exit_prog:
        mov     SYS_exit, %g1
        mov     0, %o0
3:      ta      0

// PR0000: four decimal digits of %o0 (16-bit) to wb..wb+3 (same layout as x86)
// call digd overwrites %o7; save/restore link around the whole routine.
pr0000:
        mov     %o7, %g6
        setx    wb, %g1, %o1
        mov     1000, %o2
        call    digd
        nop
        mov     100, %o2
        call    digd
        nop
        mov     10, %o2
        call    digd
        nop
        and     %o0, 0xff, %o3
        add     %o3, '0', %o3
        stb     %o3, [%o1]
        mov     %g6, %o7
        retl
        nop

digd:
        clruw   %o0, %o0
        wr      %g0, %g0, %y
        udiv    %o0, %o2, %o4
        umul    %o4, %o2, %l7
        subcc   %o0, %l7, %o0
        clruw   %o0, %o0
        add     %o4, '0', %o4
        stb     %o4, [%o1]
        add     %o1, 1, %o1
        retl
        nop

        .section ".note.openbsd.ident", "a"
        .p2align 2
        .long   8, 4, 1
        .ascii  "OpenBSD\0"
        .long   0

        .section ".openbsd.syscalls"
        .p2align 2
        .long   2b, SYS_write
        .long   3b, SYS_exit
