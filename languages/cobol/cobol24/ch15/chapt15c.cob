000010 @OPTIONS MAIN,TEST
000020 Identification Division.
000030 Program-Id.  Chapt15c.
000031* Sequentially Read An Indexed File Via Alternate Key
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
000413         05  Line 16 Column 01  Value "Consignment Percent: ".
000414         05  Line 17 Column 01  Value "Rent Amount: ".
000415         05  Line 22 Column 01
000416             Value "Display next Record? (Y/N):".
000417*
000418     03  Required-Reverse-Group Reverse-Video.
000419         05  Line 4 Column 13  Pic X(8)  From Dealer-Number.
000420         05  Line 5 Column 13  Pic X(25) From Last-Name.
000421         05  Line 5 Column 46  Pic X(15) From First-Name.
000422         05  Line 5  Column 70 Pic X(10) From Middle-Name.
000423         05  Line 6  Column 15 Pic X(50) From Address-Line-1.
000424         05  Line 7  Column 15 Pic X(50) From Address-Line-2.
000425         05  Line 8  Column 15 Pic X(40) From City.
000426         05  Line 9  Column 15 Pic X(20) From State-Or-Country.
000427         05  Line 9  Column 50 Pic X(15) From Postal-Code.
000428         05  Line 11 Column 13 Pic X(20) From Home-Phone.
000429         05  Line 11 Column 41 Pic X(20) From Work-Phone.
000430         05  Line 12 Column 13 Pic X(20) From Other-Phone.
000431         05  Line 14 Column 13 Pic 99/99/9999 From Start-Date.
000432         05  Line 14 Column 40 Pic 99/99/9999
000433             From Last-Rent-Paid-Date.
000434         05  Line 14 Column 69 Pic 99/99/9999
000435             From Next-Rent-Due-Date.
000436         05  Line 15 Column 14 Pic Z,ZZZ.99 From Rent-Amount.
000437         05  Line 16 Column 22 Pic ZZ9 From Consignment-Percent.
000438 Procedure Division.
000440 Chapt15c-Start.
000441     Perform Open-File
000442     If Dealer-Success
000443        Move "H" To Dealer-Name
000444        Start Dealer-File Key Not < Dealer-Name
000445              Invalid Key Set End-Process To True
000446        End-Start
000447        Perform Process-Screen Until Show-Next-Record = "N" Or
000448                                     Show-Next-Record = "n" Or
000449                                     End-Process
000450        Perform Close-File
000451     End-If
000452     Stop Run
000453     .
000454  Process-Screen.
000455     Read Dealer-File
000456       At End Set End-Process To True
000457       Not At End
000458           Perform Display-And-Accept
000459     End-Read
000460     .
000461 Display-And-Accept.
000462     Display Data-Entry-Screen
000463     Accept Data-Entry-Screen
000464     .
000465 Open-File.
000466     Open Input Dealer-File
000467     .
000468 Close-File.
000469     Close Dealer-File
000470     .