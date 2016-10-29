000010 @OPTIONS MAIN,TEST
000020 Identification Division.
000030 Program-Id.  Chapt17f.
000031* Sort Example With Output Procedure
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
000066            Access Is Sequential
000067            File Status Is Dealer-Status.
000068     Select Sort-Work Assign To Dealer-Sort-Work.
000069 Data Division.
000070 File Section.
000071 Fd  Dealer-File.
000077 01  Dealer-Record.
000078     03  Dealer-Number         Pic X(8).
000079     03  Dealer-Name.
000080         05  Last-Name   Pic X(25).
000081         05  First-Name  Pic X(15).
000082         05  Middle-Name Pic X(10).
000083     03  Address-Line-1      Pic X(50).
000084     03  Address-Line-2      Pic X(50).
000085     03  City                Pic X(40).
000086     03  State-Or-Country    Pic X(20).
000087     03  Postal-Code         Pic X(15).
000088     03  Home-Phone          Pic X(20).
000089     03  Work-Phone          Pic X(20).
000090     03  Other-Phone         Pic X(20).
000091     03  Start-Date          Pic 9(8).
000092     03  Last-Rent-Paid-Date Pic 9(8).
000093     03  Next-Rent-Due-Date  Pic 9(8).
000094     03  Rent-Amount         Pic 9(4)v99.
000095     03  Consignment-Percent Pic 9(3).
000096     03  Last-Sold-Amount    Pic S9(7)v99.
000097     03  Last-Sold-Date      Pic 9(8).
000098     03  Sold-To-Date        Pic S9(7)v99.
000099     03  Commission-To-Date  Pic S9(7)v99.
000100     03  Filler              Pic X(15).
000102 Fd  Dealer-Text.
000103 01  Text-Record         Pic X(376).
000121 Sd  Sort-Work.
000122 01  Sort-Record.
000123     03  Dealer-Number         Pic X(8).
000124     03  Dealer-Name.
000125         05  Last-Name   Pic X(25).
000126         05  First-Name  Pic X(15).
000127         05  Middle-Name Pic X(10).
000128     03  Filler          Pic X(318).
000147 Working-Storage Section.
000157 01  Record-Count        Pic 9(5) Value Zeros.
000167 01  Dealer-Status       Pic XX   Value "00".
000177 01  Done-Flag           Pic X    Value Spaces.
000187     88  All-Done                 Value "Y".
000390 Procedure Division.
000391 Declaratives.
000392 Dealer-File-Error Section.
000393     Use After Standard Error Procedure On Dealer-File.
000394 Dealer-Error.
000395     Display "Unhandled error on Dealer File " Dealer-Status
000396     Set All-Done To True
000397     .
000398 End Declaratives.
000399 Chapt17f-Start.
000401     Sort Sort-Work Ascending Key Dealer-Number Of Sort-Record
000431          Using Dealer-Text
000441          Output Procedure Sort-Out
000461     Display "Sort Complete with " Record-Count " Records."
000471     Stop Run
000481     .
000491 Sort-Out.
000501     Open Output Dealer-File
000511     Perform Until All-Done
000521        Return Sort-Work Into Dealer-Record
000531           At End Set All-Done To True
000541           Not At End
000551             Add 1 To Record-Count
000561             Move Zeros To Last-Sold-Amount
000571                           Last-Sold-Date
000581                           Sold-To-Date
000591                           Commission-To-Date
000592             Write Dealer-Record
000601        End-Return
000611     End-Perform
000621     Close Dealer-File
000631     .