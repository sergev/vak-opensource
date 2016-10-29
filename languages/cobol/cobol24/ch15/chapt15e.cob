000010 @OPTIONS MAIN,TEST
000020 Identification Division.
000030 Program-Id.  Chapt15e.
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
000071         05  Last-Name.
000072             10  Last-Name-First-Letter Pic X.
000073             10  Last-Name-Remainder    Pic X(24).
000074         05  First-Name  Pic X(15).
000075         05  Middle-Name Pic X(10).
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
000288         88  F7-Pressed Value X"07".
000289     03  System-Use    Pic X.
000290 01  Cursor-Position.
000291     03  Cursor-Row    Pic 9(2) Value 1.
000292     03  Cursor-Column Pic 9(2) Value 1.
000293 01  Error-Message Pic X(50) Value Spaces.
000294 Screen Section.
000304 01  Data-Entry-Screen
000305     Blank Screen, Auto
000306     Foreground-Color is 7,
000307     Background-Color is 1. 
000308*
000309     03  Screen-Literal-Group.
000310         05  Line 01 Column 30 Value "Darlene's Treasures"
000311             Highlight Foreground-Color 4 Background-Color 1.
000312         05  Line 03 Column 30 Value "Tenant Entry Program"
000313             Highlight. 
000314         05  Line 4  Column 01  Value "Number: ".            
000315         05  Line 5  Column 01  Value "Name, Last: ".
000316         05  Line 5  Column 39  Value "First: ".
000317         05  Line 5  Column 62  Value "Middle: ".
000318         05  Line 6  Column 01  Value "Address 1: ".
000319         05  Line 7  Column 01  Value "Address 2: ".
000320         05  Line 8  Column 01  Value "City: ".
000321         05  Line 9  Column 01  Value "Country/State: ".
000322         05  Line 9  Column 36  Value "Postal Code: ".
000323         05  Line 11 Column 01  Value "Phone/Home: ".
000324         05  Line 11 Column 34  Value "Work: ".
000325         05  Line 12 Column 06  Value "Other: ".
000326         05  Line 14 Column 01  Value "Start Date: ".
000327         05  Line 14 Column 24  Value "Last Paid Date: ".
000328         05  Line 14 Column 51  Value "Next Rent Due on: ".
000329         05  Line 15 Column 01  Value "Rent Amount: ".
000330         05  Line 16 Column 01  Value "Consignment Percent: ".
000331         05  Line 22 Column 01  Value "F1-Read Random Number".
000332         05  Line 22 Column 23  Value "F2-Read Random Name".
000333         05  Line 22 Column 56  Value "F3-Read Next Number".
000334         05  Line 23 Column 01  Value "F4-Read Next Name".
000335         05  Line 23 Column 23  Value "F5-Clear".
000336         05  Line 23 Column 56  Value "F6-Read via First Letter".
000337         05  Line 24 Column 01  Value "F7-Exit".
000338     03  Required-Reverse-Group Reverse-Video.
000339         05  Line 4 Column 13  Pic X(8)  Using Dealer-Number.
000340         05  Line 5 Column 13  Pic X(25) Using Last-Name.
000341         05  Line 5 Column 46  Pic X(15) Using First-Name.
000342         05  Line 5  Column 70 Pic X(10) Using Middle-Name.
000343         05  Line 6  Column 15 Pic X(50) From Address-Line-1.
000344         05  Line 7  Column 15 Pic X(50) From Address-Line-2.
000345         05  Line 8  Column 15 Pic X(40) From City.
000346         05  Line 9  Column 15 Pic X(20) From State-Or-Country.
000347         05  Line 9  Column 50 Pic X(15) From Postal-Code.
000348         05  Line 11 Column 13 Pic X(20) From Home-Phone.
000349         05  Line 11 Column 41 Pic X(20) From Work-Phone.
000350         05  Line 12 Column 13 Pic X(20) From Other-phone.
000351         05  Line 14 Column 13 Pic 99/99/9999 From Start-Date.
000352         05  Line 14 Column 40 Pic 99/99/9999 From Last-Rent-Paid-Date.
000353         05  Line 14 Column 69 Pic 99/99/9999 From Next-Rent-Due-Date.
000354         05  Line 15 Column 14 Pic Z,ZZZ.99 From Rent-Amount.
000355         05  Line 16 Column 22 Pic ZZ9 From Consignment-Percent.
000356         05  Line 20 Column 01 Pic X(50) Using Error-Message.
000357*
000439 Procedure Division.
000440 Chapt15e-Start.
000441     Perform Open-File
000442     If Not Dealer-Success
000443        String "Error Opening Dealer File "
000448               Dealer-Status
000449               Delimited By Size
000450               Into Error-Message
000451        End-String
000452        Perform Display-And-Accept
000454     Else
000455        Initialize Dealer-Record
000456        Perform Process-File Until F7-Pressed
000457        Perform Close-File
000458     End-If
000459     Stop Run
000460     .
000461 Process-File.
000462     Perform Display-And-Accept
000463     Evaluate True
000464       When F1-Pressed
000465            Perform Read-Random-Number
000466       When F2-Pressed
000467            Perform Read-Random-Name
000468       When F3-Pressed
000469            Perform Read-Next-Number
000470       When F4-Pressed
000471            Perform Read-Next-Name
000472       When F5-Pressed
000473            Perform Clear-Screen
000474       When F6-Pressed
000475            Perform Read-Next-Letter
000476       When F7-Pressed
000477            Continue
000478       When Other
000479            Continue
000480     End-Evaluate
000481     .
000482 Read-Random-Number.
000483     Read Dealer-File
000484       Invalid Key
000485          String "Error on Random Read Number "
000486                 Dealer-Status
000487                 Delimited By Size
000488                 Into Error-Message
000489     End-Read
000490     .
000491 Read-Random-Name.
000492     Read Dealer-File Key Dealer-Name
000493       Invalid Key
000494          String "Error on Random Read Name "
000495                 Dealer-Status
000496                 Delimited By Size
000497                 Into Error-Message
000498     End-Read
000499     .
000500 Read-Next-Number.
000501     Start Dealer-File Key > Dealer-Number
000502      Invalid Key
000503         String "Start Error Number "
000504                Dealer-Status
000505                Delimited By Size
000506                Into Error-Message
000507     End-Start
000508     If Dealer-Success
000509       Read Dealer-File Next
000510          At End
000511             Move "End of File, Read by Number" To Error-Message
000514       End-Read
000515     End-If
000516     .
000517 Read-Next-Name.
000518     Start Dealer-File Key > Dealer-Name
000519      Invalid Key
000520         String "Start Error Name "
000521                Dealer-Status
000522                Delimited By Size
000523                Into Error-Message
000524     End-Start
000525     If Dealer-Success
000526       Read Dealer-File Next
000527          At End
000528             Move "End of File, Read by Name" To Error-Message
000529       End-Read
000534     End-If
000535     .
000536 Read-Next-Letter.
000537     Start Dealer-File Key > Last-Name-First-Letter
000538      Invalid Key
000539         String "Start Error First Letter "
000540                Dealer-Status
000541                Delimited By Size
000542                Into Error-Message
000543     End-Start
000544     If Dealer-Success
000545       Read Dealer-File Next
000546          At End
000547             Move "End of File, Read by Name" To Error-Message
000548       End-Read
000549     End-If
000550     .
000551 Clear-Screen.
000553     Initialize Dealer-Record
000554     Move 01 To Cursor-Row Cursor-Column
000555     .
000556 Display-And-Accept.
000557     Display Data-Entry-Screen
000558     Accept Data-Entry-Screen
000559     Move Spaces To Error-Message
000560     .
000561 Open-File.
000562     Open Input Dealer-File
000563     .
000564 Close-File.
000565     Close Dealer-File
000566     .
