000010 @OPTIONS MAIN
000020 Identification Division.
000030 Program-Id.  Chapt06b.
000031* Telephone Number Format
000040 Environment Division.
000050 Configuration Section.
000051 Source-Computer.  IBM-PC.
000055 Object-Computer.  IBM-PC.
000056 Data Division.
000059 Working-Storage Section.
000067 01  Phone-Number.
000068     03  Area-Code       Pic XXX   Value "409".
000069     03  Prefix-Num      Pic XXX   Value "555".
000070     03  Last-Four       Pic X(4)  Value "1212".
000071 01  Formatted-Number    Pic X(14) Value "(XXX) YYY-ZZZZ".
000072 01  Formatted-Alternate Pic X(14) Value "(XXX) XXX-XXXX".
000073
000074 Procedure Division.
000159 Start-Of-Program.
000160     Inspect Formatted-Number
000161         Replacing All "XXX"  By Area-Code
000162                   All "YYY"  By Prefix-Num
000163                   All "ZZZZ" By Last-Four
000164     Display Formatted-Number
000165     Inspect Formatted-Alternate
000166         Replacing First "XXX"  By Area-Code
000167                   First "XXX"  By Prefix-Num
000168                   First "XXXX" By Last-Four
000169     Display Formatted-Alternate
000170     Stop Run
000171     .
000172