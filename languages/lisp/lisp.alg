begin
    comment ALGOL 60 version of program lisp(input,output).
    *** version 1, March 28, l988 ***
    *** author: F.E.J. Kruseman Aretz ***
    *** Philips Research Laboratory Eindhoven ***;
    integer maxidf, maxnbr, maxstruct;
    maxidf := 200;
    maxnbr := 200;
    maxstruct := 2000;
    begin
        integer sym, shift, lastidf, lastnbr, d24, d25, free, indentation, linewidth, dummy,
                f, args, p, id, olp, t, nilv, quote, cond, lambda, define, car, cdr, cons,
                equal, atom, numberp, lessp, greaterp, add1, sub1, add, minus, timesv, divf;
        integer array idf[0:maxidf, 0:9], alist[0:maxidf];
        real array nbr[0:maxnbr];
        integer array a, d[1:maxstruct];
        Boolean array m[1:maxstruct];

        integer procedure RESYM; begin
            integer s;
            inchar(0, "0123456789abcdefghijklmnopqrstuvwxyz#ABCDEFGHIJKLMNOPQRSTUVWXYZ#+-*/$$>$=$<$$$$$$######,.@:;$ ####()[]`'##############\t\n#$?#####", s);
            if s = 0 then
                s := 256;
            RESYM := s - 1
        end;
        procedure PRSYM(s); value s; integer s; begin
            if s = 93 then
                outstring(1, " ")
            else if s = 119 then
                outstring(1, "\n")
            else if s = 98 then
                outstring(1, "(")
            else if s = 99 then
                outstring(1, ")")
            else if s >= 0 & s <= 9 then
                outchar(1, "0123456789", s + 1)
            else if s >= 10 & s <= 35 then
                outchar(1, "abcdefghijklmnopqrstuvwxyz", s - 9)
            else if s >= 37 & s <= 62 then
                outchar(1, "ABCDEFGHIJKLMNOPQRSTUVWXYZ", s - 36)
            else begin
                outstring(1, "bad PRSYM ");
                outinteger(1, s);
                outstring(1, "\n")
            end
        end;
        procedure SPACE(n); value n; integer n; begin
            integer i;
            for i := 1 step 1 until n do
                outchar(1, " ", 1)
        end;

        comment *** error handling ***;

        procedure errorhandler(errorstring); string errorstring; begin
            outstring(1, "\n\n+++ error: ");
            outstring(1, errorstring);
            outstring(1, "\n");
            goto ex
        end errorhandler;

        comment *** representation dependent functions ***;

        procedure collect garbage; begin
            integer i, j;
            free := 0;
            for i := 1 step 1 until maxstruct do
                m[i] := true;
            outstring(1, "\ngarbage collector: ");
            mark(olp);
            for i := 0 step 1 until lastidf - 1 do
                mark(alist[i]);
            for i := 1 step 1 until maxstruct do
                if m[i] then begin
                    a[i] := free;
                    free := i
                end;
            if free = 0 then
                errorhandler("free list exhausted");
            i := 1;
            j := free;
            for j := carf(j) while j != 0 do
                i := i + 1;
            outinteger(1, i);
            outstring(1, "\n")
        end collect garbage;

        procedure mark(ref); value ref; integer ref; begin
work:       if ref < d24 then begin
                if m[ref] then begin
                    m[ref] := false;
                    mark(a[ref]);
                    ref := d[ref];
                    goto work
                end
            end
        end mark;

        integer procedure createidf; begin
            integer i, j;
            i := 0;
            for dummy := 0 while i < lastidf do begin
                for j := 0 step 1 until 9 do
                    if idf[lastidf, j] != idf[i, j] then
                        goto diff;
                goto old;
diff:           i := i + 1
            end;
new:        i := lastidf;
            alist[i] := nilv;
            lastidf := lastidf + 1;
            if lastidf = maxidf then begin
                for i := 0 step 1 until 99 do begin
                    outstring(1, "\n");
                    write(d25+i)
                end;
                errorhandler("too much identifiers")
            end;
old:        createidf := d25 + i
        end createidf;

        integer procedure createnum(x); real x; begin
            integer i;
            nbr[last nbr] := x;
            i := 0;
            for dummy := 0 while i < last nbr do begin
                if nbr[last nbr] = nbr[i] then
                    goto old;
                i := i + 1
            end;
new:        i := last nbr;
            last nbr := last nbr + 1;
            if last nbr = maxnbr then
                errorhandler("too much numbers");
old:        createnum := d24 + i
        end createnum;

        Boolean procedure atomf(x); value x; integer x; begin
            atomf := x >= d24
        end atomf;

        Boolean procedure numberpf(x); value x; integer x; begin
            numberpf := x > d24 & x < d25
        end numberpf;

        procedure getidfval(x, idf); value x; integer x, idf; begin
            idf := x - d25
        end getidfval;

        real procedure numval(x); value x; integer x; begin
            numval := nbr[x - d24]
        end numval;

        integer procedure carf(x); value x; integer x; begin
            if x > d24 then
                errorhandler("car undefined for atomic lisp value");
            carf := a[x]
        end carf;

        integer procedure cdrf(x); value x; integer x; begin
             if x > d24 then
                errorhandler("cdr undefined for atomic lisp value");
            cdrf := d[x]
        end cdrf;

        integer procedure consf(x, y); value x, y; integer x, y; begin
            integer n;
            if free = 0 then
                collect garbage;
            n := free;
            free := a[free];
            a[n] := x;
            d[n] := y;
            consf := n
        end consf;

        procedure returncell(x); value x; integer x; begin
            a[x] := free;
            free := x
        end;

        procedure returnlist(x); value x; integer x; begin
            for dummy := 0 while x != nilv do begin
                returncell(x);
                x := d[x]
            end
        end returnlist;

        procedure recycle(x); value x; integer x; begin
            for dummy := 0 while ! atomf(x) do begin
                recycle(a[x]);
                returncell(x);
                x := d[x]
            end
        end recycle;

        Boolean procedure equalf(x, y); value x, y; integer x, y; begin
            switch s := str, num, id;
work:
            if x % d24 = y % d24 then begin
                goto s[x % d24 + 1];
id: num:        equalf := x = y;
                goto ex;
str:            if equalf(a[x], a[y]) then begin
                    x := d[x];
                    y := d[y];
                    goto work
                end else
                    equalf := false
            end else
                equalf := false;
ex:
        end equalf;

        comment *** input procedures ***;

        procedure nextsym; begin
            sym := RESYM;
            if sym = 255 then
                errorhandler("eof");
            PRSYM(sym)
        end nextsym;

        procedure skipspaces; begin
            for dummy := 0 while sym = 93 | sym = 118 | sym = 119 do
                nextsym
        end skipspaces;

        integer procedure number; begin
            real x;
            Boolean signed;
            x := 0;
            signed := (sym = 65);
            if signed then begin
                nextsym;
                if sym > 9 then
                    errorhandler("digit expected in input")
            end;
            for dummy := 0 while sym < 10 do begin
                x := 10 * x + sym;
                nextsym
            end;
            number := createnum(if signed then -x else x)
        end number;

        integer procedure identifier; begin
            integer i;
            idf[lastidf, 0] := sym;
            nextsym;
            for i := 1 step 1 until 9 do
                idf[lastidf, i] := 93;
            i := 0;
            for dummy := 0 while sym < 64 & i < 9 do begin
                i := i + 1;
                idf[lastidf, i] := sym;
                nextsym
            end;
            for dummy := 0 while sym < 64 do
                nextsym;
            identifier := createidf
        end identifier;

        integer procedure nextitem; begin
            integer lv, op;
            skipspaces;
            if sym < 10 | sym = 65 then
                nextitem := number
            else if sym < 64 then
                nextitem := identifier
            else if sym = 98 then begin
                nextsym;
                skipspaces;
                if sym = 99 then begin
                    nextitem := nilv;
                    nextsym
                end else begin
                    op := olp;
                    olp := consf(nilv, op);
                    lv := a[olp] := consf(nilv, nilv);
                    nextitem := lv;
                    a[lv] := nextitem;
                    skipspaces;
                    if sym = 88 then begin
                        nextsym;
                        d[lv] := nextitem;
                        skipspaces;
                        if sym != 99 then
                            errorhandler("close missing for dotted pair in input")
                    end else
                        for dummy := 0 while sym != 99 do begin
                            lv := d[lv] := consf(nilv, nilv);
                            a[lv] := nextitem;
                            skipspaces
                        end;
                    nextsym;
                    olp := op
                end
            end else if sym = 103 then begin
                nextsym;
                op := olp;
                olp := consf(nilv, olp);
                lv := a[olp] := consf(nilv, nilv);
                nextitem := lv;
                a[lv] := quote;
                lv := d[lv] := consf(nilv, nilv);
                a[lv] := nextitem;
                olp := op
            end else
                errorhandler("illegal symbol in input")
        end nextitem;

        comment *** output procedures ***;

        procedure analyse(x, r); value x; integer x, r; begin
            integer n, l;
            Boolean simple;
            if numberpf(x) then begin
                real dg, v, absv;
                v := numval(x);
                if v > 0 then begin
                    absv := v;
                    l := 1
                end else begin
                    absv := - v;
                    l := 2
                end;
                dg := 10;
                for dummy := 0 while dg < absv do begin
                    l := l + 1;
                    dg := 10 * dg
                end;
                r := createnum(l)
            end else if atomf(x) then begin
                getidfval(x, id);
                n := 10;
                for dummy := 0 while idf[id, n-1] = 93 do
                    n := n - 1;
                r := createnum(n)
            end else if islist(x) then begin
                indentation := indentation + 1;
                analyselist(x, r, l, simple);
                indentation := indentation - 1;
                if simple & indentation + l < linewidth then begin
                    recycle(r);
                    r := createnum(l)
                end
            end else begin
                indentation := indentation + 1;
                olp := consf(nilv, olp);
                r := a[olp] := consf(nilv, nilv);
                analyse(carf(x), a[r]);
                analyse(cdrf(x), d[r]);
                indentation := indentation - 1;
                if atomf(a[r]) & atomf(d[r]) then begin
                    l := numval(carf(r)) + numval(cdrf(r)) + 5;
                    if indentation + l < linewidth then begin
                        recycle(r);
                        r := createnum(l)
                    end
                end;
                returncell(olp);
                olp := d[olp]
            end
        end analyse;

        procedure analyselist(x, r, l, simple);
            value x;
            integer x, r, l;
            Boolean simple;
        begin
            if x = nilv then begin
                r := nilv;
                l := 1;
                simple := true
            end else begin
                olp := consf(nilv, olp);
                r := a[olp] := consf(nilv, nilv);
                analyse(carf(x), a[r]);
                analyselist(cdrf(x), d[r], l, simple);
                if simple & atomf(a[r]) then
                    l := numval(a[r]) + l + 1
                else
                    simple := false;
                returncell(olp);
                olp := d[olp]
            end
        end analyselist;

        Boolean procedure islist(x); value x; integer x; begin
work:       if atomf(x) then
                islist := equalf(x, nilv)
            else begin
                x := cdrf(x);
                goto work
            end
        end islist;

        procedure writenumber(x); value x; integer x; begin
            integer n, d, v;
            v := numval(x);
            if v < 0 then
                v := - v;
            d := 10;
            for dummy := 0 while d < v do
                d := d * 10;
            for d := d % 10 while d > 0.5 do begin
                n := v % d;
                PRSYM(n);
                v := v - d * n
            end
        end writenumber;

        procedure writeidentifier(x); value x; integer x; begin
            integer i;
            getidfval(x, id);
            for i := 0 step 1 until 9 do
                if idf[id, i] != 93 then
                    PRSYM(idf[id, i])
        end writeidentifier;

        procedure writelist(x, r); value x, r; integer x, r; begin
            integer a, ind;
            Boolean simple, nl;
            PRSYM(98);
            if atomf(r) then begin
                for dummy := 0 while x != nilv do begin
                    writevalue(carf(x), r);
                    x := cdrf(x);
                    if x != nilv then
                        PRSYM(93)
                end
            end else begin
                indentation := indentation + 1;
                ind := indentation;
                for dummy := 0 while x != nilv do begin
                    a := carf(r);
                    simple := atomf(a);
                    if simple then
                        nl := numval(a) + indentation > linewidth
                    else
                        nl := indentation > ind;
                    if nl then begin
                        indentation := ind;
                        outstring(1, "\n");
                        SPACE(ind)
                    end else if indentation > ind then
                        PRSYM(93);
                    writevalue(carf(x), a);
                    if simple then
                        indentation := indentation + numval(a) + 1
                    else
                        indentation := linewidth + 1;
                    x := cdrf(x);
                    r := cdrf(r)
                end;
                indentation := ind - 1;
                outstring(1, "\n");
                SPACE(indentation)
            end;
            PRSYM(99)
        end writelist;

        procedure writepair(x, r); value x, r; integer x, r; begin
            PRSYM(98);
            if atomf(r) then begin
                writevalue(carf(x), r);
                outstring(1, " . ");
                writevalue(cdrf(x), r)
            end else begin
                indentation := indentation + 1;
                writevalue(carf(x), carf(r));
                outstring(1, "\n");
                SPACE(indentation-1);
                outstring(1, " . \n");
                SPACE(indentation);
                writevalue(cdrf(x), cdrf(r));
                outstring(1, "\n");
                SPACE(indentation)
            end;
            PRSYM(99)
        end writepair;

        procedure writevalue(x, r); value x, r; integer x, r; begin
            if numberpf(x) then
                writenumber(x)
            else if atomf(x) then
                writeidentifier(x)
            else if islist(x) then
                 writelist(x, r)
            else
                writepair(x, r)
        end writevalue;

        procedure write(x); value x; integer x; begin
            integer r;
            indentation := 0;
            analyse(x, r);
            writevalue(x, r);
            recycle(r)
        end write;

        comment *** interpreter proper ***;

        integer procedure assoc(x); value x; integer x; begin
            integer ax;
            getidfval(x, id);
            ax := alist[id];
            if ax = nilv then
                errorhandler("identifier has no value");
            assoc := carf(ax)
        end assoc;

        procedure pairlis(x, y); value x, y; integer x, y; begin
            for dummy := 0 while ! equalf(x, nilv) do begin
                getidfval(carf(x), id);
                alist[id] := consf(carf(y), alist[id]);
                x := cdrf(x);
                y := cdrf(y)
            end
        end pairlis;

        procedure depairlis(x); value x; integer x; begin
            for dummy := 0 while ! equalf(x, nilv) do begin
                getidfval(carf(x), id);
                alist[id] := cdrf(alist[id]);
                x := cdrf(x)
            end
        end depairlis;

        integer procedure eval(e); value e; integer e; begin
            integer care;
work:       if atomf(e) then begin
                if equalf(e, nilv) | equalf(e, t) | numberpf(e) then
                    eval := e
                else
                    eval := assoc(e)
            end else begin
                care := carf(e);
                if equalf(care, cond) then begin
                    e := evcon(cdrf(e));
                    goto work
                end else if equalf(care, quote) then
                    eval := carf(cdrf(e))
                else begin
                    olp := consf(nilv, olp);
                    a[olp] := evlist(cdrf(e));
                    eval := apply(care, a[olp]);
                    returnlist(a[olp]);
                    returncell(olp);
                    olp := cdrf(olp)
                end
            end
        end eval;

        integer procedure apply(f, x); value f, x; integer f, x; begin
work:       if atomf(f) then begin
                if equalf(f, car) then
                    apply := carf(carf(x))
                else if equalf(f, cdr) then
                    apply := cdrf(carf(x))
                else if equalf(f, cons) then
                    apply := consf(carf(x), carf(cdrf(x)))
                else if equalf(f, equal) then begin
                    if equalf(carf(x), carf(cdrf(x))) then
                        apply := t
                    else
                        apply := nilv
                end else if equalf(f, atom) then begin
                    if atomf(carf(x)) then
                        apply := t
                    else
                        apply := nilv
                end else if equalf(f, numberp) then begin
                    if numberpf(carf(x)) then
                        apply := t
                    else
                        apply := nilv
                end else if equalf(f, lessp) then begin
                    if numval(carf(x)) < numval(carf(cdrf(x))) then
                        apply := t
                    else
                        apply := nilv
                end else if equalf(f, greaterp) then begin
                    if numval(carf(x)) > numval(carf(cdrf(x))) then
                        apply := t
                    else
                        apply := nilv
                end else if equalf(f, add) then
                    apply := createnum(numval(carf(x)) + 1)
                else if equalf(f, sub1) then
                    apply := createnum(numval(carf(x)) - 1)
                else if equalf(f, add) then
                    apply := createnum(numval(carf(x)) + numval(carf(cdrf(x))))
                else if equalf(f, minus) then
                    apply := createnum(numval(carf(x)) - numval(carf(cdrf(x))))
                else if equalf(f, timesv) then
                    apply := createnum(numval(carf(x)) * numval(carf(cdrf(x))))
                else if equalf(f, divf) then
                    apply := entier(createnum(numval(carf(x)) / numval(carf(cdrf(x)))))
                else begin
                    f := assoc(f);
                    goto work
                end
            end else begin
                pairlis(carf(cdrf(f)), x);
                apply := eval(carf(cdrf(cdrf(f))));
                depairlis(carf(cdrf(f)))
            end
        end apply;

        integer procedure evcon(x); value x; integer x; begin
            integer r;
work:       r := carf(x);
            if equalf(eval(carf(r)), nilv) then begin
                x := cdrf(x);
                goto work
            end else
            evcon := carf(cdrf(r))
        end evcon;

        integer procedure evlist(x); value x; integer x; begin
            integer res;
            if equalf(x, nilv) then
                evlist := nilv
            else begin
                olp := consf(nilv, olp);
                a[olp] := res := consf(nilv, nilv);
                a[res] := eval(carf(x));
                d[res] := evlist(cdrf(x));
                evlist := res;
                returncell(olp);
                olp := cdrf(olp)
            end
        end evlist;

        comment *** initialization ***;

        procedure create(lv); integer lv; begin
            skipspaces;
            lv := identifier
        end create;

        procedure init; begin
            integer i, j;
            d24 := 16777216;
            d25 := 33554432;
            last idf := 0;
            sym := 93;
            nilv := d25 + 1;
            create(t);
            create(nilv);
            create(quote);
            create(cond);
            create(lambda);
            create(define);
            create(car);
            create(cdr);
            create(cons);
            create(equal);
            create(atom);
            create(numberp);
            create(lessp);
            create(greaterp);
            create(add1);
            create(sub1);
            create(add);
            create(minus);
            create(timesv);
            create(divf);
            olp := nilv;
            free := 1;
            last nbr := 0;
            linewidth := 40;
            for i := 1 step 1 until maxstruct - 1 do
                a[i] := i + 1;
            a[maxstruct] := 0
        end init;

        comment *** main program ***;

        procedure function definitions(x, a, r); value x; integer x, a, r; begin
            integer carx, lr;
            if equalf(x, nilv) then
                r := nilv
            else begin
                carx := carf(x);
                a := consf(consf(carf(carx), carf(cdrf(carx))), a);
                function definitions(cdrf(x), a, lr);
                r := consf(carf(carx), lr)
            end
        end function definitions;

        outstring(1, "Lisp interpreter version 1, Oktober 2004\n\n");
        init;
        for dummy := 0 while true do begin
            olp := consf(nilv, olp);
            a[olp] := p := consf(nilv, nilv);
            a[p] := f := nextitem;
            d[p] := args := nextitem;
            outstring(1, "\n");
            if equalf(f, define) then begin
                args := carf(args);
                PRSYM(98);
                for dummy := 0 while ! equalf(args, nilv) do begin
                    p := carf(args);
                    write(carf(p));
                    getidfval(carf(p), id);
                    alist[id] := consf(carf(cdrf(p)), nilv);
                    args := cdrf(args);
                    if ! equalf(args, nilv) then
                        SPACE(1)
                end;
                PRSYM(99)
            end else begin
                p := apply(f, args);
                outstring(1, "\n");
                write(p)
            end;
            olp := cdrf(olp)
        end;
ex:
    end
end
