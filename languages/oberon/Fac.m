MODULE Fac;
IMPORT In, Out;

VAR n, i, f: INTEGER;

BEGIN
    LOOP
        Out.String("Gimme a number: ");
        In.Int(n);
        IF n < 0 THEN
            EXIT
        END;
        i := 0;
        f := 1;
        WHILE i < n DO
            i := i + 1;
            f := f * i
        END;

        Out.String("The factorial of ");
        Out.Int(n, 0);
        Out.String(" is ");
        Out.Int(f, 0);
        Out.Ln
    END
END Fac.
