000010 @OPTIONS MAIN,TEST
000020 Identification Division.
000030 Program-Id.  Chapt12f.
000044 Environment Division.
000050 Configuration Section.
000051 Source-Computer.  IBM-PC.
000055 Object-Computer.  IBM-PC.
000056 Data Division.
000057 Working-Storage Section.
000067 01  File-Cabinet.
000069     03  Drawer-Number Occurs 3 Times Indexed By Drawer-Index.
000070         05  File-Number Pic 9(3) Occurs 10 Times
000071             Indexed By File-Index.
000072
000077 Procedure Division.
000207 Chapt12f-Start.
000208     Set Drawer-Index To 2
000216     Search File-Number
000218            At End Display "File Not Found"
000219            When File-Number (Drawer-Index, File-Index) = 123
000220            Display "File Found"
000222     End-Search
000224     Stop Run
000344     .