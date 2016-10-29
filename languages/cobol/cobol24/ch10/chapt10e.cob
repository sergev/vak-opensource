000010 @OPTIONS MAIN
000020 Identification Division.
000030 Program-Id.  Chapt10e.
000031* Perform With Go To Example
000040 Environment Division.
000050 Configuration Section.
000051 Source-Computer.  IBM-PC.
000055 Object-Computer.  IBM-PC.
000056 Data Division.
000057 Working-Storage Section.
000058 Procedure Division.
000068 Chapt10a-Section Section.
000159 Chapt10a-Start.
000161     Perform Para-2
000162     Stop Run
000163     .
000171 First-Section Section.
000181 Para-1.
000191     Display "Para 1"
000193     .
000201 Para-2.
000211     Display "Para 2"
000212     Go To Para-1
000221     .
000231 Para-3.
000241     Display "Para 3"
000298     .