000010 @OPTIONS MAIN,TEST
000020 Identification Division.
000030 Program-Id.  Chapt23d.
000031* Enter A Date For Validation
000040 Environment Division.
000050 Configuration Section.
000051 Source-Computer.  IBM-PC.
000055 Object-Computer.  IBM-PC.
000056 Data Division.
000057 Working-Storage Section.
000058 01  Passed-Date.
000059     03  Date-To-Validate      Pic 9(8).
000060     03  Date-To-Validate-X Redefines Date-To-Validate.
000061         05  Date-MM           Pic 99.
000062         05  Date-DD           Pic 99.
000063         05  Date-YYYY         Pic 9(4).
000097 01  Valid-Status          Pic X(40) Value Spaces.
000107 Screen Section.
000108 01  Date-Entry Blank Screen Auto.
000109     03  Line 01 Column 01 Value "Enter Date: ".
000110     03  Line 01 Column 13 Pic 99/99/9999 Using Date-To-Validate.
000111     03  Line 01 Column 24 Pic X(40) From Valid-Status.
000112 Procedure Division.
000159 Chapt23d-Start.
000160     Initialize Date-To-Validate
000169     Display Date-Entry
000179     Accept Date-Entry
000189     Call "Chapt23e" Using Passed-Date Valid-Status
000199     Display Date-Entry
000255     .