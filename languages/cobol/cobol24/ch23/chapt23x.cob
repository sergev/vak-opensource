000020 Identification Division.
000030 Program-Id.  Chapt23x.
000031* Compute The Weekday - Exercise Answer.
000040 Environment Division.
000050 Configuration Section.
000051 Source-Computer.  IBM-PC.
000055 Object-Computer.  IBM-PC.
000056 Data Division.
000057 Working-Storage Section.
000067 01  Reverse-Date-Area.
000077     03  Date-YYYYMMDD   Pic 9(8).
000087     03  Date-YYYYMMDD-X Redefines Date-YYYYMMDD.
000097         05  Date-YYYY   Pic 9(4).
000098         05  Date-MM     Pic 9(2).
000099         05  Date-DD     Pic 9(2).
000100 01  Weekday-Table-Area.
000101     03  Weekday-Table-Values.
000102         05  Filler Pic X(9) Value "Sunday".
000103         05  Filler Pic X(9) Value "Monday".
000104         05  Filler Pic X(9) Value "Tuesday".
000105         05  Filler Pic X(9) Value "Wednesday".
000106         05  Filler Pic X(9) Value "Thursday".
000107         05  Filler Pic X(9) Value "Friday".
000108         05  Filler Pic X(9) Value "Saturday".
000109     03  Weekday-Table Redefines Weekday-Table-Values.
000110         05  Week-Day  Pic X(9) Occurs 7 Times.
000120 01  Weekday-Element   Pic 9.
000125 Linkage Section.
000126 01  Passed-Date.
000127     03  Date-To-Validate      Pic 9(8).
000128     03  Date-To-Validate-X Redefines Date-To-Validate.
000129         05  Date-MM           Pic 99.
000130         05  Date-DD           Pic 99.
000131         05  Date-YYYY         Pic 9(4).
000132 01  Valid-Status              Pic X(40).
000133 Procedure Division Using Passed-Date Valid-Status.
000159 Chapt23x-Start.
000169     Move Corresponding Date-To-Validate-X To
000179                        Date-YYYYMMDD-X
000189     Compute Weekday-Element =
000190             Function Rem
000191             (Function Integer-Of-Date (Date-YYYYMMDD) 7) + 1
000193     Move Week-Day (Weekday-Element) To Valid-Status
000199     Exit Program
000255     .
