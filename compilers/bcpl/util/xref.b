GET "LIBHDR"

MANIFEST $(
S.NULL=0
S.LET=1
S.PROC=2
S.LAB=3
S.GLOBAL=4
S.MANIFEST=5
S.STATIC=6
S.FOR=7
S.EQ=8
S.BE=9
S.AND=10
S.NAME=11
S.GET=12
S.STRING=13
S.COLON=14
S.LPAREN=15
S.CASE=16
S.END=17
S.RSECT=18
S.REFNAME=19
$)

GLOBAL $(
NEXTSYMB:100
LOOKUPWORD:101
CMPSTR:102
DECLSYSWORDS:103
D:104
RCH:105
RDTAG:106
PERFORMGET:107
READNUMBER:108
VALUE:109
RDSTRCH:110
NEWVEC:111
LIST2:112
ADDREF:113
XREF:114
PRTREE:115
WRNAMEINFO:116
ERROR:117
MATCH:118

SYMB:120
CH:121
WORDV:122
WORDSIZE:123
CHARV:124
PTR:125
TREEVEC:126
TREEP:127
GETV:130
GETP:131
GETT:132
SOURCESTREAM:133
LINECOUNT:134
NAMETREE:135
WORDNODE:136
PAT:137
NLPENDING:138
MATCHALL:139
$)


LET NEXTSYMB() BE
$(1 SYMB := S.NULL

    IF NLPENDING THEN
    $( LINECOUNT := LINECOUNT + 1
       NLPENDING := FALSE
    $)

    SWITCHON CH INTO

$(S CASE '*P':
    CASE '*N': NLPENDING := TRUE
               RCH()
               LOOP

    CASE '*T':
    CASE '*S': RCH() REPEATWHILE CH='*S'
               LOOP

    CASE '0':CASE '1':CASE '2':CASE '3':CASE '4':
    CASE '5':CASE '6':CASE '7':CASE '8':CASE '9':
         READNUMBER(10)
         RETURN

    CASE 'A':CASE 'B':CASE 'C':CASE 'D':CASE 'E':
    CASE 'F':CASE 'G':CASE 'H':CASE 'I':CASE 'J':
    CASE 'K':CASE 'L':CASE 'M':CASE 'N':CASE 'O':
    CASE 'P':CASE 'Q':CASE 'R':CASE 'S':CASE 'T':
    CASE 'U':CASE 'V':CASE 'W':CASE 'X':CASE 'Y':
    CASE 'Z':
    CASE 'a':CASE 'b':CASE 'c':CASE 'd':CASE 'e':
    CASE 'f':CASE 'g':CASE 'h':CASE 'i':CASE 'j':
    CASE 'k':CASE 'l':CASE 'm':CASE 'n':CASE 'o':
    CASE 'p':CASE 'q':CASE 'r':CASE 's':CASE 't':
    CASE 'u':CASE 'v':CASE 'w':CASE 'x':CASE 'y':
    CASE 'z':
         RDTAG(CH)
         SYMB := LOOKUPWORD()
         UNLESS SYMB=S.GET RETURN
         PERFORMGET()
         LOOP

    CASE '$': RCH()
              SYMB := 0
              IF CH=')' DO SYMB := S.RSECT
              TEST CH='(' | CH=')'
                THEN RDTAG('$')
                ELSE RCH()
              RETURN

    CASE '[':
    CASE '(': SYMB := S.LPAREN
              RCH()
              RETURN

    CASE '=': SYMB := S.EQ
              RCH()
              RETURN

    CASE '#':
       $( LET RADIX = 8
          RCH()
          IF CH='B' DO RADIX := 2
          IF CH='X' DO RADIX := 16
          UNLESS 'O'<=CH<='7' DO RCH()
          READNUMBER(RADIX)
          RETURN  $)

    CASE '|': RCH()
              IF CH='|' DO
              $( RCH() REPEATUNTIL CH='*N' \/ CH=ENDSTREAMCH
                 LOOP
              $)
              RETURN

    CASE '/':
         RCH()
         IF CH='\' DO $( RCH(); LOOP  $)
         IF CH='/' DO
            $( RCH() REPEATUNTIL CH='*N' | CH=ENDSTREAMCH
               LOOP  $)

         UNLESS CH='**' RETURN

         $( RCH()
            IF CH='**' DO
               $( RCH() REPEATWHILE CH='**'
                  IF CH='/' BREAK  $)
            IF CH='*N' DO LINECOUNT := LINECOUNT+1
         $) REPEATUNTIL CH=ENDSTREAMCH

         RCH()
         LOOP


    CASE '<':
    CASE '>':
    CASE '\': RCH()
              IF CH='=' DO RCH()
              RETURN

    CASE '-': RCH()
              IF CH='>' DO RCH()
              RETURN

    CASE ':': RCH()
              IF CH='=' DO $( RCH(); RETURN  $)
              SYMB := S.COLON
              RETURN


    CASE '"': RCH()
              CHARV!0 := 0
              FOR I = 1 TO 255 DO
              $( IF CH='"' BREAK
                 CHARV!0 := I
                 CHARV!I := RDSTRCH()  $)
              WORDSIZE := PACKSTRING(CHARV, WORDV)
              SYMB := S.STRING
              RCH()
              RETURN

    CASE '*'':RCH()
              RDSTRCH()
              RCH()
              RETURN


    CASE '.': UNLESS GETP=0 DO CH := ENDSTREAMCH
    DEFAULT:  UNLESS CH=ENDSTREAMCH DO
              $( RCH()
                 RETURN  $)
              IF GETP=0 DO $( SYMB := S.END
                              RETURN   $)
              ENDREAD()
              GETP := GETP - 3
              SOURCESTREAM := GETV!GETP
              SELECTINPUT(SOURCESTREAM)
              LINECOUNT := GETV!(GETP+1)
              CH := GETV!(GETP+2)
              LOOP
$)S
$)1 REPEAT

LET LOOKUPWORD() = VALOF
$(1 LET P = @NAMETREE

    WORDNODE := !P

    UNTIL WORDNODE=0 DO
    $( LET CMP = CMPSTR(WORDV, WORDNODE+4)
       IF CMP=0 RESULTIS !WORDNODE
       P := WORDNODE + (CMP<0->1,2)
       WORDNODE := !P  $)

    WORDNODE := NEWVEC(WORDSIZE+4)
    WORDNODE!0, WORDNODE!1 := S.NAME, 0
    WORDNODE!2, WORDNODE!3 := 0, 0
    FOR I = 0 TO WORDSIZE DO WORDNODE!(I+4) := WORDV!I

    !P := WORDNODE
    IF MATCHALL | MATCH(1,1) DO WORDNODE!0 := S.REFNAME
    RESULTIS WORDNODE!0
$)1

AND CMPSTR(S1, S2) = VALOF
$(1 LET LEN1, LEN2 = GETBYTE(S1,0), GETBYTE(S2,0)
    FOR I = 1 TO LEN1 DO
    $( LET CH1, CH2 = GETBYTE(S1,I), GETBYTE(S2,I)
       IF I>LEN2  RESULTIS 1
       IF CH1>CH2 RESULTIS 1
       IF CH1<CH2 RESULTIS -1  $)
    IF LEN1<LEN2 RESULTIS -1
    RESULTIS 0
$)1

AND DECLSYSWORDS() BE
$(1 PTR := TABLE
      0,S.AND,
      S.BE,0,0,
      S.CASE,
      0,0,
      S.EQ,0,0,0,
      0,S.FOR,0,
      0,0,0,S.GLOBAL,S.GET,
      0,0,
      S.LET,0,0,0,0,0,0,0,
      S.MANIFEST,
      0,0,0,0,
      0,
      0,0,0,0,0,
      0,0,0,
      0,0,S.STATIC,
      0,0,0,0,0,
      0,0,
      0,0,
      0

    D("ABS/AND/*
      *BE/BREAK/BY/*
      *CASE/*
      *DO/DEFAULT/*
      *EQ/EQV/ELSE/ENDCASE/*
      *FALSE/FOR/FINISH/*
      *GOTO/GE/GR/GLOBAL/GET/*
      *IF/INTO/*
      *LET/LV/LE/LS/LOGOR/LOGAND/LOOP/LSHIFT//")

    D("MANIFEST/*
      *NE/NEEDS/NOT/NEQV/*
      *OR/*
      *RESULTIS/RETURN/REM/RSHIFT/RV/*
      *REPEAT/REPEATWHILE/REPEATUNTIL/*
      *SECTION/SWITCHON/STATIC/*
      *TO/TEST/TRUE/THEN/TABLE/*
      *UNTIL/UNLESS/*
      *VEC/VALOF/*
      *WHILE//")

$)1


AND D(WORDS) BE
$(1 LET I, LENGTH = 1, 0

    $( LET CH = GETBYTE(WORDS, I)
       TEST CH='/'
           THEN $( IF LENGTH=0 RETURN
                   CHARV!0 := LENGTH
                   WORDSIZE := PACKSTRING(CHARV, WORDV)
                   LOOKUPWORD()
                   !WORDNODE := !PTR
                   PTR := PTR + 1
                   LENGTH := 0  $)
           ELSE $( LENGTH := LENGTH + 1
                   CHARV!LENGTH := CH  $)
       I := I + 1
    $) REPEAT
$)1



LET RCH() BE CH := RDCH()

AND RDTAG(CHAR1) BE
    $( LET N = 1
       CHARV!1 := CHAR1

       $( RCH()
          UNLESS 'A'<=CH<='Z' |
                 'a'<=CH<='z' |
                 '0'<=CH<='9' |
                  CH='.' BREAK
          N := N+1
          CHARV!N := CH  $) REPEAT

       CHARV!0 := N
       WORDSIZE := PACKSTRING(CHARV, WORDV)  $)


AND PERFORMGET() BE
$( LET S = 0
   NEXTSYMB()
   IF SYMB=S.STRING DO S := FINDINPUT(WORDV)
   IF S=0 DO $( ERROR("BAD GET DIRECTIVE")
                RETURN  $)
   GETV!GETP := SOURCESTREAM
   GETV!(GETP+1) := LINECOUNT
   GETV!(GETP+2) := CH
   GETP := GETP + 3
   LINECOUNT := 1
   SOURCESTREAM := S
   SELECTINPUT(SOURCESTREAM)
   RCH()   $)



AND READNUMBER(RADIX) BE UNTIL VALUE(CH)>=RADIX DO RCH()

AND VALUE(CH) = '0'<=CH<='9' -> CH-'0',
                'A'<=CH<='F' -> CH-'A'+10,
                'a'<=CH<='f' -> CH-'a'+10,
                100

AND RDSTRCH() = VALOF
$(1 LET K = CH

    RCH()

    IF K='*N' DO ERROR("BAD STRING")

    IF K='**' DO
       $( IF CH='*N' | CH='*S' | CH='*T' DO
          $( $( IF CH='*N' DO LINECOUNT := LINECOUNT+1
                RCH()
             $) REPEATWHILE CH='*N' | CH='*S' | CH='*T'
             RCH()
             RESULTIS RDSTRCH()
          $)

          RCH()  $)

    RESULTIS K
$)1

LET NEWVEC(N) = VALOF
    $( TREEP := TREEP - N - 1
       IF TREEP-TREEVEC<=0 DO
       $( ERROR("PROGRAM TOO LARGE")
          FINISH  $)
       RESULTIS TREEP  $)

AND LIST2(X, Y) = VALOF
    $( LET P = NEWVEC(1)
       P!0, P!1 := X, Y
       RESULTIS P   $)


AND ADDREF(LINE, TYPE, NAME) BE
$(1 LET P = NAME+3
    UNTIL !P=0 DO P := !P
    !P := LIST2(0, LINE<<3 | TYPE)  $)1

AND XREF(V,SIZE) BE
$(1 LET TYPE = S.NULL
    LET CONTEXT = S.NULL

    TREEVEC, TREEP := V, V+SIZE

    NAMETREE := 0
    CONTEXT := S.NULL
    NLPENDING := FALSE
    LINECOUNT := 1
    DECLSYSWORDS()
    RCH()
    NEXTSYMB()

    UNTIL SYMB=S.END SWITCHON SYMB INTO
    $(2 CASE S.GLOBAL:
        CASE S.STATIC:
        CASE S.MANIFEST: TYPE := S.NULL
                         CONTEXT := SYMB
        DEFAULT:         NEXTSYMB()
                         LOOP

        CASE S.AND: SYMB := S.LET
        CASE S.CASE:
        CASE S.FOR:
        CASE S.LET: TYPE := SYMB
                    NEXTSYMB()
                    LOOP

        CASE S.RSECT: TYPE, CONTEXT := S.NULL, S.NULL
                      NEXTSYMB()
                      LOOP

        CASE S.COLON: IF TYPE=S.CASE DO TYPE := S.NULL
                      NEXTSYMB()
                      LOOP

        CASE S.BE:
        CASE S.EQ: IF TYPE=S.LET | TYPE=S.FOR DO TYPE := S.NULL
                   NEXTSYMB()
                   LOOP

        CASE S.REFNAME:
        CASE S.NAME:
        $( LET T = TYPE
           LET OLDSYMB = SYMB
           LET LINE, NAME = LINECOUNT, WORDNODE
           NEXTSYMB()
           IF SYMB=S.COLON TEST TYPE=S.NULL
                           THEN T := S.LAB
                           ELSE T := S.NULL

           IF (SYMB=S.COLON | SYMB=S.EQ) & CONTEXT NE S.NULL DO
              T := CONTEXT

           IF TYPE=S.LET DO T := SYMB=S.LPAREN -> S.PROC, S.LET
           IF T=S.CASE DO T := S.NULL

           IF OLDSYMB=S.REFNAME THEN ADDREF(LINE, T, NAME)
           LOOP
        $)
    $)2

    PRTREE(NAMETREE)

    WRITEF("*NSPACE USED %N*N", V+SIZE-TREEP)
$)1

AND PRTREE(T) BE UNLESS T=0 DO
$( PRTREE(T!1)
   WRNAMEINFO(T)
   PRTREE(T!2)  $)

AND WRNAMEINFO(T) BE IF !T=S.REFNAME DO
$(1 LET N = T+4
    LET L = T!3
    LET CHP = GETBYTE(N,0) + 1
//  IF L=0 | !L=0 RETURN  // IGNORE ONCE ONLY  NAMES
    WRITEF("%S ", N)
    UNTIL CHP REM 6 = 0 & CHP>=12 DO $( WRCH('*S')
                                        CHP := CHP+1  $)
    UNTIL L=0 DO
    $( LET A = L!1
       IF CHP>=60 DO $( WRITES("*N            ")
                        CHP := 12  $)
       WRITEF(" %I4%C", A>>3, (A&7)!TABLE
                        '*S','V','P','L','G','M','S','F')
       CHP := CHP+6
       L := !L
    $)

    NEWLINE()
$)1

AND ERROR(MESS) BE WRITEF("LINE %N %S*N", LINECOUNT, MESS)

LET MATCH(P, S) = VALOF
$( LET PLEN = GETBYTE(PAT, 0)
   LET PCH = GETBYTE(PAT,P)
   LET SLEN = GETBYTE(WORDV, 0)
   LET SCH = GETBYTE(WORDV, S)

   IF S>SLEN TEST P>PLEN
             THEN RESULTIS TRUE
             ELSE TEST PCH='**'
                  THEN RESULTIS MATCH(P+1, S)
                  ELSE RESULTIS FALSE

   IF P>PLEN RESULTIS FALSE

   IF PCH='**' TEST MATCH(P+1, S)
               THEN RESULTIS TRUE
               ELSE RESULTIS MATCH(P, S+1)

   IF 'a'<=SCH<='z' DO SCH := SCH + 'A' - 'a'

   IF PCH=SCH RESULTIS MATCH(P+1, S+1)

   RESULTIS FALSE
$)

AND START(PARM) BE
$(1 LET V1 = VEC 128
    AND V2 = VEC 256
    AND V3 = VEC 20

    LET SIZE = STACKEND-STACKBASE
    SIZE := SIZE - 500 - SIZE/8
    WORDV := V1
    CHARV := V2
    GETV, GETP, GETT := V3, 0, 20

    IF GETBYTE(PARM,GETBYTE(PARM,0))='*N' DO
           PUTBYTE(PARM,0,GETBYTE(PARM,0)-1)

    PAT := PARM
    MATCHALL := FALSE

    IF GETBYTE(PAT,0)=0 DO PAT := "**"

    IF GETBYTE(PAT,0)=1 & GETBYTE(PAT,1)='**' DO
       MATCHALL := TRUE
    FOR I = 1 TO GETBYTE(PAT,0) DO
    $( LET K = GETBYTE(PAT,I)
       IF 'a'<=K<='z' DO PUTBYTE(PAT,I,K+'A'-'a')
    $)

    WRITEF("*NWORK SPACE %N", SIZE)
    WRITEF("*NCROSS REFERENCE USING PATTERN *"%S*"*N*N",PAT)

    SOURCESTREAM := INPUT()
    SELECTINPUT(SOURCESTREAM)

    APTOVEC(XREF,SIZE)

$)1
