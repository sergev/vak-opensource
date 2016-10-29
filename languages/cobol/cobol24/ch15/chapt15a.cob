000010 @OPTIONS MAIN,TEST
000020 Identification Division.
000030 Program-Id.  Chapt15a.
000031* Sequentially Read An Indexed File
000041 Environment Division.
000050 Configuration Section.
000051 Source-Computer.  IBM-PC.
000055 Object-Computer.  IBM-PC.
000056 Input-Output Section.
000057 File-Control.
000058     Select Dealer-File Assign To "Dealer.Dat"
000059            Organization Indexed
000060            Access Sequential
000061            Record Key Dealer-Number
000062            Alternate Record Key Dealer-Name
000063            File Status Dealer-Status.
000065 Data Division.
000066 File Section.
000067 Fd  Dealer-File.
000068 01  Dealer-Record.
000069     03  Dealer-Number         Pic X(8).
000070     03  Dealer-Name.
000071         05  Last-Name   Pic X(25).
000072         05  First-Name  Pic X(15).
000073         05  Middle-Name Pic X(10).
000083     03  Address-Line-1      Pic X(50).
000085     03  Address-Line-2      Pic X(50).
000086     03  City                Pic X(40).
000087     03  State-Or-Country    Pic X(20).
000088     03  Postal-Code         Pic X(15).
000089     03  Home-Phone          Pic X(20).
000090     03  Work-Phone          Pic X(20).
000091     03  Other-Phone         Pic X(20).
000092     03  Start-Date          Pic 9(8).
000093     03  Last-Rent-Paid-Date Pic 9(8).
000094     03  Next-Rent-Due-Date  Pic 9(8).
000095     03  Rent-Amount         Pic 9(4)v99.
000096     03  Consignment-Percent Pic 9(3).
000097     03  Filler              Pic X(50).
000098 Working-Storage Section.
000116 01  Dealer-Status     Pic X(2) Value Spaces.
000240     88  Dealer-Success Value "00".
000276 01  Show-Next-Record  Pic X Value "Y".
000277 01  Process-Flag      Pic X Value Spaces.
000278     88  End-Process   Value "Y".
000279 Screen Section.
000321 01  Data-Entry-Screen
000322     Blank Screen, Auto
000323     Foreground-Color Is 7,
000324     Background-Color Is 1.
000325*
000326     03  Screen-Literal-Group.
000327         05  Line 01 Column 30 Value "Darlene's Treasures"
000328             Highlight Foreground-Color 4 Background-Color 1.
000329         05  Line 03 Column 30 Value "Tenant Display Program"
000330             Highlight.
000331         05  Line 4  Column 01  Value "Number: ".
000332         05  Line 5  Column 01  Value "Name, Last: ".
000333         05  Line 5  Column 39  Value "First: ".
000334         05  Line 5  Column 62  Value "Middle: ".
000335         05  Line 6  Column 01  Value "Address 1: ".
000336         05  Line 7  Column 01  Value "Address 2: ".
000337         05  Line 8  Column 01  Value "City: ".
000338         05  Line 9  Column 01  Value "Country/State: ".
000339         05  Line 9  Column 36  Value "Postal Code: ".
000340         05  Line 11 Column 01  Value "Phone/Home: ".
000341         05  Line 11 Column 34  Value "Work: ".
000342         05  Line 12 Column 06  Value "Other: ".
000343         05  Line 14 Column 01  Value "Start Date: ".
000344         05  Line 14 Column 24  Value "Last Paid Date: ".
000345         05  Line 14 Column 51  Value "Next Rent Due on: ".
000346         05  Line 15 Column 01  Value "Rent Amount: ".
000348         05  Line 16 Column 01  Value "Consignment Percent: ".
000349         05  Line 22 Column 01
000350             Value "Display next Record? (Y/N):".
000351*
000352     03  Required-Reverse-Group Reverse-Video.
000353         05  Line 4 Column 13  Pic X(8)  From Dealer-Number.
000354         05  Line 5 Column 13  Pic X(25) From Last-Name.
000355         05  Line 5 Column 46  Pic X(15) From First-Name.
000356         05  Line 5  Column 70 Pic X(10) From Middle-Name.
000359         05  Line 6  Column 15 Pic X(50) From Address-Line-1.
000361         05  Line 7  Column 15 Pic X(50) From Address-Line-2.
000363         05  Line 8  Column 15 Pic X(40) From City.
000365         05  Line 9  Column 15 Pic X(20) From State-Or-Country.
000367         05  Line 9  Column 50 Pic X(15) From Postal-Code.
000369         05  Line 11 Column 13 Pic X(20) From Home-Phone.
000371         05  Line 11 Column 41 Pic X(20) From Work-Phone.
000373         05  Line 12 Column 13 Pic X(20) From Other-Phone.
000375         05  Line 14 Column 13 Pic 99/99/9999 From Start-Date.
000377         05  Line 14 Column 40 Pic 99/99/9999
000378             From Last-Rent-Paid-Date.
000379         05  Line 14 Column 69 Pic 99/99/9999
000380             From Next-Rent-Due-Date.
000381         05  Line 15 Column 14 Pic Z,ZZZ.99 From Rent-Amount.
000382         05  Line 16 Column 22 Pic ZZ9 From Consignment-Percent.
000383         05  Line 22 Column 29 Pic X Using Show-Next-Record.
000386*
000387
000388 Procedure Division.
000389 Chapt15a-Start.
000408     Perform Open-File
000411     If Dealer-Success
000412        Perform Process-Screen Until Show-Next-Record = "N" Or
000413                                     Show-Next-Record = "n" Or
000414                                     End-Process
000415        Perform Close-File
000416     End-If
000417     Stop Run
000418     .
000419  Process-Screen.
000420     Read Dealer-File
000421       At End Set End-Process To True
000422       Not At End
000423           Perform Display-And-Accept
000424     End-Read
000425     .
000451 Display-And-Accept.
000452     Display Data-Entry-Screen
000453     Accept Data-Entry-Screen
000456     .
000457 Open-File.
000458     Open Input Dealer-File
000459     .
000460 Close-File.
000461     Close Dealer-File
000462     .
