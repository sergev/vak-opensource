000010 @OPTIONS MAIN,TEST
000020 Identification Division.
000030 Program-Id.  Chapt22a.
000031* Functions With Tables As Arguments.
000040 Environment Division.
000050 Configuration Section.
000051 Source-Computer.  IBM-PC.
000055 Object-Computer.  IBM-PC.
000056 Data Division.
000057 Working-Storage Section.
000067 01  Work-Table.
000077     03  Work-Entry Pic 9(3) Occurs 1 To 20 Times
000087                             Depending On Num-Entries.
000097 01  Num-Entries    Pic 9(3) Value 3.
000107 01  Result         Pic 9(3) Value Zeros.
000148 Procedure Division.
000159 Chapt22a-Start.
000169     Move 5 To Work-Entry (1)
000179     Move 20 To Work-Entry (2)
000189     Move 10 To Work-Entry (3)
000199     Compute Result = Function Min (Work-Entry (All))
000209     Display "Min = " Result
000309     Stop Run.
000319     .