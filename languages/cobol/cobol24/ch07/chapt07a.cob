000010 @OPTIONS MAIN
000020 Identification Division.
000030 Program-Id.  Chapt07a.
000040* String Example
000050 Environment Division.
000060 Configuration Section.
000070 Source-Computer.  IBM-PC.
000080 Object-Computer.  IBM-PC.
000090 Data Division.
000100 Working-Storage Section.
000110 01  First-Name    Pic X(25)   Value Spaces.
000120 01  Last-Name     Pic X(25)   Value Spaces.
000130 01  Combined-Name Pic X(30)   Value Spaces.
000140 Procedure Division.
000150 Chapt07a-Start.
000160     Move "First" To First-Name
000170     Move "Last"  To Last-Name
000180     String First-Name Delimited By Space
000190            " "        Delimited By Size
000200            Last-Name  Delimited By Space
000210        Into
000220            Combined-Name
000230        On Overflow
000240            Move Last-Name To Combined-Name
000250     End-String
000270     Display "1 " Combined-Name
000280     Move "A" To First-Name
000290     Move "B" To Last-Name
000300     String First-Name Delimited By Space
000310            " "        Delimited By Size
000320            Last-Name  Delimited By Space
000330        Into
000340            Combined-Name
000350        On Overflow
000360            Move Last-Name To Combined-Name
000370     End-String
000380     Display "2 " Combined-Name
000390     Move Spaces To Combined-Name
000400     Move "ReallyLongFirstName" To First-Name
000410     Move "ReallyLongLastName"  To Last-Name
000420     String First-Name Delimited By Space
000430            " "        Delimited By Size
000440            Last-Name  Delimited By Space
000450        Into
000460            Combined-Name
000470        On Overflow
000480            Move Last-Name To Combined-Name
000490     End-String
000500     Display "3 " Combined-Name
000510     Stop Run
000520     .
