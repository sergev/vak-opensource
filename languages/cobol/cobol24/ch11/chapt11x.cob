000010 @OPTIONS MAIN,TEST
000020 Identification Division.
000030 Program-Id.  Chapt11x.
000031* Your Answer To This Exercise May Be Different.
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
000072 01  Date-Field            Pic 9(8)  Value Zeros.
000073 01  Edited-Date-Field     Pic X(20) Value Spaces.
000074 01  Month-Field           Pic X(9)  Value Spaces.
000075 01  Month-Length          Pic 99    Value Zeros.
000076 01  Error-Flag            Pic X     Value Spaces.
000077     88  Month-Error       Value "Y".
000083 01  Error-Message         Pic X(50) Value Spaces.
000085 Screen Section.
000086 01  Date-Entry Blank Screen.
000087     03  Line 01 Column 01 Value " Enter Date: ".
000088     03  Line 01 Column 14 Pic 99/99/9999 Using Date-Field.
000089     03  Line 02 Column 01 Value "Edited Date: ".
000090     03  Line 02 Column 14 Pic X(20) From Edited-Date-Field.
000091     03  Line 05 Column 01 Pic X(50) From Error-Message.
000092     03  Line 20 Column 01 Value "Press F1 to Exit".
000100 Procedure Division.
000159 Chapt11x-Start.
000160     Perform Until F1-Pressed
000161        Display Date-Entry
000162        Accept Date-Entry
000163* Clear The Error Message For The Next Display
000164        Move Spaces To Error-Message
000165* If They Did Not Press F1 To Exit, It's Ok To Process The Input
000166        If Not F1-Pressed
000167           Perform Process-Input
000168        End-If
000169     End-Perform
000170     Stop Run
000171     .
000172 Process-Input.
000173* Reset The Error Flag.
000174     Move Spaces To Error-Flag
000175* Figure Out Which Month It Is.
000176* Month-Length Contains The Number Of Characters In The
000177* Month Name
000178     Evaluate Date-Field (1:2)
000179        When "01"
000180          Move "January" To Month-Field
000181          Move 7 To Month-Length
000182        When "02"
000183          Move "February" To Month-Field
000184          Move 8 To Month-Length
000185        When "03"
000186          Move "March" To Month-Field
000187          Move 5 To Month-Length
000188        When "04"
000189          Move "April" To Month-Field
000190          Move 5 To Month-Length
000191        When "05"
000192          Move "May" To Month-Field
000193          Move 3 To Month-Length
000194        When "06"
000195          Move "June" To Month-Field
000196          Move 4 To Month-Length
000197        When "07"
000198          Move "July" To Month-Field
000199          Move 4 To Month-Length
000200        When "08"
000201          Move "August" To Month-Field
000202          Move 6 To Month-Length
000203        When "09"
000204          Move "September" To Month-Field
000205          Move 9 To Month-Length
000206        When "10"
000207          Move "October" To Month-Field
000208          Move 7 To Month-Length
000209        When "11"
000210          Move "November" To Month-Field
000211          Move 8 To Month-Length
000212        When "12"
000213          Move "December" To Month-Field
000214          Move 8 To Month-Length
000215        When Other
000216          Set Month-Error To True
000217          Move "Invalid Month" To Error-Message
000218     End-Evaluate
000219     If Month-Error
000220        Continue
000221     Else
000222* Initialize The Output Since We Are Going To Use String.
000223        Move Spaces To Edited-Date-Field
000224        String Month-Field (1:month-Length)
000225               Space
000226               Date-Field (3:2)
000227               ","
000228               Date-Field (5:4)
000229               Delimited By Size
000230               Into Edited-Date-Field
000231        End-String
000232     End-If
000233     .