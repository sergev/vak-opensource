000010 @OPTIONS MAIN
000020 Identification Division.
000030 Program-Id.  Chapt09b.
000031* Simple Evaluate Example 2
000040 Environment Division.
000050 Configuration Section.
000051 Source-Computer.  IBM-PC.
000055 Object-Computer.  IBM-PC.
000056 Data Division.
000057 Working-Storage Section.
000067 01  Sale-Price          Pic 9(4)v99 Value 1000.00.
000068 01  Commission-Percent  Pic 999 Value Zeros.
000069 01  Commission          Pic 9(4)v99 Value Zeros.
000072 Procedure Division.
000159 Chapt09b-Start.
000160     Evaluate Sale-Price
000161        When 1000 Thru 10000
000162           Move 50 To Commission-Percent
000163        When 500 Thru 1000
000164           Move 25 To Commission-Percent
000165        When 250 Thru 500
000166           Move 10 To Commission-Percent
000167        When Other
000168           Move  5 To Commission-Percent
000169     End-Evaluate
000170     Display "Commission " Commission-Percent
000180     Evaluate True
000190        When Sale-Price >= 1000
000191           Move 50 To Commission-Percent
000192        When Sale-Price >= 500
000193           Move 25 To Commission-Percent
000194        When Sale-Price >= 250
000195           Move 10 To Commission-Percent
000196        When Other
000197           Move 5  To Commission-Percent
000198     End-Evaluate
000199     Display "Commission " Commission-Percent
000200     Evaluate True
000201        When Sale-Price >= 1000
000202           Move 50 To Commission-Percent
000203           Compute Commission Rounded = Sale-Price * .5
000204        When Sale-Price >= 500
000205           Move 25 To Commission-Percent
000206           Compute Commission Rounded = Sale-Price * .25
000207        When Sale-Price >= 250
000208           Move 10 To Commission-Percent
000209           Compute Commission Rounded = Sale-Price * .1
000210        When Other
000211           Move 5  To Commission-Percent
000212           Compute Commission Rounded = Sale-Price * .05
000213     End-Evaluate
000214     Display "Commission " Commission-Percent " " Commission
000215
000216     Stop Run
000217     .