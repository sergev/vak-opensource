begin
    comment First Five Hundred Primes;
    integer j, k, n, q, r;
    integer array prime[1:500];
p1: prime[1] := 2; n := 3; j := 1;
p2: j := j + 1; prime[j] := n;
p3: if j = 500 then go to p9;
p4: n := n + 2;
p5: k := 2;
p6: q := n mod prime[k]; r := n - q * prime[k];
    if r = 0 then go to p4;
p7: if q <= prime[k] then go to p2;
p8: k := k + 1; go to p6;
p9: outstring(1, "First Five Hundred Primes\n");
    for j := 1 step 1 until 50 do
    begin
      for k := 1 step 1 until 10 do
      begin
        integer p;
        p := prime[j + 50 * (k - 1)];
        if p < 1000 then outchar(1, "0", 1);
        if p < 100 then outchar(1, "0", 1);
        if p < 10 then outchar(1, "0", 1);
        outinteger(1, p)
      end k;
      outstring(1, "\n")
    end j
end
