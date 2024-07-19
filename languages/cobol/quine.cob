       IDENTIFICATION DIVISION.
        PROGRAM-ID. QUINE.
        AUTHOR. DAVE BURT.
        DATE-WRITTEN. 18-09-2002.
       ENVIRONMENT DIVISION.
       DATA DIVISION.
        WORKING-STORAGE SECTION.
        01 WS.
         05 WS-DATA.
          09 PIC X(40) VALUE "   09 PIC X(40) VALUE                   ".
          09 PIC X(40) VALUE ".                                       ".
          09 PIC X(40) VALUE "IDENTIFICATION DIVISION.                ".
          09 PIC X(40) VALUE " PROGRAM-ID. QUINE.                     ".
          09 PIC X(40) VALUE " AUTHOR. DAVE BURT.                     ".
          09 PIC X(40) VALUE " DATE-WRITTEN. 18-09-2002.              ".
          09 PIC X(40) VALUE "ENVIRONMENT DIVISION.                   ".
          09 PIC X(40) VALUE "DATA DIVISION.                          ".
          09 PIC X(40) VALUE " WORKING-STORAGE SECTION.               ".
          09 PIC X(40) VALUE " 01 WS.                                 ".
          09 PIC X(40) VALUE "  05 WS-DATA.                           ".
          09 PIC X(40) VALUE "  05 WS-ARR PIC X(40)                   ".
          09 PIC X(40) VALUE "    OCCURS 4 REDEFINES WS-DATA.         ".
          09 PIC X(40) VALUE "  05 WS-INTS USAGE IS COMPUTATIONAL.    ".
          09 PIC X(40) VALUE "   09 WS-NDX   PIC 9(4) VALUE ZERO.     ".
          09 PIC X(40) VALUE "   09 WS-START PIC 9(4) VALUE 3.        ".
          09 PIC X(40) VALUE "   09 WS-SPLIT PIC 9(4) VALUE 12.       ".
          09 PIC X(40) VALUE "   09 WS-CNT   PIC 9(4) VALUE 51.       ".
          09 PIC X(40) VALUE " 01 WS-OUT-REC.                         ".
          09 PIC X(40) VALUE "  05 FILLER      PIC X(7) VALUE SPACES. ".
          09 PIC X(40) VALUE "  05 WS-OUT-AREA.                       ".
          09 PIC X(40) VALUE "   09 WS-OUT-PFX PIC X(22).             ".
          09 PIC X(40) VALUE "   09 WS-OUT-Q1  PIC X.                 ".
          09 PIC X(40) VALUE "   09 WS-OUT-STR PIC x(40).             ".
          09 PIC X(40) VALUE "   09 WS-OUT-Q2  PIC X.                 ".
          09 PIC X(40) VALUE "   09 WS-OUT-SFX PIC X.                 ".
          09 PIC X(40) VALUE "PROCEDURE DIVISION.                     ".
          09 PIC X(40) VALUE "  000-MAIN.                             ".
          09 PIC X(40) VALUE "    PERFORM VARYING WS-NDX              ".
          09 PIC X(40) VALUE "            FROM WS-START BY 1          ".
          09 PIC X(40) VALUE "            UNTIL WS-NDX >= WS-SPLIT    ".
          09 PIC X(40) VALUE "     MOVE WS-ARR(WS-NDX) TO WS-OUT-AREA ".
          09 PIC X(40) VALUE "     DISPLAY WS-OUT-REC                 ".
          09 PIC X(40) VALUE "    END-PERFORM                         ".
          09 PIC X(40) VALUE "    MOVE WS-ARR(1) TO WS-OUT-PFX        ".
          09 PIC X(40) VALUE "    MOVE QUOTE TO WS-OUT-Q1             ".
          09 PIC X(40) VALUE "    MOVE QUOTE TO WS-OUT-Q2             ".
          09 PIC X(40) VALUE "    MOVE WS-ARR(2) TO WS-OUT-SFX        ".
          09 PIC X(40) VALUE "    PERFORM VARYING WS-NDX              ".
          09 PIC X(40) VALUE "            FROM 1 BY 1                 ".
          09 PIC X(40) VALUE "            UNTIL WS-NDX > WS-CNT       ".
          09 PIC X(40) VALUE "     MOVE WS-ARR(WS-NDX) TO WS-OUT-STR  ".
          09 PIC X(40) VALUE "     DISPLAY WS-OUT-REC                 ".
          09 PIC X(40) VALUE "    END-PERFORM                         ".
          09 PIC X(40) VALUE "    PERFORM VARYING WS-NDX              ".
          09 PIC X(40) VALUE "            FROM WS-SPLIT BY 1          ".
          09 PIC X(40) VALUE "            UNTIL WS-NDX > WS-CNT       ".
          09 PIC X(40) VALUE "     MOVE WS-ARR(WS-NDX) TO WS-OUT-AREA ".
          09 PIC X(40) VALUE "     DISPLAY WS-OUT-REC                 ".
          09 PIC X(40) VALUE "    END-PERFORM                         ".
          09 PIC X(40) VALUE "    STOP RUN.                           ".
         05 WS-ARR PIC X(40)
          OCCURS 4 REDEFINES WS-DATA.
         05 WS-INTS USAGE IS COMPUTATIONAL.
          09 WS-NDX   PIC 9(4) VALUE ZERO.
          09 WS-START PIC 9(4) VALUE 3.
          09 WS-SPLIT PIC 9(4) VALUE 12.
          09 WS-CNT   PIC 9(4) VALUE 51.
        01 WS-OUT-REC.
         05 FILLER      PIC X(7) VALUE SPACES.
         05 WS-OUT-AREA.
          09 WS-OUT-PFX PIC X(22).
          09 WS-OUT-Q1  PIC X.
          09 WS-OUT-STR PIC x(40).
          09 WS-OUT-Q2  PIC X.
          09 WS-OUT-SFX PIC X.
       PROCEDURE DIVISION.
         000-MAIN.
           PERFORM VARYING WS-NDX
                   FROM WS-START BY 1
                   UNTIL WS-NDX >= WS-SPLIT
            MOVE WS-ARR(WS-NDX) TO WS-OUT-AREA
            DISPLAY WS-OUT-REC
           END-PERFORM
           MOVE WS-ARR(1) TO WS-OUT-PFX
           MOVE QUOTE TO WS-OUT-Q1
           MOVE QUOTE TO WS-OUT-Q2
           MOVE WS-ARR(2) TO WS-OUT-SFX
           PERFORM VARYING WS-NDX
                   FROM 1 BY 1
                   UNTIL WS-NDX > WS-CNT
            MOVE WS-ARR(WS-NDX) TO WS-OUT-STR
            DISPLAY WS-OUT-REC
           END-PERFORM
           PERFORM VARYING WS-NDX
                   FROM WS-SPLIT BY 1
                   UNTIL WS-NDX > WS-CNT
            MOVE WS-ARR(WS-NDX) TO WS-OUT-AREA
            DISPLAY WS-OUT-REC
           END-PERFORM
           STOP RUN.
