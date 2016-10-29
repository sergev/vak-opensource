000020 Identification Division.
000030 Program-Id.  Chapt23h.
000031* Validate A Date
000040 Environment Division.
000050 Configuration Section.
000051 Source-Computer.  IBM-PC.
000055 Object-Computer.  IBM-PC.
000056 Data Division.
000057 Working-Storage Section.
000099 01  Work-Number           Pic 9(5) Value Zeros.
000100 01  Work-Remainder        Pic 9(5) Value Zeros.
000101 01  Work-Remainder-100    Pic 9(5) Value Zeros.
000102 01  Work-Remainder-400    Pic 9(5) Value Zeros.
000103 01  Today-Date            Pic 9(8) Value Zeros.
000104 01  Today-Integer         Pic 9(7) Value Zeros.
000105 01  Test-Integer          Pic 9(7) Value Zeros.
000106 01  Test-Range            Pic 9(7) Value Zeros.
000107 01  Day-Table-Area.
000108     03  Day-Table-Values      Pic X(24) Value
000109         "312831303130313130313031".
000110     03  Day-Table Redefines Day-Table-Values.
000111         05  Days-In-Month     Pic 99   Occurs 12 Times.
000113 Linkage Section.
000114 Copy "Dateval.Cpy".
000122 Procedure Division Using Passed-Date Valid-Status.
000159 Chapt23h-Start.
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
000224     Else
000225        Move "Valid Date" To Valid-Status
000226     End-If
000253     Exit Program
000255     .