000010 @OPTIONS MAIN,TEST
000020 Identification Division.
000030 Program-Id.  Chapt14d.
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
000060            Access Random
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
000387 01  Data-Entry-Screen
000388     Blank Screen, Auto
000389     Foreground-Color Is 7,
000390     Background-Color Is 1.
000391*
000392     03  Screen-Literal-Group.
000393         05  Line 01 Column 30 Value "Darlene's Treasures"
000394             Highlight Foreground-Color 4 Background-Color 1.
000395         05  Line 03 Column 30 Value "Tenant Entry Program"
000396             Highlight.
000397         05  Line 4  Column 01  Value "Number: ".
000398         05  Line 5  Column 01  Value "Name, Last: ".
000399         05  Line 5  Column 39  Value "First: ".
000400         05  Line 5  Column 62  Value "Middle: ".
000401         05  Line 6  Column 01  Value "Address 1: ".
000402         05  Line 7  Column 01  Value "Address 2: ".
000403         05  Line 8  Column 01  Value "City: ".
000404         05  Line 9  Column 01  Value "Country/State: ".
000405         05  Line 9  Column 36  Value "Postal Code: ".
000406         05  Line 11 Column 01  Value "Phone/Home: ".
000407         05  Line 11 Column 34  Value "Work: ".
000408         05  Line 12 Column 06  Value "Other: ".
000409         05  Line 14 Column 01  Value "Start Date: ".
000410         05  Line 14 Column 24  Value "Last Paid Date: ".
000411         05  Line 14 Column 51  Value "Next Rent Due on: ".
000412         05  Line 15 Column 01  Value "Rent Amount: ".
000413         05  Line 16 Column 01  Value "Consignment Percent: ".
000414         05  Line 22 Column 01  Value "F1-Exit    F2-Save".
000415*
000416     03  Required-Reverse-Group Reverse-Video Required.
000417         05  Line 4 Column 13  Pic X(8)  Using Dealer-Number
000418             Of Work-Record.
000419         05  Line 5 Column 13  Pic X(25) Using Last-Name
000420             Of Work-Record.
000421         05  Line 5 Column 46  Pic X(15) Using First-Name
000422             Of Work-Record.
000423*
000424     03  Reverse-Video-Group Reverse-Video.
000425         05  Line 5  Column 70 Pic X(10) Using Middle-Name
000426             Of Work-Record.
000427         05  Line 6  Column 15 Pic X(50) Using Address-Line-1
000428             Of Work-Record.
000429         05  Line 7  Column 15 Pic X(50) Using Address-Line-2
000430             Of Work-Record.
000431         05  Line 8  Column 15 Pic X(40) Using City
000432             Of Work-Record.
000433         05  Line 9  Column 15 Pic X(20) Using State-Or-Country
000434             Of Work-Record.
000435         05  Line 9  Column 50 Pic X(15) Using Postal-Code
000436             Of Work-Record.
000437         05  Line 11 Column 13 Pic X(20) Using Home-Phone
000438             Of Work-Record.
000439         05  Line 11 Column 41 Pic X(20) Using Work-Phone
000440             Of Work-Record.
000441         05  Line 12 Column 13 Pic X(20) Using Other-Phone
000442             Of Work-Record.
000443         05  Line 14 Column 13 Pic 99/99/9999 Using Start-Date
000444             Of Work-Record.
000445         05  Line 14 Column 40 Pic 99/99/9999
000446             Using Last-Rent-Paid-Date Of Work-Record.
000447         05  Line 14 Column 69 Pic 99/99/9999
000448             Using Next-Rent-Due-Date Of Work-Record.
000449         05  Line 15 Column 14 Pic Z,ZZZ.99 Using Rent-Amount
000450             Of Work-Record.
000451         05  Line 16 Column 22 Pic ZZ9 Using Consignment-Percent
000452             Of Work-Record.
000453     03  Blink-Group Highlight Blink.
000454         05  Line 20 Column 01 Pic X(60) From Error-Message.
000455*
000457 Procedure Division.
000458 Declaratives.
000459 Input-File-Error Section.
000460     Use After Standard Error Procedure On Dealer-File.
000461 Dealer-File-Error.
000462     String "Error On Dealer-File " Dealer-Status
000463       Delimited By Size Into Error-Message
000464     End-String
000465     Display Data-Entry-Screen
000466     Accept Data-Entry-Screen
000467     .
000468 End Declaratives.
000469 Chapt14d-Start Section.
000470     Perform Open-File
000471     If Dealer-Success
000472        Initialize Work-Record
000473        Perform Process-Screen Until F1-Pressed Or
000474                                 Not Dealer-Success
000475        Perform Close-File
000476     End-If
000477     Stop Run
000478     .
000479  Process-Screen.
000480     Perform Display-And-Accept
000481     If F2-Pressed
000482        Perform Save-Record
000483     End-If
000484     .
000485 Save-Record.
000486     Move Corresponding Work-Record To Dealer-Record
000487     Write Dealer-Record
000488      Invalid Key
000489        Move Dealer-Status To Write-Error-Status
000490        Move Write-Error To Error-Message
000491        Perform Display-And-Accept
000492      Not Invalid Key
000493        Initialize Work-Record
000494        Move 1 To Cursor-Row
000495                  Cursor-Column
000496     End-Write
000497     .
000498 Display-And-Accept.
000499     Display Data-Entry-Screen
000500     Accept Data-Entry-Screen
000501     .
000502 Open-File.
000503     Open Output Dealer-File
000504     .
000505 Close-File.
000506     Close Dealer-File
000507     .