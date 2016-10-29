000010 @OPTIONS MAIN,TEST
000020 Identification Division.
000030 Program-Id.  Chapt14a.
000031* Dealer File Creation
000041 Environment Division.
000050 Configuration Section.
000051 Source-Computer.  IBM-PC.
000055 Object-Computer.  IBM-PC.
000056 Input-Output Section.
000057 File-Control.
000058     Select Dealer-File Assign To "Dealer.Dat"
000059            Organization Indexed
000060            Access Sequential
000061            Record Key Dealer-Number Of Dealer-Record
000062            Alternate Record Key Dealer-Name Of Dealer-Record
000063            File Status Dealer-Status.
000064     Select Dealer-Text Assign To "Dealer.TXT"
000065            Organization Is Line Sequential
000066            File Status Dealer-Text-Status.
000070
000071 Data Division.
000072 File Section.
000073 Fd  Dealer-File.
000074 01  Dealer-Record.
000075     03  Dealer-Number         Pic X(8).
000076     03  Dealer-Name.
000077         05  Last-Name   Pic X(25).
000078         05  First-Name  Pic X(15).
000079         05  Middle-Name Pic X(10).
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
000097     03  Filler              Pic X(50).
000098 Fd  Dealer-Text.
000099 01  Text-Record.
000100     03  Dealer-Number         Pic X(8).
000101     03  Dealer-Name.
000102         05  Last-Name   Pic X(25).
000103         05  First-Name  Pic X(15).
000104         05  Middle-Name Pic X(10).
000105     03  Address-Line-1      Pic X(50).
000106     03  Address-Line-2      Pic X(50).
000107     03  City                Pic X(40).
000108     03  State-Or-Country    Pic X(20).
000109     03  Postal-Code         Pic X(15).
000110     03  Home-Phone          Pic X(20).
000111     03  Work-Phone          Pic X(20).
000112     03  Other-Phone         Pic X(20).
000113     03  Start-Date          Pic 9(8).
000114     03  Last-Rent-Paid-Date Pic 9(8).
000115     03  Next-Rent-Due-Date  Pic 9(8).
000116     03  Rent-Amount         Pic 9(4)v99.
000117     03  Consignment-Percent Pic 9(3).
000118     03  Filler              Pic X(50).
000119
000120 Working-Storage Section.
000121 01  Dealer-Status      Pic XX Value Spaces.
000122 01  Dealer-Text-Status Pic XX Value Spaces.
000132 01  Record-Counter     Pic 9(5) Value Zeros.
000372 Procedure Division.
000373 Chapt14a-Start.
000374     Open Input Dealer-Text
000384     Open Output Dealer-File
000394     Perform Until Dealer-Status Not = "00" Or
000395                   Dealer-Text-Status Not = "00"
000404         Read Dealer-Text
000405         If Dealer-Text-Status = "00"
000414            Write Dealer-Record From Text-Record
000415            If Dealer-Status Not = "00"
000416               Display
000417               "Write Error Dealer-Record " Dealer-Status
000418            Else
000419               Add 1 To Record-Counter
000420            End-If
000421         End-If
000424     End-Perform
000434     Close Dealer-Text Dealer-File
000435     Display
000436     "File Processed with " Record-Counter " Records Written"
000437     Stop Run
000444     .