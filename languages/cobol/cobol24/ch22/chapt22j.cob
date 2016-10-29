000010 @OPTIONS MAIN,TEST
000020 Identification Division.
000030 Program-Id.  Chapt22j.
000031*random Function
000040 Environment Division.
000050 Configuration Section.
000051 Source-Computer.  IBM-PC.
000055 Object-Computer.  IBM-PC.
000056 Data Division.
000057 Working-Storage Section.
000058 01  Random-Seed      Pic 9(8)      Value Zeros.
000059 01  Random-Number    Pic 99        Value Zeros.
000069 01  Random-Generate  Pic V9(18)    Value Zeros.
000148 Procedure Division.
000159 Chapt22j-Start.
000160     Move Function Current-Date (9:8) To Random-Seed
000162     Compute Random-Generate = Function Random (Random-Seed)
000163     Compute Random-Number = (Random-Generate * 21) + 1
000164     Display Random-Number
000165     Perform 19 Times
000166        Compute Random-Generate = Function Random
000167        Compute Random-Number = (Random-Generate * 21) + 1
000169        Display Random-Number
000170     End-Perform
000171     Stop Run
000172     .