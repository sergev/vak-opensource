000010 @OPTIONS MAIN,TEST
000020 Identification Division.
000030 Program-Id.  Chapt22i.
000031*numval And Numval-C
000040 Environment Division.
000050 Configuration Section.
000051 Source-Computer.  IBM-PC.
000055 Object-Computer.  IBM-PC.
000056 Data Division.
000057 Working-Storage Section.
000058 01  Field-To-Convert Pic X(15)     Value "   1234.56".
000059 01  Converted-Value  Pic S9(5)v99  Value Zeros.
000148 Procedure Division.
000159 Chapt22i-Start.
000169     Compute Converted-Value = Function Numval (Field-To-Convert)
000179     Display "Converted=" Converted-Value
000189     Move "   $12,345.67CR" To Field-To-Convert
000199     Compute Converted-Value =
000209             Function Numval-C (Field-To-Convert "$")
000219     Display "Converted=" Converted-Value
000279     Stop Run
000289     .