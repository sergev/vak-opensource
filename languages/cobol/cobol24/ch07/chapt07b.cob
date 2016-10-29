000010 @OPTIONS MAIN
000020 Identification Division.
000030 Program-Id.  Chapt07b.
000040* Unstring Example
000050 Environment Division.
000060 Configuration Section.
000070 Source-Computer.  IBM-PC.
000080 Object-Computer.  IBM-PC.
000090 Data Division.
000100 Working-Storage Section.
000101 01  Expression-In    Pic X(10)   Value Spaces.
000102 01  First-Term       Pic X(5)    Value Spaces.
000103 01  Second-Term      Pic X(5)    Value Spaces.
000104 01  Operation        Pic X       Value Spaces.
000105 01  Unstring-Pointer Pic 9(2)    Value Zeros.
000106 Screen Section.
000107 01  Main-Screen Blank Screen.
000108     03  Line 01 Column 01 Value "Enter Expression:".
000109     03  Line 01 Column 19 Pic X(10) Using Expression-In.
000110     03  Line 03 Column 01 Value "First Term ".
000111     03  Line 04 Column 01 Value "Second Term ".
000112     03  Line 05 Column 01 Value "Operation ".
000113     03  Line 03 Column 13 Pic X(5) From First-Term.
000114     03  Line 04 Column 13 Pic X(5) From Second-Term.
000115     03  Line 05 Column 13 Pic X    From Operation.
000116 Procedure Division.
000117 Chapt07b-Start.
000120     Display Main-Screen
000130     Accept Main-Screen
000140     Unstring Expression-In
000150       Delimited By "+" Or "-" Or "*" Or "/"
000151       Into First-Term
000152       Delimiter In Operation
000153       Count In Unstring-Pointer
000154     End-Unstring
000155     Add 2 To Unstring-Pointer
000156     Unstring Expression-In
000157       Delimited By "="
000158       Into Second-Term
000159       Pointer Unstring-Pointer
000160     End-Unstring
000161     Display Main-Screen
000164     Stop Run
000174     .