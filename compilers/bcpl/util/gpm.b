GET "LIBHDR"

GLOBAL $(

ST:100; SMAX:101

A:110; W:111; H:112; P:113
F:114; C:115; S:116; E:117
Q:118

TEMP1:120; TEMP2:121  $)

MANIFEST $(
MARKER=256
S.DEF=-1; S.VAL=-2; S.SET=-3; S.BIN=-4; S.DEC=-5; S.BAR=-6
$)


LET LOAD() BE TEST H=0
                  THEN WRCH(A)
                  ELSE $( ST!S := A
                          S := S+1
                          IF S>SMAX DO REPORT(12)  $)

AND NXTC() BE TEST C=0
                THEN A := RDCH()
                ELSE $( A := ST!C
                        C := C+1  $)

AND FIND(X) BE
    $(1 A, W := E, X

        $( FOR R=0 TO ST!W-1 DO
                   UNLESS ST!(W+R)=ST!(A+R+1) GOTO NEXT
           W := A+1+ST!W
           RETURN

    NEXT:  A := ST!A  $) REPEATUNTIL A<0

        REPORT(7)  $)1

AND ITEM(X) BE
    $( LET LA, LH = A, H
       H := 0
       FOR K = 1 TO ST!X=0->S-X-1,ST!X-1 DO
             $( A := ST!(X+K)
                LOAD()  $)
       IF ST!X=0 DO WRITES("...   INCOMPLETE*N")
       A, H := LA, LH  $)

AND START() BE
$( LET FROMSTREAM = FINDINPUT("FROM")
   LET TOSTREAM   = FINDOUTPUT("TO")
   UNLESS FROMSTREAM=0 DO SELECTINPUT(FROMSTREAM)
   UNLESS TOSTREAM=0   DO SELECTOUTPUT(TOSTREAM)
   APTOVEC(GPM, STACKEND-STACKBASE-500)
$)

AND MCDEF(A, B, C, CODE) BE
      $( ST!S, ST!(S+1), ST!(S+2), ST!(S+3), ST!(S+4) := E, 4, A, B, C
         ST!(S+5) := CODE
         E := S
         S := S+6  $)


AND GPM(V, SIZE) BE
    $(1 ST, SMAX := V, SIZE
        A,W,H,P := 0,0,0,0
        F,C,S,E := 0,0,0,-1
        Q := 1
        MCDEF('D', 'E', 'F', S.DEF)
        MCDEF('V', 'A', 'L', S.VAL)
        MCDEF('S', 'E', 'T', S.SET)
        MCDEF('B', 'I', 'N', S.BIN)
        MCDEF('D', 'E', 'C', S.DEC)
        MCDEF('B', 'A', 'R', S.BAR)

FIRSTINSTR:
        NXTC()

        SWITCHON A INTO

    $(2 CASE '<': Q := Q+1; GOTO Q2

        COPY:
        DEFAULT: LOAD()
                 IF Q=1 GOTO FIRSTINSTR
        Q2:      NXTC()
                 IF A='<' DO $( Q := Q+1; GOTO COPY  $)
                 UNLESS A='>' GOTO COPY
                 Q := Q-1
                 IF Q=1 GOTO FIRSTINSTR
                 GOTO COPY


        // WARNING CHARACTERS
        CASE '$': ST!S, ST!(S+1), ST!(S+2), ST!(S+3) := H, F, 0, 0
                  H, F := S+3, S+1
                  S := S+4
                  IF S>SMAX DO REPORT(12)
                  GOTO FIRSTINSTR

        CASE ',': IF H=0 GOTO COPY
                  ST!H := S-H-ST!H
                  ST!S, H := 0, S
                  S := S+1
                  IF S>SMAX DO REPORT(12)
                  GOTO FIRSTINSTR

        APPLY:
        CASE ';': IF P>F DO $( REPORT(1); GOTO COPY  $)
                  IF H=0 GOTO COPY
                  TEMP1 := ST!F
                  TEMP2 := ST!(F-1)
                  ST!H, ST!S := S-H, MARKER
                  ST!(F-1), ST!F, ST!(F+1) := S-F+2, P, C
                  P, S := F, S+1
                  IF S>SMAX DO REPORT(12)
                  F, H := TEMP1, TEMP2
                  UNLESS H=0 DO ST!H := ST!H+ST!(P-1)
                  FIND(P+2)
                  IF ST!W<0 $( MCMACRO(ST!W)
                               GOTO ENDFN  $)
                  C := W+1
                  GOTO FIRSTINSTR

        CASE '#': IF P=0 DO
                    $( UNLESS H=0 DO REPORT(2)
                       GOTO COPY  $)
                  NXTC()
                  W := P+2
                  IF A<'0' DO REPORT(3)
                  FOR R = 0 TO A-'0'-1 DO
                       $( W := W+ST!W
                          IF ST!W=MARKER DO REPORT(4)  $)
                  FOR R = 1 TO ST!W-1 DO
                       $( A := ST!(W+R)
                          LOAD()  $)
                  GOTO FIRSTINSTR

        ENDFN:
        CASE MARKER:
                  IF F>P DO $( REPORT(5); GOTO APPLY  $)
                  ST!S, A := E, S
                  WHILE ST!A>=P-1+ST!(P-1) DO
                       $( TEMP1 := ST!A
                          ST!A := ST!A-ST!(P-1)
                          A := TEMP1  $)
                  W := ST!A
                  WHILE W>P-1 DO W := ST!W
                  ST!A := W
                  E := ST!S
                  UNLESS H=0 TEST H>P
                               THEN H := H-ST!(P-1)
                               ELSE ST!H := ST!H-ST!(P-1)
                  C, S := ST!(P+1), S-ST!(P-1)
                  A, W := P-1,P-1+ST!(P-1)
                  P := ST!P
                  UNTIL A=S DO
                      $( ST!A := ST!W
                         A, W := A+1, W+1  $)
                  GOTO FIRSTINSTR

        CASE ENDSTREAMCH:
        EXIT:     UNLESS C=0 & H=0 DO REPORT(8)
                  FINISH  $)2
$)1

AND MCMACRO(CODE) BE SWITCHON CODE INTO
   $(1 CASE S.DEF: UNLESS H=0 DO ST!H := ST!H-ST!(P-1)+6
                   ST!(P-1), ST!(P+5) := 6, E
                   E := P+5
                   RETURN

       CASE S.VAL: FIND(P+6)
                   UNTIL ST!(W+1)=MARKER DO
                       $( A := ST!(W+1)
                          W := W+1
                          LOAD()  $)
                   RETURN

       CASE S.SET: FIND(P+6)
                   A := P+6+ST!(P+6)
                   IF ST!A>ST!W DO REPORT(9)
                   FOR R = 1 TO ST!A DO ST!(W+R) := ST!(A+R)
                   RETURN

       CASE S.BIN: W := 0
                   A := P+8
                   UNLESS ST!(P+7)='+' \/ ST!(P+7)='-' DO A := P+7
                   UNTIL ST!A=MARKER DO
                       $( LET X = ST!A-'0'
                          UNLESS 0<=X<=9 DO REPORT(10)
                          W := 10*W+X
                          A := A+1  $)
                   IF ST!(P+7)='-' DO W := -W
                   ST!S := W
                   S := S+1
                   IF S>SMAX DO REPORT(12)
                   RETURN

       CASE S.DEC: W := ST!(P+7)
                   IF W<0 DO $( W, A := -W, '-'
                                LOAD()  $)
                $( LET W1 = 1
                   UNTIL 10*W1>W DO W1 := 10*W1

                   $( A := W/W1+'0'
                      W := W REM W1
                      W1 := W1/10
                      LOAD()  $) REPEATUNTIL W1=0  $)
                   RETURN

       CASE S.BAR: W, A := ST!(P+9), ST!(P+11)
                   SWITCHON ST!(P+7) INTO
                   $( CASE '+': A := W+A; ENDCASE
                      CASE '-': A := W-A; ENDCASE
                      CASE '**':A := W*A; ENDCASE
                      CASE '/': A := W/A; ENDCASE
                      CASE '%': A := W REM A; ENDCASE  $)
                   LOAD()
                   RETURN  $)1

AND WRMES(MES) BE WRITEF("*NMONITOR: %S ", MES)

AND REPORT(N) BE SWITCHON N INTO
    $(1 CASE 1: WRMES("UNMATCHED ';' IN DEFINITION OF ")
                ITEM(P+2)
        M1:     WRITES("*NIF THIS HAD BEEN QUOTED THE RESULT WOULD BE*N")
                RETURN

        CASE 2: WRMES("UNQUOTED '#' IN ARGUMENT LIST OF ")
                ITEM(F+2)
                GOTO M1

        CASE 3: WRMES("IMPOSSIBLE ARG NO IN DEFINITION OF ")
                ITEM(P+2)
                GOTO M11

        CASE 4: WRMES("NO ARGUMENT ")
                H := 0
                LOAD()
                WRITES(" IN CALL FOR ")
                ITEM(P+2)
                GOTO M11

        CASE 5: WRMES("TERMINATOR IN ")
                IF C=0
                 $( WRITES("INPUT STREAM")
                    GOTO M11  $)

                WRITES("ARGUMENT LIST FOR ")
                ITEM(F+2)
                WRITES("*NPROBABLY DUE TO ';' MISSING FROM THE*
                        *DEFINITION OF ")
                ITEM(P+2)
                WRITES("*NIF A FINAL ';' IS ADDED THE RESULT IS*N")
                C := C-1
                RETURN

       CASE 7: WRMES("UNDEFINED NAME ")
               ITEM(W)
               GOTO M11

       CASE 8:
       CASE 9:
       CASE 10: GOTO M11

       CASE 12: WRITES("*NSTACK OVERFLOW - RUN ABANDONED*N")

       M11:  // GENERAL MONITOR AFTER IRREMEDIABLE ERRORS
             W := 20
             WRITES("*NCURRENT MACROS ARE")
             UNTIL P=F=0 DO
               $( LET W1 = 0
                  TEST P>F
                    THEN $( W1 := P+2
                            P := ST!P
                            WRITES("*NALREADY ENTERED ")  $)
                    ELSE $( W1 := F+2
                            F := ST!F
                            WRITES("*NNOT YET ENTERED ")  $)
                  FOR R = 1 TO W DO
                      $( ITEM(W1)
                         IF ST!W1=0 BREAK
                         W1 := W1+ST!W1
                         IF ST!W1=MARKER BREAK
                         UNLESS W=1 DO WRITEF("*NARG%N ", R)  $)

                   W := 1  $)

              NEWLINE()
              FINISH

$)1
