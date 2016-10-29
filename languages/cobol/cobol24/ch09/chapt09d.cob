000010 @OPTIONS MAIN
000020 Identification Division.
000030 Program-Id.  Chapt09d.
000031* Complex Evaluate Example 2
000040 Environment Division.
000050 Configuration Section.
000051 Source-Computer.  IBM-PC.
000055 Object-Computer.  IBM-PC.
000056 Data Division.
000057 Working-Storage Section.
000058 01  Sale-Price          Pic 9(4)v99 Value 7547.00.
000059 01  Commission-Amount   Pic 9(4)v99 Value Zeros.
000072 Procedure Division.
000159 Chapt09d-Start.
000160     Evaluate True Also True
000161       When Sale-Price >= 1000 Also Sale-Price * .5 > 750.00
000162            Move 750.00 To Commission-Amount
000163       When Sale-Price >= 1000 Also Any
000164            Compute Commission-Amount = Sale-Price * .5
000165       When Sale-Price >= 500  Also Sale-Price * .25 > 150.00
000166            Move 150.00 To Commission-Amount
000167       When Sale-Price >= 500  Also Any
000168            Compute Commission-Amount = Sale-Price * .25
000169       When Sale-Price >= 250  Also Sale-Price * .10 > 30.00
000170            Move 30.00 To Commission-Amount
000171       When Sale-Price >= 250  Also Any
000172            Compute Commission-Amount = Sale-Price * .10
000173       When Other
000174            Compute Commission-Amount = Sale-Price * .05
000175            If Commission-Amount < 1.00
000176               Move 1.00 To Commission-Amount
000177            End-If
000178            If Commission-Amount > Sale-Price
000179               Compute Commission-Amount = Sale-Price * .75
000180            End-If
000181     End-Evaluate
000188     Display "Sale Price " Sale-Price
000189     Display "Commission " Commission-Amount
000190     Stop Run
000217     .