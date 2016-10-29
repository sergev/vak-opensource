000010 @OPTIONS MAIN,TEST
000020 Identification Division.
000030 Program-Id.  Chapt12g.
000044 Environment Division.
000050 Configuration Section.
000051 Source-Computer.  IBM-PC.
000055 Object-Computer.  IBM-PC.
000056 Data Division.
000057 Working-Storage Section.
000067 01  The-Man-On-The-Road.
000072     03  Wife               Occurs 7 Times
000073         Indexed By Wife-Index.
000074         05  Wife-Name                   Pic X(20).
000075         05  Sack           Occurs 7 Times
000076         Indexed By Sack-Index.
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
000207 Chapt12g-Start.
000208     Move "Hershey" To Kitten-Name (1, 3, 2, 6)
000209     Move "Darlene" To Wife-Name (1)
000210     Move "Yellow"  To Sack-Color (1, 3)
000211     Perform With Test After
000212                     Varying Wife-Index From 1 By 1 Until
000213                     Wife-Index = 7 Or
000214                     Kitten-Found
000215        Perform With Test After
000216                        Varying Sack-Index From 1 By 1 Until
000217                        Sack-Index = 7 Or
000218                        Kitten-Found
000219           Perform With Test After
000220                           Varying Cat-Index From 1 By 1 Until
000221                           Cat-Index = 7 Or
000222                           Kitten-Found
000223              Set Kitten-Index To 1
000224              Search Kitten
000225                When
000226                 Kitten-Name (Wife-Index, Sack-Index,
000227                              Cat-Index, Kitten-Index) =
000228                 "Hershey" Set Kitten-Found To True
000229              End-Search
000230           End-Perform
000231        End-Perform
000232     End-Perform
000233     If Kitten-Found
000234        Display "Hershey found in the "
000235                Sack-Color (Wife-Index, Sack-Index)
000236                " Sack, Being carried by "
000237                Wife-Name (Wife-Index)
000238     Else
000239       Display "Hershey Escaped"
000240     End-If
000241     Stop Run
000242     .