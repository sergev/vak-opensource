000470 Identification Division.
000480 Program-Id.  Chapt03b.
000490 Environment Division.
000500 Configuration Section.
000510 Source-Computer.  IBM-PC.
000520 Object-Computer.  IBM-PC.
000530 Data Division.
000540 Working-Storage Section.
000550 01  Group-Level-Item.
000560     05  Elementary-Numeric       Pic 9(7)        Value 12345.
000570     05  Elementary-Numeric-Dec   Pic 9(5)v99     Value 123.45.
000580     05  Elementary-Numeric-Sign  Pic S9(5)v99    Value -123.45.
000590 01  Edited-Group-Item.
000600     05  Elementary-Zero-Sup      Pic Z(6)9.
000610     05  Elementary-Aster-Sup     Pic ******9.
000620     05  Elementary-Edited        Pic Z,Z(3),Z(3).
000630     05  Elementary-Edited-Dec    Pic Z,Z(3),Z(3).99.
000640 01  Group-Alphanumeric-Item.
000650     05  Elementary-Alphanum      Pic X(20)
000660         Value "ABCDEFGHIJKLMNOPQRST".
000670     05  Elementary-Alphanum-A    Pic X(6)
000680         Value "UVWXYZ".
000690 01  Group-Alphanumeric-Edited.
000700     05  Edited-Alphanumeric      Pic X(3)/X(3)/X(3).
000710 Procedure Division.
000720 Chapt03b-Start.
000730     Move Elementary-Numeric     To Elementary-Zero-Sup.
000740     Move Elementary-Numeric     To Elementary-Edited.
000750     Move Elementary-Numeric     To Elementary-Aster-Sup.
000760     Move Elementary-Numeric-Dec To Elementary-Edited-Dec.
000770     Move Elementary-Alphanum    To Edited-Alphanumeric.
000780     Display "1 Group Alphanumeric="      Group-Alphanumeric-Item.
000790     Display "2 Elementary Alpha="        Elementary-Alphanum.
000800     Display "3 Elementary Alpha A="      Elementary-Alphanum-A.
000810     Display "4 Edited Alphanumeric="     Edited-Alphanumeric.
000820     Display "5 Group Level Item="        Group-Level-Item.
000830     Display "6 Elementary Numeric="      Elementary-Numeric.
000840     Display "7 Elementary Numeric Dec="  Elementary-Numeric-Dec.
000850     Display "8 Elementary Numeric Sign=" Elementary-Numeric-Sign.
000860     Display "9 Elementary Zero Sup="     Elementary-Zero-Sup.
000870     Display "10 Elementary Aster Sup="   Elementary-Aster-Sup.
000880     Display "11 Elementary Edited="      Elementary-Edited.
000890     Display "12 Elementary Edited Dec="  Elementary-Edited-Dec.
000910     Stop Run.
