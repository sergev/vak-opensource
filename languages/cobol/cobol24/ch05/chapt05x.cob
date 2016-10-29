000010 @OPTIONS MAIN
000020 Identification Division.
000030 Program-Id.  Chapt05x.
000031* Chapter 5 Exercise Answer
000040 Environment Division.
000050 Configuration Section.
000060 Source-Computer.  IBM-PC.
000070 Object-Computer.  IBM-PC.
000080 Data Division.
000090 Working-Storage Section.
000091 01  Work-Numbers Value Zeros.
000100     03  First-Number          Pic 9(5).
000110     03  Second-Number         Pic 9(5).
000120     03  Addition-Result       Pic 9(6).
000121* Subtraction-Result Is Signed In Case The Result Is Negative.
000130     03  Subtraction-Result    Pic S9(5).
000140     03  Multiplication-Result Pic 9(9).
000150     03  Division-Result       Pic 9(6)v9(4).
000254 Screen Section.
000255 01  Main-Screen Blank Screen.
000259     03  Line 1 Column 1  Value "First Number: ".
000260     03  Line 1 Column 15 Pic Z(4)9 Using First-Number.
000261     03  Line 2 Column 1  Value "Second Number: ".
000262     03  Line 2 Column 15 Pic Z(4)9 Using Second-Number.
000263     03  Line 5 Column 1  Value "Addition Result ".
000264     03  Line 5 Column 17 Pic Z(5)9 From Addition-Result.
000265     03  Line 6 Column 1  Value "Subtraction Result ".
000266     03  Line 6 Column 20 Pic Z(4)9- From Subtraction-Result.
000267     03  Line 7 Column 1  Value "Multiplication Result ".
000268     03  Line 7 Column 23 Pic Z(8)9 From Multiplication-Result.
000269     03  Line 8 Column 1  Value "Division Result ".
000270     03  Line 8 Column 17 Pic Z(5)9.9(4) From Division-Result.
000271 Procedure Division.
000286 Chapt05x-Start.
000287     Display Main-Screen
000297     Accept Main-Screen
000307     Add First-Number Second-Number Giving Addition-Result
000317     Subtract Second-Number From First-Number
000327              Giving Subtraction-Result
000337     Multiply First-Number By Second-Number
000347              Giving Multiplication-Result
000357* If You Have Entered Zero As The Second Number, The Program Will
000358* Crash With A Message "The program has performed an illegal operation"
000359* The Details Of Which Will Be "divide exception".
000360     Divide First-Number By Second-Number
000367              Giving Division-Result
000377     Display Main-Screen
000460     Stop Run.