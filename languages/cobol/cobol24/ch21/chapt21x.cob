000010 @OPTIONS MAIN,TEST
000020 Identification Division.
000030 Program-Id.  Chapt21x.
000031* Chapter 21 Exercise Answer
000040 Environment Division.
000050 Configuration Section.
000051 Source-Computer.  IBM-PC.
000055 Object-Computer.  IBM-PC.
000056 Data Division.
000057 Working-Storage Section.
000098 01  Date-Work-Fields.
000099     03  Birthday      Pic 9(8) Value Zeros.
000100     03  Birthday-X Redefines Birthday.
000101         05  The-Month Pic 99.
000102         05  The-Day   Pic 99.
000103         05  The-Year  Pic 9(4).
000104     03  Julian-Birthday   Pic 9(7) Value Zeros.
000105     03  Julian-Birthday-X Redefines Julian-Birthday.
000106         05  Julian-Day    Pic 9(3).
000107         05  Julian-Year   Pic 9(4).
000108     03  Julian-Date       Pic 9(7) Value Zeros.
000109     03  Julian-Date-X Redefines Julian-Date.
000110         05  Julian-Year   Pic 9(4).
000111         05  Julian-Day    Pic 9(3).
000112     03  Work-Date         Pic 9(8) Value Zeros.
000113     03  Work-Date-X       Redefines Work-Date.
000114         05  The-Year      Pic 9(4).
000115         05  The-Month     Pic 99.
000116         05  The-Day       Pic 99.
000118 01  Birth-Day-Of-Week Pic X(9) Value Spaces.
000119 01  Days-Old          Pic 9(5) Value Zeros.
000120 01  Days-Old-2000     Pic 9(5) Value Zeros.
000121 01  Today-Date        Pic 9(8).
000122 01  Integer-Today     Pic 9(12).
000123 01  Integer-Birthday  Pic 9(12).
000124 01  Weekday-Number    Pic 9.
000125 01  Day-Of-Week-Table-Area.
000126     03  Day-Of-Week-Table-Values.
000127         05  Filler        Pic X(9) Value "Sunday".
000128         05  Filler        Pic X(9) Value "Monday".
000129         05  Filler        Pic X(9) Value "Tuesday".
000130         05  Filler        Pic X(9) Value "Wednesday".
000131         05  Filler        Pic X(9) Value "Thursday".
000132         05  Filler        Pic X(9) Value "Friday".
000133         05  Filler        Pic X(9) Value "Saturday".
000134     03  Day-Of-Week-Table Redefines Day-Of-Week-Table-Values.
000135         05  Week-Day-Entry Pic X(9) Occurs 7 Times.
000136 Screen Section.
000137 01  Date-Entry Blank Screen Auto.
000138     03  Line 01 Column 01 Value "Enter Birthday: ".
000139     03  Line 01 Column 17 Pic 99/99/9999 Using Birthday.
000140     03  Line 03 Column 1  Value "Born on ".
000141     03  Line 03 Column 9  Pic X(9) From Birth-Day-Of-Week.
000142     03  Line 04 Column 1  Value "Days Old Today: ".
000143     03  Line 04 Column 17 Pic Z(5)9 From Days-Old.
000144     03  Line 05 Column 1  Value "Days old 01/01/2000: ".
000145     03  Line 05 Column 24 Pic Z(5)9 From Days-Old-2000.
000146     03  Line 06 Column 1  Value "Julian Birthday:".
000147     03  Line 06 Column 18 Pic 999/9999 From Julian-Birthday.
000148 Procedure Division.
000159 Chapt21x-Start.
000169     Display Date-Entry
000179     Accept Date-Entry
000189     Move Function Current-Date (1:8) To Today-Date
000190     Compute Integer-Today =
000191             Function Integer-Of-Date (Today-Date)
000192     Move Corresponding Birthday-X To Work-Date-X
000193     Compute Integer-Birthday =
000194             Function Integer-Of-Date (Work-Date)
000199     Compute Weekday-Number =
000209             (Function Rem (Integer-Birthday 7) + 1)
000219     Move Week-Day-Entry (Weekday-Number) To Birth-Day-Of-Week
000229     Compute Days-Old = Integer-Today - Integer-Birthday
000239     Compute Days-Old-2000 = Function Integer-Of-Date (20000101)
000249                           - Integer-Birthday
000259     Compute Julian-Date =
000269             Function Day-Of-Integer (Integer-Birthday)
000279     Move Corresponding Julian-Date-X To Julian-Birthday-X
000289     Display Date-Entry
000299     Stop Run
000309     .