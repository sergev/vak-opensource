000010 @OPTIONS MAIN
000020 Identification Division.
000030 Program-Id.  Chapt09a.
000031* Simple Evaluate Example
000040 Environment Division.
000050 Configuration Section.
000051 Source-Computer.  IBM-PC.
000055 Object-Computer.  IBM-PC.
000056 Data Division.
000057 Working-Storage Section.
000067 01  Data-Item-A         Pic X Value "A".
000068 01  Data-Item-B         Pic X Value "B".
000072 Procedure Division.
000159 Chapt09a-Start.
000160     Evaluate Data-Item-A = Data-Item-B
000170        When True
000180           Display "Items are Equal"
000190        When False
000191           Display "Items are not equal"
000192     End-Evaluate
000193     Evaluate True
000194        When Data-Item-A = Data-Item-B
000195           Display "Items are Equal"
000196        When Data-Item-A Not = Data-Item-B
000197           Display "Items are not equal"
000198     End-Evaluate
000199     Stop Run
000200     .