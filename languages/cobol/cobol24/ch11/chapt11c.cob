000010 @OPTIONS MAIN,TEST
000020 Identification Division.
000030 Program-Id.  Chapt11c.
000031* Inline Perform With Inline If Example
000044 Environment Division.
000050 Configuration Section.
000051 Special-Names.
000052       Crt Status Is Keyboard-Status.
000054 Source-Computer.  IBM-PC.
000055 Object-Computer.  IBM-PC.
000056 Data Division.
000057 Working-Storage Section.
000067 01  Keyboard-Status.
000068     03  Accept-Status     Pic 9.
000069     03  Function-Key      Pic X.
000070         88 F1-Pressed     Value X"01".
000071     03  System-Use        Pic X.
000073 01  Temp-Field            Pic X(20) Value Spaces.
000074 01  Formatted-Number      Pic X(14) Value "(XXX) XXX-XXXX".
000075 01  Formatted-Alternate   Pic X(8)  Value "XXX-XXXX".
000076 01  Name-Length           Pic 99    Value Zeros.
000077 01  Counter               Pic 99    Value Zeros.
000079 01  Input-Output-Fields.
000080     03  Last-Name         Pic X(20) Value Spaces.
000081     03  First-Name        Pic X(20) Value Spaces.
000082     03  Phone-Number      Pic 9(10) Value Zeros.
000083     03  The-Edited-Number Pic X(14) Value Spaces.
000084     03  Combined-Name     Pic X(40) Value Spaces.
000085 Screen Section.
000086 01  Phone-Entry Blank Screen.
000087     03  Line 01 Column 01 Value " Enter Phone Number: ".
000088     03  Line 01 Column 22 Pic Z(10) Using Phone-Number.
000089     03  Line 02 Column 01 Value "    Enter Last Name: ".
000090     03  Line 02 Column 22 Pic X(20) Using Last-Name.
000091     03  Line 03 Column 01 Value "   Enter First Name: ".
000092     03  Line 03 Column 22 Pic X(20) Using First-Name.
000093     03  Line 05 Column 01 Value "          Full Name: ".
000094     03  Line 05 Column 22 Pic X(40) From Combined-Name.
000095     03  Line 07 Column 01 Value "Edited Phone Number: ".
000096     03  Line 07 Column 22 Pic X(14) From The-Edited-Number.
000097     03  Line 20 Column 01 Value "Press F1 to Exit".
000098 Procedure Division.
000159 Chapt11c-Start.
000160     Perform Until F1-Pressed
000161        Display Phone-Entry
000162        Accept Phone-Entry
000163* Prepare To Format The Numbers
000164        Move "(XXX) XXX-XXXX" To Formatted-Number
000165        Move "XXX-XXXX" To Formatted-Alternate
000166* Format Based On Size
000167        If Phone-Number > 9999999
000168           Inspect Formatted-Number
000169              Replacing First "XXX"  By Phone-Number (1:3)
000170                        First "XXX"  By Phone-Number (4:3)
000171                        First "XXXX" By Phone-Number (7:4)
000172           Move Formatted-Number To The-Edited-Number
000173        Else
000174           Inspect Formatted-Alternate
000175              Replacing First "XXX"  By Phone-Number (4:3)
000176                        First "XXXX" By Phone-Number (7:4)
000177           Move Formatted-Alternate To The-Edited-Number
000180        End-If
000181* Left Justify The First Name
000182* If It's Blank It's A Waste Of Time
000183        If First-Name > Spaces
000184           Perform Varying Counter From 1 By 1 Until
000186                           First-Name (Counter:1) > Space
000187              Continue
000188           End-Perform
000189* Counter Contains The Starting Offset
000190           Move First-Name (Counter:) To Temp-Field
000191           Move Temp-Field To First-Name
000192        End-If
000193* Left Justify The Last Name
000194        If Last-Name > Spaces
000195           Perform Varying Counter From 1 By 1 Until
000196                           Last-Name (Counter:1) > Space
000197              Continue
000198           End-Perform
000199           Move Last-Name (Counter:) To Temp-Field
000200           Move Temp-Field To Last-Name
000201        End-If
000202* Now Put Them Together
000203        Perform Varying Name-Length From 20 By -1
000204           Until First-Name (Name-Length:1) > Space
000205              Or Name-Length = Zeros
000206           Continue
000207        End-Perform
000208        If Name-Length = Zeros
000209           Move Last-Name To Combined-Name
000210        Else
000211           String First-Name (1:name-Length)
000212                  Space
000213                  Last-Name
000214                  Delimited By Size
000215                  Into Combined-Name
000216        End-If
000217* Now We Repeat
000218     End-Perform
000219     Stop Run
000229     .