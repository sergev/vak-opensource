000010 @OPTIONS MAIN
000020 Identification Division.
000030 Program-Id.  Chapt11a.
000044 Environment Division.
000050 Configuration Section.
000051 Source-Computer.  IBM-PC.
000055 Object-Computer.  IBM-PC.
000056 Data Division.
000057 Working-Storage Section.
000067 01  Counter Pic 9(2) Value Zeros.
000077 Procedure Division.
000159 Chapt11a-Start.
000160     Perform Count-Routine With Test After Varying Counter
000161             From 1 By 1 Until Counter = 10
000164     Perform Count-Routine With Test After Varying Counter
000165             From 10 By -1 Until Counter = 1
000166     Stop Run
000167     .
000168 Count-Routine.
000169     Display Counter
000170     .