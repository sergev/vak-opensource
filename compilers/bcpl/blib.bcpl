//   BLIB

GET "LIBHDR"

LET writes(s) BE
    FOR i = 1 TO getbyte(s, 0) DO wrch(getbyte(s, i))

AND unpackstring(s, v) BE
    FOR i = 0 TO getbyte(s, 0) DO
        v!i := getbyte(s, i)

AND packstring(v, s) = VALOF
$(
    LET n = v!0 & 255
    LET i = n/4
    FOR p = 0 TO n DO putbyte(s, p, v!p)
    SWITCHON n&3 INTO $(
        CASE 0: putbyte(s, n+3, 0)
        CASE 1: putbyte(s, n+2, 0)
        CASE 2: putbyte(s, n+1, 0)
        CASE 3:
    $)
    RESULTIS i
$)

// THE DEFINITIONS THAT FOLLOW ARE MACHINE INDEPENDENT

AND writed(n, d) BE
$(1
    LET t = VEC 20
    AND i, k = 0, n
    IF n<0 DO
        d, k := d-1, -n
    t!i, k, i := k REM 10, k/10, i+1 REPEATUNTIL k=0
    FOR j = i+1 TO d DO
        wrch('*S')
    IF n<0 DO
        wrch('-')
    FOR j = i-1 TO 0 BY -1 DO
        wrch(t!j+'0')
$)1

AND writen(n) BE writed(n, 0)

AND newline() BE wrch('*N')

AND newpage() BE wrch('*P')

AND readn() = VALOF
$(1
    LET sum = 0
    AND neg = FALSE

L:  terminator := rdch()
    SWITCHON terminator INTO $(
        CASE '*S':
        CASE '*T':
        CASE '*N': GOTO L

        CASE '-':  neg := TRUE
        CASE '+':  terminator := rdch()
    $)
    WHILE '0'<=terminator<='9' DO $(
        sum := 10*sum + terminator - '0'
        terminator := rdch()
    $)
    IF neg DO
        sum := -sum
    RESULTIS sum
$)1

AND writeoct(n, d) BE
$(
    IF d>1 DO
        writeoct(n>>3, d-1)
    wrch((n/\7)+'0')
$)

AND writehex(n, d) BE
$(
    IF d>1 DO
        writehex(n>>4, d-1)
    wrch((n&15)!TABLE
        '0','1','2','3','4','5','6','7',
        '8','9','A','B','C','D','E','F')
$)

AND writef(format, a, b, c, d, e, f, g, h, i, j, k) BE
$(1
    LET t = @a

    FOR p = 1 TO getbyte(format, 0) DO $(2
        LET k = getbyte(format, p)

        TEST k='%' THEN $(3
            LET f, q, n = 0, t!0, 0
            AND TYPE = getbyte(format, p+1)
            p := p + 1
            SWITCHON TYPE INTO $(
                DEFAULT: wrch(TYPE); ENDCASE

                CASE 'S': f := writes; GOTO L
                CASE 'C': f := wrch; GOTO L
                CASE 'O': f := writeoct; GOTO M
                CASE 'X': f := writehex; GOTO M
                CASE 'I': f := writed; GOTO M
                CASE 'N': f := writed; GOTO L

                M: p := p + 1
                   n := getbyte(format, p)
                   n := '0'<=n<='9' -> n-'0', n-'A'+10

                L: f(q, n); t := t + 1
        $)3
        OR wrch(k)
    $)2
$)1

//AND mapstore() BE writes("*Nmapstore NOT IMPLEMENTED*N")
