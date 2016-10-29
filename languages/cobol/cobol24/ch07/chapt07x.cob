000010 @OPTIONS MAIN
000020 Identification Division.
000030 Program-Id.  Chapt07x.
000040* Chapter 7 Exercise Solution
000050 Environment Division.
000060 Configuration Section.
000070 Source-Computer.  IBM-PC.
000080 Object-Computer.  IBM-PC.
000090 Data Division.
000100 Working-Storage Section.
000101 01  Word-In          Pic X(40)   Value Spaces.
000102 01  Word-Out         Pic X(40)   Value Spaces.
000103 01  Work-Field-1     Pic X(40)   Value Spaces.
000104 01  Work-Field-2     Pic X(40)   Value Spaces.
000106 Screen Section.
000107 01  Main-Screen Blank Screen.
000108     03  Line 01 Column 01 Value "Enter Word:".
000109     03  Line 01 Column 13 Pic X(40) Using Word-In.
000110     03  Line 03 Column 01 Value "Word Result:".
000114     03  Line 03 Column 14 Pic X(40) From Word-Out.
000116 Procedure Division.
000117 Chapt07x-Start.
000120     Display Main-Screen
000130     Accept Main-Screen
000131     Unstring Word-In 
000132       Delimited by "-" 
000133       Into Work-Field-1 Work-Field-2
000134     End-Unstring
000135     String Work-Field-1 
000136            Work-Field-2
000137            Delimited By Space
000138            Into Word-Out
000139     End-String
000161     Display Main-Screen
000164     Stop Run
000174     .
