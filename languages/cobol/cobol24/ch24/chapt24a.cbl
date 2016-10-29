000010 @OPTIONS MAIN,TEST
000011 Identification Division.
000020 Program-Id. Chapt24a.
000030
000040* Title - Tenant Display Program
000050* Description - Tenant Display Program
000060
000070 Environment Division.
000080 Configuration Section.
000090 Source-Computer. IBM-PC.
000100 Object-Computer. IBM-PC.
000110 Input-Output Section.
000111 File-Control.
000112     Select Dealer-File Assign To "\Tycobol\Dealer.Dat"
000113            Organization Indexed
000114            Access Dynamic
000115            Record Key Dealer-Number
000116            Alternate Record Key Dealer-Name
000117            File Status Dealer-Status.
000120 Data Division.
000121 File Section.
000122 Fd  Dealer-File.
000123 01  Dealer-Record.
000124     03  Dealer-Number         Pic X(8).
000125     03  Dealer-Name.
000126         05  Last-Name   Pic X(25).
000127         05  First-Name  Pic X(15).
000128         05  Middle-Name Pic X(10).
000129     03  Address-Line-1      Pic X(50).
000130     03  Address-Line-2      Pic X(50).
000131     03  City                Pic X(40).
000132     03  State-Or-Country    Pic X(20).
000133     03  Postal-Code         Pic X(15).
000134     03  Home-Phone          Pic X(20).
000135     03  Work-Phone          Pic X(20).
000136     03  Other-Phone         Pic X(20).
000137     03  Start-Date          Pic 9(8).
000138     03  Last-Rent-Paid-Date Pic 9(8).
000139     03  Next-Rent-Due-Date  Pic 9(8).
000140     03  Rent-Amount         Pic 9(4)v99.
000141     03  Consignment-Percent Pic 9(3).
000142     03  Filler              Pic X(50).
000143 Working-Storage Section.
000144 01  Dealer-Status     Pic X(2) Value Spaces.
000145     88  Dealer-Success Value "00" Thru "09".
000150 01  Error-Message     Pic X(60) Value Spaces.
000151 Copy "sp2.cpy".
000160
000170 Copy "chapt24a.cpy".
000180 01  Date-Reverse-Area.
000181     03  Date-YYYYMMDD     Pic 9(8).
000182     03  Date-YYYYMMDD-X Redefines Date-YYYYMMDD.
000183         05  Date-YYYY     Pic 9(4).
000184         05  Date-MM       Pic 9(2).
000185         05  Date-DD       Pic 9(2).
000186     03  Date-MMddyyyy     Pic 9(8).
000187     03  Date-MMddyyyy-X Redefines Date-MMddyyyy.
000188         05  Date-MM       Pic 9(2).
000189         05  Date-DD       Pic 9(2).
000190         05  Date-YYYY     Pic 9(4).
000191 Procedure Division.
000200 Mainline.
000210******************
000220* Mainline Logic *
000230******************
000240     Perform Proc-Open-File
000250     Move Low-Values To Chapt24a-Data
000260     Move "chapt24a" To Chapt24a-Next-Panel
000270     Move "y" To Chapt24a-New-Window
000280     Move Low-Values To Chapt24a-Fields
000290     Move Low-Values To Chapt24a-Colrs
000300     Move Low-Values To Chapt24a-Types
000301     Perform Open-File
000302     If Not Dealer-Success
000303* Message Box Display!
000304        Move Low-Values To Sp2-Ms-Data
000305        Move "b" To Sp2-Ms-Icon
000306        Move "File Error"                  To Sp2-Ms-Title
000307        Move "o"                           To Sp2-Ms-Button
000309	      Move 1				 To Sp2-Ms-Line-Cnt
000310	      String "Error Opening Dealer File "
000311               Dealer-Status
000312               Delimited By Size
000313               Into Sp2-Ms-Text
000314        End-String
000316        Call "SP2" Using Sp2-Display-Message Sp2-Message-Data
000318     Else
000319* There Is No Reason To Perform These If The Open Fails
000320        Perform Proc-Con-Chapt24a
000321        Perform Proc-Close-Window
000322        Perform Close-File
000323     End-If
000330     Perform Proc-Close-File
000340     Perform Proc-End-Session
000350     Stop Run
000360     .
000370 Open-File.
000371     Open Input Dealer-File
000372     .
000373 Close-File.
000374     Close Dealer-File
000375     .
000380 Proc-Open-File.
000390*****************
000400* Open Sp2 File *
000410*****************
000420     Move Low-Values To Sp2-Fi-Data
000430     Move "C:\SPFJ3224\chapt24.pan" To Sp2-Fi-Name
000440     Call "SP2" Using Sp2-Open-File Sp2-File-Def
000450     .
000460
000470 Proc-Con-Chapt24a.
000480******************
000490* Converse Panel *
000500******************
000501     Perform With Test After Until
000502             Chapt24a-Key = Sp2-Key-Close Or
000503             Chapt24a-Exit-Hit
000510        Call "SP2" Using Sp2-Converse-Panel Chapt24a-Converse-Data
000520        Move Low-Value To Chapt24a-New-Window
000521        Perform Determine-Action
000522     End-Perform
000530     .
000540 Determine-Action.
000541     Evaluate True
000542        When Chapt24a-Exit-Hit
000543        When Chapt24a-Key = Sp2-Key-Close
000544           Continue
000545        When Chapt24a-Read-Hit
000546          Evaluate Chapt24a-Operation-Type
000547             When "Read Random Number"
000548                  Perform Read-Random-Number
000549             When "Read Random Name"
000550                  Perform Read-Random-Name
000551             When "Read Next Number"
000552                  Perform Read-Next-Number
000553             When "Read Next Name"
000554                  Perform Read-Next-Name
000555          End-Evaluate
000556        When Chapt24a-Clear-Hit
000557           Initialize Chapt24a-Fields
000558           Move "Read Random Number" To Chapt24a-Operation-Type
000559        When Other
000560           Continue
000561     End-Evaluate
000562     .
000563 Read-Random-Number.
000564     Move Chapt24a-Number To Dealer-Number
000565     Read Dealer-File
000566       Invalid Key
000570         String "Error on Random Read Number "
000571                 Dealer-Status
000572                 Delimited By Size
000573                 Into Error-Message
000574          End-String
000575          Perform Show-Error-Message
000576          Not Invalid Key
000577              Perform Fill-Panel-Data
000579     End-Read
000580     .
000581 Read-Random-Name.
000582     Move Chapt24a-Last To Last-Name
000583     Move Chapt24a-First To First-Name
000584     Move Chapt24a-Middle To Middle-Name
000586     Read Dealer-File Key Dealer-Name
000587        Invalid Key
000588          String "Error on Random Read Name "
000594                  Dealer-Status
000595                  Delimited By Size
000596                Into Sp2-Ms-Text
000597          End-String
000598          Perform Show-Error-Message
000599        Not Invalid Key
000600            Perform Fill-Panel-Data
000601     End-Read
000602     .
000603 Read-Next-Number.
000604     Move Chapt24a-Number To Dealer-Number
000605     Start Dealer-File Key > Dealer-Number
000606      Invalid Key
000607         String "Start Error Number "
000608                Dealer-Status
000609                Delimited By Size
000610                Into Error-Message
000611         End-String
000612         Perform Show-Error-Message
000613     End-Start
000614     If Dealer-Success
000615       Read Dealer-File Next
000616          At End
000617             Move "End of File, Read by Number" To Error-Message
000618             Perform Show-Error-Message
000619          Not At End
000620             Perform Fill-Panel-Data
000621       End-Read
000622     End-If
000623     .
000624 Read-Next-Name.
000625     Move Chapt24a-Last To Last-Name
000626     Move Chapt24a-First To First-Name
000627     Move Chapt24a-Middle To Middle-Name
000628     Start Dealer-File Key > Dealer-Name
000629      Invalid Key
000630         String "Start Error Name "
000631                Dealer-Status
000632                Delimited By Size
000633                Into Error-Message
000634         End-String
000635         Perform Show-Error-Message
000636     End-Start
000637     If Dealer-Success
000638       Read Dealer-File Next
000639          At End
000640             Move "End of File, Read by Name" To Error-Message
000641             Perform Show-Error-Message
000642          Not At End
000643             Perform Fill-Panel-Data
000644       End-Read
000645     End-If
000646     .
000647 Fill-Panel-Data.
000648     Move Dealer-Number       To Chapt24a-Number
000649     Move Last-Name           To Chapt24a-Last
000650     Move First-Name          To Chapt24a-First
000651     Move Middle-Name         To Chapt24a-Middle
000652     Move Address-Line-1      To Chapt24a-Address-Line-1
000653     Move Address-Line-2      To Chapt24a-Address-Line-2
000654     Move City                To Chapt24a-City
000655     Move State-Or-Country    To Chapt24a-State-Or-Country
000656     Move Postal-Code         To Chapt24a-Postal-Code
000657     Move Home-Phone          To Chapt24a-Home-Phone
000658     Move Work-Phone          To Chapt24a-Work-Phone
000659     Move Other-Phone         To Chapt24a-Other-Phone
000660     Move Start-Date          To Date-MMddyyyy
000661     Move Corresponding Date-MMddyyyy-X To Date-YYYYMMDD-X
000662     Move Date-YYYYMMDD       To Chapt24a-Start-Date
000663     Move Last-Rent-Paid-Date To Date-MMddyyyy
000664     Move Corresponding Date-MMddyyyy-X To Date-YYYYMMDD-X
000665     Move Date-YYYYMMDD       To Chapt24a-Last-Rent-Paid-Date
000666     Move Next-Rent-Due-Date To Date-MMddyyyy
000667     Move Corresponding Date-MMddyyyy-X To Date-YYYYMMDD-X
000668     Move Date-YYYYMMDD       To Chapt24a-Next-Rent-Due-Date
000670     Move Rent-Amount         To Chapt24a-Rent-Amount
000671     Move Consignment-Percent To Chapt24a-Consignment-Percent
000672     .
000673 Show-Error-Message.
000674     Move Low-Values To Sp2-Ms-Data
000675     Move "b" To Sp2-Ms-Icon
000676     Move "File Error"                  To Sp2-Ms-Title
000677     Move "o"                           To Sp2-Ms-Button
000678	   Move 1				 To Sp2-Ms-Line-Cnt
000679     Move Error-Message To Sp2-Ms-Text
000680     Move Spaces To Error-Message
000681* Spaces Are Moved Into Error-Message Is Preparation Of The Next
000682* String Statement That Will Occur Using The Field.
000683     Call "SP2" Using Sp2-Display-Message Sp2-Message-Data
000684     .
000685 Proc-Close-Window.
000686************************
000687* Close Current Window *
000688************************
000689     Call "SP2" Using Sp2-Close-Window Sp2-Null-Parm
000690     .
000691
000692 Proc-Close-File.
000693**********************
000694* Close Current File *
000695**********************
000696     Call "SP2" Using Sp2-Close-File Sp2-Null-Parm
000697     .
000698
000699 Proc-End-Session.
000700*******************
000710* End Sp2 Session *
000720*******************
000730     Call "SP2" Using Sp2-End-Session Sp2-Null-Parm
000740     .