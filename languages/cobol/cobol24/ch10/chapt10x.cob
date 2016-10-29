000010 @OPTIONS MAIN
000020 Identification Division.
000030 Program-Id.  Chapt10x.
000031* Accept And Format Phone Number 5 Times
000040*
000041* This Example Is But One Possible Solution To The Exercise.
000042* Yours May Differ.
000043*
000044 Environment Division.
000050 Configuration Section.
000051 Source-Computer.  IBM-PC.
000055 Object-Computer.  IBM-PC.
000056 Data Division.
000057 Working-Storage Section.
000067 01  Formatted-Number     Pic X(14) Value "(XXX) XXX-XXXX".
000072 01  Formatted-Alternate   Pic X(8)  Value "XXX-XXXX".
000073 01  Input-Fields.
000075     03  Phone-Number      Pic 9(10) Value Zeros.
000076     03  The-Edited-Number Pic X(14) Value Spaces.
000077 Screen Section.
000078 01  Phone-Entry Blank Screen.
000079     03  Line 01 Column 01 Value " Enter Phone Number: ".
000080     03  Line 01 Column 22 Pic Z(10) Using Phone-Number.
000081     03  Line 03 Column 01 Value "Edited Phone Number: ".
000082     03  Line 03 Column 22 Pic X(14) From The-Edited-Number.
000083 Procedure Division.
000159 Chapt10x-Start.
000160     Perform Display-And-Accept 5 Times
000161* After The Last One We Need To Display The Result Before Stopping.
000162     Display Phone-Entry
000163     Stop Run
000164     .
000165 Display-And-Accept.
000168     Display Phone-Entry
000169     Accept  Phone-Entry
000170* Reset Our Edit Patterns For The Next Set Of Numbers
000171     Move "(XXX) XXX-XXXX" To Formatted-Number
000172     Move "XXX-XXXX" To Formatted-Alternate
000173     If Phone-Number > 9999999
000174* Number Large Enough To Contain Area Code
000175        Perform Format-Large-Number
000181     Else
000182* Number Not Large Enough To Contain An Area Code
000183        Perform Format-Small-Number
000188     End-If
000189     .
000190 Format-Large-Number.
000191     Inspect Formatted-Number
000192       Replacing First "XXX"  By Phone-Number (1:3)
000193                 First "XXX"  By Phone-Number (4:3)
000194                 First "XXXX" By Phone-Number (7:4)
000195     Move Formatted-Number To The-Edited-Number
000196     .
000197 Format-Small-Number.
000198     Inspect Formatted-Alternate
000199       Replacing First "XXX"  By Phone-Number (4:3)
000200                 First "XXXX" By Phone-Number (7:4)
000201     Move Formatted-Alternate To The-Edited-Number
000202     .