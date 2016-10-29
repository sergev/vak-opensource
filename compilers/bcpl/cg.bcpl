//    CG1

GET "LIBHDR"

MANIFEST $(   // OCODE OPERATORS
    S.TRUE=4; S.FALSE=5
    S.RV=8; S.FNAP=10
    S.MULT=11; S.DIV=12; S.REM=13; S.PLUS=14; S.MINUS=15
    S.QUERY:16; S.NEG=17
    S.EQ=20; S.NE=21; S.LS=22; S.GR=23; S.LE=24; S.GE=25
    S.NOT=30; S.LSHIFT=31; S.RSHIFT=32; S.LOGAND=33; S.LOGOR=34
    S.EQV=35; S.NEQV=36; S.COND=37
    S.LP=40; S.LG=41; S.LN=42; S.LSTR=43; S.LL=44
    S.LLP=45; S.LLG=46; S.LLL=47

    S.RTAP=51; S.GOTO=52
    S.RETURN=67; S.FINISH=68
    S.SWITCHON=70
    S.GLOBAL=76
$)

MANIFEST $(
    S.SP=80; S.SG=81; S.SL=82; S.STIND=83
    S.JUMP=85; S.JT=86; S.JF=87
    S.LAB=90; S.STACK=91; S.STORE=92; S.RSTACK=93; S.ENTRY=94
    S.SAVE=95; S.FNRN=96; S.RTRN=97; S.RES=98; S.RESLAB=99
    S.DATALAB=100; S.ITEML=101; S.ITEMN=102; S.ENDPROC=103; S.END=104
    S.CHAR=105; ERROR:108; S.DEBUG:109
$)

MANIFEST $(
    M.N=0; M.I=1; M.P=2; M.IP=3; M.L=4; M.IL=5; M.G=6; M.IG=7
    F.L='L'; F.S='S'; F.A='A'; F.J='J'; F.T='T'; F.F='F'; F.K='K'; F.X='X'
    F.D='D'; F.C='C'
    NIL=0; AD=1; AC=2; ACAD=3
$)

GLOBAL $(
    ch:100; wordv:102
    intcode:105; sysprint:106; sysin:107
    ssp:110; state:111; ad.a:113; ad.k:114
    datav:152; datap:153; datat:154
    proglength:155; linep:156; param:157
    op:171
    readop:210; rdn:211; rdl:212
    gencode:230
    force.nil:240; force.ad:241; force.ac:242; force.acad:243
    load:244
    storein:246
    cgstring:250; data:251
    code:260; complab:261; opcode:262; wr:263; wrn:264
    wrdata:265
$)

STATIC $(
    wp = 0
    strsize = 0
$)

LET t(s) = VALOF
$(
    FOR i = 0 TO strsize DO UNLESS s!i=wordv!i RESULTIS FALSE
    RESULTIS TRUE
$)

LET readop() = VALOF
$(1
    LET s = VEC 20

    ch := rdch() REPEATWHILE ch='*N' \/ ch='*S'
    wp := 0

    WHILE 'A'<=ch<='Z' DO $(
        wp := wp + 1
        s!wp := ch
        ch := rdch()
    $)

    s!0 := wp
    strsize := packstring(s, wordv)

    SWITCHON s!1 INTO $(
    DEFAULT: IF ch=ENDSTREAMCH RESULTIS S.END
             RESULTIS ERROR

    CASE 'D':
    RESULTIS t("DATALAB") -> S.DATALAB,
             t("DIV") -> S.DIV,
             t("DEBUG") -> S.DEBUG, ERROR

    CASE 'E':
    RESULTIS t("EQ") -> S.EQ,
             t("ENTRY") -> S.ENTRY,
             t("EQV") -> S.EQV,
             t("ENDPROC") -> S.ENDPROC,
             t("END") -> S.END, ERROR

    CASE 'F':
    RESULTIS t("FNAP") -> S.FNAP,
             t("FNRN") -> S.FNRN,
             t("FALSE") -> S.FALSE,
             t("FINISH") -> S.FINISH, ERROR


    CASE 'G':
    RESULTIS t("GOTO") -> S.GOTO,
             t("GE") -> S.GE,
             t("GR") -> S.GR,
             t("GLOBAL") -> S.GLOBAL, ERROR

    CASE 'I':
    RESULTIS t("ITEMN") -> S.ITEMN,
             t("ITEML") -> S.ITEML,  ERROR

    CASE 'J':
    RESULTIS t("JUMP") -> S.JUMP,
             t("JF") -> S.JF,
             t("JT") -> S.JT,  ERROR

    CASE 'L':
    IF wp=2 DO
         SWITCHON s!2 INTO $(
            DEFAULT: RESULTIS ERROR
            CASE 'E': RESULTIS S.LE
            CASE 'N': RESULTIS S.LN
            CASE 'G': RESULTIS S.LG
            CASE 'P': RESULTIS S.LP
            CASE 'L': RESULTIS S.LL
            CASE 'S': RESULTIS S.LS
         $)

    RESULTIS t("LAB") -> S.LAB,
             t("LLG") -> S.LLG,
             t("LLL") -> S.LLL,
             t("LLP") -> S.LLP,
             t("LOGAND") -> S.LOGAND,
             t("LOGOR") -> S.LOGOR,
             t("LSHIFT") -> S.LSHIFT,
             t("LSTR") -> S.LSTR, ERROR

    CASE 'M':
    RESULTIS t("MINUS") -> S.MINUS,
             t("MULT") -> S.MULT, ERROR

    CASE 'N':
    RESULTIS  t("NE") -> S.NE,
              t("NEG") -> S.NEG,
              t("NEQV") -> S.NEQV,
              t("NOT") -> S.NOT,  ERROR

    CASE 'P':
    RESULTIS t("PLUS") -> S.PLUS, ERROR

    CASE 'Q':
    RESULTIS t("QUERY") -> S.QUERY, ERROR

    CASE 'R':
    RESULTIS t("RES") -> S.RES,
             t("REM") -> S.REM,
             t("RTAP") -> S.RTAP,
             t("RTRN") -> S.RTRN,
             t("RSHIFT") -> S.RSHIFT,
             t("RSTACK") -> S.RSTACK,
             t("RV") -> S.RV, ERROR

    CASE 'S':
    RESULTIS t("SG") -> S.SG,
             t("SP") -> S.SP,
             t("SL") -> S.SL,
             t("STIND") -> S.STIND,
             t("STACK") -> S.STACK,
             t("SAVE") -> S.SAVE,
             t("SWITCHON") -> S.SWITCHON,
             t("STORE") -> S.STORE, ERROR

    CASE 'T':
    RESULTIS t("TRUE") -> S.TRUE, ERROR
$)1


AND rdn() = VALOF
$(1
    LET a, neg = 0, FALSE

    ch := rdch() REPEATWHILE ch='*N' \/ ch='*S'
    IF ch='-' DO $(
        neg := TRUE
        ch := rdch()
    $)

    WHILE '0' LE ch LE '9' DO $(
        a := a*10 + ch - '0'
        ch := rdch()
    $)

    RESULTIS neg -> -a, a
$)1

AND rdl() = VALOF
$(1
    LET a = 0

    ch := rdch() REPEATWHILE ch='*N' \/ ch='*S'

    IF ch='L' DO ch := rdch()

    WHILE '0' LE ch LE '9' DO $(
        a := a*10 + ch - '0'
        ch := rdch()
    $)

    RESULTIS a
$)1

AND start(parm) BE
$(1
    LET v = VEC 4000
    datav, datat := v, 4000

    $(
        LET v = VEC 50
        wordv := v

        sysin := input()
        sysprint := output()
        intcode := findoutput("INTCODE")
        IF intcode=0 DO intcode := sysprint

        proglength := 0

        selectinput(sysin)
        selectoutput(intcode)

        $(
            ssp, state := 2, NIL
            datap, linep,  param := 0, 0, 500
            gencode()
        $) REPEATWHILE op=S.GLOBAL

        selectoutput(sysprint)
        writef("*NPROGRAM LENGTH = %N*N", proglength)
        FINISH
$)1

AND gencode() BE
$(1
NEXT:
    op := readop()

    SWITCHON op INTO $(
    DEFAULT:    selectoutput(sysprint)
                writef("*NUNKNOWN KEY WORD:  %S*N", wordv)
                selectoutput(intcode)
                GOTO NEXT

    CASE S.END: RETURN

    CASE S.DEBUG:
         selectoutput(sysprint)
         writef("*NSTATE=%N, SSP=%N, AD.A=%N, AD.K=%N*N",
                   state,    ssp,    ad.a,    ad.k)
         selectoutput(intcode)
         GOTO NEXT

    CASE S.LP: load(rdn(), M.IP); GOTO NEXT
    CASE S.LG: load(rdn(), M.IG); GOTO NEXT
    CASE S.LL: load(rdl(), M.IL); GOTO NEXT
    CASE S.LN: load(rdn(), M.N); GOTO NEXT

    CASE S.LSTR: cgstring(rdn()); GOTO NEXT

    CASE S.TRUE:  load(-1, M.N); GOTO NEXT
    CASE S.FALSE: load(0, M.N); GOTO NEXT


    CASE S.LLP: load(rdn(), M.P); GOTO NEXT
    CASE S.LLG: load(rdn(), M.G); GOTO NEXT
    CASE S.LLL: load(rdl(), M.L); GOTO NEXT

    CASE S.SP: storein(rdn(), M.P); GOTO NEXT
    CASE S.SG: storein(rdn(), M.G); GOTO NEXT
    CASE S.SL: storein(rdl(), M.L); GOTO NEXT

    CASE S.STIND: force.acad()
                  code(F.S, ad.a, ad.k)
                  ssp, state := ssp-2, NIL
                  GOTO NEXT

    CASE S.MULT:CASE S.DIV:CASE S.REM:
    CASE S.MINUS:CASE S.EQ:CASE S.NE:
    CASE S.LS:CASE S.GR:CASE S.LE:CASE S.GE:
    CASE S.LSHIFT:CASE S.RSHIFT:
    CASE S.LOGAND:CASE S.LOGOR:CASE S.NEQV:CASE S.EQV:
         force.acad()
         code(F.L, ad.a, ad.k)
         code(F.X, opcode(op), M.N)
         state, ssp := AC, ssp-1
         GOTO NEXT

    CASE S.RV:CASE S.NEG:CASE S.NOT:
         force.ac()
         code(F.X, opcode(op), M.N)
         GOTO NEXT

    CASE S.PLUS: force.acad()
                 code(F.A, ad.a, ad.k)
                 state, ssp := AC, ssp-1
                 GOTO NEXT

    CASE S.JUMP: force.nil()
                 code(F.J, rdl(), M.L)
                 GOTO NEXT

    CASE S.JT:CASE S.JF:
              force.ac()
              code(op=S.JT->F.T,F.F, rdl(), M.L)
              ssp, state := ssp-1, NIL
              GOTO NEXT

    CASE S.GOTO: force.ad()
                 code(F.J, ad.a, ad.k)
                 ssp, state := ssp-1, NIL
                 GOTO NEXT

    CASE S.LAB: force.nil()
                complab(rdl())
                GOTO NEXT

    CASE S.QUERY: force.nil(); ssp := ssp + 1; GOTO NEXT

    CASE S.STACK: force.nil(); ssp := rdn(); GOTO NEXT

    CASE S.STORE: force.nil(); GOTO NEXT

    CASE S.ENTRY: $(
                    LET n = rdn()
                    LET l = rdl()
                    wr('*N'); wr('$')
                    FOR I = 1 TO n DO rdn()
                    wr(' ')
                    complab(l)
                    GOTO NEXT
                $)

    CASE S.SAVE: ssp := rdn(); GOTO NEXT

    CASE S.ENDPROC: rdn(); GOTO NEXT
    CASE S.RTAP:
    CASE S.FNAP: $(
                    LET k = rdn()
                    force.ac()
                    code(F.K, k, M.N)
                    TEST op=S.FNAP THEN
                        ssp, state := k+1, AC
                    OR
                        ssp, state := k, NIL
                    GOTO NEXT
                $)

    CASE S.FNRN: force.ac()
                 ssp := ssp - 1
    CASE S.RTRN: code(F.X, opcode(S.RTRN), M.N)
                 state := NIL
                 GOTO NEXT

    CASE S.RES: force.ac()
                code(F.J, rdl(), M.L)
                ssp, state := ssp-1, NIL
                GOTO NEXT

    CASE S.RSTACK: force.nil()
                   ssp, state := rdn()+1, AC
                   GOTO NEXT

    CASE S.FINISH: code(F.X, opcode(op), M.N); GOTO NEXT

    CASE S.SWITCHON: $(
                        LET n = rdn()
                        LET d = rdl()
                        force.ac()
                        code(F.X, opcode(op), M.N)
                        code(F.D, n, M.N)
                        code(F.D, d, M.L)
                        ssp, state := ssp-1, NIL
                        FOR I = 1 TO n DO $(
                            code(F.D, rdn(), M.N)
                            code(F.D, rdl(), M.L)
                        $)
                        GOTO NEXT
                    $)

      CASE S.GLOBAL:
           wr('*N')
           FOR I = 0 TO datap-2 BY 2 DO wrdata(datav!I, datav!(I+1))
           wr('*N')
           FOR I = 1 TO rdn() DO $(
                wr('G'); wrn(rdn())
                wr('L'); wrn(rdl()); wr('*S')
            $)
            wr('*N'); wr('Z'); wr('*N')
            RETURN

    CASE S.DATALAB:
    CASE S.ITEML: data(op, rdl())
                  GOTO NEXT

    CASE S.ITEMN: data(op, rdn())
                  GOTO NEXT
$)1

AND force.nil() BE
    SWITCHON state INTO $(
        CASE ACAD: code(F.S, ssp-2, M.P)

        CASE AD: code(F.L, ad.a, ad.k)

        CASE AC: code(F.S, ssp-1, M.P)
                 state := NIL

        CASE NIL:
    $)

AND force.ad() BE
    SWITCHON state INTO $(
        CASE ACAD: code(F.S, ssp-2, M.P)
                   GOTO L

        CASE AC:   code(F.S, ssp-1, M.P)

        CASE NIL:  ad.a, ad.k := ssp-1, M.IP
L:                 state := AD

        CASE AD:
    $)

AND force.ac() BE
    SWITCHON state INTO $(
        CASE NIL:  code(F.L, ssp-1, M.IP)
                   GOTO L

        CASE ACAD: code(F.S, ssp-2, M.P)

        CASE AD:   code(F.L, ad.a, ad.k)
L:                 state := AC

        CASE AC:
    $)

AND force.acad() BE
    SWITCHON state INTO $(
        CASE AD:   code(F.L, ssp-2, M.IP)
                   GOTO L

        CASE AC:   code(F.S, ssp-1, M.P)

        CASE NIL:  code(F.L, ssp-2, M.IP)
                   ad.a, ad.k := ssp-1, M.IP
L:                 state := ACAD

        CASE ACAD:
    $)

AND load(a, k) BE
    SWITCHON state INTO $(
        CASE NIL: state := AD
                  GOTO M

        CASE ACAD:
        CASE AD:  force.ac()
        CASE AC:  state := ACAD
M:                ad.a, ad.k := a, k
                  ssp := ssp + 1
    $)


AND storein(a, k) BE
$(
    force.ac()
    code(F.S, a, k)
    ssp, state := ssp-1, NIL
$)

AND cgstring(n) BE
$(1
    LET l = nextparam()
    data(S.DATALAB, l)
    data(S.CHAR, n)
    FOR i = 1 TO n DO
        data(S.CHAR, rdn())
    load(l, M.L)
    RETURN
$)1

AND data(k, v) BE
$(
    LET p = datap
    datav!p, datav!(p+1) := k, v
    datap := datap + 2
    IF datap>datat DO $(
        selectoutput(sysprint)
        writes("*NTOO MANY CONSTANTS*N")
        selectoutput(intcode)
        datap := 0
    $)
$)

AND nextparam() = VALOF
$(
    param := param - 1
    RESULTIS param
$)

AND code(f, a, k) BE
$(
    wr(f)
    SWITCHON k INTO $(
        CASE M.I: wr('I')
        CASE M.N: ENDCASE

        CASE M.IG: wr('I')
        CASE M.G:  wr('G')
                   ENDCASE

        CASE M.IP: wr('I')
        CASE M.P:  wr('P'); ENDCASE

        CASE M.IL: wr('I')
        CASE M.L:  wr('L'); ENDCASE
    $)

    wrn(a)
    wr(' ')
    proglength := proglength + 1
$)

AND complab(n) BE
$(
    wrn(n)
    wr(' ')
$)

AND wrdata(k, n) BE
    SWITCHON k INTO $(
        CASE S.DATALAB: complab(n); RETURN

        CASE S.ITEMN: code(F.D, n, M.N); RETURN

        CASE S.ITEML: code(F.D, n, M.L); RETURN

        CASE S.CHAR:  code(F.C, n, M.N); RETURN
    $)

AND opcode(op) = VALOF
    SWITCHON op INTO $(
        CASE S.RV: RESULTIS 1
        CASE S.NEG:RESULTIS 2
        CASE S.NOT:RESULTIS 3
        CASE S.RTRN:RESULTIS 4
        CASE S.MULT:  RESULTIS 5
        CASE S.DIV:   RESULTIS 6
        CASE S.REM:   RESULTIS 7
        CASE S.PLUS:  RESULTIS 8
        CASE S.MINUS: RESULTIS 9
        CASE S.EQ:    RESULTIS 10
        CASE S.NE:    RESULTIS 11
        CASE S.LS:    RESULTIS 12
        CASE S.GE:    RESULTIS 13
        CASE S.GR:    RESULTIS 14
        CASE S.LE:    RESULTIS 15
        CASE S.LSHIFT:RESULTIS 16
        CASE S.RSHIFT:RESULTIS 17
        CASE S.LOGAND:RESULTIS 18
        CASE S.LOGOR: RESULTIS 19
        CASE S.NEQV:  RESULTIS 20
        CASE S.EQV:   RESULTIS 21
        CASE S.FINISH:RESULTIS 22
        CASE S.SWITCHON:RESULTIS 23

        DEFAULT: selectoutput(sysprint)
                 writef("*NUNKNOWN OP %N*N", op)
                 selectoutput(intcode)
                 RESULTIS 0
    $)

AND wr(ch) BE
$(
    IF ch='*N' DO $(
        wrch('*N')
        linep := 0
        RETURN
    $)

    IF linep=71 DO $(
        wrch('/')
        wrch('*N')
        linep := 0
    $)
    linep := linep + 1
    wrch(ch)
$)

AND wrn(n) BE
$(
    IF n<0 DO $(
        wr('-');
        n := -n
    $)
    IF n>9 DO
        wrn(n/10)
    wr(n REM 10 + '0')
$)
