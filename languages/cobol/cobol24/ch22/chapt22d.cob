000010 @OPTIONS MAIN,TEST
000020 Identification Division.
000030 Program-Id.  Chapt22d.
000031* Center A String
000040 Environment Division.
000050 Configuration Section.
000051 Source-Computer.  IBM-PC.
000055 Object-Computer.  IBM-PC.
000056 Data Division.
000057 Working-Storage Section.
000107 01  String-Length        Pic 9(6) Value Zeros.
000117 01  Counter              Pic 9(6) Value Zeros.
000127 01  String-To-Center     Pic X(60) Value
000137     "Teach Yourself COBOL in 24 Hours".
000147 01  Centered-String      Pic X(60) Value Spaces.
000148 Procedure Division.
000159 Chapt22d-Start.
000169     If String-To-Center > Spaces
000170        Compute String-Length =
000171                Function Length (String-To-Center)
000179        Perform Varying Counter From
000189           String-Length By -1 Until
000199           String-To-Center (Counter:1) > Spaces
000200           Continue
000209        End-Perform
000219        Compute Counter Rounded = (String-Length - Counter) / 2
000229        Move String-To-Center To
000239             Centered-String (Counter:)
000249     End-If
000259     Display "Centered-String=" Centered-String
000309     Stop Run.
000319     .