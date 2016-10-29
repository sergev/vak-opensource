000010 @OPTIONS MAIN,TEST
000020 Identification Division.
000030 Program-Id.  Chapt18a.
000031* Transaction Entry
000043 Environment Division.
000050 Configuration Section.
000055 Source-Computer.  IBM-PC.
000056 Object-Computer.  IBM-PC.
000057 Special-Names.
000058     Crt Status Is Keyboard-Status
000059     Cursor Is Cursor-Position.
000061 Input-Output  Section.
000062 File-Control.
000063     Select Optional Trans-File Assign To "TRANS.TXT"
000064         Organization Is Line Sequential
000065         File Status  Is Trans-File-Status.
000066 Data Division.
000067 File Section.
000068 Fd  Trans-File.
000069 01  Trans-Record.
000070     03  Transaction-Date   Pic  9(8).
000071     03  Transaction-Type   Pic  X(4).
000072     03  Transaction-Dealer Pic  X(8).
000073     03  Transaction-Price  Pic S9(7)v99.
000074     03  Transaction-Qty    Pic  9(3).
000075     03  Filler             Pic  X(40).
000076 Working-Storage Section.
000077 01  Keyboard-Status.
000078     03  Accept-Status      Pic 9.
000079     03  Function-Key       Pic X.
000080         88 F1-Pressed      Value X"01".
000081         88 F3-Pressed      Value X"03".
000082         88 F4-Pressed      Value X"04".
000083     03  System-Use         Pic X.
000084 01  Cursor-Position.
000085     03  Cursor-Row    Pic 9(2) Value 1.
000086     03  Cursor-Column Pic 9(2) Value 1.
000088 01  File-Error-Flag        Pic X Value Space.
000089     88  File-Error         Value "Y".
000090 01  Trans-File-Status      Pic XX Value Spaces.
000091     88  Trans-File-Success Value "00" Thru "09".
000092 01  Error-Message         Pic X(50) Value Spaces.
000093 01  Open-Error-Message.
000094     03  Filler        Pic X(31)
000095         Value "Error Opening Transaction File ".
000096     03  Open-Status   Pic XX    Value Spaces.
000097 01  Write-Error-Message.
000098     03  Filler        Pic X(31)
000099         Value "Error Writing Transaction File ".
000100     03  Write-Status  Pic XX    Value Spaces.
000101 Screen Section.
000102 01  Data-Entry-Screen
000103     Blank Screen, Auto
000104     Foreground-Color Is 7,
000105     Background-Color Is 1.
000106*
000107     03  Screen-Literal-Group.
000108         05  Line 01 Column 30 Value "Darlene's Treasures"
000109             Highlight Foreground-Color 4 Background-Color 1.
000110         05  Line 03 Column 28 Value "Transaction Entry Program"
000111             Highlight.
000112         05  Line 4  Column 01  Value "Date: ".
000113         05  Line 5  Column 01  Value "Category: ".
000114         05  Line 6  Column 01  Value "Dealer Number: ".
000115         05  Line 7  Column 01  Value "Price: ".
000116         05  Line 8  Column 01  Value "Quantity: ".
000122         05  Line 22 Column 01  Value "F1-Save Record".
000124         05  Line 22 Column 23  Value "F3-Exit".
000125         05  Line 22 Column 56  Value "F4-Clear".
000127     03  Required-Reverse-Group Reverse-Video Required.
000128         05  Line 4 Column 16  Pic 99/99/9999
000129             Using Transaction-Date.
000130         05  Line 5 Column 16  Pic X(4)
000131             Using Transaction-Type.
000132         05  Line 6 Column 16  Pic X(8)
000133             Using Transaction-Dealer.
000134         05  Line 7 Column 16  Pic ZZ,ZZZ.99-
000135             Using Transaction-Price
000136             Blank When Zero.
000137         05  Line 8 Column 16  Pic ZZ9
000138             Using Transaction-Qty
000139             Blank When Zero.
000143     03  Highlight-Display Highlight.
000163         05  Line 20 Column 01 Pic X(50) From Error-Message
000164             Foreground-Color 5 Background-Color 1.
000166 Procedure Division.
000167 Chapt18a-Start.
000168     Perform Open-File
000170     If Not File-Error
000171        Initialize Trans-Record
000172        Perform Process-Input Until F3-Pressed Or
000173                                    File-Error
000174        Perform Close-File
000175     End-If
000176     Stop Run
000177     .
000178 Open-File.
000187     Open Extend Trans-File
000197     If Not Trans-File-Success
000198        Move Trans-File-Status To Open-Status
000199        Move Open-Error-Message To Error-Message
000200        Perform Display-And-Accept-Error
000257     End-If
000267     .
000277 Process-Input.
000288     Display Data-Entry-Screen
000297     Accept Data-Entry-Screen
000298     Move Spaces To Error-Message
000299     Evaluate True
000300        When F1-Pressed
000301             Perform Write-Record
000302        When F4-Pressed
000303             Initialize Trans-Record
000304        When F3-Pressed
000305             Continue
000306        When Other
000307             Continue
000308     End-Evaluate
000317     .
000327 Write-Record.
000337     Write Trans-Record
000338     If Trans-File-Success
000339        Initialize Trans-Record
000340        Move "Record Written" To Error-Message
000341        Move "0101" To Cursor-Position
000342     Else
000351        Move Trans-File-Status To Write-Status
000352        Move Write-Error-Message To Error-Message
000357        Perform Display-And-Accept-Error
000397     End-If
000407     .
000417 Display-And-Accept-Error.
000427     Set File-Error To True
000437     Display Data-Entry-Screen
000447     Accept Data-Entry-Screen
000457     .
000467 Close-File.
000477     Close Trans-File
000487     .