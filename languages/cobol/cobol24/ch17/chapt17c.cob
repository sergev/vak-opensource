000010 @OPTIONS MAIN,TEST
000020 Identification Division.
000030 Program-Id.  Chapt17c.
000031* Simple Sort Example
000041 Environment Division.
000050 Configuration Section.
000051 Source-Computer.  IBM-PC.
000056 Object-Computer.  IBM-PC.
000057 Input-Output Section.
000058 File-Control.
000059     Select Dealer-Text Assign To "Dealer.TXT"
000060            Organization Line Sequential
000061            Access Sequential.
000062     Select Sort-Work Assign To Dealer-Sort-Work.
000065 Data Division.
000066 File Section.
000067 Fd  Dealer-Text.
000068 01  Dealer-Record.
000069     03  Dealer-Number         Pic X(8).
000070     03  Dealer-Name.
000071         05  Last-Name   Pic X(25).
000074         05  First-Name  Pic X(15).
000075         05  Middle-Name Pic X(10).
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
000104     03  Dealer-Number         Pic X(8).
000105     03  Dealer-Name.
000106         05  Last-Name   Pic X(25).
000107         05  First-Name  Pic X(15).
000108         05  Middle-Name Pic X(10).
000109     03  Address-Line-1      Pic X(50).
000110     03  Address-Line-2      Pic X(50).
000111     03  City                Pic X(40).
000112     03  State-Or-Country    Pic X(20).
000113     03  Postal-Code         Pic X(15).
000114     03  Home-Phone          Pic X(20).
000115     03  Work-Phone          Pic X(20).
000116     03  Other-Phone         Pic X(20).
000117     03  Start-Date          Pic 9(8).
000118     03  Last-Rent-Paid-Date Pic 9(8).
000119     03  Next-Rent-Due-Date  Pic 9(8).
000120     03  Rent-Amount         Pic 9(4)v99.
000121     03  Consignment-Percent Pic 9(3).
000122     03  Last-Sold-Amount    Pic S9(7)v99.
000123     03  Last-Sold-Date      Pic 9(8).
000124     03  Sold-To-Date        Pic S9(7)v99.
000125     03  Commission-To-Date  Pic S9(7)v99.
000126     03  Filler              Pic X(15).
000128 Working-Storage Section.
000390 Procedure Division.
000391 Chapt17c-Start.
000401     Sort Sort-Work Descending Key State-Or-Country Of Sort-Record
000402                    Ascending  Key Last-Name Of Sort-Record
000411                                   First-Name Of Sort-Record
000421                                   Middle-Name Of Sort-Record
000431          Using Dealer-Text
000441          Giving Dealer-Text
000461     Display "Sort Complete"
000471     Stop Run
000481     .