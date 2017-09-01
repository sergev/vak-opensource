PROC factorial = (INT n) INT:
BEGIN
    INT result := 1;
    FOR i FROM 2 TO n
    DO
        result := result * i
    OD;
    result
END;

PROC factrec = (INT n) INT:
BEGIN
    IF n <= 1
    THEN
        1
    ELSE
        n * factrec(n - 1)
    FI
END;

BEGIN
    INT n := read int;
    INT result_it := factorial(n);
    INT result_rec := factrec(n);
    print((result_it,  newline, result_rec))
END
