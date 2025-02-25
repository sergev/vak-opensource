MODULE echo;

FROM Args     IMPORT GetArg, Narg;
FROM StrIO    IMPORT WriteString, WriteLn;
FROM NumberIO IMPORT WriteCard;

VAR
    count, item: CARDINAL;
    arg: ARRAY [0..100] OF CHAR;
BEGIN
    count := Narg();

    WriteString ('Count =');
    WriteCard (count, 4);
    WriteLn;

    item := 0;
    REPEAT
        WriteCard (item, 4);
        WriteString (' : ');
        IF GetArg (arg, item) THEN
            WriteString (arg)
        END;
        WriteLn;
        INC (item)
    UNTIL item = count
END echo.
