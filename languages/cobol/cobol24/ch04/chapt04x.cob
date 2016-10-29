000020 Identification Division.
000030 Program-Id.  Chapt04x.
000031* Chapter 4 Exercise Solution
000040 Environment Division.
000050 Configuration Section.
000051 Special-Names.
000052       Crt Status Is Keyboard-Status
000053       Cursor Is Cursor-Position.
000054 Source-Computer.  IBM-PC.
000055 Object-Computer.  IBM-PC.
000056 Data Division.
000057 Working-Storage Section.
000058 01  Keyboard-Status.
000059     03  Accept-Status Pic 9.
000060     03  Filler        Pic X.
000061     03  System-Use    Pic X.
000062     03  Function-Key  Pic X.
000063 01  Cursor-Position.
000064     03  Cursor-Row    Pic 9(2) Value 1.
000065     03  Cursor-Column Pic 9(2) Value 1.
000066 01  Screen-Items.
000075     03  Last-Name           Pic X(25)   Value Spaces.
000085     03  First-Name          Pic X(15)   Value Spaces.
000095     03  Middle-Name         Pic X(10)   Value Spaces.
000096     03  E-Mail              Pic X(50)   Value Spaces.
000105     03  Address-Line-1      Pic X(50)   Value Spaces.
000115     03  Address-Line-2      Pic X(50)   Value Spaces.
000140     03  City                Pic X(40)   Value Spaces.
000150     03  State-Or-Country    Pic X(20)   Value Spaces.
000160     03  Postal-Code         Pic X(15)   Value Spaces.
000170     03  Home-Phone          Pic X(20)   Value Spaces.
000180     03  Work-Phone          Pic X(20)   Value Spaces.
000190     03  Other-Phone         Pic X(20)   Value Spaces.
000200     03  Start-Date          Pic 9(8)    Value Zeros.
000210     03  Last-Rent-Paid-Date Pic 9(8)    Value Zeros.
000220     03  Next-Rent-Due-Date  Pic 9(8)    Value Zeros.
000230     03  Rent-Amount         Pic 9(4)v99 Value Zeros.
000240     03  Consignment-Percent Pic 9(3)    Value Zeros.
000254 Screen Section.
000255 01  Data-Entry-Screen
000256     Blank Screen, Auto
000257     Foreground-Color Is 7,
000258     Background-Color Is 1.
000259*
000260     03  Screen-Literal-Group.
000261         05  Line 01 Column 30 Value "Darlene's Treasures"
000262             Highlight Foreground-Color 4 Background-Color 1.
000263         05  Line 03 Column 30 Value "Tenant Entry Program"
000264             Highlight.
000265         05  Line 5  Column 01  Value "Name, Last: ".
000266         05  Line 5  Column 39  Value "First: ".
000267         05  Line 5  Column 62  Value "Middle: ".
000268         05  Line 6  Column 01  Value "Address 1: ".
000269         05  Line 7  Column 01  Value "Address 2: ".
000270         05  Line 8  Column 01  Value "City: ".
000271         05  Line 9  Column 01  Value "Country/State: ".
000272         05  Line 9  Column 36  Value "Postal Code: ".
000273         05  Line 11 Column 01  Value "Phone/Home: ".
000274         05  Line 11 Column 34  Value "Work: ".
000275         05  Line 12 Column 06  Value "Other: ".
000276         05  Line 13 Column 05  Value "E-Mail: ".
000277         05  Line 14 Column 01  Value "Start Date: ".
000278         05  Line 14 Column 24  Value "Last Paid Date: ".
000279         05  Line 14 Column 50  Value "Next Rent Due on: ".
000280         05  Line 16 Column 01  Value "Consignment Percent: ".
000281*
000282     03  Required-Reverse-Group Reverse-Video Required.
000284         05  Line 5 Column 13  Pic X(25) Using Last-Name.
000285         05  Line 5 Column 46  Pic X(15) Using First-Name.
000286*
000287     03  Reverse-Video-Group Reverse-Video.
000288         05  Line 5  Column 70 Pic X(10) Using Middle-Name.
000289         05  Line 6  Column 15 Pic X(50) Using Address-Line-1.
000292         05  Line 7  Column 15 Pic X(50) Using Address-Line-2.
000295         05  Line 8  Column 15 Pic X(40) Using City.
000300         05  Line 9  Column 15 Pic X(20) Using State-Or-Country.
000302         05  Line 9  Column 50 Pic X(15) Using Postal-Code.
000305         05  Line 11 Column 13 Pic X(20) Using Home-Phone.
000307         05  Line 11 Column 41 Pic X(20) Using Work-Phone.
000310         05  Line 12 Column 13 Pic X(20) Using Other-Phone.
000311         05  Line 13 Column 13 Pic X(50) Using E-Mail.
000313         05  Line 14 Column 13 Pic 99/99/9999
000314             Using Start-Date.
000315         05  Line 14 Column 40 Pic 99/99/9999
000316             Using Last-Rent-Paid-Date.
000317         05  Line 14 Column 68 Pic 99/99/9999
000318             Using Next-Rent-Due-Date.
000321         05  Line 16 Column 22 Pic ZZ9 Using Consignment-Percent.
000325*
000326 Procedure Division.
000327 Chapt04x-Start.
000328     Display Data-Entry-Screen.
000329     Accept Data-Entry-Screen.
000330     Stop Run.
