begin
    comment JAZ164, R743, Outer Planets;
    integer k, t;
    real a, k2, x;
    Boolean fi;
    array y, ya, z, za[1:15], m[0:5], e[1:60], d[1:33];

    real procedure SUM(k, l, u, ak);
        value l, u;
        integer k, l, u;
        real ak;
    begin
        real s;
        s := 0;
        for k := l step 1 until u do
            s := s + ak;
        SUM := s
    end;

    real procedure f(k); integer k; begin
        integer i, j, i3, j3;
        real p;
        own real array d[1:5, 1:5], r[1:5];

        if k != 1 then goto A;
        for i := 1 step 1 until 4 do begin
            i3 := 3*i;
            for j := i+1 step 1 until 5 do begin
                j3 := 3*j;
                p := (y[i3-2] - y[j3-2])**2 + (y[i3-1] - y[j3-1])**2 + (y[i3] - y[j3])**2;
                d[i, j] := d[j, i] := 1/p/sqrt(p)
            end
        end;
        for i := 1 step 1 until 5 do begin
            i3 := 3*i;
            d[i, i] := 0;
            p := y[i3-2]**2 + y[i3-1]**2 + y[i3]**2;
            r[i] := 1/p/sqrt(p)
        end;
A:      i := (k - 1) % 3 + 1;
        f := k2 * (- m[0] * y[k] * r[i] +
             SUM(j, 1, 5, m[j]*((y[3*(j-i)+k]-y[k])*d[i, j]-y[3*(j-i)+k]*r[j])))
    end f;

    procedure RK3n(x, a, b, y, ya, z, za, fxyj, j, e, d, fi, n);
        value b, fi, n;
        integer j, n;
        real x, a, b, fxyj;
        Boolean fi;
        array y, ya, z, za, e, d;
    begin
        integer jj;
        real xl, h, hmin, int, hl, absh, fhm, discry, discrz, toly, tolz, mu, mu1, fhy, fhz;
        Boolean last, first, reject;
        array yl, zl, k0, k1, k2, k3, k4, k5[1:n], ee[1:4*n];

        if fi then begin
            d[3] := a;
            for jj := 1 step 1 until n do begin
                d[jj+3] := ya[jj];
                d[n+jj+3] := za[jj]
            end
        end;
        d[1] := 0;
        xl := d[3];
        for jj := 1 step 1 until n do begin
            yl[jj] := d[jj+3];
            zl[jj] := d[n+jj+3]
        end;
        if fi then
            d[2] := b - d[3];
        absh := h := abs(d[2]);
        if b - xl < 0 then
            h := - h;
        int := abs(b - xl);
        hmin := int * e[1] + e[2];
        for jj := 2 step 1 until 2*n do begin
            hl := int * e[2*jj-1] + e[2*jj];
            if hl < hmin then
                hmin := hl
        end;
        for jj := 1 step 1 until 4*n do
            ee[jj] := e[jj]/int;
        first := reject := true;
        if fi then begin
            last := true;
            goto nstep
        end;
test:   absh := abs(h);
        if absh < hmin then begin
            h := if h > 0 then hmin else - hmin;
            absh := hmin
        end;
        if h >= b - xl == h >= 0 then begin
            d[2] := h;
            last := true;
            h := b - xl;
            absh := abs(h)
        end else
            last := false;
nstep:  if reject then begin
            x := xl;
            for jj := 1 step 1 until n do
                y[jj] := yl[jj];
            for j := 1 step 1 until n do
                k0[j] := fxyj * h
        end else begin
            fhy := h/hl;
            for jj := 1 step 1 until n do
                k0[jj] := k5[jj] * fhy
        end;
        x := xl + .27639 32022 50021 * h;
        for jj := 1 step 1 until n do
            y[jj] := yl[jj] + (zl[jj] * .27639 32022 50021 +
                     k0[jj] * .03819 66011 25011) * h;
        for j := 1 step 1 until n do
            k1[j] := fxyj * h;
        x := xl + .72360 67977 49979 * h;
        for jj := 1 step 1 until n do
            y[jj] := yl[jj] + (zl[jj] * .72360 67977 49979 +
                     k1[jj] * .26180 33988 74989) * h;
        for j := 1 step 1 until n do
            k2[j] := fxyj * h;
        x := xl + h * .5;
        for jj := 1 step 1 until n do
            y[jj] := yl[jj] + (zl[jj] * .5 +
                     k0[jj] * .04687 5 +
                     k1[jj] * .07982 41558 39840 -
                     k2[jj] * .00169 91558 39840) * h;
        for j := 1 step 1 until n do
            k4[j] := fxyj * h;
        x := if last then b else xl + h;
        for jj := 1 step 1 until n do
            y[jj] := yl[jj] + (zl[jj] +
                     k0[jj] * .30901 69943 74947 +
                     k2[jj] * .19098 30056 25053) * h;
        for j := 1 step 1 until n do
            k3[j] := fxyj * h;
        for jj := 1 step 1 until n do
            y[jj] := yl[jj] + (zl[jj] +
                     k0[jj] * .08333 33333 33333 +
                     k1[jj] * .30150 28323 95825 +
                     k2[jj] * .11516 38342 70842) * h;
        for j := 1 step 1 until n do
            k5[j] := fxyj * h;
        reject := false;
        fhm := 0;
        for jj := 1 step 1 until n do begin
            discry := abs((- k0[jj] * .5 + k1[jj] * 1.80901 69943 74947 +
                          k2[jj] * .69098 30056 25053 - k4[jj] * 2) * h);
            discrz := abs((k0[jj] - k3[jj]) * 2 - (k1[jj] + k2[jj]) * 10 +
                          k4[jj] * 16 + k5[jj] * 4);
            toly := absh * (abs(zl[jj]) * ee[2*jj-1] + ee[2*jj]);
            tolz := abs(k0[jj]) * ee[2*(jj+n)-1] + absh * ee[2*(jj+n)];
            reject := discry > toly | discrz > tolz | reject;
            fhy := discry/toly;
            fhz := discrz/tolz;
            if fhz > fhy then
                fhy := fhz;
            if fhy > fhm then
                fhm := fhy
        end;
        mu := 1/(1 + fhm) + .45;
        if reject then begin
            if absh <= hmin then begin
                d[1] := d[1] + 1;
                for jj := 1 step 1 until n do begin
                    y[jj] := yl[jj];
                    z[jj] := zl[jj]
                end;
                first := true;
                goto next
            end;
            h := mu * h;
            goto test
        end rej;
        if first then begin
            first := false;
            hl := h;
            h := mu * h;
            goto acc
        end;
        fhy := mu * h/hl + mu - mu1;
        hl := h;
        h := fhy * h;
acc:    mu1 := mu;
        for jj := 1 step 1 until n do
            z[jj] := zl[jj] + (k0[jj] + k3[jj]) * .08333 33333 33333 +
                     (k1[jj] + k2[jj]) * .41666 66666 66667;
next:   if b != x then begin
            xl := x;
            for jj := 1 step 1 until n do begin
                yl[jj] := y[jj];
                zl[jj] := z[jj]
            end;
            goto test
        end;
        if ! last then
            d[2] := h;
        d[3] := x;
        for jj := 1 step 1 until n do begin
            d[jj+3] := y[jj];
            d[n+jj+3] := z[jj]
        end
    end RK3n;

    procedure TYP(x);
        array x;
    begin
        integer k;
        outstring(1, "\nT = ");
        outreal(1, t+a);
        outstring(1, "\n\n");
        for k := 1 step 1 until 5 do begin
            if k=1 then outstring(1, "J ") else
            if k=2 then outstring(1, "S ") else
            if k=3 then outstring(1, "U ") else
            if k=4 then outstring(1, "N ") else
                outstring(1, "P ");
            outreal(1, x[3*k-2]);
            outreal(1, x[3*k-1]);
            outreal(1, x[3*k]);
            outstring(1, "\n")
        end
    end TYP;

    inreal(0, a);
    for k := 1 step 1 until 15 do begin
        inreal(0, ya[k]);
        inreal(0, za[k])
    end;
    for k := 0 step 1 until 5 do
        inreal(0, m[k]);
    inreal(0, k2);
    inreal(0, e[1]);

    for k := 2 step 1 until 60 do
        e[k] := e[1];
    outstring(1, "JAZ164, R743, Outer Planets\n\n");
    for k := 1 step 1 until 15 do begin
        outreal(1, ya[k]);
        outreal(1, za[k]);
        outstring(1, "\n")
    end;
    outstring(1, "\n");
    for k := 0 step 1 until 5 do begin
        outreal(1, m[k]);
        outstring(1, "\n")
    end;
    outstring(1, "\n");
    outreal(1, k2);
    outstring(1, "\n\neps = ");
    outreal(1, e[1]);
    outstring(1, "\n");
    t := 0;
    TYP(ya);
    fi := true;
    for t := 500, 1000 do begin
        RK3n(x, 0, t, y, ya, z, za, f(k), k, e, d, fi, 15);
        fi := false;
        TYP(y)
    end
end
