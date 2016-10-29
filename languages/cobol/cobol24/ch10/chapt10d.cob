000010 @OPTIONS MAIN
000020 Identification Division.
000030 Program-Id.  Chapt10d.
000031* Go To Vs Perform Logic
000040 Environment Division.
000050 Configuration Section.
000051 Special-Names.
000052       Crt Status Is Keyboard-Status.
000054 Source-Computer.  IBM-PC.
000055 Object-Computer.  IBM-PC.
000056 Data Division.
000061 Working-Storage Section.
000062 01  Keyboard-Status.
000063     03  Accept-Status Pic 9.
000064     03  Function-Key  Pic X.
000065         88 F1-Pressed Value X"01".
000066     03  System-Use    Pic X.
000067 01  Screen-Items.
000068     03  Last-Name              Pic X(20) Value Spaces.
000069     03  First-Name             Pic X(20) Value Spaces.
000070     03  E-Mail                 Pic X(30) Value Spaces.
000085 Screen Section.
000255 01  Entry-Screen
000256     Blank Screen, Auto
000257     Foreground-Color Is 7,
000258     Background-Color Is 1.
000259*
000260     03  Screen-Literal-Group.
000261         05  Line 01 Column 30 Value "Name and E-mail Entry"
000262             Highlight Foreground-Color 4 Background-Color 1.
000273         05  Line 06 Column 05 Value "E-mail: ".
000283         05  Line 08 Column 05 Value "  Last: ".
000284         05  Line 09 Column 05 Value " First: ".
000285         05  Line 22 Column 05 Value "Press F1 to Exit".
000287     03  Reverse-Video-Group Reverse-Video.
000289         05  Line 06 Column 13 Pic X(30) Using E-Mail.
000290         05  Line 08 Column 13 Pic X(20) Using Last-Name.
000291         05  Line 09 Column 13 Pic X(20) Using First-Name.
000326 Procedure Division.
000327 Chapt10d-Start.
000328* First The Perform Code.
000330     Perform Display-And-Accept-Screen Until F1-Pressed
000331* Now The Go To Code
000332     Initialize Screen-Items
000333     Go To Process-Screen
000334     .
000335 Display-And-Accept-Screen.
000336     Display Entry-Screen
000337     Accept  Entry-Screen
000347     If F1-Pressed
000357        Continue
000358     Else
000359        Perform Process-Data-Fields
000360     End-If
000361     .
000362 Process-Data-Fields.
000363     If Last-Name > Spaces
000364        Perform Process-Last-Name
000365     Else
000366        Move "********************" To Last-Name
000367     End-If
000368     If First-Name > Spaces
000369        Perform Process-First-Name
000370     Else
000371        Move "********************" To First-Name
000372     End-If
000373     If E-Mail > Spaces
000374        Perform Process-E-Mail
000375     Else
000376        Move "******************************" To E-Mail
000377     End-If
000386     .
000396 Process-Last-Name.
000406     Inspect Last-Name Converting "abcdefghijklmnopqrstuvwxyz"
000416                       To         "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
000426     .
000436 Process-First-Name.
000446     Inspect First-Name Converting "abcdefghijklmnopqrstuvwxyz"
000456                        To         "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
000466     .
000476 Process-E-Mail.
000486     Inspect E-Mail Converting "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
000496                    To         "abcdefghijklmnopqrstuvwzyz"
000506     .
000507* Go To Method
000516 Process-Screen.
000526     Display Entry-Screen
000536     Accept Entry-Screen
000546     If F1-Pressed
000556        Stop Run
000566     End-If
000576     If Last-Name > Spaces
000586        Go To Process-Last-Name-Goto
000596     Else
000606        Move "********************" To Last-Name
000616     End-If
000617     .
000626 Check-First-Name.
000636     If First-Name > Spaces
000646        Go To Process-First-Name-Goto
000656     Else
000666        Move "********************" To First-Name
000676     End-If
000686     .
000696 Check-E-Mail.
000706     If E-Mail > Spaces
000716        Go To Process-E-Mail-Goto
000726     Else
000736        Move "******************************" To E-Mail
000746     End-If
000756     Go To Process-Screen
000766     .
000776 Process-Last-Name-Goto.
000777     Inspect Last-Name Converting "abcdefghijklmnopqrstuvwxyz"
000778                       To         "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
000780     Go To Check-First-Name
000781     .
000786 Process-First-Name-Goto.
000787     Inspect First-Name Converting "abcdefghijklmnopqrstuvwxyz"
000788                        To         "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
000790     Go To Check-E-Mail
000791     .
000796 Process-E-Mail-Goto.
000797     Inspect E-Mail Converting "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
000798                    To         "abcdefghijklmnopqrstuvwzyz"
000806     Go To Process-Screen
000816     .