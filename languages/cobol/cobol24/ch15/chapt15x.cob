000010 @OPTIONS MAIN,TEST
000020 Identification Division.
000030 Program-Id.  Chapt15x.
000031* Chapter 15 Exercise Solution
000041 Environment Division.
000050 Configuration Section.
000051 Special-Names.
000052       Crt Status Is Keyboard-Status
000053       Cursor Is Cursor-Position.
000055 Source-Computer.  IBM-PC.
000056 Object-Computer.  IBM-PC.
000057 Input-Output Section.
000058 File-Control.
000059     Select Dealer-File Assign To "Dealer.Dat"
000060            Organization Indexed
000061            Access Sequential
000062            Record Key Dealer-Number Of Dealer-Record
000063            Alternate Record Key Dealer-Name Of Dealer-Record
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
000097     03  Filler              Pic X(50).
000098 Working-Storage Section.
000108 01  Match-Flag        Pic X Value Spaces.
000109     88  Match-Found   Value "M".
000110     88  End-Of-File   Value "E".
000111* The Initialization Of Match-Name To Spaces Is Important In Case They
000112* Press The Find Next, Without Ever Entering Any Match Criteria.
000113 01  Match-Name        Value Spaces.
000114     03  Match-Last    Pic X(25).
000115     03  Match-First   Pic X(15).
000116     03  Match-Middle  Pic X(10).
000118 01  Dealer-Status     Pic X(2) Value Spaces.
000240     88  Dealer-Success Value "00" Thru "09".
000279 01  Keyboard-Status.
000280     03  Accept-Status Pic 9.
000281     03  Function-Key  Pic X.
000282         88  F1-Pressed Value X"01".
000283         88  F2-Pressed Value X"02".
000284         88  F3-Pressed Value X"03".
000285         88  F4-Pressed Value X"04".
000286     03  System-Use    Pic X.
000290 01  Cursor-Position.
000291     03  Cursor-Row    Pic 9(2) Value 1.
000292     03  Cursor-Column Pic 9(2) Value 1.
000293 01  Eof-Message   Pic X(11) Value "End Of File".
000294 01  Error-Message Pic X(50) Value Spaces.
000295* A Working Storage Record Is Used, Because We Are Reading Multiple
000296* Records From The File, But Not Necessarying Displaying Them.
000297* If We Did Not Use A Separate Area, The Last Record Displayed Would Not
000298* Remain When We Fail To Find Another Match.
000299 01  Dealer-Work.
000300     03  Dealer-Number         Pic X(8)  Value Spaces.
000301     03  Dealer-Name                     Value Spaces.
000302         05  Last-Name   Pic X(25).
000303         05  First-Name  Pic X(15).
000304         05  Middle-Name Pic X(10).
000305     03  Address-Line-1      Pic X(50)   Value Spaces.
000306     03  Address-Line-2      Pic X(50)   Value Spaces.
000307     03  City                Pic X(40)   Value Spaces.
000308     03  State-Or-Country    Pic X(20)   Value Spaces.
000309     03  Postal-Code         Pic X(15)   Value Spaces.
000310     03  Home-Phone          Pic X(20)   Value Spaces.
000311     03  Work-Phone          Pic X(20)   Value Spaces.
000312     03  Other-Phone         Pic X(20)   Value Spaces.
000313     03  Start-Date          Pic 9(8)    Value Zeros.
000314     03  Last-Rent-Paid-Date Pic 9(8)    Value Zeros.
000315     03  Next-Rent-Due-Date  Pic 9(8)    Value Zeros.
000316     03  Rent-Amount         Pic 9(4)v99 Value Zeros.
000317     03  Consignment-Percent Pic 9(3)    Value Zeros.
000318     03  Filler              Pic X(50)   Value Spaces.
000319 Screen Section.
000321 01  Data-Entry-Screen
000322     Blank Screen, Auto
000323     Foreground-Color Is 7,
000324     Background-Color Is 1.
000325*
000326     03  Screen-Literal-Group.
000327         05  Line 01 Column 30 Value "Darlene's Treasures"
000328             Highlight Foreground-Color 4 Background-Color 1.
000329         05  Line 03 Column 30 Value "Tenant Entry Program"
000330             Highlight.
000331         05  Line 4  Column 01  Value "Number: ".
000332         05  Line 5  Column 01  Value "Name, Last: ".
000333         05  Line 5  Column 39  Value "First: ".
000334         05  Line 5  Column 62  Value "Middle: ".
000335         05  Line 6  Column 01  Value "Address 1: ".
000336         05  Line 7  Column 01  Value "Address 2: ".
000337         05  Line 8  Column 01  Value "City: ".
000338         05  Line 9  Column 01  Value "Country/State: ".
000339         05  Line 9  Column 36  Value "Postal Code: ".
000340         05  Line 11 Column 01  Value "Phone/Home: ".
000341         05  Line 11 Column 34  Value "Work: ".
000342         05  Line 12 Column 06  Value "Other: ".
000343         05  Line 14 Column 01  Value "Start Date: ".
000344         05  Line 14 Column 24  Value "Last Paid Date: ".
000345         05  Line 14 Column 51  Value "Next Rent Due on: ".
000346         05  Line 15 Column 01  Value "Rent Amount: ".
000347         05  Line 16 Column 01  Value "Consignment Percent: ".
000348         05  Line 22 Column 01  Value "F1-Find New Match".
000349         05  Line 22 Column 23  Value "F2-Find Next Match".
000350         05  Line 22 Column 56  Value "F3-Clear".
000351         05  Line 23 Column 01  Value "F4-Exit".
000354     03  Required-Reverse-Group Reverse-Video.
000355         05  Line 4 Column 13  Pic X(8)  From Dealer-Number
000356                    Of Dealer-Work.
000357         05  Line 5 Column 13  Pic X(25) Using Last-Name
000358                    Of Dealer-Work.
000360         05  Line 5 Column 46  Pic X(15) Using First-Name
000361                    Of Dealer-Work.
000362         05  Line 5  Column 70 Pic X(10) Using Middle-Name
000363                    Of Dealer-Work.
000364         05  Line 6  Column 15 Pic X(50) From Address-Line-1
000365                    Of Dealer-Work.
000366         05  Line 7  Column 15 Pic X(50) From Address-Line-2
000367                    Of Dealer-Work.
000368         05  Line 8  Column 15 Pic X(40) From City
000369                    Of Dealer-Work.
000370         05  Line 9  Column 15 Pic X(20) From State-Or-Country
000371                    Of Dealer-Work.
000372         05  Line 9  Column 50 Pic X(15) From Postal-Code
000373                    Of Dealer-Work.
000374         05  Line 11 Column 13 Pic X(20) From Home-Phone
000375                    Of Dealer-Work.
000376         05  Line 11 Column 41 Pic X(20) From Work-Phone
000377                    Of Dealer-Work.
000378         05  Line 12 Column 13 Pic X(20) From Other-Phone
000379                    Of Dealer-Work.
000380         05  Line 14 Column 13 Pic 99/99/9999 From Start-Date
000381                    Of Dealer-Work.
000382         05  Line 14 Column 40 Pic 99/99/9999
000383                    From Last-Rent-Paid-Date Of Dealer-Work.
000385         05  Line 14 Column 69 Pic 99/99/9999
000386                    From Next-Rent-Due-Date Of Dealer-Work.
000388         05  Line 15 Column 14 Pic Z,ZZZ.99 From Rent-Amount
000389                    Of Dealer-Work.
000390         05  Line 16 Column 22 Pic ZZ9 From Consignment-Percent
000391                    Of Dealer-Work.
000392         05  Line 20 Column 01 Pic X(50) Using Error-Message.
000393*
000394 Procedure Division.
000395 Chapt15x-Start.
000408     Perform Open-File
000418     If Not Dealer-Success
000438        String "Error Opening Dealer File "
000448               Dealer-Status
000449               Delimited By Size
000450               Into Error-Message
000451        End-String
000452        Perform Display-And-Accept
000454     Else
000455        Perform Process-File Until F4-Pressed
000457        Perform Close-File
000458     End-If
000459     Stop Run
000460     .
000461 Process-File.
000462     Perform Display-And-Accept
000463     Evaluate True
000464       When F1-Pressed
000465            Perform Find-First-Match
000466       When F2-Pressed
000467* If We Already Reached The End Of The File, We Don't Want
000468* To Attempt To Read Another Record.  Since The Error Message
000469* Gets Cleared After Every Accept, We Have To Put It Back If It
000470* Is End Of File
000471            If End-Of-File
000472               Move Eof-Message To Error-Message
000473            Else
000475               Move Spaces To Match-Flag
000476               Perform Find-Next-Match Until Match-Found Or
000477                                             End-Of-File
000478            End-If
000479       When F3-Pressed
000480            Perform Clear-Screen
000481* The F4-Pressed Condition Could Be Detected With Other, But This
000482* Method Is A Little Clearer
000483       When F4-Pressed
000484            Continue
000485       When Other
000486            Continue
000487     End-Evaluate
000488     .
000489 Find-First-Match.
000490     Move Corresponding
000491          Dealer-Name Of Dealer-Work To
000492          Dealer-Name Of Dealer-Record
000494     Move First-Name Of Dealer-Work To Match-First
000495     Move Middle-Name Of Dealer-Work To Match-Middle
000496     Move Last-Name Of Dealer-Work To Match-Last
000497* Start The File Based On The Name Key.  If They Entered A Last Name
000498* All The Better, Because It Will Save Processing.  If They Did Not Enter
000499* A Last Name We Will Be Processing From The Beginning Of The File
000500
000501* The "Not <" Is Used So That If All Three Names Are Entered, You Will
000502* Position The File On That Record And Not On A Later One, As Would Happen
000503* If You Used > In The Start.
000504     Start Dealer-File Key Not < Dealer-Name Of Dealer-Record
000505           Invalid Key
000506             String "Error Starting Dealer File " Dealer-Status
000507                    Delimited By Size Into Error-Message
000508           Not Invalid Key
000509             Move Spaces To Match-Flag
000510             Perform Find-Next-Match Until Match-Found Or
000511                                           End-Of-File
000512     End-Start
000513     .
000514 Find-Next-Match.
000515     Read Dealer-File Next Record
000523          At End Set End-Of-File To True
000524                 Move Eof-Message To Error-Message
000533          Not At End
000534* Determine If All Of The Search Names That Were Entered,
000535* Match The Search Names In The Record Retrieved
000540         Evaluate True
000541            When Match-First Not = Spaces And
000542                 Match-First Not = First-Name Of Dealer-Record
000543            When Match-Last Not = Spaces And
000544                 Match-Last Not = Last-Name Of Dealer-Record
000545            When Match-Middle Not = Spaces And
000546                 Match-Middle Not = Middle-Name Of Dealer-Record
000547                 Move Spaces To Match-Flag
000548            When Other
000549                 Set Match-Found To True
000550* Found A Match, Move In That Record!
000551                 Move Dealer-Record To Dealer-Work
000552         End-Evaluate
000553*
000554* This Evaluate Is A Very Efficient Way To Test And Determine If
000555* This Record Matches The Entered Criteria.  It Uses "negative logic"
000556* Which Confuses Some Programmer, But Consider What You Are Trying To Do.
000557* The Stacked When Statements Are All Of The Conditions When The Match Is
000558* Not Made, Anything Else Is A Match.
000559*
000561     End-Read
000562     .
000563 Clear-Screen.
000564     Initialize Dealer-Work
000565     Move 01 To Cursor-Row Cursor-Column
000566     .
000567 Display-And-Accept.
000568     Display Data-Entry-Screen
000569     Accept Data-Entry-Screen
000570     Move Spaces To Error-Message
000571     .
000572 Open-File.
000573     Open Input Dealer-File
000574     .
000575 Close-File.
000576     Close Dealer-File
000577     .