000010 @OPTIONS MAIN,TEST
000020 Identification Division.
000030 Program-Id.  Chapt21h.
000031* Compute The Date Of Easter For The Given Year
000040 Environment Division.
000050 Configuration Section.
000051 Source-Computer.  IBM-PC.
000055 Object-Computer.  IBM-PC.
000056 Data Division.
000057 Working-Storage Section.
000085 01  Easter-Work-Fields.
000086     03  The-Year          Pic 9(4) Value Zeros.
000087     03  Easter-Date       Pic 9(8) Value Zeros.
000088     03  Easter-Date-X Redefines Easter-Date.
000089         05  Easter-Month  Pic 99.
000090         05  Easter-Day    Pic 99.
000091         05  Easter-Year   Pic 9(4).
000092     03  Golden-Number     Pic 9(6).
000093     03  Century           Pic 9(3).
000094     03  Skipped-Leap-Year Pic 9(6).
000095     03  Correction        Pic 9(8).
000096     03  Factor            Pic 9(8).
000097     03  Epact             Pic 9(8).
000098 01  Temp-Work             Pic 9(8).
000099 01  Temp-Work-1           Pic 9(8).
000100 Screen Section.
000101 01  Date-Entry Blank Screen Auto.
000102     03  Line 01 Column 01 Value "Enter Year: ".
000103     03  Line 01 Column 14 Pic 9(4) Using The-Year.
000104     03  Line 03 Column 01 Value "Easter is: ".
000105     03  Line 03 Column 15 Pic 99/99/9999 From Easter-Date.
000106 Procedure Division.
000159 Chapt21h-Start.
000169     Display Date-Entry
000179     Accept Date-Entry
000180     Move The-Year To Easter-Year
000181*
000182     Compute Golden-Number = Function Rem (The-Year 19)
000183     Add 1 To Golden-Number
000184*
000190     Divide The-Year By 100 Giving Century
000200     Add 1 To Century
000210*
000211     Compute Temp-Work = 3 * Century
000220     Divide Temp-Work By 4 Giving Skipped-Leap-Year
000230     Subtract 12 From Skipped-Leap-Year
000240*
000241     Compute Temp-Work = (8 * Century) + 5
000250     Divide Temp-Work By 25 Giving Correction
000260     Subtract 5 From Correction
000270*
000271     Compute Temp-Work = 5 * The-Year
000272     Divide Temp-Work By 4 Giving Factor
000273     Subtract Skipped-Leap-Year From Factor
000274     Subtract 10 From Factor
000275*
000276     Compute Temp-Work = (11 * Golden-Number) + 20
000277                        + Correction - Skipped-Leap-Year
000278     Compute Epact = Function Rem (Temp-Work 30)
000280*
000281     If Epact = 25 And Golden-Number > 11 Or
000282        Epact = 24
000283        Add 1 To Epact
000290     End-If
000300*
000310     Compute Temp-Work = 44 - Epact
000320     If Temp-Work < 21
000330        Add 30 To Temp-Work
000340     End-If
000350*
000351     Compute Temp-Work-1 = Factor + Temp-Work
000360     Compute Easter-Day =  Temp-Work + 7 -
000370             Function Rem (Temp-Work-1 7)
000380*
000390     If Easter-Day > 31
000400        Move 4 To Easter-Month
000410        Subtract 31 From Easter-Day
000420     Else
000430        Move 3 To Easter-Month
000440     End-If
000450     Move The-Year To Easter-Year
000460*
000470     Display Date-Entry
000480     Stop Run
000490     .