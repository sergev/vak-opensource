//
// Table of primes
//
#define Halt    0
#define Fputs   7
#define StdOut  1

#define L       500     // The number of primes to find
#define t       $255    // Temporary storage

n      GREG             // Local variables
q      GREG
r      GREG
jj     GREG
kk     GREG
pk     GREG

#define mm      kk      // Reuse the same register

        .data
PRIME1  .hword  2
        .space  2*L-2, 0
ptop    GREG    @
j0      GREG    PRIME1+2-@
BUF     .quad

        .text
        .global _init
_init   SET     n, 3
        SET     jj, j0
2H      STWU    n, ptop, jj
        INCL    jj, 2
3H      BZ      jj, 2F
4H      INCL    n, 2
5H      SET     kk, j0
6H      LDWU    pk, ptop, kk
        DIV     q, n, pk
        GET     r, rR
        BZ      r, 4B
7H      CMP     t, q, pk
        BNP     t, 2B
8H      INCL    kk, 2
        JMP     6B

2H      LDA     t, Title
        TRAP    0, Fputs, StdOut
        NEG     mm, 2
3H      ADD     mm, mm, j0
        LDA     t, Blanks
        TRAP    0, Fputs, StdOut
2H      LDWU    pk, ptop, mm
0H      GREG    #2030303030000000
        STOU    0B, BUF
        LDA     t, BUF+4
1H      DIV     pk, pk, 10
        GET     r, rR
        INCL    r, '0'
        STBU    r, t, 0
        SUB     t, t, 1
        PBNZ    pk, 1B
        LDA     t, BUF
        TRAP    0, Fputs, StdOut
        INCL    mm, 2*L/10
        PBN     mm, 2B
        LDA     t, NewLn
        TRAP    0, Fputs, StdOut
        CMP     t, mm, 2*(L/10-1)
        PBNZ    t, 3B
        TRAP    0, Halt, 0

        .section .rodata
Title   .ascii  "First Five Hundred Primes"
NewLn   .asciz  "\n"
Blanks  .asciz  "   "
