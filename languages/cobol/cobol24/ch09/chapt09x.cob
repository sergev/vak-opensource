000010 @OPTIONS MAIN
000020 Identification Division.
000030 Program-Id.  Chapt09x.
000031* Chapter 9 Exercise Solution
000040 Environment Division.
000050 Configuration Section.
000051 Source-Computer.  IBM-PC.
000055 Object-Computer.  IBM-PC.
000056 Data Division.
000057 Working-Storage Section.
000067 01  Name-1              Pic X(20) Value Spaces.
000068 01  Name-2              Pic X(20) Value Spaces.
000074 01  The-Message         Pic X(40) Value Spaces.
000075 Screen Section.
000076 01  Name-Entry Blank Screen.
000077     03  Line 01 Column 01 Value "Enter Name 1: ".
000078     03  Line 01 Column 15 Pic X(20) Using Name-1.
000079     03  Line 02 Column 01 Value "Enter Name 2: ".
000080     03  Line 02 Column 15 Pic X(20) Using Name-2.
000081     03  Line 20 Column 01 Pic X(40) From The-Message.
000083 Procedure Division.
000159 Chapt09x-Start.
000160     Display Name-Entry
000161     Accept  Name-Entry
000171* Evaluate Method
000181     Evaluate Name-1 (1:1)
000191        When Name-2 (1:1)
000201             Move "Names begin with the same letter" To
000211                  The-Message
000221        When Other
000231             Move "Names begin with different letters" To
000241                  The-Message
000251     End-Evaluate
000261     Display Name-Entry
000271     Accept Name-Entry
000281* If Method
000291     If Name-1 (1:1) = Name-2 (1:1)
000301        Move "Names begin with the same letter" To
000311             The-Message
000321     Else
000331        Move "Names begin with different letters" To
000341             The-Message
000351     End-If
000361     Display Name-Entry
000371     Stop Run
000381     .
000391*
000401*  As You Can See, For A Simple Condition, Comparing Two Items,
000402*  "Evaluate" Is Overkill.  An If Statement Is Easier To Read And
000403*  Understand.  However, If There Were More Conditions That The Name
000404*  Field Were To Be Tested Against, Using "Evaluate" Would Make
000405*  More Sense.
000441*
000451