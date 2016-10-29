000010 @OPTIONS MAIN,TEST
000020 Identification Division.
000030 Program-Id.  Chapt15d.
000031* Dynamic Access Example
000041 Environment Division.
000050 Configuration Section.
000051 Special-Names.
000052       Crt Status Is Keyboard-Status
000053       Cursor Is Cursor-Position.
000055 Source-Computer.  IBM-PC.
000056 Object-Computer.  IBM-PC.
000057 Input-Output Section.
000058 File-Control.
000059     Select Dealer-File Assign To "Dealer.Dat"
000060            Organization Indexed
000061            Access Dynamic
000062            Record Key Dealer-Number
000063            Alternate Record Key Dealer-Name
000064            File Status Dealer-Status.
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
000240     88  Dealer-Success Value "00" Thru "09".
000279 01  Keyboard-Status.
000280     03  Accept-Status Pic 9.
000281     03  Function-Key  Pic X.
000282         88  F1-Pressed Value X"01".
000283         88  F2-Pressed Value X"02".
000284         88  F3-Pressed Value X"03".
000285         88  F4-Pressed Value X"04".
000286         88  F5-Pressed Value X"05".
000287         88  F6-Pressed Value X"06".
000289     03  System-Use    Pic X.
000290 01  Cursor-Position.
000291     03  Cursor-Row    Pic 9(2) Value 1.
000292     03  Cursor-Column Pic 9(2) Value 1.
000293 01  Error-Message Pic X(50) Value Spaces.
000294 Screen Section.
000442 01  Data-Entry-Screen
000443     Blank Screen, Auto
000444     Foreground-Color is 7,
000445     Background-Color is 1. 
000447     03  Screen-Literal-Group.
000448         05  Line 01 Column 30 Value "Darlene's Treasures"
000449             Highlight Foreground-Color 4 Background-Color 1.
000450         05  Line 03 Column 30 Value "Tenant Entry Program"
000451             Highlight. 
000452         05  Line 4  Column 01  Value "Number: ".            
000453         05  Line 5  Column 01  Value "Name, Last: ".
000454         05  Line 5  Column 39  Value "First: ".
000455         05  Line 5  Column 62  Value "Middle: ".
000456         05  Line 6  Column 01  Value "Address 1: ".
000457         05  Line 7  Column 01  Value "Address 2: ".
000458         05  Line 8  Column 01  Value "City: ".
000459         05  Line 9  Column 01  Value "Country/State: ".
000460         05  Line 9  Column 36  Value "Postal Code: ".
000461         05  Line 11 Column 01  Value "Phone/Home: ".
000462         05  Line 11 Column 34  Value "Work: ".
000463         05  Line 12 Column 06  Value "Other: ".
000464         05  Line 14 Column 01  Value "Start Date: ".
000465         05  Line 14 Column 24  Value "Last Paid Date: ".
000466         05  Line 14 Column 51  Value "Next Rent Due on: ".
000467         05  Line 15 Column 01  Value "Rent Amount: ".
000468         05  Line 16 Column 01  Value "Consignment Percent: ".
000469         05  Line 22 Column 01  Value "F1-Read Random Number".
000470         05  Line 22 Column 23  Value "F2-Read Random Name".
000471         05  Line 22 Column 56  Value "F3-Read Next Number".
000472         05  Line 23 Column 01  Value "F4-Read Next Name".
000473         05  Line 23 Column 23  Value "F5-Clear".
000474         05  Line 23 Column 56  Value "F6-Exit".
000475     03  Required-Reverse-Group Reverse-Video.
000476         05  Line 4 Column 13  Pic X(8)  Using Dealer-Number.
000477         05  Line 5 Column 13  Pic X(25) Using Last-Name.
000478         05  Line 5 Column 46  Pic X(15) Using First-Name.
000479         05  Line 5  Column 70 Pic X(10) Using Middle-Name.
000480         05  Line 6  Column 15 Pic X(50) From Address-Line-1.
000481         05  Line 7  Column 15 Pic X(50) From Address-Line-2.
000482         05  Line 8  Column 15 Pic X(40) From City.
000483         05  Line 9  Column 15 Pic X(20) From State-Or-Country.
000484         05  Line 9  Column 50 Pic X(15) From Postal-Code.
000485         05  Line 11 Column 13 Pic X(20) From Home-Phone.
000486         05  Line 11 Column 41 Pic X(20) From Work-Phone.
000487         05  Line 12 Column 13 Pic X(20) From Other-Phone.
000488         05  Line 14 Column 13 Pic 99/99/9999 From Start-Date.
000489         05  Line 14 Column 40 Pic 99/99/9999 From Last-Rent-Paid-Date.
000490         05  Line 14 Column 69 Pic 99/99/9999 From Next-Rent-Due-Date.
000491         05  Line 15 Column 14 Pic Z,ZZZ.99 From Rent-Amount.
000492         05  Line 16 Column 22 Pic ZZ9 From Consignment-Percent.
000493         05  Line 20 Column 01 Pic X(50) Using Error-Message.
000496 Procedure Division.
000497 Chapt15d-Start.
000498     Perform Open-File
000499     If Not Dealer-Success
000500        String "Error Opening Dealer File "
000501               Dealer-Status
000502               Delimited By Size
000503               Into Error-Message
000504        End-String
000505        Perform Display-And-Accept
000506     Else
000507        Initialize Dealer-Record
000508        Perform Process-File Until F6-Pressed
000509        Perform Close-File
000510     End-If
000511     Stop Run
000512     .
000513 Process-File.
000514     Perform Display-And-Accept
000515     Evaluate True
000516       When F1-Pressed
000517            Perform Read-Random-Number
000518       When F2-Pressed
000519            Perform Read-Random-Name
000520       When F3-Pressed
000521            Perform Read-Next-Number
000522       When F4-Pressed
000523            Perform Read-Next-Name
000524       When F5-Pressed
000525            Perform Clear-Screen
000526       When F6-Pressed
000527            Continue
000528       When Other
000529            Continue
000530     End-Evaluate
000531     .
000532 Read-Random-Number.
000533     Read Dealer-File
000534       Invalid Key
000535          String "Error on Random Read Number "
000536                 Dealer-Status
000537                 Delimited By Size
000538                 Into Error-Message
000539     End-Read
000540     .
000541 Read-Random-Name.
000542     Read Dealer-File Key Dealer-Name
000543       Invalid Key
000544          String "Error on Random Read Name "
000545                 Dealer-Status
000546                 Delimited By Size
000547                 Into Error-Message
000548     End-Read
000549     .
000550 Read-Next-Number.
000551     Start Dealer-File Key > Dealer-Number
000552      Invalid Key
000553         String "Start Error Number "
000554                Dealer-Status
000555                Delimited By Size
000556                Into Error-Message
000557     End-Start
000558     If Dealer-Success
000559       Read Dealer-File Next
000560          At End
000561             Move "End of File, Read by Number" To Error-Message
000562       End-Read
000563     End-If
000564     .
000565 Read-Next-Name.
000566     Start Dealer-File Key > Dealer-Name
000567      Invalid Key
000568         String "Start Error Name "
000569                Dealer-Status
000570                Delimited By Size
000571                Into Error-Message
000572     End-Start
000573     If Dealer-Success
000574       Read Dealer-File Next
000575          At End
000576             Move "End of File, Read by Name" To Error-Message
000577       End-Read
000578     End-If
000579     .
000580 Clear-Screen.
000581     Initialize Dealer-Record
000582     Move 01 To Cursor-Row Cursor-Column
000583     .
000584 Display-And-Accept.
000585     Display Data-Entry-Screen
000586     Accept Data-Entry-Screen
000587     Move Spaces To Error-Message
000588     .
000589 Open-File.
000590     Open Input Dealer-File
000591     .
000592 Close-File.
000593     Close Dealer-File
000594     .
