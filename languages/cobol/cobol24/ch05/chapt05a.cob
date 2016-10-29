000010 @OPTIONS MAIN
000020 Identification Division.
000030 Program-Id.  Chapt05a.
000031* Data Entry Screen
000040 Environment Division.
000050 Configuration Section.
000051 Source-Computer.  IBM-PC.
000055 Object-Computer.  IBM-PC.
000056 Data Division.
000057 Working-Storage Section.
000067 01  Field-Group-1.
000077     03  Fg1-First        Pic 9(2) Value 99.
000087     03  Fg1-Second       Pic 9(2) Value 2.
000097     03  Fg1-Third        Pic 9(2) Value 3.
000107 01  Field-Group-2.
000117     03  Fg1-First        Pic 9(2) Value 10.
000127     03  Fg1-Second       Pic 9(2) Value 20.
000137     03  Fg1-Third        Pic 9(2) Value 30.
000147 01  Data-Item-2          Pic 9999 Value 2.
000148 01  Data-Item-3          Pic 9999 Value 2.
000149 01  Data-Item-4          Pic 9999.
000150 01  Data-Item-5          Pic 9999.
000151 01  Edited-Field         Pic XX/xx/xx.
000152 01  Data-Field-1.
000153     03  Data-Field-Contents Pic X(6) Value "ABCDEF".
000154 01  Some-Number          Pic S9(5)v99 Value -1234.56.
000155 01  Some-Field           Pic X(10).
000156 01  Alpha-Number         Pic X(5) Value "12345".
000157 01  Number-Number        Pic 9(4).
000158 Procedure Division.
000159 Required-Paragraph.
000167     Add Corresponding Field-Group-1 To Field-Group-2
000168        On Size Error
000169           Display "Size error"
000170     End-Add
000177     Display Field-Group-2
000178     Add 10 20 30 Giving Data-Item-2 Data-Item-3
000179     Multiply 10  By 30  Giving Data-Item-2 Data-Item-3
000180     Display Data-Item-2
000181     Divide 10 By 3 Giving Data-Item-2 Data-Item-3
000182     Display "divide " Data-Item-3
000183     Divide 10 By 3 Giving Data-Item-2 Remainder Data-Item-4
000184     Display "divide 2 " Data-Item-4 " " Data-Item-5
000187     Compute Data-Item-4 = 3 * (1 / 3)
000188     Display Data-Item-4
000189     Move Data-Field-Contents To Edited-Field.
000190     Display "E1 " Edited-Field.
000191     Move Data-Field-1 To Edited-Field.
000192     Display "E2 " Edited-Field.
000193     Move Some-Number To Some-Field.
000194     Display "Sf " Some-Field.
000195     Move Alpha-Number To Number-Number.
000196     Display Number-Number.
000197     Stop Run
000198     .