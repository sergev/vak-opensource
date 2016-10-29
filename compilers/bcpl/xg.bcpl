// Copyright (c) 2004 Robert Nordier.  All rights reserved.

GET "LIBHDR"

MANIFEST $(
    GSZ = 500
    LSZ = 500
$)

MANIFEST $(
    M.N = 0
    M.L = 1
    M.P = 2
    M.G = 3
$)

GLOBAL $(
    sysprint : 150
    G        : 151
    L        : 152
    ln       : 153
    sect     : 154
    loff     : 155
    ocode    : 156
$)

STATIC $(
    line = 0
    col  = 0
    ch   = '*N'
$)

LET start() = VALOF
$(
    LET gvec = VEC GSZ
    LET lvec = VEC LSZ
    sysprint := output()
    ocode := findoutput("ASM")
    selectoutput(ocode)
    G := gvec
    L := lvec
    FOR i = 0 TO GSZ DO G!i := 0
    loff := 2000
    sect := 0
    ln := 0
    assem()
    epilog()
    endwrite()
    RESULTIS 0
$)

AND assem() BE
$(
    LET F,A,I,K = 0,0,0,0
    rch()
    F := ch
    SWITCHON F INTO $(
    DEFAULT:
        error(6)
    CASE ENDSTREAMCH:
        gener(0, 0, 0, 0)
        RETURN
    CASE '*S':CASE '*N':
        LOOP
    CASE 'G':
        rch()
        A := rdn()
        UNLESS A<GSZ error(5)
        UNLESS ch='L' error(4)
        rch()
        G!A := loff + rdn()
        LOOP
    CASE 'Z':
        loff := loff + 500
        LOOP
    CASE '$':
        LOOP
    CASE '0':CASE '1':CASE '2': CASE '3': CASE '4':
    CASE '5':CASE '6':CASE '7': CASE '8': CASE '9':
        F := 'B'
        A := loff + rdn()
        ENDCASE
    CASE 'L':CASE 'S':CASE 'A':CASE 'J':
    CASE 'T':CASE 'F':CASE 'K':CASE 'X':
        rch()
        IF ch='I' THEN $( I := TRUE; rch() $)
        K := ch='G' -> M.G, ch='P' -> M.P, ch='L' -> M.L, M.N
        UNLESS K=0 THEN rch()
        A := rdn()
        IF K=M.L THEN
            A := A + loff
        ENDCASE
    CASE 'D':
        rch()
        IF ch='L' THEN $( K := K + 1; rch() $)
        A := rdn()
        IF K=M.L THEN
            A := A + loff
        ENDCASE
    CASE 'C':
        rch()
        A := rdn()
        ENDCASE
    $)
    gener(F, A, I, K)
$) REPEAT

AND epilog() BE
$(
    sect := 1
    emit(".globl G")
    emit(".align 4")
    emit("G:")
    FOR i=0 TO GSZ - 1
        emit(".long %S%N # %N", G!i=0 -> "", "L", G!i, i)
$)

AND gener(F1, A1, I1, K1) BE
$(
    STATIC $(
        XL = 0
        F  = 0
        A  = 0
        I  = 0
        K  = 0
        F0 = 0
        A0 = 0
        I0 = 0
        K0 = 0
    $)
    SWITCHON F INTO $(
    DEFAULT:
        error(7)
    CASE 0:
        ENDCASE
    CASE 'B':
        UNLESS ln<=LSZ THEN error(1)
        L!ln := A
        ln := ln + 1
        ENDCASE
    CASE 'L':
        IF F1='X' DO
            TEST (5 <= A1 <= 21)
                ENDCASE
            OR IF A1=32 | (35 <= A1 <= 37)
                emit("movl %%eax,%%ebx")
    CASE 'A':
        TEST NOT I $(
            emit("%S %S,%%e%Sx",
                 (F='L' & (K=M.P | K=M.G) -> "lea",
                 (F='A' & NOT I & K=M.N -> "addl", "movl")),
                 addr(A, I, K),
                 (F='A' & K NE M.N -> "b", "a"))
            UNLESS K=M.N THEN $(
                emit("shrl $2,%%e%Sx", (F='A' -> "b", "a"))
                IF F='A' THEN
                    emit("addl %%ebx,%%eax")
            $)
        $) OR
               emit("%S %S,%%eax", (F='A' -> "addl", "movl"),
                     addr(A, TRUE, K))
        ENDCASE
    CASE 'S':
        TEST NOT I THEN
            emit("movl %%eax,%S", addr(A, TRUE, K))
        OR $(
            emit("movl %S,%%ebx", addr(A, I, K))
            emit("movl %%eax,(,%%ebx,4)")
        $)
        ENDCASE
    CASE 'J':CASE 'T':CASE 'F':
        IF F NE 'J' & I=TRUE | K NE M.L THEN
            error(8)
        UNLESS F='J' DO $(
            IF (F0='X' & (10 <= A0 <= 15)) $(
                TEST F='T' DO
                    emit("j%S %SL%N",
                         (A0=10 -> "e",
                          A0=11 -> "ne",
                          A0=12 -> "l",
                          A0=13 -> "ge",
                          A0=14 -> "g", "le"),
                          (I -> "**", ""), A)
                OR
                    emit("j%S %SL%N",
                         (A0=10 -> "ne",
                          A0=11 -> "e",
                          A0=12 -> "ge",
                          A0=13 -> "l",
                          A0=14 -> "le", "g"),
                          (I -> "**", ""), A)
                ENDCASE
            $)
            emit("testl %%eax,%%eax")
        $)
        emit("j%S %SL%N", (F='T' -> "nz" , F='F' -> "z" , "mp"),
             (I -> "**" , ""), A)
        ENDCASE
    CASE 'K':
        IF I | K NE M.N THEN
            error(8)
        emit("movl %%ebp,%%ebx")
        emit("addl $%N,%%ebp", A << 2)
        emit("movl %%ebx,(%%ebp)")
        emit("movl $1f,4(%%ebp)")
        emit("jmpl **%%eax")
        emit("1:")
        ENDCASE
    CASE 'X':
        SWITCHON A INTO $(
        DEFAULT:
            error(9)
        CASE 1:
            emit("movl (,%%eax,4),%%eax")
            ENDCASE
        CASE 2:
            emit("negl %%eax")
            ENDCASE
        CASE 3:
            emit("xorl $-1,%%eax")
            ENDCASE
        CASE 4:
            emit("movl 4(%%ebp),%%ebx")
            emit("movl (%%ebp),%%ebp")
            emit("jmp **%%ebx")
            ENDCASE
        CASE 5:CASE 6:CASE 7:
            UNLESS A=5 DO emit("cltd")
            TEST NOT I0 & K0=M.N DO $(
                emit("movl %S,%%ecx", addr(A0, I0, K0))
                emit("i%Sl %%ecx", (A=5 -> "mul", "div"))
            $) OR
                emit("i%Sl %S", (A=5 -> "mul", "div"),
                     addr(A0, I0, K0))
            IF A=7 DO emit("movl %%edx,%%eax")
            ENDCASE
        CASE 8:
            emit("addl %S,%%eax", addr(A0, I0, K0))
            ENDCASE
        CASE 9:
            emit("subl %S,%%eax", addr(A0, I0, K0))
            ENDCASE
        CASE 10:CASE 11:CASE 12:CASE 13:CASE 14:CASE 15:
            emit("cmpl %S,%%eax", addr(A0, I0, K0))
            IF F1='F' | F1='T' ENDCASE
            emit("set%S %%al",
                 A=10 -> "ne",
                 A=11 -> "e",
                 A=12 -> "ge",
                 A=13 -> "l",
                 A=14 -> "le", "g")
            emit("movzbl %%al,%%eax")
            emit("decl %%eax")
            ENDCASE
        CASE 16:CASE 17:
            TEST NOT I0 & K0=M.N & A0<32 DO
                emit("sh%Cl $%N,%%eax", A=16 -> 'l', 'r', A0)
            ELSE $(
                emit("movl %S,%%ecx", addr(A0, I0, K0))
                emit("jecxz 1f")
                emit("decl %%ecx")
                emit("sh%Cl $1,%%eax", A=16 -> 'l', 'r')
                emit("sh%Cl cl,%%eax", A=16 -> 'l', 'r')
                emit("1:")
            $)
            ENDCASE
        CASE 18:CASE 19:CASE 20:
            emit("%Sl %S,%%eax",
                 (A=18 -> "and", A=19 -> "or", "xor"),
                 addr(A0, I0, K0))
            ENDCASE
        CASE 21:
            emit("xorl $-1,%%eax")
            emit("xorl %S,%%eax", addr(A0, I0, K0))
            ENDCASE
        CASE 22:
            emit("jmp finish")
            ENDCASE
        CASE 23:
            emit("movl $L%N,%%esi", XL)
            emit("movl (%%esi),%%ecx")
            emit("movl 4(%%esi),%%edx")
            emit("jecxz 2f")
            emit("1:")
            emit("addl $8,%%esi")
            emit("cmpl (%%esi),%%eax")
            emit("je 3f")
            emit("loop 1b")
            emit("2:")
            emit("jmp **%%edx")
            emit("3:")
            emit("jmp **4(%%esi)")
            L!ln := XL
            ln := ln + 1
            XL := XL + 1
            ENDCASE
        CASE 24:
            emit("movl %%eax,(%%esp)")
            emit("call selectinput")
            ENDCASE
        CASE 25:
            emit("call selectoutput")
            ENDCASE
        CASE 26:
            emit("call rdch")
            ENDCASE
        CASE 27:
            emit("call wrch")
            ENDCASE
        CASE 28:
            emit("movl %%eax,(%%esp)")
            emit("call findinput")
            ENDCASE
        CASE 29:
            emit("call findoutput")
            ENDCASE
        CASE 30:
            emit("jmp stop")
            ENDCASE
        CASE 31:
            emit("movl (%%ebp),%%eax")
            ENDCASE
        CASE 32:
            emit("movl %%eax,%%ebp")
            emit("jmp **%%ebx")
            ENDCASE
        CASE 33:
            emit("call endread")
            ENDCASE
        CASE 34:
            emit("call endwrite")
            ENDCASE
        CASE 35:
            emit("movl %%ebp,%%esi")
            emit("movl %%ebx,%%ecx")
            emit("incl %%ecx")
            emit("shll $2,%%ecx")
            emit("addl %%ecx,%%esi")
            emit("movl (%%ebp),%%ecx")
            emit("movl %%ecx,(%%esi)")
            emit("movl 4(%%ebp),%%ecx")
            emit("movl %%ecx,4(%%esi)")
            emit("movl %%ebp,%%ecx")
            emit("shrl $2,%%ecx")
            emit("movl %%ecx,8(%%esi)")
            emit("movl %%ebx,12(%%esi)")
            emit("movl %%esi,%%ebp")
            emit("jmp **%%eax")
            ENDCASE
        CASE 36:
            emit("shll $2,%%eax")
            emit("addl %%ebx,%%eax")
            emit("movzbl (%%eax),%%eax")
            ENDCASE
        CASE 37:
            emit("shll $2,%%eax")
            emit("addl %%ebx,%%eax")
            emit("movl 16(%%ebp),%%ebx")
            emit("movb %%bl,(%%eax)")
            ENDCASE
        CASE 38:
            emit("call input")
            ENDCASE
        CASE 39:
            emit("call output")
            ENDCASE
        CASE 40:
            emit("call unrdch")
            ENDCASE
        CASE 41:
            emit("call rewind")
        $)
        ENDCASE
    CASE 'D':
        sect := 1
        emit(".long %S%N", (K=M.L -> "L", ""), A)
        sect := 0
        ENDCASE
    CASE 'C':
        sect := 1
        emit(".byte %N", A)
        IF F1 NE 'C' THEN
            emit(".align 4,0")
        sect := 0
    $)
    F0 := F; A0 := A; I0 := I; K0 := K
    F := F1; A := A1; I := I1; K := K1
$)

AND addr(A, I, K) = VALOF
$(
    LET T = TABLE 0,0,0,0,0,0,0,0,0,0,0,0,0,0
    AND ADDBYTE(S, C) BE
    $(
        LET N = getbyte(S, 0)
        N := N + 1
        putbyte(S, N, C)
        putbyte(S, 0, N)
    $)
    AND APPEND(D, S) BE
    $(
        LET N = getbyte(S, 0)
        FOR J = 1 TO N DO
            ADDBYTE(D, getbyte(S, J))
    $)
    AND WRN(S, N) BE
    $(
        IF N<0 DO $( ADDBYTE(S, '-'); N := -N $)
        IF N>=10 DO WRN(S, N / 10)
        ADDBYTE(S, '0' + N REM 10)
    $)
    putbyte(T, 0, 0)
    IF NOT I & (K=M.N | K=M.L) THEN
        ADDBYTE(T, '$')
    IF K=M.L THEN
        ADDBYTE(T, 'L')
    WRN(T, K=M.L -> A, NOT I & K=M.N -> A, A << 2)
    IF K=M.P | K=M.G THEN
        APPEND(T, K=M.P -> "(%ebp)", "(%edi)")
    RESULTIS T
$)

AND emit(FMT, A, B, C, D, E) BE
$(
    STATIC $(
        psect = 0
    $)
    UNLESS sect=psect DO $(
        emit1(sect=0 -> ".text", ".data")
        psect := sect
    $)
    UNLESS ln=0 $(
        emit1("#.align 4")
        FOR I = 0 TO ln - 1 DO
            writef("L%N:*N", L!I)
        ln := 0
    $)
    emit1(FMT, A, B, C, D, E)
$)

AND emit1(FMT, A, B, C, D, E) BE
$(
    writef(FMT, A, B, C, D, E)
    wrch('*N')
$)

AND rdn() = VALOF
$(
    LET X, N, I = 0, FALSE, 0
    IF ch='-' THEN $(
        N := TRUE
        rch()
    $)
    WHILE '0' <= ch <= '9' $(
        X := X * 10 + (ch - '0')
        I := I + 1
        rch()
    $)
    IF I=0 THEN
        error(3)
    RESULTIS N -> -X, X
$)

AND rch() BE
$(
    rch1()
    UNLESS ch='/' RETURN
    $( rch1(); IF ch=ENDSTREAMCH DO error(2) $) REPEATUNTIL ch='*N'
$) REPEAT

AND rch1() BE
$(
    TEST ch='*N' THEN $(
        line := line + 1
        col := 1
    $) OR
        col := col + 1
    ch := rdch()
$)

AND error(N) BE
$(
    selectoutput(sysprint)
    UNLESS line=0
        writef("SYSIN(%N,%N): ", line, col)
    writef("error %N*N", N)
    stop(1)
$)
