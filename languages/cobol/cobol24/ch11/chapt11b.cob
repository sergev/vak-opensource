000010 @OPTIONS MAIN
000020 Identification Division.
000030 Program-Id.  Chapt11b.
000031* Inline Perform Example, Name Join
000044 Environment Division.
000050 Configuration Section.
000051 Source-Computer.  IBM-PC.
000055 Object-Computer.  IBM-PC.
000056 Data Division.
000057 Working-Storage Section.
000058 01  Last-Name            Pic X(20) Value Spaces.
000059 01  First-Name           Pic X(20) Value Spaces.
000060 01  Combined-Name        Pic X(40) Value Spaces.
000061 01  Name-Length          Pic 99    Value Zeros.
000067 Screen Section.
000078 01  Name-Entry Blank Screen.
000079     03  Line 01 Column 01 Value " Last Name: ".
000080     03  Line 01 Column 13 Pic X(20) Using Last-Name.
000081     03  Line 03 Column 01 Value "First Name: ".
000082     03  Line 03 Column 13 Pic X(20) Using First-Name.
000083     03  Line 05 Column 01 Value " Full Name: ".
000084     03  Line 05 Column 13 Pic X(40) From Combined-Name.
000085 Procedure Division.
000159 Chapt11b-Start.
000160     Display Name-Entry
000161     Accept  Name-Entry
000171     Perform Varying Name-Length From 20 By -1
000181        Until First-Name (Name-Length:1) > Space
000191           Or Name-Length = Zeros
000201        Continue
000211     End-Perform
000221     If Name-Length = Zeros
000231        Move Last-Name To Combined-Name
000241     Else
000251        String First-Name (1:name-Length)
000261               Space
000271               Last-Name
000281               Delimited By Size
000291               Into Combined-Name
000301     End-If
000311     Display Name-Entry
000321     Stop Run
000331     .