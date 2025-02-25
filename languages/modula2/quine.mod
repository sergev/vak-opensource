MODULE Quine;
FROM FormatStrings IMPORT Sprintf1;
FROM StrIO IMPORT WriteString, WriteLn;

CONST src = "MODULE Quine;\nFROM FormatString IMPORT FormatString;\nFROM Terminal IMPORT WriteString,ReadChar;\n\nCONST src = \x022%s\x022;\nVAR buf : ARRAY[0..2048] OF CHAR;\nBEGIN\n    FormatString(src, buf, src);\n    WriteString(buf);\n    ReadChar\nEND Quine.\n";
BEGIN
    WriteString(Sprintf1(src, src));
    WriteLn
END Quine.
