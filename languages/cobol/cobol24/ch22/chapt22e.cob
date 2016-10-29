000010 @OPTIONS MAIN,TEST
000020 Identification Division.
000030 Program-Id.  Chapt22e.
000031* Variable Length Table Function Length
000040 Environment Division.
000050 Configuration Section.
000051 Source-Computer.  IBM-PC.
000055 Object-Computer.  IBM-PC.
000056 Data Division.
000057 Working-Storage Section.
000067 01  Variable-Table.
000077     03  Table-Items Occurs 1 To 500 Times
000087                     Depending On Table-Occurrences.
000097         05  Table-Element   Pic 9(3).
000098 01  Table-Occurrences       Pic 9(3) Value 237.
000108 01  Item-Length     Pic 9(5) Value Zeros.
000148 Procedure Division.
000159 Chapt22e-Start.
000309     Compute Item-Length = Function Length (Variable-Table)
000310     Display "Item Length " Item-Length.
000311     Stop Run.
000319     .