//     This program is an ASCII INTCODE assembler and interpreter
// for a 16 bit EBCDIC machine,  hence the need for the ASCII and
// EBCDIC tables near the end.  It has been tested on the IBM 370
// (a 32 bit EBCDIC machine).

GET "LIBHDR"

GLOBAL $(
    SYSPRINT:100; SOURCE:101
    ETOA:102; ATOE:103
$)

MANIFEST $(
    FSHIFT = 13

    IBIT = #10000
    PBIT =  #4000
    GBIT =  #2000
    DBIT =  #1000

    ABITS = #777

    WORDSIZE = 16
    BYTESIZE = 8

    LIG1= #012001

    K2  = #140002

    X22 = #160026
$)

GLOBAL $(
    G:110; P:111; CH:112; CYCLECOUNT:113
    LABV:120; CP:121; A:122; B:123; C:124; D:125; W:126
$)

LET ASSEMBLE() BE
$(1   LET V = VEC 500
      LET F = 0
      LABV := V

CLEAR:FOR I = 0 TO 500 DO LABV!I := 0
      CP := 0

NEXT: RCH()
SW:   SWITCHON CH INTO

$(S   DEFAULT: IF CH=ENDSTREAMCH RETURN
               writef("*NBAD CH %C AT P = %N*N", CH, P)
               GOTO NEXT

      CASE '0':CASE '1':CASE '2':CASE '3':CASE '4':
      CASE '5':CASE '6':CASE '7':CASE '8':CASE '9':
               SETLAB(RDN())
               CP := 0
               GOTO SW

      CASE '$':CASE '*S':CASE '*N': GOTO NEXT

      CASE 'L': F := 0; ENDCASE
      CASE 'S': F := 1; ENDCASE
      CASE 'A': F := 2; ENDCASE
      CASE 'J': F := 3; ENDCASE
      CASE 'T': F := 4; ENDCASE
      CASE 'F': F := 5; ENDCASE
      CASE 'K': F := 6; ENDCASE
      CASE 'X': F := 7; ENDCASE

      CASE 'C': RCH(); STC(RDN()); GOTO SW

      CASE 'D': RCH()
                TEST CH='L'
                  THEN $( RCH()
                          STW(0)
                          LABREF(RDN(), P-1)  $)
                  OR STW(RDN())
                GOTO SW

      CASE 'G': RCH()
                A := RDN() + G
                TEST CH='L' THEN RCH()
                      OR writef("*NBAD CODE AT P = %N*N", P)
                !A := 0
                LABREF(RDN(), A)
                GOTO SW

      CASE 'Z': FOR I = 0 TO 500 DO
                   IF LABV!I>0 DO writef("L%N UNSET*N", I)
                GOTO CLEAR  $)S


      W := F<<FSHIFT
      RCH()
      IF CH='I' DO $( W := W+IBIT; RCH() $)
      IF CH='P' DO $( W := W+PBIT; RCH() $)
      IF CH='G' DO $( W := W+GBIT; RCH() $)

      TEST CH='L'

        THEN $( RCH()
                STW(W+DBIT)
                STW(0)
                LABREF(RDN(), P-1)  $)

        OR   $( LET A = RDN()
                TEST (A&ABITS)=A
                  THEN STW(W+A)
                  OR $( STW(W+DBIT); STW(A)  $)  $)

      GOTO SW   $)1

AND STW(W) BE $( !P := W
                 P, CP := P+1, 0  $)

AND STC(C) BE $( IF CP=0 DO $( STW(0); CP := WORDSIZE  $)
                 CP := CP - BYTESIZE
                 !(P-1) := !(P-1) + (C<<CP)  $)

AND RCH() BE $(1 CH := rdch()
                 UNLESS CH='/' RETURN
                 UNTIL CH='*N' DO CH := rdch()  $)1 REPEAT

AND RDN() = VALOF
    $( LET A, B = 0, FALSE
       IF CH='-' DO $( B := TRUE; RCH()  $)
       WHILE '0'<=CH<='9' DO $( A := 10*A + CH - '0'; RCH()  $)
       IF B DO A := -A
       RESULTIS A  $)

AND SETLAB(N) BE
     $( LET K = LABV!N
        IF K<0 DO writef("L%N ALREADY SET TO %N AT P = %N*N",N,-K,P)
        WHILE K>0 DO $( LET N = !K
                        !K := P
                        K := N  $)
        LABV!N := -P  $)


AND LABREF(N, A) BE
    $( LET K = LABV!N
       TEST K<0 THEN K := -K OR LABV!N := A
       !A := !A + K  $)


AND INTERPRET() = VALOF
$(1

FETCH: CYCLECOUNT := CYCLECOUNT + 1
       W := !C
       C := C + 1

       TEST (W&DBIT)=0
         THEN D := W&ABITS
         OR $( D := !C; C := C+1  $)

       IF (W & PBIT) NE 0 DO D := D + P
       IF (W & GBIT) NE 0 DO D := D + G
       IF (W & IBIT) NE 0 DO D := !D

       SWITCHON W>>FSHIFT INTO

   $(  ERROR:
       DEFAULT: selectoutput(SYSPRINT)
                writef("*NINTCODE ERROR AT C = %N*N", C-1)
                RESULTIS -1

       CASE 0: B := A; A := D; GOTO FETCH

       CASE 1: !D := A; GOTO FETCH

       CASE 2: A := A + D; GOTO FETCH

       CASE 3: C := D; GOTO FETCH

       CASE 4: A := NOT A

       CASE 5: UNLESS A DO C := D; GOTO FETCH

       CASE 6: D := P + D
               D!0, D!1 := P, C
               P, C := D, A
               GOTO FETCH

       CASE 7: SWITCHON D INTO

       $(  DEFAULT: GOTO ERROR

           CASE 1:  A := !A; GOTO FETCH
           CASE 2:  A := -A;     GOTO FETCH
           CASE 3:  A := NOT A; GOTO FETCH
           CASE 4:  C := P!1
                    P := P!0
                    GOTO FETCH
           CASE 5:  A := B * A; GOTO FETCH
           CASE 6:  A := B / A; GOTO FETCH
           CASE 7:  A := B REM A; GOTO FETCH
           CASE 8:  A := B + A; GOTO FETCH
           CASE 9:  A := B - A; GOTO FETCH
           CASE 10: A := B = A; GOTO FETCH
           CASE 11: A := B NE A; GOTO FETCH
           CASE 12: A := B < A; GOTO FETCH
           CASE 13: A := B >= A; GOTO FETCH
           CASE 14: A := B > A; GOTO FETCH
           CASE 15: A := B <= A; GOTO FETCH
           CASE 16: A := B << A; GOTO FETCH
           CASE 17: A := B >> A; GOTO FETCH
           CASE 18: A := B & A; GOTO FETCH
           CASE 19: A := B LOGOR A; GOTO FETCH
           CASE 20: A := B NEQV A; GOTO FETCH
           CASE 21: A := B EQV A;  GOTO FETCH

           CASE 22: RESULTIS 0   // FINISH

           CASE 23: B, D := C!0, C!1   // SWITCHON
                    UNTIL B=0 DO
                        $( B, C := B-1, C+2
                           IF A=C!0 DO
                                  $( D := C!1
                                     BREAK  $)  $)
                     C := D
                     GOTO FETCH

// CASES 24 UPWARDS ARE ONLY CALLED FROM THE FOLLOWING
// HAND WRITTEN INTCODE LIBRARY - ICLIB:

//    11 LIP2 X24 X4 G11L11 /selectinput
//    12 LIP2 X25 X4 G12L12 /selectoutput
//    13 X26 X4      G13L13 /rdch
//    14 LIP2 X27 X4 G14L14 /wrch
//    42 LIP2 X28 X4 G42L42 /findinput
//    41 LIP2 X29 X4 G41L41 /findoutput
//    30 LIP2 X30 X4 G30L30 /stop
//    31 X31 X4 G31L31 /level
//    32 LIP3 LIP2 X32 G32L32 /longjump
//    46 X33 X4 G46L46 /endread
//    47 X34 X4 G47L47 /endwrite
//    40 LIP3 LIP2 X35 G40L40 /aptovec
//    85 LIP3 LIP2 X36 X4 G85L85 / getbyte
//    86 LIP3 LIP2 X37 X4 G86L86 / putbyte
//    Z

           CASE 24: selectinput(A); GOTO FETCH
           CASE 25: selectoutput(A); GOTO FETCH
           CASE 26: A := ETOA!rdch(); GOTO FETCH
           CASE 27: wrch(ATOE!A); GOTO FETCH
           CASE 28: A := findinput(STRING370(A)); GOTO FETCH
           CASE 29: A := findoutput(STRING370(A)); GOTO FETCH
           CASE 30: RESULTIS A  // stop(A)
           CASE 31: A := P!0; GOTO FETCH  // USED IN level()
           CASE 32: P, C := A, B;         // USED IN longjump(P,L)
                    GOTO FETCH
           CASE 33: endread(); GOTO FETCH
           CASE 34: endwrite(); GOTO FETCH
           CASE 35: D := P+B+1        // USED IN aptovec(F, N)
                    D!0, D!1, D!2, D!3 := P!0, P!1, P, B
                    P, C := D, A
                    GOTO FETCH
           CASE 36: A := icgetbyte(A, B)  // getbyte(S, I)
                    GOTO FETCH
           CASE 37: icputbyte(A, B, P!4)  // putbyte(S, I, CH)
                    GOTO FETCH
      $)  $)  $)1


AND STRING370(S) = VALOF
     $( LET T = TABLE 0,0,0,0,0,0,0,0

        putbyte(T, 0, icgetbyte(S, 0))
        FOR I = 1 TO icgetbyte(S,0) DO
                  putbyte(T,I,ATOE!icgetbyte(S,I))

        RESULTIS T  $)

AND icgetbyte(S, I) = VALOF
     $( LET W = S!(I/2)
        IF (I&1)=0 DO W := W>>8
        RESULTIS W&255  $)

AND icputbyte(S, I, CH) BE
     $( LET P = @S!(I/2)
        LET W = !P
        TEST (I&1)=0 THEN !P := W&#X00FF \/ CH<<8
                     OR   !P := W&#XFF00 \/ CH    $)

LET start(PARM) BE
$(1

LET PROGVEC = VEC 20000
LET GLOBVEC = VEC 400

G, P := GLOBVEC, PROGVEC

SYSPRINT := findoutput("SYSPRINT")
selectoutput(SYSPRINT)

writes("INTCODE SYSTEM ENTERED*N")

SOURCE := findinput("INTIN")
selectinput(SOURCE)
ASSEMBLE()
SOURCE := findinput("SYSIN")
UNLESS SOURCE=0 DO selectinput(SOURCE)

writef("*NPROGRAM SIZE = %N*N", P-PROGVEC)

ATOE := 1+TABLE -1,
          0,  0,  0,  0,  0,  0,  0,  0,  // ASCII TO EBCDIC
          0,  5, 21,  0, 12,  0,  0,  0,  // '*T' '*N' '*P'
          0,  0,  0,  0,  0,  0,  0,  0,
          0,  0,  0,  0,  0,  0,  0,  0,

         64, 90,127,123, 91,108, 80,125, // '*S' ! " # $ % & '
         77, 93, 92, 78,107, 96, 75, 97, //   (  ) * + , - . /
        240,241,242,243,244,245,246,247, //   0  1 2 3 4 5 6 7
        248,249,122, 94, 76,126,110,111, //   8  9 : ; < = > ?
        124,193,194,195,196,197,198,199, //   @  A B C D E F G
        200,201,209,210,211,212,213,214, //   H  I J K L M N O
        215,216,217,226,227,228,229,230, //   P  Q R S T U V W
        231,232,233, 66, 98, 67,101,102, //   X  Y Z [ \ ] ^ _
         64,129,130,131,132,133,134,135, //      a b c d e f g
        136,137,145,146,147,148,149,150, //   h  i j k l m n o
        151,152,153,162,163,164,165,166, //   p  q r s t u v w
        167,168,169, 64, 79, 64, 95,255  //   x  y z   |   ~


ETOA := 1+TABLE -1,
      0,   0,   0,   0,   0, #11,   0,   0,
      0,   0,   0, #13, #14, #15,   0,   0,
      0,   0,   0,   0,   0, #12,   0,   0,
      0,   0,   0,   0,   0,   0,   0,   0,
      0,   0,   0,   0,   0, #12,   0,   0,
      0,   0,   0,   0,   0,   0,   0,   0,
      0,   0,   0,   0,   0,   0,   0,   0,
      0,   0,   0,   0,   0,   0,   0,   0,
    #40,   0,#133,#135,   0,   0,   0,   0,
      0,   0,   0, #56, #74, #50, #53,#174,
    #46,   0,   0,   0,   0,   0,   0,   0,
      0,   0, #41, #44, #52, #51, #73,#176,
    #55, #57,#134,   0,   0,#136,#137,   0,
      0,   0,   0, #54, #45,#140, #76, #77,
      0,   0,   0,   0,   0,   0,   0,   0,
      0,   0, #72, #43,#100, #47, #75, #42,
      0,#141,#142,#143,#144,#145,#146,#147,
   #150,#151,   0,   0,   0,   0,   0,   0,
      0,#152,#153,#154,#155,#156,#157,#160,
   #161,#162,   0,   0,   0,   0,   0,   0,
      0,   0,#163,#164,#165,#166,#167,#170,
   #171,#172,   0,   0,   0,   0,   0,   0,
      0,   0,   0,   0,   0,   0,   0,   0,
      0,   0,   0,   0,   0,   0,   0,   0,
      0,#101,#102,#103,#104,#105,#106,#107,
   #110,#111,   0,   0,   0,   0,   0,   0,
      0,#112,#113,#114,#115,#116,#117,#120,
   #121,#122,   0,   0,   0,   0,   0,   0,
      0,   0,#123,#124,#125,#126,#127,#130,
   #131,#132,   0,   0,   0,   0,   0,   0,
    #60, #61, #62, #63, #64, #65, #66, #67,
    #70, #71,   0,   0,   0,   0,   0,   0



C := TABLE LIG1, K2, X22

CYCLECOUNT := 0
A := INTERPRET()

selectoutput(SYSPRINT)
writef("*N*NEXECUTION CYCLES = %N, CODE = %N*N", CYCLECOUNT, A)
//IF A<0 DO mapstore()
FINISH  $)1
