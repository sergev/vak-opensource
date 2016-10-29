000010 @OPTIONS MAIN
000020 Identification Division.
000030 Program-Id.  Chapt08b.
000031* Intelligent Name Separation
000040 Environment Division.
000050 Configuration Section.
000051 Source-Computer.  IBM-PC.
000055 Object-Computer.  IBM-PC.
000056 Data Division.
000057 Working-Storage Section.
000067 01  Name-Entered        Pic X(50) Value Spaces.
000068 01  First-Name          Pic X(30) Value Spaces.
000072 01  Middle-Name         Pic X(30) Value Spaces.
000073 01  Last-Name           Pic X(30) Value Spaces.
000074 01  Unstring-Fields               Value Spaces.
000075     03  First-Field     Pic X(30).
000076     03  Second-Field    Pic X(30).
000077     03  Third-Field     Pic X(30).
000078 01  Number-Of-Fields    Pic 9     Value Zeros.
000079     88  Last-Name-Only            Value 1.
000080     88  First-And-Last            Value 2.
000081     88  First-Last-Middle         Value 3.
000082 Screen Section.
000083 01  Name-Entry Blank Screen.
000084     03  Line 01 Column 01 Value "Enter Name: ".
000085     03  Line 01 Column 13 Pic X(50) Using Name-Entered.
000086     03  Line 03 Column 01 Value " First: ".
000087     03  Line 03 Column 09 Pic X(30) From First-Name.
000088     03  Line 04 Column 01 Value "Middle: ".
000089     03  Line 04 Column 09 Pic X(30) From Middle-Name.
000090     03  Line 05 Column 01 Value "  Last: ".
000091     03  Line 05 Column 09 Pic X(30) From Last-Name.
000092 Procedure Division.
000159 Chapt08b-Start.
000160     Display Name-Entry
000161     Accept  Name-Entry
000171* Unstring Into Possible 3 Fields, Allow For Multiple Spaces
000172* Between Names
000173     Unstring Name-Entered Delimited By All Space
000174         Into First-Field, Second-Field, Third-Field
000175         Tallying In Number-Of-Fields
000176     End-Unstring
000177* Now, Move As Appropriate
000178     If Last-Name-Only
000179        Move First-Field  To Last-Name
000180     End-If
000181     If First-And-Last
000182        Move First-Field  To First-Name
000183        Move Second-Field To Last-Name
000184     End-If
000185     If First-Last-Middle
000186        Move First-Field  To First-Name
000187        Move Second-Field To Middle-Name
000188        Move Third-Field  To Last-Name
000189     End-If
000190     Display Name-Entry
000191     Stop Run
000192     .