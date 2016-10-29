000020 Identification Division.
000030 Program-Id.  Chapt23c Is Initial.
000031* Days Between Dates
000040 Environment Division.
000050 Configuration Section.
000051 Source-Computer.  IBM-PC.
000055 Object-Computer.  IBM-PC.
000056 Data Division.
000057 Working-Storage Section.
000058 01  First-Date                Value Zeros.
000059     03  Date-MM               Pic 99.
000060     03  Date-DD               Pic 99.
000061     03  Date-YYYY             Pic 9(4).
000062 01  Second-Date               Value Zeros.
000063     03  Date-MM               Pic 99.
000064     03  Date-DD               Pic 99.
000065     03  Date-YYYY             Pic 9(4).
000066 01  Days-Between              Pic S9(12) Value Zeros.
000076 01  Integer-First-Date        Pic  9(12).
000077 01  Integer-Second-Date       Pic  9(12).
000078 01  Date-Formatting-Items.
000079     03  YYYYMMDD-Format-Date.
000080         05  Date-YYYY         Pic 9(4).
000081         05  Date-MM           Pic 99.
000082         05  Date-DD           Pic 99.
000083     03  YYYYMMDD-Format-Date-N Redefines
000084         YYYYMMDD-Format-Date  Pic 9(8).
000085     03  Format-Indicator-F    Pic X(8) Value "MMDDYYYY".
000086     03  Format-Indicator-S    Pic X(8) Value "MMDDYYYY".
000087 Screen Section.
000088 01  Date-Entry Blank Screen Auto.
000089     03  Line 01 Column 01 Value "Enter First Date: ".
000090     03  Line 01 Column 21 Pic X(8) From Format-Indicator-F
000091                                    To   First-Date.
000092     03  Line 03 Column 01 Value "Enter Second Date: ".
000093     03  Line 03 Column 21 Pic X(8) From Format-Indicator-S
000094                                    To   Second-Date.
000095     03  Line 05 Column 01 Value "Days between dates: ".
000096     03  Line 05 Column 21 Pic -Zzz,ZZ9 From Days-Between.
000097 Procedure Division.
000159 Chapt23c-Start.
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
000188     Compute Days-Between = Integer-Second-Date -
000189                            Integer-First-Date
000190     Display Date-Entry
000191     Accept Date-Entry
000192     Exit Program
000193     .
000194