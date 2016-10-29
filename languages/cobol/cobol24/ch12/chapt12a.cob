000001 @OPTIONS MAIN,TEST
000002 Identification Division.
000003 Program-Id.  Chapt12a.
000004 Environment Division.
000005 Configuration Section.
000006 Special-Names.
000007       Crt Status Is Keyboard-Status.
000008 Source-Computer.  IBM-PC.
000009 Object-Computer.  IBM-PC.
000010 Data Division.
000011 Working-Storage Section.
000012 01  Keyboard-Status.
000013     03  Accept-Status     Pic 9.
000014     03  Function-Key      Pic X.
000015         88 F1-Pressed     Value X"01".
000016     03  System-Use        Pic X.
000017 01  Date-Field            Pic 9(8)  Value Zeros.
000018 01  Date-Field-Split      Redefines Date-Field.
000019     03  Month-Portion     Pic 99.
000020     03  Filler            Pic X(6).
000021 01  Edited-Date-Field     Pic X(20) Value Spaces.
000022 01  Error-Flag            Pic X     Value Spaces.
000023     88  Month-Error       Value "Y".
000024 01  Error-Message         Pic X(50) Value Spaces.
000025 01  Month-Table-Area.
000026     03  Month-Descriptions.
000027         05  Filler            Pic X(9) Value "January".
000028         05  Filler            Pic X(9) Value "February".
000029         05  Filler            Pic X(9) Value "March".
000030         05  Filler            Pic X(9) Value "April".
000031         05  Filler            Pic X(9) Value "May".
000032         05  Filler            Pic X(9) Value "June".
000033         05  Filler            Pic X(9) Value "July".
000034         05  Filler            Pic X(9) Value "August".
000035         05  Filler            Pic X(9) Value "September".
000036         05  Filler            Pic X(9) Value "October".
000037         05  Filler            Pic X(9) Value "November".
000038         05  Filler            Pic X(9) Value "December".
000039     03  Month-Table Redefines Month-Descriptions.
000040         05  Month-Name        Pic X(9) Occurs 12 Times.
000041 Screen Section.
000042 01  Date-Entry Blank Screen.
000043     03  Line 01 Column 01 Value " Enter Date: ".
000044     03  Line 01 Column 14 Pic 99/99/9999 Using Date-Field.
000045     03  Line 02 Column 01 Value "Edited Date: ".
000046     03  Line 02 Column 14 Pic X(20) From Edited-Date-Field.
000047     03  Line 05 Column 01 Pic X(50) From Error-Message.
000048     03  Line 20 Column 01 Value "Press F1 to Exit".
000049 Procedure Division.
000050 Chapt12a-Start.
000051     Perform Until F1-Pressed
000052        Display Date-Entry
000053        Accept Date-Entry
000054* Clear The Error Message For The Next Display
000055        Move Spaces To Error-Message
000056* If They Did Not Press F1 To Exit, It's Ok To Process The Input
000057        If Not F1-Pressed
000058           Perform Process-Input
000059        End-If
000060     End-Perform
000061     Stop Run
000062     .
000063 Process-Input.
000064* Reset The Error Flag.
000065     Move Spaces To Error-Flag
000066     If Month-Portion < 01 Or Month-Portion > 12
000067        Set Month-Error To True
000068        Move "Invalid Month" To Error-Message
000069     Else
000070        Move Spaces To Edited-Date-Field
000071        String Month-Name (Month-Portion) Delimited By Space
000072               Space
000073               Date-Field (3:2)
000074               ","
000075               Date-Field (5:4)
000076               Delimited By Size
000077               Into Edited-Date-Field
000078        End-String
000079     End-If
000080     .