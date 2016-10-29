000010 @OPTIONS MAIN,TEST
000020 Identification Division.
000030 Program-Id.  Chapt15b.
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
000388 01  Data-Entry-Screen
000389     Blank Screen, Auto
000390     Foreground-Color Is 7,
000391     Background-Color Is 1.
000392*
000393     03  Screen-Literal-Group.
000394         05  Line 01 Column 30 Value "Darlene's Treasures"
000395             Highlight Foreground-Color 4 Background-Color 1.
000396         05  Line 03 Column 30 Value "Tenant Display Program"
000397             Highlight.
000398         05  Line 4  Column 01  Value "Number: ".
000399         05  Line 5  Column 01  Value "Name, Last: ".
000400         05  Line 5  Column 39  Value "First: ".
000401         05  Line 5  Column 62  Value "Middle: ".
000402         05  Line 6  Column 01  Value "Address 1: ".
000403         05  Line 7  Column 01  Value "Address 2: ".
000404         05  Line 8  Column 01  Value "City: ".
000405         05  Line 9  Column 01  Value "Country/State: ".
000406         05  Line 9  Column 36  Value "Postal Code: ".
000407         05  Line 11 Column 01  Value "Phone/Home: ".
000408         05  Line 11 Column 34  Value "Work: ".
000409         05  Line 12 Column 06  Value "Other: ".
000410         05  Line 14 Column 01  Value "Start Date: ".
000411         05  Line 14 Column 24  Value "Last Paid Date: ".
000412         05  Line 14 Column 51  Value "Next Rent Due on: ".
000413         05  Line 15 Column 01  Value "Rent Amount: ".
000415         05  Line 16 Column 01  Value "Consignment Percent: ".
000416         05  Line 22 Column 01
000417             Value "Display next Record? (Y/N):".
000418*
000419     03  Required-Reverse-Group Reverse-Video.
000420         05  Line 4 Column 13  Pic X(8)  From Dealer-Number.
000421         05  Line 5 Column 13  Pic X(25) From Last-Name.
000422         05  Line 5 Column 46  Pic X(15) From First-Name.
000423         05  Line 5  Column 70 Pic X(10) From Middle-Name.
000424         05  Line 6  Column 15 Pic X(50) From Address-Line-1.
000425         05  Line 7  Column 15 Pic X(50) From Address-Line-2.
000426         05  Line 8  Column 15 Pic X(40) From City.
000427         05  Line 9  Column 15 Pic X(20) From State-Or-Country.
000428         05  Line 9  Column 50 Pic X(15) From Postal-Code.
000429         05  Line 11 Column 13 Pic X(20) From Home-Phone.
000430         05  Line 11 Column 41 Pic X(20) From Work-Phone.
000431         05  Line 12 Column 13 Pic X(20) From Other-Phone.
000432         05  Line 14 Column 13 Pic 99/99/9999 From Start-Date.
000433         05  Line 14 Column 40 Pic 99/99/9999
000434             From Last-Rent-Paid-Date.
000435         05  Line 14 Column 69 Pic 99/99/9999
000436             From Next-Rent-Due-Date.
000437         05  Line 15 Column 14 Pic Z,ZZZ.99 From Rent-Amount.
000438         05  Line 16 Column 22 Pic ZZ9 From Consignment-Percent.
000439         05  Line 22 Column 29 Pic X Using Show-Next-Record.
000440*
000442 Procedure Division.
000443 Chapt15b-Start.
000444     Perform Open-File
000445     If Dealer-Success
000446        Move "C" To Dealer-Number
000447        Start Dealer-File Key Not < Dealer-Number
000448              Invalid Key Set End-Process To True
000449        End-Start
000450        Perform Process-Screen Until Show-Next-Record = "N" Or
000451                                     Show-Next-Record = "n" Or
000452                                     End-Process
000453        Perform Close-File
000454     End-If
000455     Stop Run
000456     .
000457  Process-Screen.
000458     Read Dealer-File
000459       At End Set End-Process To True
000460       Not At End
000461           Perform Display-And-Accept
000462     End-Read
000463     .
000464 Display-And-Accept.
000465     Display Data-Entry-Screen
000466     Accept Data-Entry-Screen
000467     .
000468 Open-File.
000469     Open Input Dealer-File
000470     .
000471 Close-File.
000472     Close Dealer-File
000473     .
