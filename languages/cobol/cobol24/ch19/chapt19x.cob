000010 @OPTIONS MAIN,TEST
000020 Identification Division.
000030 Program-Id.  Chapt19x.
000031* Chapter 19 Exercise Answer
000043 Environment Division.
000050 Configuration Section.
000055 Source-Computer.  IBM-PC.
000056 Object-Computer.  IBM-PC.
000061 Input-Output  Section.
000062 File-Control.
000063     Select Report-File Assign To Printer.
000066     Select Dealer-File Assign To "Dealer.Dat"
000067         Organization Indexed
000068         Access Sequential
000069         Record Key Dealer-Number
000070         Alternate Record Key Dealer-Name Of Dealer-Record
000071         File Status Dealer-Status.
000072     Select Sort-File Assign To Sort-Work.
000076 Data Division.
000077 File Section.
000078 Fd  Report-File.
000079 01  Report-Record Pic X(80).
000093 Fd  Dealer-File.
000094 01  Dealer-Record.
000095     03  Dealer-Number         Pic X(8).
000096     03  Dealer-Name.
000097         05  Last-Name   Pic X(25).
000098         05  First-Name  Pic X(15).
000099         05  Middle-Name Pic X(10).
000100     03  Address-Line-1      Pic X(50).
000101     03  Address-Line-2      Pic X(50).
000102     03  City                Pic X(40).
000103     03  State-Or-Country    Pic X(20).
000104     03  Postal-Code         Pic X(15).
000105     03  Home-Phone          Pic X(20).
000106     03  Work-Phone          Pic X(20).
000107     03  Other-Phone         Pic X(20).
000108     03  Start-Date          Pic 9(8).
000109     03  Last-Rent-Paid-Date Pic 9(8).
000110     03  Next-Rent-Due-Date  Pic 9(8).
000111     03  Rent-Amount         Pic 9(4)v99.
000112     03  Consignment-Percent Pic 9(3).
000113     03  Last-Sold-Amount    Pic S9(7)v99.
000114     03  Last-Sold-Date      Pic 9(8).
000115     03  Sold-To-Date        Pic S9(7)v99.
000116     03  Commission-To-Date  Pic S9(7)v99.
000117     03  Filler              Pic X(15).
000127 Sd  Sort-File.
000137 01  Sort-Record.
000138* It Is Not Necessary To Duplicate The Entire Dealer File In The Sort,
000139* As We Are Going To Only Use A Few Fields.  This Will Make The Sort Faster.
000141     03  Dealer-Name.
000142         05  Last-Name       Pic X(25).
000143         05  First-Name      Pic X(15).
000144         05  Middle-Name     Pic X(10).
000145     03  Last-Rent-Paid-Date Pic 9(8).
000146     03  Next-Rent-Due-Date  Pic 9(8).
000147     03  Rent-Amount         Pic 9(4)v99.
000148 Working-Storage Section.
000206 01  Dealer-Status           Pic XX Value Zeros.
000207     88  Dealer-Success  Value "00" Thru "09".
000209 01  Heading-Line-1.
000210     03  Filler      Pic X(12) Value "Created by:".
000211     03  Filler      Pic X(8)  Value "CHAPT19X".
000212     03  Filler      Pic X(11) Value Spaces.
000213     03  Filler      Pic X(23) Value "Dealer File Rent Report".
000214     03  Filler      Pic X(10) Value Spaces.
000215     03  Filler      Pic X(5)  Value "Page".
000216     03  Page-No     Pic Z(4)9 Value Zeros.
000217 01  Heading-Line-2.
000218     03  Filler      Pic X(12) Value "Created on:".
000219     03  Date-MM     Pic 99.
000220     03  Filler      Pic X     Value "/".
000221     03  Date-DD     Pic 99.
000222     03  Filler      Pic X     Value "/".
000223     03  Date-YY     Pic 99.
000224 01  Heading-Line-3.
000225     03  Filler      Pic X(12) Value "At:".
000226     03  Time-HH     Pic 99.
000227     03  Filler      Pic X     Value ":".
000228     03  Time-MM     Pic 99.
000229     03  Filler      Pic X     Value ":".
000230     03  Time-SS     Pic 99.
000231 01  Heading-Line-4.
000232     03  Filler      Pic X(41) Value Spaces.
000233     03  Filler      Pic X(27) Value "Last Rent  Next Rent".
000234     03  Filler      Pic X(4)  Value "Rent".
000235 01  Heading-Line-5.
000236     03  Filler   Pic X(44) Value "Name".
000237     03  Filler   Pic X(29) Value "Paid       Due         Amount".
000238 01  Detail-Line.
000239     03  Detail-Name         Pic X(40)       Value Spaces.
000240     03  Filler              Pic X           Value Spaces.
000241     03  Last-Rent-Paid-Date Pic 99/99/9999.
000242     03  Filler              Pic X           Value Spaces.
000243     03  Next-Rent-Due-Date  Pic 99/99/9999.
000244     03  Filler              Pic X           Value Spaces.
000245     03  Rent-Amount         Pic $$$,$$$.99.
000246 01  Line-Count           Pic 99          Value 99.
000247 01  Page-Count           Pic 9(4)        Value Zeros.
000248 01  Max-Lines            Pic 99          Value 60.
000249 01  Date-And-Time-Area.
000250     03  Work-Date            Pic 9(6).
000251     03  Work-Date-X          Redefines Work-Date.
000252         05  Date-YY          Pic 99.
000253         05  Date-MM          Pic 99.
000254         05  Date-DD          Pic 99.
000255     03  Work-Time            Pic 9(8).
000256     03  Work-Time-X          Redefines Work-Time.
000257         05  Time-HH          Pic 99.
000258         05  Time-MM          Pic 99.
000259         05  Time-SS          Pic 99.
000260         05  Filler           Pic XX.
000261 01  Done-Flag            Pic X Value Spaces.
000262     88  All-Done               Value "Y".
000263 Procedure Division.
000264 Declaratives.
000265 Dealer-File-Error Section.
000266     Use After Standard Error Procedure On Dealer-File
000267     .
000268 Dealer-Error-Paragraph.
000269     Display "Error on Dealer File " Dealer-Status
000270     .
000271 End Declaratives.
000272 Chapt19x-Start.
000273     Display "Begin Process Chapt19x"
000274     Sort Sort-File Descending Key Rent-Amount Of Sort-Record
000275          Input Procedure Sort-In
000276          Output Procedure Print-Report
000277     Stop Run
000278     .
000279 Sort-In.
000280     Open Input Dealer-File
000281* No Need To Do A "Start", We Have A Sort!
000282     Perform Until Not Dealer-Success
000283        Read Dealer-File
000284          At End Continue
000285          Not At End
000286* No Need To Release It To The Sort If Its Not $40.00 Or More.
000287              If Rent-Amount Of Dealer-Record >= 40
000288                 Move Corresponding Dealer-Record To Sort-Record
000289                 Release Sort-Record
000290              End-If
000291        End-Read
000292     End-Perform
000293     Close Dealer-File
000294     .
000295 Print-Report.
000296     Open Output Report-File
000297     Perform Fill-Initial-Headings
000298     Perform Until All-Done
000299        Return Sort-File
000300            At End Set All-Done To True
000301            Not At End
000302                Perform Print-This-Record
000303        End-Return
000304     End-Perform
000305     Close Report-File
000306     .
000319 Print-This-Record.
000320     Perform Format-Name
000321     Move Corresponding Sort-Record To Detail-Line
000322     If Line-Count >= Max-Lines
000323        Perform Heading-Routine
000324     End-If
000325     Write Report-Record From Detail-Line After 1
000326     Add 1 To Line-Count
000327     .
000328 Heading-Routine.
000329     Add 1 To Page-Count
000330     Move Page-Count To Page-No
000331     If Page-Count = 1
000332        Write Report-Record From Heading-Line-1 After Zero
000333     Else
000334        Write Report-Record From Heading-Line-1 After Page
000335     End-If
000336     Write Report-Record From Heading-Line-2 After 1
000337     Write Report-Record From Heading-Line-3 After 1
000338     Write Report-Record From Heading-Line-4 After 2
000339     Write Report-Record From Heading-Line-5 After 1
000340     Write Report-Record From Heading-Line-5 Before 2
000341     Move 7 To Line-Count
000342     .
000343 Format-Name.
000344     Move Spaces To Detail-Name
000345     String First-Name  Of Sort-Record Delimited By Space
000346            " "         Delimited By Size
000347            Middle-Name Of Sort-Record Delimited By Space
000348            " "         Delimited By Size
000349            Last-Name   Of Sort-Record Delimited By Space
000350            Into Detail-Name
000351     End-String
000352     .
000353 Fill-Initial-Headings.
000354     Accept Work-Date From Date
000355     Accept Work-Time From Time
000356     Move Corresponding Work-Date-X To
000357                        Heading-Line-2
000358     Move Corresponding Work-Time-X To
000359                        Heading-Line-3
000360     .
