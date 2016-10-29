000010 @OPTIONS MAIN,TEST
000020 Identification Division.
000030 Program-Id.  Chapt12e.
000044 Environment Division.
000050 Configuration Section.
000051 Source-Computer.  IBM-PC.
000055 Object-Computer.  IBM-PC.
000056 Data Division.
000057 Working-Storage Section.
000067 01  State-Table-Area.
000068     03  State-Table-Data.
000077         05  Filler Pic X(37) Value "ALBirmingham     Alabama".
000087         05  Filler Pic X(37) Value "ALMontgomery     Alabama".
000097         05  Filler Pic X(37) Value "AZPhoenix        Arizona".
000107         05  Filler Pic X(37) Value "AZTucson         Arizona".
000199     03  State-Table Redefines State-Table-Data.
000200         05  State-Table-Occurrences  Occurs 4 Times
000201             Indexed By Table-Index
000202             Ascending Key State-Abbrev City-Name.
000203             10  State-Abbrev        Pic XX.
000204             10  City-Name           Pic X(15).
000205             10  State-Name          Pic X(20).
000206 Procedure Division.
000207 Chapt12e-Start.
000216     Search All State-Table-Occurrences
000218            At End Display "State Not Found"
000219            When State-Abbrev (Table-Index) = "AZ"
000220            And  City-Name (Table-Index) = "Phoenix"
000221                 Display "State = "
000222                         State-Name (Table-Index)
000223     End-Search
000224     Stop Run
000344     .
