000001 @OPTIONS MAIN,TEST,CHECK(1)
000002 Identification Division.
000003 Program-Id.  Chapt12z.
000004 Environment Division.
000005 Configuration Section.
000006* Show Boundary Violation
000007 Special-Names.
000008       Crt Status Is Keyboard-Status.
000009 Source-Computer.  IBM-PC.
000010 Object-Computer.  IBM-PC.
000011 Data Division.
000012 Working-Storage Section.
000013 01  Keyboard-Status.
000014     03  Accept-Status     Pic 9.
000015     03  Function-Key      Pic X.
000016         88 F1-Pressed     Value X"01".
000017     03  System-Use        Pic X.
000018 01  Date-Field            Pic 9(8)  Value Zeros.
000019 01  Date-Field-Split      Redefines Date-Field.
000020     03  Month-Portion     Pic 99.
000021     03  Filler            Pic X(6).
000022 01  Edited-Date-Field     Pic X(20) Value Spaces.
000023 01  Error-Flag            Pic X     Value Spaces.
000024     88  Month-Error       Value "Y".
000025 01  Error-Message         Pic X(50) Value Spaces.
000026 01  Month-Table-Area.
000027     03  Month-Descriptions.
000028         05  Filler            Pic X(9) Value "January".
000029         05  Filler            Pic X(9) Value "February".
000030         05  Filler            Pic X(9) Value "March".
000031         05  Filler            Pic X(9) Value "April".
000032         05  Filler            Pic X(9) Value "May".
000033         05  Filler            Pic X(9) Value "June".
000034         05  Filler            Pic X(9) Value "July".
000035         05  Filler            Pic X(9) Value "August".
000036         05  Filler            Pic X(9) Value "September".
000037         05  Filler            Pic X(9) Value "October".
000038         05  Filler            Pic X(9) Value "November".
000039         05  Filler            Pic X(9) Value "December".
000040     03  Month-Table Redefines Month-Descriptions.
000041         05  Month-Name        Pic X(9) Occurs 12 Times.
000042 Screen Section.
000043 01  Date-Entry Blank Screen.
000044     03  Line 01 Column 01 Value " Enter Date: ".
000045     03  Line 01 Column 14 Pic 99/99/9999 Using Date-Field.
000046     03  Line 02 Column 01 Value "Edited Date: ".
000047     03  Line 02 Column 14 Pic X(20) From Edited-Date-Field.
000048     03  Line 05 Column 01 Pic X(50) From Error-Message.
000049     03  Line 20 Column 01 Value "Press F1 to Exit".
000050 Procedure Division.
000051 Chapt12z-Start.
000052     Perform Until F1-Pressed
000053        Display Date-Entry
000054        Accept Date-Entry
000055* Clear The Error Message For The Next Display
000056        Move Spaces To Error-Message
000057* If They Did Not Press F1 To Exit, It's Ok To Process The Input
000058        If Not F1-Pressed
000059           Perform Process-Input
000060        End-If
000061     End-Perform
000062     Stop Run
000063     .
000064 Process-Input.
000065* Reset The Error Flag.
000066     Move Spaces To Error-Flag
000067*     If Month-Portion < 01 Or Month-Portion > 12
000068*        Set Month-Error To True
000069*        Move "Invalid Month" To Error-Message
000070*     Else
000071        Move Spaces To Edited-Date-Field
000072        String Month-Name (Month-Portion) Delimited By Space
000073               Space
000074               Date-Field (3:2)
000075               ","
000076               Date-Field (5:4)
000077               Delimited By Size
000078               Into Edited-Date-Field
000079        End-String
000080*     End-If
000081     .