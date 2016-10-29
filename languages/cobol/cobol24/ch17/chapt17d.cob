000010 @OPTIONS MAIN,TEST
000020 Identification Division.
000030 Program-Id.  Chapt17d.
000031* Create An Indexed File From A Sequential File Using Sort
000041 Environment Division.
000050 Configuration Section.
000051 Source-Computer.  IBM-PC.
000056 Object-Computer.  IBM-PC.
000057 Input-Output Section.
000058 File-Control.
000059     Select Dealer-Text Assign To "Dealer.TXT"
000060            Organization Line Sequential
000061            Access Sequential.
000062     Select Dealer-File Assign To "Dealer.Dat"
000063            Organization Is Indexed
000064            Record Key Dealer-Number Of Dealer-Record
000065            Alternate Key Dealer-Name Of Dealer-Record
000066            Access Is Sequential.
000067     Select Sort-Work Assign To Dealer-Sort-Work.
000068 Data Division.
000069 File Section.
000070 Fd  Dealer-File.
000071 01  Dealer-Record.
000072     03  Dealer-Number         Pic X(8).
000073     03  Dealer-Name.
000074         05  Last-Name   Pic X(25).
000075         05  First-Name  Pic X(15).
000076         05  Middle-Name Pic X(10).
000077     03  Filler          Pic X(318).
000096 Fd  Dealer-Text.
000097 01  Text-Record         Pic X(376).
000121 Sd  Sort-Work.
000122 01  Sort-Record.
000123     03  Dealer-Number         Pic X(8).
000124     03  Dealer-Name.
000125         05  Last-Name   Pic X(25).
000126         05  First-Name  Pic X(15).
000127         05  Middle-Name Pic X(10).
000128     03  Filler          Pic X(318).
000147 Working-Storage Section.
000390 Procedure Division.
000391 Chapt17d-Start.
000401     Sort Sort-Work Ascending Key Dealer-Number Of Sort-Record
000431          Using Dealer-Text
000441          Giving Dealer-File
000461     Display "Sort Complete"
000471     Stop Run
000481     .
