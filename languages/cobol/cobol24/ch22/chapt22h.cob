000010 @OPTIONS MAIN,TEST
000020 Identification Division.
000030 Program-Id.  Chapt22h.
000040*Assemble Full Name From First And Last
000050 Environment Division.
000060 Configuration Section.
000070 Source-Computer.  IBM-PC.
000080 Object-Computer.  IBM-PC.
000090 Data Division.
000100 Working-Storage Section.
000110 01  First-Name       Pic X(15)     Value Spaces.
000120 01  Last-Name        Pic X(25)     Value Spaces.
000130 01  Work-Field       Pic X(15)     Value Spaces.
000140 01  Full-Name        Pic X(51)     Value Spaces.
000150 01  Trailing-Spaces  Pic 9(3)      Value Zeros.
000160 01  Field-Length     Pic 9(3)      Value Zeros.
000170 Screen Section.
000180 01  Data-Entry Blank Screen Auto.
000190     03  Line 01 Column 1  Value "First Name: ".
000200     03  Line 01 Column 13 Pic X(15) Using First-Name.
000210     03  Line 03 Column 1  Value "Last Name: ".
000220     03  Line 03 Column 13 Pic X(25) Using Last-Name.
000230     03  Line 06 Column 1  Value "Full Name: ".
000240     03  Line 06 Column 13 Pic X(51) From Full-Name.
000250 Procedure Division.
000260 Chapt22h-Start.
000270     Display Data-Entry
000280     Accept Data-Entry
000290     Move Function Reverse (First-Name) To Work-Field
000300     Inspect Work-Field Tallying Trailing-Spaces For
000310                        Leading Spaces
000320     Compute Field-Length = Function Length (First-Name)
000330     String First-Name (1:Field-Length - Trailing-Spaces)
000340            " "
000350            Last-Name
000360            Delimited By Size, Into Full-Name
000370     Display Data-Entry
000380     Stop Run
000390     .
