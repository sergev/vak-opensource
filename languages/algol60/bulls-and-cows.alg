begin
    integer secret, count, guess, cows, bulls, i, j, seed;

    integer procedure digit(num, ps);
        value num;
        integer num, ps;
    begin
        num := num % 10**ps;
        digit := num - num % 10 * 10
    end;

    Boolean procedure malformed(num);
        value num;
        integer num;
    begin
        malformed := false;
        if num > 9876 | num < 0 then begin
            malformed := true
        end else
            for i := 0 step 1 until 2 do
                for j := i+1 step 1 until 3 do
                    if digit(num, j) = digit(num, i) then begin
                        malformed := true;
                        goto done
                    end;
done:
    end;

    comment Ad hoc pseudo random generator;
    real procedure RANDOM; begin
        seed := seed * 16598013 + 12820163;
        seed := seed - seed % 16777216 * 16777216;
        if seed < 0 then
            seed := -seed;
        RANDOM := seed / 16777216.0
    end;
    procedure SETRANDOM(x); value x; real x; begin
        seed := x
    end;

    SETRANDOM(123);
    secret := 0;
    for i := 0 while malformed(secret) do
        secret := entier(RANDOM * 9877);

    count := 0;
    guess := 0;
    for i := 0 while guess != secret do begin
        count := count + 1;
again:
        outstring(1, "Guess a number.\n");
        ininteger(0, guess);
        if malformed(guess) then
            goto again;

        cows := 0;
        bulls := 0;

        for i := 0 step 1 until 3 do
            for j := 0 step 1 until 3 do
                if digit(secret, i) = digit(guess, j) then
                    if i = j then
                        bulls := bulls + 1
                    else
                        cows := cows + 1;

        outstring(1, "You scored ");
        outinteger(1, bulls);
        outstring(1, "bulls and ");
        outinteger(1, cows);
        outstring(1, "cows.\n")
    end;
    outstring(1, "Correct. That took you ");
    outinteger(1, count);
    outstring(1, "guesses.\n")
end
