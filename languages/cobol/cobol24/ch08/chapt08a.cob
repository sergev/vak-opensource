000010 @OPTIONS MAIN
000020 Identification Division.
000030 Program-Id.  Chapt08a.
000031* Intelligent Telephone Number Format
000040 Environment Division.
000050 Configuration Section.
000051 Source-Computer.  IBM-PC.
000055 Object-Computer.  IBM-PC.
000056 Data Division.
000057 Working-Storage Section.
000067 01  Phone-Number        Pic 9(10) Value Zeros.
000068 01  Formatted-Number    Pic X(14) Value "(XXX) XXX-XXXX".
000072 01  Formatted-Alternate Pic X(8)  Value "XXX-XXXX".
000073 01  The-Edited-Number   Pic X(14) Value Spaces.
000074 Screen Section.
000075 01  Phone-Entry Blank Screen.
000076     03  Line 01 Column 01 Value " Enter Phone Number: ".
000077     03  Line 01 Column 22 Pic Z(10) Using Phone-Number.
000078     03  Line 03 Column 01 Value "Edited Phone Number: ".
000079     03  Line 03 Column 22 Pic X(14) From The-Edited-Number.
000080 Procedure Division.
000159 Chapt08a-Start.
000160     Display Phone-Entry
000161     Accept  Phone-Entry
000162     If Phone-Number > 9999999
000163* Number Large Enough To Contain Area Code
000164        Inspect Formatted-Number
000166          Replacing First "XXX"  By Phone-Number (1:3)
000167                    First "XXX"  By Phone-Number (4:3)
000168                    First "XXXX" By Phone-Number (7:4)
000169        Move Formatted-Number To The-Edited-Number
000170     Else
000171* Number Not Large Enough To Contain An Area Code
000172        Inspect Formatted-Alternate
000173          Replacing First "XXX"  By Phone-Number (4:3)
000174                    First "XXXX" By Phone-Number (7:4)
000175        Move Formatted-Alternate To The-Edited-Number
000176     End-If
000177     Display Phone-Entry
000178     Stop Run
000180     .