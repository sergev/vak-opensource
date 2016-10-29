000010 @OPTIONS MAIN,TEST
000020 Identification Division.
000030 Program-Id.  Chapt16x.
000031* Chapter 16 Exercise Solution
000041 Environment Division.
000050 Configuration Section.
000051 Source-Computer.  IBM-PC.
000056 Object-Computer.  IBM-PC.
000057 Input-Output Section.
000058 File-Control.
000059     Select Dealer-File Assign To "Dealer.Dat"
000060            Organization Indexed
000061            Access Sequential
000062            Record Key Dealer-Number
000063            Alternate Record Key Dealer-Name
000064            File Status Dealer-Status.
000065 Data Division.
000066 File Section.
000067 Fd  Dealer-File.
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
000102 Working-Storage Section.
000118 01  Dealer-Status     Pic X(2) Value Spaces.
000240     88  Dealer-Success Value "00" Thru "09".
000250 01  Error-Message     Pic X(60) Value Spaces.
000260 01  Open-Error-Message.
000270     03  Filler      Pic X(26) Value "Error Opening Dealer File ".
000280     03  Open-Status Pic X(2)  Value Spaces.
000290 01  Rewrite-Error-Message.
000300     03  Filler         Pic X(28) Value "Error Rewriting Dealer File ".
000310     03  Rewrite-Status Pic X(2) Value Spaces.
000390 Procedure Division.
000391 Chapt16x-Start.
000401* It's Nice To Show That Something Is Happening
000402     Display "Begin Process Chapt16x"
000408     Perform Open-File
000418     If Not Dealer-Success
000438        Move Dealer-Status To Open-Status
000453        Display Open-Error-Message
000454     Else
000455        Perform Process-File Until Not Dealer-Success
000457        Perform Close-File
000458     End-If
000459     Display "Update Complete"
000460     Stop Run
000461     .
000462 Process-File.
000471     Read Dealer-File At End
000472          Continue
000473          Not At End
000474          Perform Update-Record
000475     End-Read
000476     .
000477 Update-Record.
000481     Move Zeros To Last-Sold-Amount
000491                   Last-Sold-Date
000501                   Sold-To-Date
000511                   Commission-To-Date
000521     Rewrite Dealer-Record
000522             Invalid Key
000541             Move Dealer-Status To Rewrite-Status
000573             Display Rewrite-Error-Message
000574     End-Rewrite
000575     .
000576 Open-File.
000577     Open I-O Dealer-File
000578     .
000579 Close-File.
000580     Close Dealer-File
000581     .