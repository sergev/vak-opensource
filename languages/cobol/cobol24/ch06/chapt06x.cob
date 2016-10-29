000010 @OPTIONS MAIN
000020 Identification Division.
000030 Program-Id.  Chapt06x.
000031* Chapter 6 Exercise Solution
000040 Environment Division.
000050 Configuration Section.
000051 Source-Computer.  IBM-PC.
000055 Object-Computer.  IBM-PC.
000056 Data Division.
000057 Working-Storage Section.
000058 01  Screen-Items.
000068     03  Name-Entry             Pic X(60) Value Spaces.
000070     03  Last-Name              Pic X(30) Value Spaces.
000071     03  First-Name             Pic X(30) Value Spaces.
000075 01  Work-Number                Pic 99 Value Zeros.
000076 01  Work-Number-1              Pic 99 Value Zeros.
000085 Screen Section.
000255 01  Name-Entry-Screen
000256     Blank Screen, Auto
000257     Foreground-Color Is 7,
000258     Background-Color Is 1.
000260     03  Screen-Literal-Group.
000263         05  Line 05 Column 05 Value "  Name: ".
000283         05  Line 08 Column 05 Value "  Last: ".
000284         05  Line 09 Column 05 Value " First: ".
000287     03  Reverse-Video-Group Reverse-Video.
000288         05  Line 05 Column 13 Pic X(60) Using Name-Entry.
000290         05  Line 08 Column 13 Pic X(30) From  Last-Name.
000291         05  Line 09 Column 13 Pic X(30) From  First-Name.
000326 Procedure Division.
000327 Chapt06x-Start.
000328     Display Name-Entry-Screen
000329     Accept  Name-Entry-Screen
000330* Change To Uppercase Before Splitting The Names.
000331     Inspect Name-Entry Converting "abcdefghijklmnopqrstuvwxyz"
000332                        To         "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
000333* Split The First And Last Name Out Into Separate Fields
000334     Inspect Name-Entry Tallying Work-Number
000335             For Characters Before " "
000336     Move Name-Entry (1:work-Number) To First-Name
000337     Add 1 To Work-Number
000338* You Need To Exclude The Extra Spaces If Any
000339     Inspect Name-Entry (Work-Number:)
000340             Tallying Work-Number-1 For Leading Spaces
000342     Move Name-Entry (Work-Number + Work-Number-1:) To Last-Name
000346* Show The Results
000347     Display Name-Entry-Screen
000348     Stop Run
000358     .