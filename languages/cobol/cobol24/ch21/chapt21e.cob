000010 @OPTIONS MAIN,TEST
000020 Identification Division.
000030 Program-Id.  Chapt21e.
000031* Validate A Date
000040 Environment Division.
000050 Configuration Section.
000051 Source-Computer.  IBM-PC.
000055 Object-Computer.  IBM-PC.
000056 Data Division.
000057 Working-Storage Section.
000058 01  Date-Validation-Work-Fields.
000059     03  Date-To-Validate      Pic 9(8) Value Zeros.
000060     03  Date-To-Validate-X Redefines Date-To-Validate.
000061         05  Date-MM           Pic 99.
000062         05  Date-DD           Pic 99.
000063         05  Date-YYYY         Pic 9(4).
000069     03  YYYYMMDD-Format-Date  Pic 9(8) Value Zeros.
000078     03  YYYYMMDD-Format-Date-X Redefines YYYYMMDD-Format-Date.
000079         05  Date-YYYY         Pic 9(4).
000080         05  Date-MM           Pic 99.
000081         05  Date-DD           Pic 99.
000092     03  Day-Table-Values      Pic X(24) Value
000093         "312831303130313130313031".
000094     03  Day-Table Redefines Day-Table-Values.
000095         05  Days-In-Month     Pic 99   Occurs 12 Times.
000097 01  Valid-Status          Pic X(40) Value Spaces.
000099 01  Work-Number           Pic 9(5) Value Zeros.
000100 01  Work-Remainder        Pic 9(5) Value Zeros.
000101 01  Work-Remainder-100    Pic 9(5) Value Zeros.
000102 01  Work-Remainder-400    Pic 9(5) Value Zeros.
000103 01  Today-Date            Pic 9(8) Value Zeros.
000104 01  Today-Integer         Pic 9(7) Value Zeros.
000105 01  Test-Integer          Pic 9(7) Value Zeros.
000106 01  Test-Range            Pic 9(7) Value Zeros.
000107 Screen Section.
000108 01  Date-Entry Blank Screen Auto.
000109     03  Line 01 Column 01 Value "Enter Date: ".
000110     03  Line 01 Column 13 Pic 99/99/9999 Using Date-To-Validate.
000111     03  Line 01 Column 24 Pic X(40) From Valid-Status.
000112 Procedure Division.
000159 Chapt21e-Start.
000169     Display Date-Entry
000179     Accept Date-Entry
000199     Divide Date-YYYY Of Date-To-Validate-X By 4
000200            Giving Work-Number Remainder
000201                   Work-Remainder
000202     Divide Date-YYYY Of Date-To-Validate-X By 100
000203            Giving Work-Number Remainder
000204                   Work-Remainder-100
000205     Divide Date-YYYY Of Date-To-Validate-X By 400
000206            Giving Work-Number Remainder
000207                   Work-Remainder-400
000209     If Work-Remainder = Zeros And
000210        (Work-Remainder-100 Not = Zeros Or
000211         Work-Remainder-400 = Zeros)
000212           Move 29 To Days-In-Month (2)
000213     Else
000214           Move 28 To Days-In-Month (2)
000215     End-If
000217     If Date-MM Of Date-To-Validate-X  > 12 Or
000218        Date-MM Of Date-To-Validate-X  < 01 Or
000219        Date-YYYY Of Date-To-Validate-X < 1601 Or
000220        Date-DD Of Date-To-Validate-X Not > Zero Or
000221        Date-DD Of Date-To-Validate-X >
000222        Days-In-Month (Date-MM Of Date-To-Validate-X)
000223        Move "Invalid Date" To Valid-Status
000224     End-If
000235     If Valid-Status = Spaces
000236        Move Corresponding Date-To-Validate-X To
000237                           YYYYMMDD-Format-Date-X
000238        Move Function Current-Date (1:8) To Today-Date
000239        Compute Test-Range =
000240                Function Integer-Of-Date (YYYYMMDD-Format-Date) -
000242                Function Integer-Of-Date (Today-Date)
000246        If Test-Range > 30
000247           Move "Date Valid, but out of Range" To Valid-Status
000248        End-If
000249     End-If
000250     If Valid-Status = Spaces
000251        Move "Date Valid and Within Range" To Valid-Status
000252     End-If
000253     Display Date-Entry
000255     .