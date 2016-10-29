000020 Identification Division.
000030 Program-Id.  Chapt03a.
000040 Environment Division.
000050 Configuration Section.
000060 Source-Computer.  IBM-PC.
000070 Object-Computer.  IBM-PC.
000080 Data Division.
000090 Working-Storage Section.
000100 01  Hello-Text.
000110     03  Part-One Pic X(6) Value "Hello ".
000120     03  Part-Two Pic X(5) Value "World".
000130 Procedure Division.
000140 Chapt03a-Start.
000150     Display Part-One.
000160     Display Part-Two.
000170     Display Hello-Text.
000180     Stop Run.
