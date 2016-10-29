000010 @OPTIONS MAIN
000020 Identification Division.
000030 Program-Id.  Chapt08x.
000031* Exercise 8 Solution
000032*
000033* This Solution Will Seem Complex And Cumbersome.  It Is.  However,
000034* The Lesson In Chapter 21 Will Show You Some Methods In Cobol To
000035* Make This Type Of Problem Very Simple To Solve.
000036*
000040 Environment Division.
000050 Configuration Section.
000051 Source-Computer.  IBM-PC.
000055 Object-Computer.  IBM-PC.
000056 Data Division.
000057 Working-Storage Section.
000058 01  Number-1            Pic 99    Value Zeros.
000059 01  Number-2            Pic 99    Value Zeros.
000060 01  Number-3            Pic 99    Value Zeros.
000061 01  Number-4            Pic 99    Value Zeros.
000071 01  High-Number         Pic 99    Value Zeros.
000081 01  Low-Number          Pic 99    Value Zeros.
000082 Screen Section.
000083 01  Number-Entry Blank Screen.
000084     03  Line 01 Column 01 Value "Enter Number 1: ".
000085     03  Line 01 Column 17 Pic Z9 Using Number-1.
000086     03  Line 02 Column 01 Value "Enter Number 2: ".
000087     03  Line 02 Column 17 Pic Z9 Using Number-2.
000088     03  Line 03 Column 01 Value "Enter Number 3: ".
000089     03  Line 03 Column 17 Pic Z9 Using Number-3.
000090     03  Line 04 Column 01 Value "Enter Number 4: ".
000091     03  Line 04 Column 17 Pic Z9 Using Number-4.
000092     03  Line 07 Column 01 Value " Low Number is: ".
000093     03  Line 07 Column 17 Pic Z9 From  Low-Number.
000094     03  Line 08 Column 01 Value "High Number is: ".
000095     03  Line 08 Column 17 Pic Z9 From  High-Number.
000096
000097 Procedure Division.
000159 Chapt08x-Start.
000160     Display Number-Entry
000161     Accept  Number-Entry
000171* No Matter What, The First Number Starts As The High And The
000181* Low Number.  It Is The Number All Others Are Compared Against.
000191     Move Number-1 To High-Number, Low-Number
000201     If Number-2 < Low-Number
000211        Move Number-2 To Low-Number
000221     End-If
000231     If Number-3 < Low-Number
000241        Move Number-3 To Low-Number
000251     End-If
000261     If Number-4 < Low-Number
000271        Move Number-4 To Low-Number
000281     End-If
000291     If Number-2 > High-Number
000301        Move Number-2 To High-Number
000311     End-If
000321     If Number-3 > High-Number
000331        Move Number-3 To High-Number
000341     End-If
000351     If Number-4 > High-Number
000361        Move Number-4 To High-Number
000371     End-If
000381     Display Number-Entry
000382     Accept Number-Entry
000391* Another Method, Not Using The Baseline Numbers.
000392* Next Statement Clears The Numbers.
000401     Move Zeros To High-Number Low-Number
000402* <= Is Used In Case Some Numbers Are Equal
000411     If Number-1 <= Number-2 And Number-3 And Number-4
000421        Move Number-1 To Low-Number
000431        Else
000441        If Number-2 <= Number-1 And Number-3 And Number-4
000451           Move Number-2 To Low-Number
000461           Else
000471           If Number-3 <= Number-1 And Number-2
000481                         And Number-4
000491           Move Number-3 To Low-Number
000501           Else
000502* Fourth Is The Only One Left, It Must Be The Lowest!
000511              Move Number-4 To Low-Number
000521           End-If
000531        End-If
000541     End-If
000551* Now Find The Highest
000552     If Number-1 >= Number-2 And Number-3 And Number-4
000553        Move Number-1 To High-Number
000554        Else
000555        If Number-2 >= Number-1 And Number-3 And Number-4
000556           Move Number-2 To High-Number
000557           Else
000558           If Number-3 >= Number-1 And Number-2
000559                         And Number-4
000560           Move Number-3 To High-Number
000561           Else
000562* Fourth Is The Only One Left, It Must Be The Highest!
000563              Move Number-4 To High-Number
000564           End-If
000565        End-If
000566     End-If
000567     Display Number-Entry
000568     Stop Run
000569     .
000570