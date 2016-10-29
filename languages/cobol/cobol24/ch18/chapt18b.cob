000010 @OPTIONS MAIN,TEST
000020 Identification Division.
000030 Program-Id.  Chapt18b.
000031* Transaction Entry - With Data Validation
000043 Environment Division.
000050 Configuration Section.
000055 Source-Computer.  IBM-PC.
000056 Object-Computer.  IBM-PC.
000057 Special-Names.
000058     Crt Status Is Keyboard-Status
000059     Cursor Is Cursor-Position.
000061 Input-Output  Section.
000062 File-Control.
000063     Select Optional Trans-File Assign To "Trans.Txt"
000064         Organization Is Line Sequential
000065         File Status  Is Trans-File-Status.
000066     Select Dealer-File Assign To "Dealer.Dat"
000067         Organization Indexed
000068         Access Random
000069         Record Key Dealer-Number
000070         Alternate Record Key Dealer-Name
000071         File Status Dealer-Status.
000073 Data Division.
000074 File Section.
000075 Fd  Trans-File.
000076 01  Trans-Record.
000077     03  Transaction-Date   Pic  9(8).
000078     03  Transaction-Text.
000079         05  Transaction-Type   Pic  X(4).
000080         05  Transaction-Dealer Pic  X(8).
000081     03  Transaction-Price  Pic S9(7)v99.
000082     03  Transaction-Qty    Pic  9(3).
000083     03  Filler             Pic  X(40).
000084 Fd  Dealer-File.
000085 01  Dealer-Record.
000086     03  Dealer-Number         Pic X(8).
000087     03  Dealer-Name.
000088         05  Last-Name   Pic X(25).
000089         05  First-Name  Pic X(15).
000090         05  Middle-Name Pic X(10).
000091     03  Address-Line-1      Pic X(50).
000092     03  Address-Line-2      Pic X(50).
000093     03  City                Pic X(40).
000094     03  State-Or-Country    Pic X(20).
000095     03  Postal-Code         Pic X(15).
000096     03  Home-Phone          Pic X(20).
000097     03  Work-Phone          Pic X(20).
000098     03  Other-Phone         Pic X(20).
000099     03  Start-Date          Pic 9(8).
000100     03  Last-Rent-Paid-Date Pic 9(8).
000101     03  Next-Rent-Due-Date  Pic 9(8).
000102     03  Rent-Amount         Pic 9(4)v99.
000103     03  Consignment-Percent Pic 9(3).
000104     03  Last-Sold-Amount    Pic S9(7)v99.
000105     03  Last-Sold-Date      Pic 9(8).
000106     03  Sold-To-Date        Pic S9(7)v99.
000107     03  Commission-To-Date  Pic S9(7)v99.
000108     03  Filler              Pic X(15).
000109 Working-Storage Section.
000110 01  Keyboard-Status.
000111     03  Accept-Status      Pic 9.
000112     03  Function-Key       Pic X.
000113         88 F1-Pressed      Value X"01".
000114         88 F3-Pressed      Value X"03".
000115         88 F4-Pressed      Value X"04".
000116     03  System-Use         Pic X.
000117 01  Cursor-Position.
000118     03  Cursor-Row    Pic 9(2) Value 1.
000119     03  Cursor-Column Pic 9(2) Value 1.
000120 01  File-Error-Flag        Pic X Value Space.
000121     88  File-Error         Value "Y".
000122 01  Validate-Flag          Pic X Value "Y".
000123     88  Validation-Passed  Value "Y".
000124     88  Validation-Error   Value "N".
000125 01  Trans-File-Status      Pic XX Value Spaces.
000126     88  Trans-File-Success Value "00" Thru "09".
000127 01  Dealer-Status     Pic X(2) Value Spaces.
000128     88  Dealer-Success Value "00" Thru "09".
000130 01  Error-Message         Pic X(50) Value Spaces.
000131 01  Open-Error-Message.
000132     03  Filler        Pic X(31)
000133         Value "Error Opening Transaction File ".
000134     03  Open-Status   Pic XX    Value Spaces.
000135 01  Dealer-Open-Error-Message.
000136     03  Filler        Pic X(31)
000137         Value "Error Opening Dealer File ".
000138     03  Open-Dealer-Status   Pic XX    Value Spaces.
000139 01  Write-Error-Message.
000140     03  Filler        Pic X(31)
000141         Value "Error Writing Transaction File ".
000142     03  Write-Status  Pic XX    Value Spaces.
000143 Screen Section.
000144 01  Data-Entry-Screen
000145     Blank Screen, Auto
000146     Foreground-Color Is 7,
000147     Background-Color Is 1.
000148*
000149     03  Screen-Literal-Group.
000150         05  Line 01 Column 30 Value "Darlene's Treasures"
000151             Highlight Foreground-Color 4 Background-Color 1.
000152         05  Line 03 Column 28 Value "Transaction Entry Program"
000153             Highlight.
000154         05  Line 4  Column 01  Value "Date: ".
000155         05  Line 5  Column 01  Value "Category: ".
000156         05  Line 6  Column 01  Value "Dealer Number: ".
000157         05  Line 7  Column 01  Value "Price: ".
000158         05  Line 8  Column 01  Value "Quantity: ".
000159         05  Line 22 Column 01  Value "F1-Save Record".
000160         05  Line 22 Column 23  Value "F3-Exit".
000161         05  Line 22 Column 56  Value "F4-Clear".
000162     03  Required-Reverse-Group Reverse-Video Required.
000163         05  Line 4 Column 16  Pic 99/99/9999
000164             Using Transaction-Date.
000165         05  Line 5 Column 16  Pic X(4)
000166             Using Transaction-Type.
000167         05  Line 6 Column 16  Pic X(8)
000168             Using Transaction-Dealer.
000169         05  Line 7 Column 16  Pic ZZ,ZZZ.99-
000170             Using Transaction-Price
000171             Blank When Zero.
000172         05  Line 8 Column 16  Pic ZZ9
000173             Using Transaction-Qty
000174             Blank When Zero.
000175     03  Highlight-Display Highlight.
000176         05  Line 20 Column 01 Pic X(50) From Error-Message
000177             Foreground-Color 5 Background-Color 1.
000178 Procedure Division.
000179 Chapt18b-Start.
000180     Perform Open-File
000181     If Not File-Error
000182        Perform Open-Dealer-File
000183     End-If
000184     If Not File-Error
000185        Initialize Trans-Record
000186        Perform Process-Input Until F3-Pressed Or
000187                                    File-Error
000188        Perform Close-File
000189        Perform Close-Dealer-File
000190     End-If
000191     Stop Run
000192     .
000193 Open-File.
000194     Open Extend Trans-File
000197     If Not Trans-File-Success
000198        Move Trans-File-Status To Open-Status
000199        Move Open-Error-Message To Error-Message
000200        Perform Display-And-Accept-Error
000257     End-If
000267     .
000268 Open-Dealer-File.
000269     Open Input Dealer-File
000270     If Not Dealer-Success
000271        Move Dealer-Status To Open-Dealer-Status
000272        Move Dealer-Open-Error-Message To Error-Message
000273        Perform Display-And-Accept-Error
000274     End-If
000275     .
000277 Process-Input.
000288     Display Data-Entry-Screen
000297     Accept Data-Entry-Screen
000298     Move Spaces To Error-Message
000299     Evaluate True
000300        When F1-Pressed
000301             Perform Validate-Data
000302             If Validation-Passed
000303                Perform Write-Record
000304             End-If
000305        When F4-Pressed
000306             Initialize Trans-Record
000307        When F3-Pressed
000308             Continue
000309        When Other
000310             Continue
000311     End-Evaluate
000317     .
000318 Validate-Data.
000319     Inspect Transaction-Text Converting
000320             "abcdefghijklmnopqrstuvwxyz" To
000321             "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
000322     Move "Y" To Validate-Flag
000323     If Not (Transaction-Type = "ANTI" Or "CRAF" Or "HOLI" Or "JEWL" Or
000324             "MISC" Or "XMAS")
000325        Set Validation-Error To True
000326        Move "0516" To Cursor-Position
000327        Move
000328        "Invalid Category, Must be ANTI, CRAF, HOLI, JEWL, MISC or XMAS"
000329        To Error-Message
000330     End-If
000331     Move Transaction-Dealer To Dealer-Number
000332     Read Dealer-File
000333          Invalid Key
000334            Set Validation-Error To True
000335            Move "0616" To Cursor-Position
000336            Move "Invalid Dealer Number Entered" To Error-Message
000339     End-Read
000340     .
000341 Write-Record.
000342     Write Trans-Record
000343     If Trans-File-Success
000344        Initialize Trans-Record
000345        Move "Record Written" To Error-Message
000346        Move "0101" To Cursor-Position
000347     Else
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
000497 Close-Dealer-File.
000507     Close Dealer-File
000517     .
