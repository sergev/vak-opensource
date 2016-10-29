000010 @OPTIONS MAIN,TEST
000020 Identification Division.
000030 Program-Id.  Chapt14b.
000031* Dealer Data Entry
000041 Environment Division.
000050 Configuration Section.
000051 Special-Names.
000052       Crt Status Is Keyboard-Status
000053       Cursor Is Cursor-Position.
000054 Source-Computer.  IBM-PC.
000055 Object-Computer.  IBM-PC.
000056 Input-Output Section.
000057 File-Control.
000058     Select Dealer-File Assign To "Dealer.Dat"
000059            Organization Indexed
000060            Access Sequential
000061            Record Key Dealer-Number Of Dealer-Record
000062            Alternate Record Key Dealer-Name Of Dealer-Record
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
000099 01  Keyboard-Status.
000100     03  Accept-Status Pic 9.
000101     03  Function-Key  Pic X.
000102         88  F1-Pressed Value X"01".
000103         88  F2-Pressed Value X"02".
000104     03  System-Use    Pic X.
000105 01  Cursor-Position.
000106     03  Cursor-Row    Pic 9(2) Value 1.
000107     03  Cursor-Column Pic 9(2) Value 1.
000116 01  Dealer-Status     Pic X(2) Value Spaces.
000240     88  Dealer-Success Value "00".
000250 01  Error-Message     Pic X(60) Value Spaces.
000251 01  Open-Error.
000252     03  Filler        Pic X(26)
000253         Value "Error Opening Dealer File ".
000254     03  Open-Error-Status  Pic X(2).
000255 01  Write-Error.
000256     03  Filler        Pic X(26)
000257         Value "Error Writing Dealer File ".
000258     03  Write-Error-Status Pic X(2).
000259 01  Work-Record.
000260     03  Dealer-Number         Pic X(8).
000261     03  Dealer-Name.
000262         05  Last-Name   Pic X(25).
000263         05  First-Name  Pic X(15).
000264         05  Middle-Name Pic X(10).
000265     03  Address-Line-1      Pic X(50).
000266     03  Address-Line-2      Pic X(50).
000267     03  City                Pic X(40).
000268     03  State-Or-Country    Pic X(20).
000269     03  Postal-Code         Pic X(15).
000270     03  Home-Phone          Pic X(20).
000271     03  Work-Phone          Pic X(20).
000272     03  Other-Phone         Pic X(20).
000273     03  Start-Date          Pic 9(8).
000274     03  Last-Rent-Paid-Date Pic 9(8).
000275     03  Next-Rent-Due-Date  Pic 9(8).
000276     03  Rent-Amount         Pic 9(4)v99.
000277     03  Consignment-Percent Pic 9(3).
000278
000279 Screen Section.
000321 01  Data-Entry-Screen
000322     Blank Screen, Auto
000323     Foreground-Color Is 7,
000324     Background-Color Is 1.
000325*
000326     03  Screen-Literal-Group.
000327         05  Line 01 Column 30 Value "Darlene's Treasures"
000328             Highlight Foreground-Color 4 Background-Color 1.
000329         05  Line 03 Column 30 Value "Tenant Entry Program"
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
000347         05  Line 16 Column 01  Value "Consignment Percent: ".
000348         05  Line 22 Column 01  Value "F1-Exit    F2-Save".
000349*
000350     03  Required-Reverse-Group Reverse-Video Required.
000351         05  Line 4 Column 13  Pic X(8)  Using Dealer-Number
000352             Of Work-Record.
000353         05  Line 5 Column 13  Pic X(25) Using Last-Name
000354             Of Work-Record.
000355         05  Line 5 Column 46  Pic X(15) Using First-Name
000356             Of Work-Record.
000357*
000358     03  Reverse-Video-Group Reverse-Video.
000359         05  Line 5  Column 70 Pic X(10) Using Middle-Name
000360             Of Work-Record.
000361         05  Line 6  Column 15 Pic X(50) Using Address-Line-1
000362             Of Work-Record.
000363         05  Line 7  Column 15 Pic X(50) Using Address-Line-2
000364             Of Work-Record.
000365         05  Line 8  Column 15 Pic X(40) Using City
000366             Of Work-Record.
000367         05  Line 9  Column 15 Pic X(20) Using State-Or-Country
000368             Of Work-Record.
000369         05  Line 9  Column 50 Pic X(15) Using Postal-Code
000370             Of Work-Record.
000371         05  Line 11 Column 13 Pic X(20) Using Home-Phone
000372             Of Work-Record.
000373         05  Line 11 Column 41 Pic X(20) Using Work-Phone
000374             Of Work-Record.
000375         05  Line 12 Column 13 Pic X(20) Using Other-Phone
000376             Of Work-Record.
000377         05  Line 14 Column 13 Pic 99/99/9999 Using Start-Date
000378             Of Work-Record.
000379         05  Line 14 Column 40 Pic 99/99/9999
000380             Using Last-Rent-Paid-Date Of Work-Record.
000382         05  Line 14 Column 69 Pic 99/99/9999
000383             Using Next-Rent-Due-Date Of Work-Record.
000385         05  Line 15 Column 14 Pic Z,ZZZ.99 Using Rent-Amount
000386             Of Work-Record.
000387         05  Line 16 Column 22 Pic ZZ9 Using Consignment-Percent
000388             Of Work-Record.
000389     03  Blink-Group Highlight Blink.
000390         05  Line 20 Column 01 Pic X(60) From Error-Message.
000391*
000392
000393 Procedure Division.
000394 Chapt14b-Start.
000395     Perform Open-File
000396     If Dealer-Success
000397        Initialize Work-Record
000398        Perform Process-Screen Until F1-Pressed Or
000399                                 Not Dealer-Success
000400        Perform Close-File
000401     End-If
000402     Stop Run
000403     .
000404  Process-Screen.
000405     Display Data-Entry-Screen
000406     Accept Data-Entry-Screen
000407     If F2-Pressed
000408        Perform Save-Record
000409     End-If
000410     .
000411 Save-Record.
000412     Move Corresponding Work-Record To Dealer-Record
000413     Write Dealer-Record
000414     If Not Dealer-Success
000415        Move Dealer-Status To Write-Error-Status
000416        Move Write-Error To Error-Message
000417        Perform Display-And-Accept-Error
000418     Else
000419        Initialize Work-Record
000420        Move 1 To Cursor-Row
000421                  Cursor-Column
000422     End-If
000423     .
000424 Open-File.
000425     Open Output Dealer-File
000426     If Not Dealer-Success
000427        Move Dealer-Status To Open-Error-Status
000428        Move Open-Error To Error-Message
000429        Perform Display-And-Accept-Error
000430     End-If
000431     .
000432 Close-File.
000433     Close Dealer-File
000434     .
000435 Display-And-Accept-Error.
000436     Display Data-Entry-Screen
000437     Accept Data-Entry-Screen
000438     .