000010 @OPTIONS MAIN,TEST
000020 Identification Division.
000030 Program-Id.  Chapt14x.
000031* Dealer Data Entry Exercise
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
000278 01  End-Of-Process-Flag     Pic X Value Spaces.
000279     88  End-Process         Value "Y".
000280 Screen Section.
000388 01  Data-Entry-Screen
000389     Blank Screen, Auto
000390     Foreground-Color Is 7,
000391     Background-Color Is 1.
000392*
000393     03  Screen-Literal-Group.
000394         05  Line 01 Column 30 Value "Darlene's Treasures"
000395             Highlight Foreground-Color 4 Background-Color 1.
000396         05  Line 03 Column 30 Value "Tenant Entry Program"
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
000414         05  Line 16 Column 01  Value "Consignment Percent: ".
000415         05  Line 22 Column 01  Value "F1-Exit    F2-Save".
000416*
000417     03  Required-Reverse-Group Reverse-Video Required.
000418         05  Line 4 Column 13  Pic X(8)  Using Dealer-Number
000419             Of Work-Record.
000420         05  Line 5 Column 13  Pic X(25) Using Last-Name
000421             Of Work-Record.
000422         05  Line 5 Column 46  Pic X(15) Using First-Name
000423             Of Work-Record.
000424*
000425     03  Reverse-Video-Group Reverse-Video.
000426         05  Line 5  Column 70 Pic X(10) Using Middle-Name
000427             Of Work-Record.
000428         05  Line 6  Column 15 Pic X(50) Using Address-Line-1
000429             Of Work-Record.
000430         05  Line 7  Column 15 Pic X(50) Using Address-Line-2
000431             Of Work-Record.
000432         05  Line 8  Column 15 Pic X(40) Using City
000433             Of Work-Record.
000434         05  Line 9  Column 15 Pic X(20) Using State-Or-Country
000435             Of Work-Record.
000436         05  Line 9  Column 50 Pic X(15) Using Postal-Code
000437             Of Work-Record.
000438         05  Line 11 Column 13 Pic X(20) Using Home-Phone
000439             Of Work-Record.
000440         05  Line 11 Column 41 Pic X(20) Using Work-Phone
000441             Of Work-Record.
000442         05  Line 12 Column 13 Pic X(20) Using Other-Phone
000443             Of Work-Record.
000444         05  Line 14 Column 13 Pic 99/99/9999 Using Start-Date
000445             Of Work-Record.
000446         05  Line 14 Column 40 Pic 99/99/9999
000447             Using Last-Rent-Paid-Date Of Work-Record.
000448         05  Line 14 Column 69 Pic 99/99/9999
000449             Using Next-Rent-Due-Date Of Work-Record.
000450         05  Line 15 Column 14 Pic Z,ZZZ.99 Using Rent-Amount
000451             Of Work-Record.
000452         05  Line 16 Column 22 Pic ZZ9 Using Consignment-Percent
000453             Of Work-Record.
000454     03  Blink-Group Highlight Blink.
000455         05  Line 20 Column 01 Pic X(60) From Error-Message.
000456*
000458 Procedure Division.
000459 Declaratives.
000460 Input-File-Error Section.
000461     Use After Standard Error Procedure On Dealer-File.
000462 Dealer-File-Error.
000463     String "Error On Dealer-File " Dealer-Status
000464       Delimited By Size Into Error-Message
000465     End-String
000466     Display Data-Entry-Screen
000467     Accept Data-Entry-Screen
000468     Set End-Process To True
000469     .
000470 End Declaratives.
000471 Chapt14x-Start Section.
000472     Perform Open-File
000473     If Dealer-Success
000474        Initialize Work-Record
000475        Perform Process-Screen Until F1-Pressed Or
000476                                     End-Process
000477        Perform Close-File
000478     End-If
000479     Stop Run
000480     .
000481  Process-Screen.
000482     Perform Display-And-Accept
000483     If F2-Pressed
000484        Perform Save-Record
000485     End-If
000486     .
000487 Save-Record.
000488     Move Corresponding Work-Record To Dealer-Record
000489     Write Dealer-Record
000490      Invalid Key
000491        Perform Generate-Proper-Error
000492      Not Invalid Key
000493        Initialize Work-Record
000494        Move 1 To Cursor-Row
000495                  Cursor-Column
000496     End-Write
000497     .
000498 Generate-Proper-Error.
000499     If Dealer-Status = "22"
000500        Move "Duplicate Information, record not written"
000501        To   Error-Message
000502     Else
000503        Move Dealer-Status To Write-Error-Status
000504        Move Write-Error To Error-Message
000505        Perform Display-And-Accept
000506        Set End-Process To True
000507     End-If
000508     .
000509 Display-And-Accept.
000510     Display Data-Entry-Screen
000511     Accept Data-Entry-Screen
000512* Clear So Next Display Does Not Show The Old Error Message
000513     Move Spaces To Error-Message
000514     .
000515 Open-File.
000516     Open Output Dealer-File
000517     .
000518 Close-File.
000519     Close Dealer-File
000520     .