000010 @OPTIONS MAIN,TEST
000020 Identification Division.
000030 Program-Id.  Chapt21d.
000031* Days Between Dates, With Weekday
000040 Environment Division.
000050 Configuration Section.
000051 Source-Computer.  IBM-PC.
000055 Object-Computer.  IBM-PC.
000056 Data Division.
000057 Working-Storage Section.
000058 01  First-Date    Value Zeros.
000059     03  Date-MM           Pic 99.
000060     03  Date-DD           Pic 99.
000061     03  Date-YYYY         Pic 9(4).
000062 01  Second-Date   Value Zeros.
000063     03  Date-MM           Pic 99.
000064     03  Date-DD           Pic 99.
000065     03  Date-YYYY         Pic 9(4).
000066 01  Days-Between          Pic S9(12).
000076 01  Integer-First-Date    Pic  9(12).
000077 01  Integer-Second-Date   Pic  9(12).
000078 01  Date-Formatting-Items.
000079     03  YYYYMMDD-Format-Date.
000080         05  Date-YYYY         Pic 9(4).
000081         05  Date-MM           Pic 99.
000082         05  Date-DD           Pic 99.
000083     03  YYYYMMDD-Format-Date-N
000084         Redefines YYYYMMDD-Format-Date Pic 9(8).
000085     03  Format-Indicator-F    Pic X(8) Value "MMDDYYYY".
000086     03  Format-Indicator-S    Pic X(8) Value "MMDDYYYY".
000087 01  Weekday-First         Pic X(9) Value Spaces.
000088 01  Weekday-Second        Pic X(9) Value Spaces.
000089 01  Weekday-Table-Area.
000090     03  Weekday-Table-Values.
000091         05  Filler Pic X(27) Value "Sunday   Monday   Tuesday".
000092         05  Filler Pic X(27) Value "WednesdayThursday Friday".
000093         05  Filler Pic X(9)  Value "Saturday".
000094     03  Weekday-Table Redefines Weekday-Table-Values.
000095         05  The-Day    Pic X(9) Occurs 7 Times.
000096 01  Weeks              Pic 9(12) Value Zeros.
000097 01  Remainder-Days     Pic 9.
000098 Screen Section.
000099 01  Date-Entry Blank Screen Auto.
000100     03  Line 01 Column 01 Value "Enter First Date: ".
000101     03  Line 01 Column 21 Pic X(8) From Format-Indicator-F
000102                                    To   First-Date.
000103     03  Line 01 Column 30 Pic X(9) From Weekday-First.
000104     03  Line 03 Column 01 Value "Enter Second Date: ".
000105     03  Line 03 Column 21 Pic X(8) From Format-Indicator-S
000106                                    To   Second-Date.
000107     03  Line 03 Column 30 Pic X(9) From Weekday-Second.
000108     03  Line 05 Column 01 Value "Days between dates: ".
000109     03  Line 05 Column 21 Pic -Zzz,ZZ9 From Days-Between.
000110 Procedure Division.
000159 Chapt21d-Start.
000169     Display Date-Entry
000179     Accept Date-Entry
000180     Move Corresponding First-Date To YYYYMMDD-Format-Date
000181     Compute Integer-First-Date =
000182             Function Integer-Of-Date (YYYYMMDD-Format-Date-N)
000183     Move First-Date To Format-Indicator-F
000184     Move Corresponding Second-Date To YYYYMMDD-Format-Date
000185     Compute Integer-Second-Date =
000186             Function Integer-Of-Date (YYYYMMDD-Format-Date-N)
000187     Move Second-Date To Format-Indicator-S
000188     Compute Days-Between =
000189            Integer-Second-Date - Integer-First-Date
000190     Divide Integer-First-Date By 7 Giving Weeks
000191                                 Remainder Remainder-Days
000192     Add 1 To Remainder-Days
000193     Move The-Day (Remainder-Days) To Weekday-First
000194     Divide Integer-Second-Date By 7 Giving Weeks
000195                                  Remainder Remainder-Days
000196     Add 1 To Remainder-Days
000197     Move The-Day (Remainder-Days) To Weekday-Second
000198     Display Date-Entry
000199     Stop Run
000201     .