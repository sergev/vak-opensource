000010 @OPTIONS MAIN
000020 Identification Division.
000030 Program-Id.  Chapt06c.
000031* Name And E-Mail Edit
000040 Environment Division.
000050 Configuration Section.
000051 Source-Computer.  IBM-PC.
000055 Object-Computer.  IBM-PC.
000056 Data Division.
000057 Working-Storage Section.
000058 01  Screen-Items.
000068     03  Name-Entry             Pic X(40) Value Spaces.
000069     03  E-Mail                 Pic X(30) Value Spaces.
000070     03  Last-Name              Pic X(30) Value Spaces.
000071     03  First-Name             Pic X(30) Value Spaces.
000075 01  Work-Number                Pic 99 Value Zeros.
000076 01  Work-Number-1              Pic 99 Value Zeros.
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
000287     03  Reverse-Video-Group Reverse-Video.
000288         05  Line 05 Column 13 Pic X(40) Using Name-Entry.
000289         05  Line 06 Column 13 Pic X(30) Using E-Mail.
000290         05  Line 08 Column 13 Pic X(30) From  Last-Name.
000291         05  Line 09 Column 13 Pic X(30) From  First-Name.
000326 Procedure Division.
000327 Chapt06c-Start.
000328     Display Name-Entry-Screen
000329     Accept  Name-Entry-Screen
000330* Split The First And Last Name Out Into Separate Fields
000331     Inspect Name-Entry Tallying Work-Number
000332             For Characters Before ","
000334     Move Name-Entry (1:work-Number) To Last-Name
000335     Add 2 To Work-Number
000336* You Need To Exclude The Leading Spaces, After The Comma
000337     Inspect Name-Entry (Work-Number:)
000338             Tallying Work-Number-1 For Leading Spaces
000342     Move Name-Entry (Work-Number + Work-Number-1:) To First-Name
000343* Change The E-Mail Address To All Lower Case Letters.
000344     Inspect E-Mail Converting "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
000345                    To         "abcdefghijklmnopqrstuvwxyz"
000346* Show The Results
000347     Display Name-Entry-Screen
000348     Stop Run
000358     .