000010 @OPTIONS MAIN
000020 Identification Division.
000030 Program-Id.  Chapt09c.
000031* Complex Evaluate Example
000040 Environment Division.
000050 Configuration Section.
000051 Source-Computer.  IBM-PC.
000055 Object-Computer.  IBM-PC.
000056 Data Division.
000057 Working-Storage Section.
000058 01  Last-Name           Pic X(20) Value "Smith".
000059 01  First-Name          Pic X(20) Value "John".
000060 01  Group-Id            Pic 9     Value Zero.
000072 Procedure Division.
000159 Chapt09c-Start.
000160     Evaluate Last-Name (1:1) Also First-Name (1:1)
000161        When "A" Thru "F" Also "A" Thru "F"
000162           Move 1 To Group-Id
000163        When "A" Thru "F" Also "G" Thru "N"
000164           Move 2 To Group-Id
000165        When "A" Thru "F" Also Any
000166           Move 3 To Group-Id
000167        When "G" Thru "N" Also "A" Thru "F"
000168           Move 4 To Group-Id
000169        When "G" Thru "N" Also "G" Thru "N"
000170           Move 5 To Group-Id
000171        When "G" Thru "N" Also Any
000172           Move 6 To Group-Id
000173        When Any Also "A" Thru "F"
000174           Move 7 To Group-Id
000175        When Any Also "G" Thru "N"
000176           Move 8 To Group-Id
000177        When Other
000178           Move 9 To Group-Id
000179     End-Evaluate
000180     Display "Group-ID = " Group-Id
000181     Stop Run
000217     .