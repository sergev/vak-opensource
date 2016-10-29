000010 @OPTIONS MAIN,TEST
000020 Identification Division.
000030 Program-Id.  Chapt22g.
000031* Function Reverse
000040 Environment Division.
000050 Configuration Section.
000051 Source-Computer.  IBM-PC.
000055 Object-Computer.  IBM-PC.
000056 Data Division.
000057 Working-Storage Section.
000067 01  Input-Field          Pic X(15) Value "COBOL".
000108 01  Output-Field         Pic X(15) Value Spaces.
000118 01  Trailing-Spaces      Pic 9(3)  Value Zeros.
000148 Procedure Division.
000159 Chapt22g-Start.
000160     Move Function Reverse (Input-Field) To Output-Field
000161     Inspect Output-Field Tallying
000162             Trailing-Spaces For Leading Spaces
000190     Display "Trailing Spaces = " Trailing-Spaces
000200     Stop Run.
000319     .