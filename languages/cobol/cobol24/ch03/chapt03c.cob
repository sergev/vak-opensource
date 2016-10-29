000020 Identification Division.
000030 Program-Id.  Chapt03c.
000040 Environment Division.
000050 Configuration Section.
000060 Source-Computer.  IBM-PC.
000070 Object-Computer.  IBM-PC.
000080 Data Division.
000090 Working-Storage Section.
000100 01  Group-Level-Item.
000110     05  Elementary-Numeric       Pic 9(7)        Value 12345.
000120     05  Elementary-Numeric-Dec   Pic 9(5)v99     Value 123.45.
000130     05  Elementary-Numeric-Sign  Pic S9(5)v99    Value -123.45.
000140 01  Edited-Group-Item.
000150     05  Elementary-Zero-Sup      Pic Z(6)9.
000160     05  Elementary-Aster-Sup     Pic ******9.
000170     05  Elementary-Formatted     Pic Z,Z(3),Z(3).
000180     05  Elementary-Formatted-Dec Pic Z,Z(3),Z(3).99.
000190 01  Group-Alphanumeric-Item.
000200     05  Elementary-Alphanum      Pic X(20)
000210         Value "ABCDEFGHIJKLMNOPQRST".
000220     05  Elementary-Alphanum-A    Pic X(6)
000230         Value "UVWXYZ".
000240 01  Group-Alphanumeric-Edited.
000250     05  Elementary-Slash         Pic X(3)/X(3)/X(3).
000260 Procedure Division.
000270 Chapt03c-Start.
000280     Move Elementary-Numeric To Elementary-Zero-Sup.
000290     Move Elementary-Numeric To Elementary-Formatted.
000300     Move Elementary-Numeric To Elementary-Aster-Sup.
000310     Move Elementary-Numeric-Dec To Elementary-Formatted-Dec.
000320     Move Elementary-Alphanum    To Elementary-Slash.
000330     Display "Group Alphanumeric " Group-Alphanumeric-Item.
000340     Display "Elementary Alpha " Elementary-Alphanum.
000350     Display "Elementary Alpha A " Elementary-Alphanum-A.
000360     Display "Elementary Slash " Elementary-Slash.
000370     Display "Group Level Item " Group-Level-Item.
000380     Display "Elementary Numeric " Elementary-Numeric.
000390     Display "Elementary Numeric Dec " Elementary-Numeric-Dec.
000400     Display "Elementary Numeric Sign " Elementary-Numeric-Sign.
000410     Display "Elementary Zero Sup " Elementary-Zero-Sup.
000420     Display "Elementary Aster Sup " Elementary-Aster-Sup.
000430     Display "Elementary Formatted " Elementary-Formatted.
000440     Display "Elementary Formatted Dec " Elementary-Formatted-Dec.
000450     Display "Elementary Slash " Elementary-Slash.
000460     Stop Run.
