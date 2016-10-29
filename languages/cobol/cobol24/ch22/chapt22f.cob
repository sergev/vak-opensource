000010 @OPTIONS MAIN,TEST
000020 Identification Division.
000030 Program-Id.  Chapt22f.
000031* Function Char
000040 Environment Division.
000050 Configuration Section.
000051 Source-Computer.  IBM-PC.
000055 Object-Computer.  IBM-PC.
000056 Data Division.
000057 Working-Storage Section.
000108 01  Character-Returned   Pic X.
000118 01  Position-Returned    Pic 999 Value Zeros.
000148 Procedure Division.
000159 Chapt22f-Start.
000169     Move Function Char (89) To Character-Returned
000179     Display "Character = " Character-Returned
000189     Compute Position-Returned = Function Ord ("Q")
000190     Display "Position = " Position-Returned
000311     Stop Run.
000319     .