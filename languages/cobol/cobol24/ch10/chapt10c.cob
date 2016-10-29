000010 @OPTIONS MAIN
000020 Identification Division.
000030 Program-Id.  Chapt10c.
000031* Name And E-Mail Edit - Processing Loop.
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
000068     03  Name-Entry             Pic X(40) Value Spaces.
000069     03  E-Mail                 Pic X(30) Value Spaces.
000070     03  Output-Fields.
000071         05  Last-Name          Pic X(30) Value Spaces.
000072         05  First-Name         Pic X(30) Value Spaces.
000073         05  Error-Message      Pic X(60) Value Spaces.
000074 01  Work-Numbers.
000075     03  Work-Number            Pic 99 Value Zeros.
000076     03  Work-Number-1          Pic 99 Value Zeros.
000077     03  Work-Number-2          Pic 99 Value Zeros.
000085 Screen Section.
000255 01  Name-Entry-Screen
000256     Blank Screen, Auto
000257     Foreground-Color Is 7,
000258     Background-Color Is 1.
000259*
000260     03  Screen-Literal-Group.
000261         05  Line 01 Column 30 Value "Name and E-mail Entry"
000262             Highlight Foreground-Color 4 Background-Color 1.
000263         05  Line 05 Column 05 Value "  Name: ".
000273         05  Line 06 Column 05 Value "E-mail: ".
000283         05  Line 08 Column 05 Value "  Last: ".
000284         05  Line 09 Column 05 Value " First: ".
000285         05  Line 22 Column 05 Value "Press F1 to Exit".
000287     03  Reverse-Video-Group Reverse-Video.
000288         05  Line 05 Column 13 Pic X(40) Using Name-Entry.
000289         05  Line 06 Column 13 Pic X(30) Using E-Mail.
000290         05  Line 08 Column 13 Pic X(30) From  Last-Name.
000291         05  Line 09 Column 13 Pic X(30) From  First-Name.
000301         05  Line 20 Column 01 Pic X(60)
000311                  Highlight From Error-Message.
000326 Procedure Division.
000327 Chapt10c-Start.
000328     Perform Display-And-Accept-Screen Until F1-Pressed
000329     Stop Run
000330     .
000331 Display-And-Accept-Screen.
000332     Display Name-Entry-Screen
000333     Accept  Name-Entry-Screen
000334* Reset The Working Fields
000335     Initialize Output-Fields
000336                Work-Numbers
000337* Make Sure There Is A Comma In The Name
000342     Inspect Name-Entry Tallying Work-Number-2 For All ","
000343* Only Try To Split If There Is One
000344     If Work-Number-2 > Zeros
000345        Perform Process-The-Data
000346     Else
000347        Move "Name must contain a comma" To Error-Message
000348     End-If
000349     .
000350 Process-The-Data.
000351* Split The First And Last Name Out Into Separate Fields
000352     Inspect Name-Entry Tallying Work-Number
000353             For Characters Before ","
000354     Move Name-Entry (1:work-Number) To Last-Name
000355     Add 2 To Work-Number
000356* We Need To Exclude The Leading Spaces, After The Comma
000357     Inspect Name-Entry (Work-Number:)
000358             Tallying Work-Number-1 For Leading Spaces
000359     Move Name-Entry (Work-Number + Work-Number-1:) To First-Name
000360* Change The E-Mail Address To All Lower Case Letters.
000361     Inspect E-Mail Converting "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
000362                    To         "abcdefghijklmnopqrstuvwxyz"
000363     .