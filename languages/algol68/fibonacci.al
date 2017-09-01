PROC fibonacci = (INT n) INT:
BEGIN
    INT f1 := 1, f2 := 1;
    FOR i FROM 2 TO n
    DO
        INT tmp := f2;
        f2 := f1 + f2;
        f1 := tmp
    OD;
    f1
END;

PROC fibrec = (INT n) INT:
BEGIN
    IF n <= 2
    THEN
        1
    ELSE
        fibrec(n - 1) + fibrec(n - 2)
    FI
END;

BEGIN
    INT n := read int;
    INT i_it := fibonacci(n);
    INT i_rec := fibrec(n);
    print((i_it, newline, i_rec))
END
