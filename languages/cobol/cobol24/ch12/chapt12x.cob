000010 @OPTIONS MAIN,TEST
000020 Identification Division.
000030 Program-Id.  Chapt12x.
000040* This Is But One Solution, Yours May Differ.
000044 Environment Division.
000050 Configuration Section.
000051 Source-Computer.  IBM-PC.
000055 Object-Computer.  IBM-PC.
000056 Data Division.
000057 Working-Storage Section.
000058* Setup A Table With The Different Sack Colors.
000059 01  Sack-Table-Area.
000060     03  Sack-Colors.
000061         05  Filler Pic X(10) Value "Blue".
000062         05  Filler Pic X(10) Value "Red".
000063         05  Filler Pic X(10) Value "Yellow".
000064         05  Filler Pic X(10) Value "Green".
000065         05  Filler Pic X(10) Value "Purple".
000066         05  Filler Pic X(10) Value "Black".
000067         05  Filler Pic X(10) Value "White".
000068     03  The-Sacks Redefines Sack-Colors.
000069         05  Sack-Color-To-Use Pic X(10) Occurs 7 Times
000070             Indexed By Sack-Color-Index.
000071 01  The-Man-On-The-Road.
000072     03  Wife               Occurs 7 Times
000073         Indexed By Wife-Index.
000074         05  Wife-Name                   Pic X(20).
000075         05  Sack           Occurs 7 Times
000076             Indexed By Sack-Index.
000077             10  Sack-Color              Pic X(10).
000078             10  Cat        Occurs 7 Times
000079                 Indexed By Cat-Index.
000080                 15  Cat-Name            Pic X(20).
000081                 15  Kitten Occurs 7 Times
000082                     Indexed By Kitten-Index.
000083                     20  Kitten-Name     Pic X(20).
000084 01  Found-Flag  Pic X Value Spaces.
000085     88  Kitten-Found Value "Y".
000086 Procedure Division.
000207 Chapt12x-Start.
000208* Color The Different Wives Sacks.
000209*
000210* The Set Is Used To Properly Set The Correct Index For
000211* The Actual Sack.  In Our Example, The Sack Color Number Is Going
000212* Into That Sack Item, So They Are The Same.
000213*
000215     Perform Varying Wife-Index From 1 By 1 Until Wife-Index > 7
000216        Perform Varying Sack-Color-Index From 1 By 1
000217                        Until Sack-Color-Index > 7
000218                 Set Sack-Index To Sack-Color-Index
000220                 Move Sack-Color-To-Use (Sack-Color-Index) To
000221                     Sack-Color (Wife-Index, Sack-Index)
000222        End-Perform
000223     End-Perform
000224* Setup The Rest Of The Search So We Can Find Hershey
000225     Move "Hershey" To Kitten-Name (1, 3, 2, 6)
000226     Move "Darlene" To Wife-Name (1)
000227     Perform With Test After
000228                     Varying Wife-Index From 1 By 1 Until
000229                     Wife-Index = 7 Or
000230                     Kitten-Found
000231        Perform With Test After
000232                        Varying Sack-Index From 1 By 1 Until
000233                        Sack-Index = 7 Or
000234                        Kitten-Found
000235           Perform With Test After
000236                           Varying Cat-Index From 1 By 1 Until
000237                           Cat-Index = 7 Or
000238                           Kitten-Found
000239              Set Kitten-Index To 1
000240              Search Kitten
000241                When
000242                 Kitten-Name (Wife-Index, Sack-Index,
000243                              Cat-Index, Kitten-Index) =
000244                 "Hershey"
000245                 Set Kitten-Found To True
000246              End-Search
000247           End-Perform
000248        End-Perform
000249     End-Perform
000250     If Kitten-Found
000251        Display "Hershey found in the "
000252                Sack-Color (Wife-Index, Sack-Index)
000253                " Sack, Being carried by "
000254                Wife-Name (Wife-Index)
000255     Else
000256       Display "Hershey Escaped"
000257     End-If
000258     Stop Run
000259     .