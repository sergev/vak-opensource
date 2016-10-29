000010 @OPTIONS MAIN,TEST
000020 Identification Division.
000030 Program-Id.  Chapt17e.
000031* Sort Example With An Input Procedure.
000041 Environment Division.
000050 Configuration Section.
000051 Source-Computer.  IBM-PC.
000056 Object-Computer.  IBM-PC.
000057 Input-Output Section.
000058 File-Control.
000059     Select Dealer-File Assign To "Dealer.Dat"
000060            Organization Indexed
000061            Record Key Dealer-Number Of Dealer-Record
000062            Alternate Record Key Dealer-Name Of Dealer-Record
000064            Access Sequential
000065            File Status Dealer-Status.
000066     Select Address-File Assign To "Address.Txt"
000067            Organization Line Sequential
000068            Access Sequential.
000069     Select Sort-Work Assign To Dealer-Sort-Work.
000070 Data Division.
000071 File Section.
000072 Fd  Dealer-File.
000073 01  Dealer-Record.
000074     03  Dealer-Number         Pic X(8).
000075     03  Dealer-Name.
000076         05  Last-Name   Pic X(25).
000077         05  First-Name  Pic X(15).
000078         05  Middle-Name Pic X(10).
000083     03  Address-Line-1      Pic X(50).
000085     03  Address-Line-2      Pic X(50).
000086     03  City                Pic X(40).
000087     03  State-Or-Country    Pic X(20).
000088     03  Postal-Code         Pic X(15).
000089     03  Home-Phone          Pic X(20).
000090     03  Work-Phone          Pic X(20).
000091     03  Other-Phone         Pic X(20).
000092     03  Start-Date          Pic 9(8).
000093     03  Last-Rent-Paid-Date Pic 9(8).
000094     03  Next-Rent-Due-Date  Pic 9(8).
000095     03  Rent-Amount         Pic 9(4)v99.
000096     03  Consignment-Percent Pic 9(3).
000097     03  Last-Sold-Amount    Pic S9(7)v99.
000098     03  Last-Sold-Date      Pic 9(8).
000099     03  Sold-To-Date        Pic S9(7)v99.
000100     03  Commission-To-Date  Pic S9(7)v99.
000101     03  Filler              Pic X(15).
000102 Sd  Sort-Work.
000103 01  Sort-Record.
000106     03  Dealer-Name.
000107         05  Last-Name   Pic X(25).
000108         05  First-Name  Pic X(15).
000109         05  Middle-Name Pic X(10).
000110     03  Address-Line-1      Pic X(50).
000111     03  Address-Line-2      Pic X(50).
000112     03  City                Pic X(40).
000113     03  State-Or-Country    Pic X(20).
000114     03  Postal-Code         Pic X(15).
000124 Fd  Address-File.
000125 01  Address-Record          Pic X(225).
000128 Working-Storage Section.
000138 01  Done-Flag   Pic X Value Spaces.
000148     88  All-Done      Value "Y".
000158 01  Dealer-Status Pic XX Value "00".
000390 Procedure Division.
000391 Declaratives.
000392 Dealer-File-Error Section.
000393     Use After Standard Error Procedure On Dealer-File.
000394 Dealer-Error.
000395     Display "Unhandled error on Dealer File " Dealer-Status
000396     Set All-Done To True
000397     .
000398 End Declaratives.
000399 Chapt17e-Start.
000401     Sort Sort-Work Ascending  Key Last-Name Of Sort-Record
000411                                   First-Name Of Sort-Record
000421                                   Middle-Name Of Sort-Record
000431          Input Procedure Sort-In
000441          Giving Address-File
000461     Display "Sort Complete"
000471     Stop Run
000481     .
000491 Sort-In.
000492     Open Input Dealer-File
000501     Perform Until All-Done
000502        Read Dealer-File
000503             At End Set All-Done To True
000504             Not At End
000505             If State-Or-Country Of Dealer-Record = "CA"
000506                Move Corresponding Dealer-Record To Sort-Record
000507                Release Sort-Record
000508             End-If
000510        End-Read
000511     End-Perform
000512     Close Dealer-File
000521     .