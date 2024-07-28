begin
comment         ALGOL 60 - version of the ALGOL 60 - translator
                for the EL - X8, F.E.J. Kruseman Aretz;

comment         basic symbols;
integer plus, minus, mul, div, idi, ttp, equ, uqu, les, mst, mor, lst,
        non, qvl, imp, or, and, gotox, forx, stepx, untilx, whilex, dox,
        comma, period, ten, colon, semicolon, colonequal, space sbl,
        ifx, thenx, elsex, commentx, open, close, sub, bus, quote, unquote,
        beginx, endx, ownx, rea, integ, boole, stri, arrayx, proced, switchx,
        labelx, valuex, truex, falsex, new line, underlining, bar;

comment         other global integers;
integer case, lower case, stock, stock1, last symbol, line counter,
        last identifier, last identifierl,
        quote counter, run number, shift,
        type, chara, character, valuex character, arr decla macro,
        valuex of constant, decimal exponent, decimal count,
        word count, nlp, last nlp, n, integerx labelx,
        block cell pointer, next block cell pointer,
        dimension, forx count, instruct counter, dp0,
        function letter, function digit, c variant,
        nl base, prog base, text base, text pointer,
        endx of text, endx of memory, start, endx of list,
        d0, d1, d2, d3, d4, d5, d6, d7, d8, d9, d10, d11, d12, d13, d14,
        d15, d16, d17, d18, d19, d20, d21, d22, d23, d24, d25,
        re, in, bo, st, ar, nondes, des, un, arbo, intlab;

comment         macro identifiers;
integer STACK, NEG, ADD, SUB, MUL, DIV, IDI, TTP,
        EQU, UQU, LES, MST, MOR, LST, STAB, NON, QVL, IMP, OR, AND,
        STAA, TSR, TSI, TSB, TSST, TFSU, TSL, TFSL, TCST,
        STSR, STSI, SSTSI, STSB, STSST, STFSU,
        ENTRIS, TFD, SAS, DECS, FAD, TASR, TASI, TASB, TASST, TASU,
        EXITIS, FADCV, TRSCV, TISCV, TSCVU, EXIT, TEST1, TEST2,
        CRV, CIV, CBV, CSTV, CLV, CEN, CLPN, TAV, TIAV,
        RAD, IAD, BAD, STAD, ORAD, OIAD, OBAD, OSTAD,
        LOS, EXITP, EXITPC, REJST, JUA, EMPTY,
        ABS, SIGN, ENTIER, SQRT, EXP, LN, ENDx;

comment         macro2 identifiers;
integer TRV, TIV, TRC, TIC, TSIC, TBV, TBC, TSTV, TLV, TAK, TSWE,
        STR, STI, SSTI, STB, STST, DOS, DOS2, DOS3,
        JU, JU1, LJU, LJU1, COJU, YCOJU, SUBJ, ISUBJ, DECB, DOx,
        TBL, ENTRB, DPTR, INCRB, TDL, ENTRPB, NIL, LAST,
        LAD, TDA, TNA, TAA, SWP, EXITB, EXITC, EXITSV,
        CODEx, SLNC, RLNC, LNC;

comment         global Booleans;
Boolean letter last symbol, digit last symbol, arr declarator last symbol,
        type declarator last symbol, in arrayx declaration, in formal list,
        text in memory, ownx type, int labels, realx number, small,
        erroneous, derroneous, wanted;





comment         global arrays;
integer  array internal representation[0 : 127], word delimiter[0 : 23],
               macro list[0 : 511], tabel[5 : 59],
               instruct list[0 : 203], mask[0 : 9];

comment         start of initialization;
        plus:= read;    minus:= read;   mul:= read;     div:= read;
        idi:= read;     ttp:= read;     equ:= read;     uqu:= read;
        les:= read;     mst:= read;     mor:= read;     lst:= read;
        non:= read;     qvl:= read;     imp:= read;     or:= read;
        and:= read;     gotox:= read;    forx:= read;     stepx:= read;
        untilx:= read;   whilex:= read;   dox:= read;      comma:= read;
        period:= read;  ten:= read;     colon:= read;   semicolon:= read;
        colonequal:= read; space sbl:= read; ifx:= read; thenx:= read;
        elsex:= read;    commentx:= read; open:= read;    close:= read;
        sub:= read;     bus:= read;     quote:= read;   unquote:= read;
        beginx:= read;   endx:= read;     ownx:= read;     rea:= read;
        integ:= read;   boole:= read;   stri:= read;    arrayx:= read;
        proced:= read;  switchx:= read;  labelx:= read;   valuex:= read;
        truex:= read;    falsex:= read;   new line:= read;
        underlining:= read; bar:= read; lower case:= read;

        STACK:= read;   NEG:= read;     ADD:= read;     SUB:= read;
        MUL:= read;     DIV:= read;     IDI:= read;     TTP:= read;
        EQU:= read;     UQU:= read;     LES:= read;     MST:= read;
        MOR:= read;     LST:= read;     STAB:= read;    NON:= read;
        QVL:= read;     IMP:= read;     OR:= read;      AND:= read;
        STAA:= read;    TSR:= read;     TSI:= read;     TSB:= read;
        TSST:= read;    TFSU:= read;    TSL:= read;     TFSL:= read;
        TCST:= read;    STSR:= read;    STSI:= read;    SSTSI:= read;
        STSB:= read;    STSST:= read;   STFSU:= read;   ENTRIS:= read;
        TFD:= read;     SAS:= read;     DECS:= read;    FAD:= read;
        TASR:= read;    TASI:= read;    TASB:= read;    TASST:= read;
        TASU:= read;    EXITIS:= read;  FADCV:= read;   TRSCV:= read;
        TISCV:= read;   TSCVU:= read;   EXIT:= read;    TEST1:= read;
        TEST2:= read;   CRV:= read;     CIV:= read;     CBV:= read;
        CSTV:= read;    CLV:= read;     CEN:= read;     CLPN:= read;
        TAV:= read;     TIAV:= read;    RAD:= read;     IAD:= read;
        BAD:= read;     STAD:= read;    ORAD:= read;    OIAD:= read;
        OBAD:= read;    OSTAD:= read;   LOS:= read;     EXITP:= read;
        EXITPC:= read;  REJST:= read;   JUA:= read;     EMPTY:= read;
        ABS:= read;     SIGN:= read;    ENTIER:= read;  SQRT:= read;
        EXP:= read;     LN:= read;      ENDx:= read;





        TRV:= read;     TIV:= read;     TRC:= read;      TIC:= read;
        TSIC:= read;    TBV:= read;     TBC:= read;      TSTV:= read;
        TLV:= read;     TAK:= read;     TSWE:= read;     STR:= read;
        STI:= read;     SSTI:= read;    STB:= read;      STST:= read;
        DOS:= read;     DOS2:= read;    DOS3:= read;     JU:= read;
        JU1:= read;     LJU:= read;     LJU1:= read;     COJU:= read;
        YCOJU:= read;   SUBJ:= read;    ISUBJ:= read;    DECB:= read;
        DOx:= read;      TBL:= read;     ENTRB:= read;    DPTR:= read;
        INCRB:= read;   TDL:= read;     ENTRPB:= read;   NIL:= read;
        LAST:= read;    LAD:= read;     TDA:= read;      TNA:= read;
        TAA:= read;     SWP:= read;     EXITB:= read;    EXITC:= read;
        EXITSV:= read;  CODEx:= read;    SLNC:= read;     RLNC:= read;
        LNC:= read;

        d0 := 1;        d1 := 2;        d2 := 4;         d3 := 8;
        d4 := 16;       d5 := 32;       d6 := 64;        d7 := 128;
        d8 := 256;      d9 := 512;      d10:= 1024;      d11:= 2048;
        d12:= 4096;     d13:= 8192;     d14:= 16384;     d15:= 32768;
        d16:= 65536;    d17:= 131072;   d18:= 262144;    d19:= 524288;
        d20:= 1048576;  d21:= 2097152;  d22:= 4194304;   d23:= 8388608;
        d24:= 16777216; d25:= 33554432;

        re:= 0;         in:= 1;         bo:= 2;          st:= 3;
        ar:= 4;         nondes:= 5;     des:= 6;         un:= 7;
        arbo:= 8;       intlab:= 9;

        function letter:= read; function digit:= read; c variant:= read;

        for n:= 0 step 1 until 127 do internal representation[n]:= read;
        for n:= 0 step 1 until  23 do word delimiter[n]:= read;
        for n:= 0 step 1 until 511 do macro list[n]:= read;
        for n:= 5 step 1 until  59 do tabel[n]:= read;
        for n:= 0 step 1 until 203 do instruct list[n]:= read;
        for n:= 0 step 1 until   9 do mask[n]:= d20  * read;

        endx of memory:= read;
        endx of list:= instruct list[174];
        text in memory:= truex; erroneous:= derroneous:= falsex;
        wanted:= read = 0;


begin   integer array space[0 : endx of memory];

procedure ERRORMESSAGE (n); integer n;
begin   integer i;
        erroneous:= truex;
        if n = 122  |  n = 123  |  n = 126  |  n = 127  |  n = 129
        then derroneous:= truex;
        if n  >=  run number
        then begin NLCR; PRINTTEXT ("er"); print (n);
                   print (line counter); print (last symbol);
                   for i:= 0 step 1 until word count do
                   print (space[nl base - last nlp - i])
             end
end ERRORMESSAGE;





integer procedure next symbol;
begin   integer symbol;
next0:  symbol:= if stock1  >=  0 then stock1 else next basic symbol;
        stock1 := -1;
        if (last symbol = semicolon  |  last symbol = beginx)  &
           symbol = commentx
        then begin skip0: symbol:= next basic symbol;
                          if symbol  !=  semicolon then goto skip0;
                          goto next0
             end;
        if last symbol = endx
        then begin
             skip1: if symbol  !=  endx  &  symbol  !=  semicolon  &  symbol  !=  elsex
                    then begin symbol:= next basic symbol; goto skip1 end
             end
        else
        if symbol = 125
        then begin stock1:= next basic symbol;
                   if stock1 > 9  &  stock1 < 64
                   then begin skip2: stock1:= next basic symbol;
                              if stock1 > 9  &  stock1 < 64
                              then goto skip2;
                              if stock1 = colon
                              then stock1:= next basic symbol
                              else ERRORMESSAGE (100);
                              if stock1 = open  then stock1:= - stock1
                                                else ERRORMESSAGE (101);
                              symbol:= comma
                        end
                   else symbol:= close
             end;
        digit last symbol := symbol < 10  |  symbol = period  |
                             symbol = ten;
        letter last symbol:= symbol < 64  &   !  digit last symbol;
        next symbol:= last symbol:= symbol;
        outsymbol (run number, symbol);
        test pointers
end  next symbol;


integer procedure  next basic symbol;
begin   integer  symbol;
next0:  insymbol (run number, symbol);
        if symbol = new line
        then begin line counter:= line counter + 1;
                   if quote counter = 0
                   then begin outsymbol (run number, symbol);
                              goto next0
                        end
             end;
        next basic symbol:= symbol
end  next basic symbol;







procedure  insymbol (source, destination); integer  source, destination;
begin   integer  symbol, i;
        if (source = 200  |  source = 300)  &  text in memory
        then
        begin destination:= bit stringx(d8  * shift, shift,
                            space[text base + text pointer]);
              if shift < 257
              then shift:= d8  * shift
              else begin shift:= 1; text pointer:= text pointer + 1 end
        end
        else
        begin symbol:= if stock > 0 then stock else next tape symbol;
              stock:= - 1;
              if symbol > bus
              then
              begin if symbol = 123 then symbol:= space sbl;
                    if quote counter > 0
                    then
                    begin if symbol = bar
                          then
                          begin next0: stock:= next tape symbol;
                                       if stock = bar then goto next0;
                                if stock = les
                                then quote counter:= quote counter + 1
                                else
                                if stock = mor
                                then
                                begin if quote counter = 1
                                      then begin symbol:= unquote;
                                                 stock:= - symbol
                                           end
                                      else quote counter:=
                                           quote counter - 1
                                end
                          end
                          else if symbol = 124
                               then symbol:= colon
                               else if symbol = 125 then symbol:= close
                    end
                    else
                    if symbol  >=  newline
                    then
                    begin if symbol = bar
                          then
                          begin next1: symbol:= next tape symbol;
                                       if symbol = bar then goto next1;
                              symbol:= if symbol = and then ttp else
                                       if symbol = equ then uqu else
                                       if symbol = les then quote else
                                       if symbol = mor then unquote
                                                       else 160
                          end
                          else
                          if symbol = underlining
                          then
                          begin symbol:= the underlined symbol;
                                if symbol > 63
                                then symbol:=
                                     if symbol = 124 then idi else
                                     if symbol = les then mst else
                                     if symbol = mor then lst else
                                     if symbol = non then imp else
                                     if symbol = equ then qvl
                                                     else 161
                                     else
                                     begin stock:= next tape symbol;
                                           if stock = underlining
                                           then
                                           begin
                                            symbol:= the underlined symbol +
                                                     d7  * symbol;
                                            for i:= 0 step 1 until 23 do
                                            begin
                                            if word delimiter[i]  %  d7 = symbol
                                            then
                                             begin
                                             symbol:= word delimiter[i];
                                             symbol:= symbol -
                                                      symbol  %  d7  * d7;
                                             goto next2
                                             end
                                            end;
                                            symbol:= 162;
                                     next2: stock:= next tape symbol;
                                            if stock = underlining
                                            then
                                            begin the underlined symbol;
                                                  goto next2
                                            end
                                           end
                                           else symbol:= 161
                                     end
                                end
                                else
                                if symbol = 124
                                then begin stock:= next tape symbol;
                                           if stock = equ
                                           then begin symbol:= colonequal;
                                                      stock:= - symbol
                                                end
                                           else symbol:= colon
                                     end
                          end
                          else insymbol (runnumber, symbol)
                    end;
                    destination:= symbol
        end
end  insymbol;





integer procedure  the underlined symbol;
begin   integer  symbol;
        symbol:= next tape symbol;
        the underlined symbol:= if symbol = underlining
                                then the underlined symbol
                                else symbol
end  the underlined symbol;

integer procedure  next tape symbol;
begin   integer  symbol, head;
        symbol:= internal representation[REHEP];
        if symbol > 0
        then begin head:= symbol  %  d8;
                   next tape symbol:= abs (if case = lower case
                                           then symbol - d8  * head
                                           else head)
             end
        else begin if symbol < - 2 then case:= - symbol else
                   if symbol =   0 then ERRORMESSAGE (102) else
                   if symbol = - 1 then ERRORMESSAGE (103);
                   next tape symbol:= next tape symbol
             end
end  next tape symbol;

procedure  outsymbol (destination, source); integer destination, source;
begin   if destination = 100  &  text in memory
        then begin space[text base + text pointer]:=
                   space[text base + text pointer] + shift  * source;
                   if shift < 257
                   then shift:= d8  * shift
                   else begin shift:= 1; text pointer:= text pointer + 1;
                              space[text base + text pointer]:= 0
                        end
             end
end  outsymbol;


Boolean procedure  arithoperator last symbol;
begin   arithoperator last symbol:= last symbol = plus   |
                                    last symbol = minus  |
                                    last symbol = mul    |
                                    last symbol = div    |
                                    last symbol = idi    |
                                    last symbol = ttp
end  arithoperator last symbol;






Boolean procedure  relatoperator last symbol;
begin   relatoperator last symbol:= last symbol = les  |
                                    last symbol = mst  |
                                    last symbol = equ  |
                                    last symbol = lst  |
                                    last symbol = mor  |
                                    last symbol = uqu
end  relatoperator last symbol;


Boolean procedure  booloperator last symbol;
begin   booloperator last symbol:= last symbol = qvl  |
                                   last symbol = imp  |
                                   last symbol = or   |
                                   last symbol = and
end  booloperator last symbol;


Boolean procedure  declarator last symbol;
begin   ownx type:= last symbol = ownx; if  ownx type then  next symbol;
        type:= if last symbol = rea   then 0 else
               if last symbol = integ then 1 else
               if last symbol = boole then 2 else
               if last symbol = stri  then 3 else 1000;
        if type < 4 then next symbol
                    else begin if ownx type then  ERRORMESSAGE (104);
                               if last symbol = arrayx then  type:= 0
                         end;
        arr declarator last symbol:= last symbol = arrayx;
        if arr declarator last symbol  &  run number = 300
        then arr decla macro:= if ownx type
                               then (if type = 0 then ORAD else
                                     if type = 1 then OIAD else
                                     if type = 2 then OBAD else OSTAD)
                               else (if type = 0 then RAD else
                                     if type = 1 then IAD else
                                     if type = 2 then BAD else STAD);
        chara:= if arr declarator last symbol
                then 8
                else if last symbol = switchx
                     then 14
                     else if last symbol = proced
                          then (if type < 4 then 16 else 24)
                          else type;
        type declarator last symbol:= chara < 4;
        if ownx type  &  chara > 8 then ERRORMESSAGE (105);
        if type < 4  &  last symbol = switchx then ERRORMESSAGE (106);
        if chara < 25  &  run number = 100
        then character:= ((if type declarator last symbol
                           then type
                           else if type < 4
                                then type + chara
                                else chara) +
                           (if ownx type then 32 else 0))  * d19;
        declarator last symbol:= chara < 25
end  declarator last symbol;






Boolean procedure  specifier last symbol;
begin   type:= if last symbol = rea   then 0 else
               if last symbol = integ then 1 else
               if last symbol = boole then 2 else
               if last symbol = stri  then 3 else
               if last symbol = arrayx then 5 else 1000;
        if type < 4 then next symbol;
        chara:= if last symbol = labelx  then 6 else
                if last symbol = switchx then 14 else 1000;
        if type + chara < 1000 then ERRORMESSAGE(107);
        chara:= if last symbol = arrayx  then 8 else
                if last symbol = proced then (if type < 4 then 16
                                                          else 24)
                                        else chara;
        if chara < 25 then next symbol;
        if chara + type < 2000  &  run number = 100
        then begin valuex character:= (if chara > 8 then type else
                                      if chara = 6 then    6 else
                                      if  type = 5 then    8
                                      else type + chara) + 64;
                   character:= ((if type > 5
                                 then chara
                                 else (if type > 1 then type else 4) +
                                      (if chara < 1000 then chara
                                                       else 0))
                                + 96)  * d19
             end;
        specifier last symbol:= chara + type < 2000
end  specifier last symbol;


Boolean procedure  operator last symbol;
begin   operator last symbol:= arithoperator last symbol  |
                               relatoperator last symbol  |
                               booloperator last symbol
end  operator last symbol;






procedure  unsigned number;
begin   integer  sign of exponent;
        if last symbol < 10
        then begin valuex of constant:= unsigned integerx (0);
                   realx number:= digit last symbol
             end
        else begin valuex of constant:= if last symbol = ten then 1
                                                            else 0;
                   realx number:= truex
             end;
        decimal exponent:= 0;
        if realx number
        then begin
             next0: if last symbol < 10
                    then begin decimal exponent:= decimal exponent + 1;
                               next symbol; goto next0
                         end;
                    if last symbol = period
                    then begin next symbol;
                               valuex of constant:=
                               unsigned integerx (valuex of constant);
                               decimal exponent:= decimal exponent -
                                                  decimal count;
                        next1: if last symbol < 10
                               then begin next symbol; goto next1 end
                         end;
                    if last symbol = ten
                    then begin next symbol; sign of exponent:= 1;
                               if last symbol = plus
                               then next symbol
                               else if last symbol = minus
                                    then begin next symbol;
                                               sign of exponent:= - 1
                                         end;
                               decimal exponent:= decimal exponent +
                                                  sign of exponent  *
                                                  unsigned integerx (0);
                               if last symbol < 10
                               then begin  ERRORMESSAGE (108);
                                    next2: if next symbol < 9
                                           then goto next2
                                    end
                         end
             end;
        small:= valuex of constant < d15  &   !  realx number
end  unsigned number;






integer procedure  unsigned integerx (start); integer start;
begin   integer  word;
        word:= start; decimal count:= 0;
        if last symbol > 9 then ERRORMESSAGE (109);
next0:  if last symbol < 10
        then begin if word < 6710886  |  (word = 6710886  &  last symbol < 4)
                   then begin word:= 10  * word + last symbol;
                              decimal count:= decimal count + 1;
                              next symbol; goto next0
                        end
             end;
        unsigned integerx:= word
end  unsigned integerx;


procedure  read identifier;
begin   integer  word, count;
        word:= count:= word count:= 0;
        if letter last symbol
        then
        begin
        next0: if last symbol < 64
               then
               begin if count = 4
                     then begin word count:= word count + 1;
                                word:= count:= 0
                          end;
                     word:= space[nl base - nlp - word count]:=
                     d6  * word - last symbol - 1;
                     count:= count + 1; next symbol; goto next0
               end
               else
               begin last identifier:= space[nl base - nlp];
                     last identifierl:= if word count = 0
                                        then 0
                                        else space[nl base - nlp - 1]
               end
        end
        else begin ERRORMESSAGE (110); space[nl base - nlp]:= - 1 end;
        space[nl base - nlp - word count - 1]:= 127  * d19
end  read identifier;


integer procedure  next pointer (n); integer n;
begin   integer  word, pointer;
        pointer:= n;
next0:  word:= - space[nl base - pointer];
        if word  <=    0 then begin pointer:= pointer + 1; goto next0 end;
        if word  >=  d25 then begin pointer:= word - word  %  d13  * d13;
                                 goto next0
                           end;
        next pointer:= pointer
end  next pointer;






integer procedure  look up;
begin   integer  count, pointer;
        pointer:= block cell pointer +
                  (if in formal list  |  in arrayx declaration
                   then 5 else 4);
next0:  pointer:= next pointer (pointer);
        for count:= 0 step 1 until word count do
        begin if space[nl base - pointer - count]  !=
                 space[nl base - last nlp - count]
              then goto next1
        end;
        pointer:= pointer + word count + 1;
        if space[nl base - pointer] < 0
        then begin next1: pointer:= pointer + 1;
                          goto if space[nl base - pointer] < 0 then next1
                                                               else next0
             end;
        look up:= pointer
end  look up;


Boolean procedure  in name list;
begin   integer head;
        if realx number  |   !  int labels
        then in name list:= falsex
        else begin head:= valuex of constant  %  d18;
                   space[nl base - nlp]:= - d12 - head;
                   space[nl base - nlp - 1]:=
                   (head - 1)  * d18 - valuex of constant;
                   word count:= 1;
                   space[nl base - nlp - 2]:= 6  * d19;
                   last nlp:= nlp; integerx labelx:= look up;
                   in name list:= integerx labelx < nlp
             end
end  in name list;


integer procedure  next identifier (n); integer n;
begin   integer  pointer;
        pointer:= next pointer (n) + 1;
next0:  if space[nl base - pointer] < 0
        then begin pointer:= pointer + 1; goto next0 end;
        next identifier:= pointer
end  next identifier;






procedure  skip identifier;
begin   if last symbol < 64 then begin next symbol; skip identifier end
end  skip identifier;


procedure  skip type declaration;
begin   if letter last symbol then skip identifier;
        if last symbol = comma
        then begin next symbol; skip type declaration end
end  skip type declaration;


procedure  skip valuex list;
begin   if last symbol = valuex
        then begin next symbol; skip type declaration;
                   if last symbol = semicolon then next symbol
             end
end  skip valuex list;


procedure  skip specification list;
begin   if specifier last symbol
        then begin skip type declaration;
                   if last symbol = semicolon then next symbol;
                   skip specification list
             end
end  skip specification list;


procedure skip stringx;
begin   quote counter:= 1;
next0:  if next symbol  !=  unquote then goto next0;
        quote counter:= 0
end  skip stringx;


procedure skip rest of statement (pr); procedure pr;
begin   if last symbol = dox
        then begin next symbol; pr end
        else
        if last symbol = gotox  |  last symbol = forx  |
           last symbol = beginx
        then pr;
        if last symbol = quote then skip stringx;
        if last symbol  !=  semicolon  &  last symbol  !=  endx
        then begin next symbol;
                   skip rest of statement (pr)
             end
end  skip rest of statement;






integer procedure bit stringx (kn, n, codex word); integer kn,n,codex word;
begin   integer  k;
        k:= codex word  %  kn; bit stringx:= (codex word - k  * kn)  %  n
end  bit stringx;


integer procedure  display level;
begin   display level:=
        bit stringx (d6, d0, space[nl base - block cell pointer - 1])
end  display level;


integer procedure  top of display;
begin   top of display:=
        bit stringx (d13, d6, space[nl base - block cell pointer - 1])
end  top of display;


integer procedure  local space;
begin   local space:= space[nl base - block cell pointer - 1]  %  d13
end  local space;


integer procedure  proc level;
begin   proc level:=
        bit stringx (d6, d0, space[nl base - block cell pointer - 2])
end  proc level;


Boolean procedure  use of counter stack;
begin   use of counter stack:=
        bit stringx (d7, d6, space[nl base - block cell pointer - 2]) = 1
end  use of counter stack;


integer procedure  status;
begin   status:= space[nl base - block cell pointer - 2]  %  d13
end status;


Boolean procedure  in codex (n); integer n;
begin   in codex:= bit stringx (d25, d24, space[nl base - n - 1]) = 1
end  in codex;


integer procedure  type bits (n); integer n;
begin   type bits:= bit stringx (d22, d19, space[nl base - n])
end type bits;






Boolean procedure  local labelx (n); integer n;
begin   local labelx:=
        nonformal labelx (n)  &
        bit stringx(d6, d0,
        space[nl base - corresponding block cell pointer (n) - 1]) =
        display level
end  local labelx;


Boolean procedure  nonformal labelx (n); integer n;
begin   nonformal labelx:= space[nl base - n]  %  d19 = 6
end nonformal labelx;


integer procedure  corresponding block cell pointer (n); integer n;
begin   integer p;
        p:= block cell pointer;
next0:  if n < p  |  (n > space[nl base - p - 2]  %  d13  &  p > 0)
        then begin p:= space[nl base - p]  %  d13; goto next0 end;
        corresponding block cell pointer:= p
end  corresponding block cell pointer;


procedure  entrance block;
begin   block cell pointer:= next block cell pointer;
        next block cell pointer:=
        bit stringx (d13, d0, space[nl base - block cell pointer])
end  entrance block;


procedure  exit block;
begin   block cell pointer:= space[nl base - block cell pointer]  %  d13
end  exit block;


procedure  init;
begin   stock:= stock1:= last symbol:= word count:= - 1;
        shift:= 1;
        line counter:= quote counter:= forx count:= 0;
        in formal list:= in arrayx declaration:= falsex;
        case:= lower case; text pointer:= 0
end  init;






procedure test pointers;
begin   integer fprog, fnl, i, shift;
        if text in memory
        then
        begin fprog:= text base +
                      (if runnumber = 300 then text pointer else 0) -
                      instruct counter;
              fnl:= nl base - nlp -
                    (text base +
                     (if runnumber = 100 then text pointer
                                         else endx of text));
              if fprog + fnl < 40
              then begin text in memory:= falsex; test pointers end
              else if fprog < 20
                   then begin shift:= (fnl - fprog)  %  2;
                              for i:= text base + text pointer
                              step - 1 until text base do
                              space[i + shift]:= space[i];
                              text base:= text base + shift
                        end
                   else if fnl < 20
                        then
                        begin shift:= (fprog - fnl)  %  2;
                              for i:= text base step 1
                              until text base + text pointer do
                              space[i]:= space[i + shift];
                              text base:= text base - shift
                        end
        end
        else if nl base - nlp - instruct counter < 20
             then begin ERRORMESSAGE (492); goto endrun end
end  test pointers;






procedure prescan0;
begin   integer old block cell pointer, displ level, prc level,
                global count, local count, labelx count, local forx count,
                max forx count, internal block depth, stringx occurrence,
                subcount, arrayx pointer;


procedure  Program;
begin   integer  n;
        character:= 6  * d19;
        if letter last symbol
        then begin read identifier;
                   if last symbol = colon
                   then begin n:= Process identifier;
                              Labelx declaration (n)
                        end
                   else ERRORMESSAGE (111);
                   Program
             end
        else
        if digit last symbol
        then begin unsigned number;
                   if last symbol = colon then Int lab declaration
                                          else ERRORMESSAGE (112);
                   Program
             end
        else
        if last symbol = beginx
        then Beginx statement
        else begin ERRORMESSAGE (113); next symbol; Program end
end  Program;


integer procedure  Block (proc identifier); integer  proc identifier;
begin   integer  dump1, dump2, dump3, dump4, dump5, dump6, dump7, dump8,
                 n, formal count;
        dump1:= block cell pointer; dump2:= local forx count;
        dump3:= max forx count;      dump4:= local count;
        dump5:= labelx count;        dump6:= internal block depth;
        dump7:= stringx occurrence;  dump8:= prc level;
        local forx count:= max forx count:= local count:= labelx count:=
        internal block depth:= stringx occurrence:= 0;
        block cell pointer:= nlp + 1;
        space[nl base - old block cell pointer]:=
        space[nl base - old block cell pointer] + block cell pointer;
        old block cell pointer:= block cell pointer;
        space[nl base - block cell pointer]:= dump1  * d13;
        space[nl base - block cell pointer - 1]:= displ level:=
                                                  displ level + 1;
        space[nl base - block cell pointer - 3]:= 0;
        nlp:= nlp + 6;




        if proc identifier > 0
        then
        begin prc level:= displ level; formal count:= 0;
              space[nl base - block cell pointer - 4]:= - d25 - nlp;
              if last symbol = open
              then begin  character:= 127  * d19;
                   next0: next symbol; Identifier;
                          space[nl base - nlp]:= 0; nlp:= nlp + 1;
                          formal count:= formal count + 1;
                          if last symbol = comma then goto next0;
                          if last symbol = close then next symbol
                                                 else ERRORMESSAGE (114)
                   end;
              if last symbol = semicolon then next symbol
                                         else ERRORMESSAGE (115);
              space[nl base - proc identifier - 1]:=
                                         d22 + formal count + 1 ;
              if last symbol = valuex
              then
              begin
              next1: next symbol; n:= Identifier;
                     if n > last nlp  then ERRORMESSAGE (116)
                                      else space[nl base - n]:= 95  * d19;
                     nlp:= last nlp;
                     if last symbol = comma then goto next1;
                     if last symbol = semicolon then next symbol
                                                else ERRORMESSAGE (117)
              end;
        next2: if specifier last symbol
              then
              begin
              next3: n:= Identifier;
                     if n > last nlp
                     then ERRORMESSAGE (118)
                     else if space[nl base - n] = 127  * d19
                          then space[nl base - n]:= character
                          else if space[nl base - n]  !=   95  * d19
                               then ERRORMESSAGE (119)
                               else if valuex character > 75
                                    then ERRORMESSAGE (120)
                                    else
                                    begin space[nl base - n]:=
                                          valuex character  * d19;
                                          if type = 3
                                          then stringx occurrence:= d6
                                    end;
                     nlp:= last nlp;
                     if last symbol = comma
                     then begin next symbol; goto next3 end;
                     if last symbol = semicolon then next symbol
                                                else ERRORMESSAGE (121);
                     goto next2
              end;




              space[nl base - nlp]:= - d25 - 4 - dump1; nlp:= nlp + 1;
              space[nl base - block cell pointer - 4]:= - d25 - nlp;
              if last symbol = quote
              then begin  space[nl base - proc identifier - 1]:=
                          space[nl base - proc identifier - 1] + d24;
                   next4: next symbol;
                          if last symbol  !=  unquote then goto next4;
                          next symbol
                   end
              else
              if last symbol = beginx
              then begin next symbol;
                         if declarator last symbol then Declaration list;
                         Compound tail; next symbol
                   end
              else Statement
        end
        else
        begin space[nl base - nlp]:= - d25 - 4 - dump1; nlp:= nlp + 1;
              space[nl base - block cell pointer - 4]:= - d25 - nlp;
              Declaration list; Compound tail
        end;

        space[nl base - block cell pointer - 2]:=
        d13  * nlp + stringx occurrence + prc level;
        for n:= 0 step 1 until max forx count - 1 do
        space[nl base - nlp - 1]:= d19;
        space[nl base - block cell pointer - 1]:=
        space[nl base - block cell pointer - 1] +
        d6  * (internal block depth + 1);
        if prc level > 1
        then space[nl base - block cell pointer - 1]:=
             space[nl base - block cell pointer - 1] +
             d13  * (max forx count + local count)
        else global count:= global count + max forx count +
                            local count + labelx count;
        nlp:= nlp + max forx count;
        space[nl base - nlp]:= - d25 - 5 - block cell pointer;
        nlp:= nlp + 1;
        space[nl base - block cell pointer + 1]:= - d25 - nlp;
        displ level:= space[nl base - dump1 - 1];
        Block:= internal block depth + 1;
        block cell pointer:= dump1; local forx count:= dump2;
        max forx count:= dump3;      local count:= dump4;
        labelx count:= dump5;        internal block depth:= dump6;
        stringx occurrence:= dump7;  prc level:= dump8
end  Block;


procedure  Compound tail;
begin   Statement; if  last symbol = semicolon
                   then begin next symbol; Compound tail end
end  Compound tail;






procedure  Declaration list;
begin   integer  n, count;
next0:  if type declarator last symbol
        then begin  count:= 0;
             next1: count:= count + 1;
                    n:= Identifier;
                    if n < last nlp then ERRORMESSAGE (122);
                    if last symbol = comma
                    then begin next symbol; goto next1 end;
                    if type = 0  |  type = 3 then count:= 2  * count;
                    if ownx type then global count:= global count + count
                                else local count:= local count + count;
                    if type = 3 then stringx occurrence:= d6
             end
        else
        if arr declarator last symbol
        then begin  count:= arrayx pointer:= 0;
            next2:  count:= count + 1;
                    next symbol; n:= Identifier;
                    if n < last nlp then ERRORMESSAGE (123);
                    space[nl base - nlp]:= arrayx pointer;
                    arrayx pointer:= nlp; nlp:= nlp + 1;
                    if last symbol = comma then goto next2;
                    dimension:= 0;
                    if last symbol = sub
                    then
                    begin  subcount:= 1;
                    next3: next symbol;
                           if letter last symbol
                           then skip identifier
                           else if digit last symbol
                                then begin unsigned number;
                                           Store numerical constant
                                     end;
                           if last symbol = quote then skip stringx;
                           if last symbol = colon
                           then begin dimension:= dimension + 1;
                                      goto next3
                                end;
                           if last symbol = sub
                           then begin subcount:= subcount + 1;
                                      goto next3
                                end;
                           if last symbol  !=  bus then goto next3;
                           if subcount > 1
                           then begin subcount:= subcount - 1;
                                      goto next3
                                end;
                           next symbol;
                           if dimension = 0 then ERRORMESSAGE (124)
                           else dimension:= dimension + 1
                    end
                    else ERRORMESSAGE (125);




             next4: n:= space[nl base - arrayx pointer];
                   space[nl base - arrayx pointer]:= dimension;
                   arrayx pointer:= n;
                   if n  !=  0 then goto next4;
                   if ownx type
                   then global count:=
                        global count + (3  * dimension + 3)  * count
                   else local count:= local count + count;
                   if last symbol = comma
                   then begin count:= 0; goto next2 end;
                   if type = 3 then stringx occurrence:= d6
             end
        else
        if last symbol = switchx
        then begin next symbol; n:= Identifier;
                   if n < last nlp then ERRORMESSAGE (126);
                   space[nl base - nlp]:= 0; nlp:= nlp + 1;
            next5: next symbol;
                   if letter last symbol
                   then skip identifier
                   else if digit last symbol
                        then begin unsigned number;
                                   Store numerical constant
                             end;
                   if last symbol = quote then skip stringx;
                   if last symbol  !=  semicolon then goto next5
            end
        else begin next symbol; n:= Identifier;
                   if n < last nlp then ERRORMESSAGE (127);
                   nlp:= nlp + 1;
                   if type < 4
                   then begin space[nl base - nlp]:= type  * d19;
                              nlp:= nlp + 1
                        end;
                   Block (n)
             end;
        if last symbol = semicolon then next symbol
                                   else ERRORMESSAGE (128);
        if declarator last symbol then goto next0
end  Deciaration list;






procedure  Statement;
begin   integer  n, lfc;
        lfc:= local forx count;
next0:  character:= 6  * d19;
next1:  if letter last symbol
        then begin read identifier;
                   if last symbol = colon
                   then begin n:= Process identifier;
                              Labelx declaration (n);
                              goto next1
                        end
             end
        else
        if digit last symbol
        then begin unsigned number;
                   if last symbol = colon
                   then begin Int lab declaration; goto next1 end
                   else Store numerical constant
             end
        else
        if last symbol = forx
        then begin local forx count:= local forx count + 1;
                   if local forx count > max forx count
                   then max forx count:= local forx count
             end
        else
        if last symbol = beginx
        then begin Beginx statement; next symbol; goto next1 end
        else
        if last symbol = quote then skip stringx;
        if last symbol  !=  semicolon  &  last symbol  !=  endx
        then begin next symbol; goto next1 end;
        local forx count:= lfc
end  Statement;


procedure  Labelx declaration (n); integer n;
begin   if n < last nlp then ERRORMESSAGE (129);
        if labelx count = 0
        then space[nl base - block cell pointer - 3]:= d13  * (nlp - 1);
        labelx count:= labelx count + 2;
        space[nl base - nlp]:= d18; nlp:= nlp + 1;
        next symbol
end  Labelx declaration;


procedure Int lab declaration;
begin   if realx number
        then begin ERRORMESSAGE (130); next symbol end
        else begin int labels:= truex;
                   in name list; nlp:= nlp + 3;
                   Labelx declaration (integerx labelx)
             end
end  Int lab declaration;






procedure Beginx statement;
begin   integer n;
        next symbol;
        if declarator last symbol
        then begin n:= Block (0);
                   if n > internal block depth
                   then internal block depth:= n
             end
        else Compound tail
end  Beginx statement;


procedure  Store numerical constant;
begin   if  !  small
        then begin space[prog base + instruct counter]:=
                   valuex of constant;
                   space[prog base + instruct counter + 1]:=
                   decimal exponent;
                   instruct counter:= instruct counter + 2
             end
end  Store numerical constant;


integer procedure  Process identifier;
begin   last nlp:= nlp; nlp:= nlp + word count + 2;
        space[nl base - nlp + 1]:= character;
        Process identifier:= look up
end  Process identifier;


integer procedure  Identifier;
begin   read identifier;
        Identifier:= Process identifier
end  Identifier;






main program of prescan0:
        runnumber:= 100; init;
        local forx count:= max forx count:= local count:= labelx count:=
        global count:= internal block depth:= stringx occurrence:=
        displ level:= prc level:= 0;
        old block cell pointer:= block cell pointer:= nlp;
        int labels:= falsex;
        space[text base]:=
        space[nl base - block cell pointer]:=
        space[nl base - block cell pointer - 1]:=
        space[nl base - block cell pointer - 3]:= 0;
        nlp:= block cell pointer + 6;
        space[nl base - block cell pointer - 4]:= - d25 - nlp;
        next symbol;
        Program;
        space[nl base - block cell pointer - 1]:=
                (global count + max forx count + labelx count)  * d13 +
                (internal block depth + 1)  * (d13 + d6);
        space[nl base - block cell pointer - 2]:= nlp  * d13;
        for n:= 0 step 1 until max forx count - 1 do
        space[nl base - nlp - n]:= d19;
        nlp:= nlp + max forx count;
        space[nl base - block cell pointer - 5]:= - d25 - nlp;
        endx of text:= text pointer;
        output
end  prescan0;






procedure prescan1;
begin


procedure  Arithexp;
begin   if last symbol = ifx then Ifclause (Arithexp)
                            else Simple arithexp
end  Arithexp;


procedure  Simple arithexp;
begin   integer n;
        if last symbol = plus  |  last symbol = minus
        then
next0:  next symbol;
        if last symbol = open
        then begin next symbol; Arithexp;
                   if last symbol = close then next symbol
             end
        else
        if digit last symbol then unsigned number
        else
        if letter last symbol
        then begin n:= Identifier; Arithmetic (n);
                   Subscripted variable(n); Function designator(n)
             end
        else
        if last symbol = ifx then Arithexp;
        if arithoperator last symbol then goto next0
end  Simple arithexp;


procedure  Subscripted variable (n); integer n;
begin   if last symbol = sub then begin Subscrvar (n);
                                        dimension:= Subscrlist;
                                        List length (n)
                                  end
end  Subscripted variable;


integer procedure  Subscrlist;
begin next symbol;  Arithexp;
      if last symbol = comma then Subscrlist:= Subscrlist + 1
                             else begin if last symbol = bus
                                        then next symbol;
                                        Subscrlist:= 1
                                  end
end  Subscrlist;


procedure  Boolexp;
begin   if last symbol = ifx then Ifclause (Boolexp)
                            else Simple booleanx
end  Boolexp;






procedure  Simple booleanx;
begin   integer  n, type;
        if last symbol = non  then next symbol;
        if last symbol = open then begin next symbol; Exp (type);
                                         if last symbol = close
                                         then next symbol
                                   end
        else
        if letter last symbol then begin n:= Identifier;
                                         Subscripted variable (n);
                                         Function designator (n);
                                         if arithoperator last symbol  |
                                            relatoperator last symbol
                                         then Arithmetic (n)
                                         else Booleanx (n)
                                   end
        else
        if digit last symbol  |  last symbol = plus  |  last symbol = minus
        then Simple arithexp
        else
        if last symbol = truex  |  last symbol = falsex then next symbol;
        Rest of exp (type)
end  Simple booleanx;


procedure  Stringexp;
begin   if last symbol = ifx then Ifclause (Stringexp)
                            else Simple stringexp
end  Stringexp;


procedure  Simple stringexp;
begin   integer n;
        if last symbol = open
        then begin next symbol; Stringexp;
                   if last symbol = close then next symbol
             end
        else
        if letter last symbol
        then begin n:= Identifier; Stringx (n);
                   Subscripted variable (n);
                   Function designator (n)
             end
        else
        if last symbol = quote
        then begin  quote counter:= 1;
             next0: next symbol;
                    if last symbol = unquote
                    then begin quote counter:= 0;
                               next symbol
                         end
                    else goto next0
             end
end  Simple stringexp;






procedure  Desigexp;
begin   if last symbol = ifx then Ifclause (Desigexp)
                            else Simple desigexp
end  Desigexp;


procedure  Simple desigexp;
begin   integer n;
        if last symbol = open
        then begin next symbol; Desigexp;
                   if last symbol = close then next symbol
             end
        else
        if letter last symbol
        then begin n:= Identifier; Designational (n);
                   Subscripted variable (n)
             end
        else
        if digit last symbol
        then begin unsigned number;
                   if in name list
                   then Designational (integerx labelx)
             end
end  Simple desigexp;


procedure  Exp (type); integer type;
begin   if last symbol = ifx
        then begin next symbol; Boolexp;
                   next symbol; Simplexp (type);
                   if last symbol = elsex
                   then begin next symbol; Type exp (type) end
             end
        else Simplexp (type)
end  Exp;


procedure  Type exp (type); integer type;
begin   if type = ar  |  type = re  |  type = in
        then Arithexp
        else if type = bo
             then Boolexp
             else if type = st
                  then Stringexp
                  else if type = des
                       then Desigexp
                       else Exp (type)
end  Type exp;






procedure  Simplexp (type); integer type;
begin   integer n;
        type:= un;
        if last symbol = open
        then begin next symbol; Exp ( type);
                   if last symbol = close then next symbol
             end
        else
        if letter last symbol
        then begin n:= Identifier; Subscripted variable (n);
                   Function designator (n);
                   if arithoperator last symbol  |
                      relatoperator last symbol
                   then Arithmetic (n)
                   else if booloperator last symbol
                        then Booleanx (n)
                        else begin if nonformal labelx (n)
                                   then Designational (n);
                                   type:= type bits (n)
                             end
              end
        else
        if digit last symbol
        then begin unsigned number;
                   if in name list
                   then Designational (integerx labelx)
                   else type:= ar
             end
        else
        if last symbol = plus  |  last symbol = minus
        then begin Simple arithexp; type:= ar end
        else
        if last symbol = non  |  last symbol = truex  |  last symbol = falsex
        then begin Simple booleanx; type:= bo end
        else
        if last symbol = quote
        then begin Simple stringexp; type:= st; goto endx end;
        Rest of exp (type);
endx:
end  Simplexp;


procedure Rest of exp (type); integer type;
begin   if arithoperator last symbol
        then begin next symbol; Simple arithexp;
                   type:= ar
             end;
        if relatoperator last symbol
        then begin next symbol; Simple arithexp;
                   type:= bo
             end;
        if booloperator last symbol
        then begin next symbol; Simple booleanx;
                   type:= bo
             end
end  Rest of exp;






procedure  Assignstat (n); integer n;
begin   Subscripted variable (n);
        if  last symbol = colonequal then  Right hand side (n)
end  Assignstat;


procedure  Right hand side (n); integer n;
begin   integer  m, type, type n;
        Assigned tox (n); type n:= type bits (n);
        next symbol;
        if letter last symbol
        then begin m:= Identifier; Subscripted variable (m);
                   if  last symbol = colonequal
                   then
                   begin Insert (type n, m);
                         Right hand side (m); type:= type bits (m)
                   end
                   else
                   begin Function designator (m);
                         if arithoperator last symbol  |
                            relatoperator last symbol
                         then Arithmetic (m)
                         else if booloperator last symbol
                              then Booleanx (m)
                              else
                              begin Arbost (m);
                                    type:= if type n = re  |  type n = in
                                           then ar
                                           else type n;
                                    Insert (type, m);
                                    type:= type bits (m);
                                    if type = re  |  type = in
                                    then type:= ar
                              end;
                         Rest of exp (type)
                   end
             end
        else begin m:= type n; Type exp (type n);
                   if m  !=  nondes then type n:= m;
                   type:= if type n = re  |  type n = in then ar
                                                       else type n
             end;
        Insert (type, n)
end  Right hand side;






procedure  Insert (type, n); integer  type, n;
begin   if type = re
        then Realx (n)
        else if type = in
             then Integerx (n)
             else if type = bo
                  then Booleanx (n)
                  else if type = ar then Arithmetic (n)
end  Insert;


procedure  Function designator (n); integer  n;
begin   if last symbol = open then begin Function (n);
                                         dimension:= Parlist;
                                         List length (n)
                                   end
end  Function designator;


integer procedure  Parlist;
begin   next symbol; Actual parameter;
        if last symbol = comma
        then Parlist:= Parlist + 1
        else begin if last symbol = close then next symbol;
                   Parlist:= 1
             end
end  Parlist;


procedure  Actual parameter;
begin   integer  type;
        Exp (type)
end  Actual parameter;


procedure  Procstat (n); integer n;
begin   Proc (n);
        dimension:= if last symbol = open then Parlist else 0;
        List length (n)
end  Procstat;






procedure  Statement;
begin   integer n;
        if letter last symbol
        then begin n:= Identifier;
                   if last symbol = colon
                   then Labelled statement (n)
                   else begin if last symbol = sub  |
                                 last symbol = colonequal
                              then Assignstat (n)
                              else Procstat (n)
                        end
            end
        else
        if digit last symbol
        then begin unsigned number;
                   if last symbol = colon
                   then  Intlabelled statement
             end
        else
        if last symbol = gotox then Gotostat
        else
        if last symbol = beginx
        then begin next symbol;
                   if declarator last symbol then Block
                                             else Compound tail;
                   next symbol
             end
        else
        if last symbol = ifx then Ifclause (Statement)
        else
        if last symbol = forx then Forstat
end  Statement;


procedure  Gotostat;
begin   integer n;
        next symbol;
        if letter last symbol
        then begin n:= Identifier;
                   if  !  local labelx (n)
                   then begin Designational (n);
                              Subscripted variable (n)
                        end
             end
        else Desigexp
end Gotostat;


procedure  Compound tail;
begin   Statement;
        if last symbol  !=  semicolon  &  last symbol  !=  endx
        then skip rest of statement (Statement);
        if last symbol = semicolon
        then begin next symbol; Compound tail end
end  Compound tail;






procedure  Ifclause (pr); procedure  pr;
begin   next symbol; Boolexp;
        if last symbol = thenx then next symbol;
        pr;
       if last symbol = elsex then begin next symbol; pr end
end Ifclause;


procedure  Forstat;
begin   integer n;
        next symbol;
        if letter last symbol
        then begin  n:= Identifier; Arithmetic (n);
                    Subscripted variable (n);
                    if last symbol = colonequal
                    then
             next0: next symbol; Arithexp;
                    if last symbol = stepx
                    then begin next symbol; Arithexp;
                               if last symbol = untilx
                               then begin next symbol;
                                          Arithexp
                                    end
                         end
                    else
                    if last symbol = whilex
                    then begin next symbol; Boolexp end;
                    if last symbol = comma then goto next0;
                    if last symbol = dox    then next symbol;
                    forx count:= forx count + 1;
                    Statement;
                    forx count:= forx count - 1
             end
end  Forstat;


procedure  Switchx declaration;
begin   integer n;
        next symbol;
        if letter last symbol
        then begin n:= Identifier;
                   if last symbol = colonequal
                   then begin dimension:= Switchlist;
                              Switchx length (n)
                        end
             end
end Switchx declaration;


integer procedure  Switchlist;
begin   next symbol; Desigexp;
        if last symbol = comma then Switchlist:= Switchlist + 1
                               else Switchlist:= 1
end Switchlist;






procedure  Arrayx declaration;
begin   integer  i, n, count;
        next symbol; n:= Identifier; count:= 1;
next0:  if last symbol = comma then begin next symbol;
                                          if letter last symbol
                                          then skip identifier;
                                          count:= count + 1; goto next0
                                    end;
        if last symbol = sub   then begin in arrayx declaration:= truex;
                                          dimension:= Bound pair list;
                                          in arrayx declaration:= falsex
                                    end
                               else dimension:= 0;
        Check dimension (n);
        if ownx type then for i:= 1 step 1 until count do
                         begin Address (n, instruct counter);
                               instruct counter:= instruct counter +
                                                  3  * dimension + 6;
                               n:= next identifier (n)
                         end;
        if last symbol = comma then Arrayx declaration
end  Arrayx declaration;


integer procedure  Bound pair list;
begin   next symbol; Arithexp;
        if last symbol = colon then begin next symbol; Arithexp end;
        if last symbol = comma
        then Bound pair list:= Bound pair list + 1
        else begin if last symbol = bus then next symbol;
                   Bound pair list:= 1
             end
end Bound pair list;






procedure  Procedurex declaration;
begin   integer  n, m;
        next symbol; n:= Identifier; entrance block;
        if last symbol = open
        then begin  in formal list:= truex ;
             next0: next symbol; m:= Identifier;
                    if space[nl base - m] = 95  * d19
                    then begin ERRORMESSAGE (201);
                               space[nl base - m]:= 127  * d19
                         end;
                    if last symbol = comma then goto next0;
                    if last symbol = close then next symbol;
                    in formal list:= falsex
             end;
        if last symbol = semicolon then next symbol;
        skip valuex list; skip specification list;
        if in codex (n)
        then Scan codex (n)
        else begin if space[nl base - n]  %  d19 = 19  &
                       !  use of counter stack
                   then space[nl base - block cell pointer - 2]:=
                        space[nl base - block cell pointer - 2] + 64;
                   if last symbol = beginx
                   then begin next symbol;
                              if declarator last symbol
                              then Declaration list;
                              Compound tail; next symbol
                        end
                   else Statement;
                   Addressing of block identifiers (n)
            end
end Procedurex declaration;


procedure  Block;
begin   entrance block; Declaration list; Compound tail;
        Addressing of block identifiers (0)
end Block;


procedure  Declaration list;
begin   if typedeclarator last symbol then skip type declaration
        else
        if arr declarator last symbol then Arrayx declaration
        else
        if last symbol = switchx       then Switchx declaration
                                      else Procedurex declaration;
        if last symbol = semicolon    then next symbol;
        if declarator last symbol     then Declaration list
end  Declaration list;






procedure  Program;
begin   integer  n;
        if letter last symbol
        then begin n:= Identifier;
                   if last symbol = colon
                   then Labelx declaration (n);
                   Program
             end
        else
        if digit last symbol
        then begin unsigned number;
                   if in name list
                   then Labelx declaration (integerx labelx);
                   Program
             end
        else
        if last symbol = beginx
        then begin next symbol;
                   if declarator last symbol
                   then Block
                   else Compound tail
             end
        else begin next symbol; Program end
end  Program;


procedure  Labelled statement (n); integer  n;
begin   if nonformal labelx (n) then Labelx declaration (n);
        Statement
end  Labelled statement;


procedure  Intlabelled statement;
begin   if in name list then Labelx declaration (integerx labelx);
        Statement
end  Intlabelled statement;


procedure  Labelx declaration (n); integer n;
begin   if proc level = 0
        then begin Designational (n); Address (n, instruct counter);
                   space[nl base - n - 1]:=
                   space[nl base - n - 1] + instruct counter +
                   d20  * forx count;
                   space[prog base + instruct counter]:= 0;
                   space[prog base + instruct counter + 1]:=
                   d18  * display level + dp0;
                   instruct counter:= instruct counter + 2
             end
        else space[nl base - n - 1]:= space[nl base - n - 1] +
                                      d20  * forx count;
             next symbol
end  Labelx declaration;






procedure  Addressing of block identifiers (n); integer n;
begin   integer  counter, f, codex, codex1;
        if n = 0 then space[nl base - block cell pointer - 1]:=
                      space[nl base - block cell pointer - 1] + d13;
        if proc level > 0
        then
        begin counter:= d9  * display level + d8;
              if n = 0
              then counter:= counter + 1 + d18
              else
              begin counter:= counter + display level + top of display;
                    f:= block cell pointer + 5;
             next0: f:= next identifier (f);
                    if f > block cell pointer
                    then
                    begin Address (f, counter);
                          codex1:= space[nl base - f]  %  d18;
                          codex:= codex1  %  2;
                          counter:= counter +
                          (if codex = 64  |  codex = 67  |  codex = 70
                           then 2
                           else if codex < 96
                                then 1
                                else if codex1 = 2  * codex
                                     then 2 else 4);
                           goto next0
                    end;
                    counter:= counter + d18;
                    codex:= space[nl base - n]  %  d19;
                    if codex  !=  24
                    then
                    begin f:= if wanted then 3 else
                              if codex = 16  |  codex = 19 then 2 else 1;
                          Address (n + 2, counter);
                          counter:= counter + f;
                          space[nl base - block cell pointer - 1] :=
                          space[nl base - block cell pointer - 1] +
                          d13  * f
                    end
              end;




              f:= status;
       next1: if space[nl base - f] > 0
              then begin Address (f, counter); counter:= counter + 1;
                         f:= f + 1;
                         goto next1
                   end;
              f:= block cell pointer + 4;
       next2: f:= next identifier (f); codex:= space[nl base - f]  %  d19;
              if f > block cell pointer  &  f < status  &  codex < 64
              then begin if codex > 24
                       then
                       begin if codex < 36
                             then
                             begin Address (f, instruct counter);
                                   instruct counter:=
                                   instruct counter +
                                   (if codex= 32  |  codex = 35
                                    then 2 else 1)
                             end
                         end
                         else
                         if codex < 14
                         then
                         begin if codex  !=  6  |
                                 (codex = 6  &
                                   bit stringx (d19, d18,
                                   space[nl base - f - 1]) = 0)
                               then
                               begin Address (f, counter);
                                     counter:=
                                     counter +
                                     (if codex = 0  |  codex = 3  |  codex = 6
                                      then 2 else 1)
                               end
                         end;
                         goto next2
                   end;
              if counter > d18 + d9  * (display level + 1)
              then ERRORMESSAGE (202);
              exit block
        end
        else Static addressing
end  Addressing of block identifiers;






procedure  Static addressing;
begin   integer  f, codex;
        f:= status;
next0:  if space[nl base - f] > 0
        then begin Address (f, instruct counter);
                   instruct counter:= instruct counter + 1; f:= f + 1;
                   goto next0
             end;
        f:= block cell pointer + 4;
next1:  f:= next identifier (f); codex:= space[nl base - f]  %  d19;
        if f > block cell pointer  &  f < status
        then begin if codex > 24  &  codex < 36  |  codex < 14  &  codex  !=  6
                   then begin Address (f, instruct counter);
                              instruct counter:=
                              instruct counter +
                              (if codex = 0  |  codex = 3  |
                                  codex = 32  |  codex = 35 then 2
                                                        else 1)
                        end;
                   goto next1
             end;
        exit block
end Static addressing;






procedure  Add type (n, t); integer  n, t;
begin   integer codex, new codex, type;
        new codex:= codex:= space[nl base - n]  %  d19;
        if codex > 95
        then begin if codex = 127
                   then new codex:= 96 + t
                   else if codex = 120  &  t < 6
                        then new codex:= 112 + t
                        else
                        begin type:= codex - codex  %  8  * 8;
                              if type = un  |  (type = nondes  &  t < 5)  |
                                 (type = ar  &  t < 2)
                              then new codex:= codex - type + t
                        end;
                   space[nl base - n]:=
                   space[nl base - n] - (codex - new codex)  * d19
             end
end  Add type;


procedure  Realx (n); integer n; begin Add type (n, re) end Realx;


procedure  Integerx (n); integer n; begin Add type (n, in) end Integerx;


procedure  Booleanx (n); integer n; begin Add type (n, bo) end Booleanx;


procedure  Stringx (n); integer n; begin Add type (n, st) end Stringx;


procedure  Arithmetic (n); integer  n;
begin  Add type (n, ar) end  Arithmetic;


procedure  Arbost (n); integer n;
begin   Add type (n, nondes) end  Arbost;






procedure  Designational (n); integer n;
begin   integer  p;
        if nonformal labelx (n)
        then
        begin if bit stringx (d19, d18, space[nl base - n - 1]) = 1
              then
              begin space[nl base - n - 1]:=
                    abs (space[nl base - n - 1] - d18);
                    p:= corresponding block cell pointer (n);
                    if bit stringx (d6, d0, space[nl base - p - 2]) > 0
                    then begin space[nl base - p - 3]:=
                               space[nl base - p - 3] + 1;
                               space[nl base - p - 1]:=
                               space[nl base - p - 1] + d14
                         end
              end
        end
        else Add type (n, des)
end  Designational;


procedure  Assigned tox (n); integer n;
begin   integer  codex;
        codex:= space[nl base - n]  %  d19;
        if codex > 95
        then
        begin if codex = 127 then codex:= 101;
              if codex < 102 then space[nl base - n]:= codex  * d19 + d18
                            else Add type (n, nondes)
        end
end Assigned tox;


procedure  Subscrvar (n); integer  n;
begin   integer codex, new codex;
        codex:= space[nl base - n]  %  d19;
        if codex > 95
        then begin new codex:= if codex = 127
                              then 111
                              else if codex < 104
                                   then codex + 8
                                   else codex;
                   space[nl base - n]:= space[nl base - n] +
                                        (new codex - codex)  * d19
             end
end  Subscrvar;






procedure  Proc (n); integer  n;
begin   integer  codex, new codex;
        codex:= space[nl base - n]  %  d19;
        if codex > 95
        then begin new codex:= if codex = 127
                              then 120
                              else if codex < 102
                                   then codex + 16
                                   else codex;
                   space[nl base - n]:= space[nl base - n] +
                                        (new codex- codex)  * d19
             end
end  Proc;


procedure  Function (n); integer  n;
begin   Arbost (n); Proc (n) end Function;


procedure  List length (n); integer  n;
begin   integer  word;
        if space[nl base - n]  %  d19 > 95
        then begin word:= space[nl base - n - 1 ];
                   if bit stringx (d18, d0, word) = 0
                   then space[nl base - n - 1]:= word + dimension + 1
             end
end  List length;


procedure  Switchx length (n);  integer  n;
begin   space[nl base - n - 1]:= dimension + 1 end  Switchx length;


procedure  Address (n, m); integer  n, m;
begin   integer  word;
        word:= space[nl base - n]  %  d18;
        space[nl base - n]:= word  * d18 + m
end  Address;


procedure  Check dimension (n); integer  n;
begin   if space[nl base - n - 1]  !=  dimension + 1
        then begin ERRORMESSAGE (203);
                   space[nl base - n - 1]:= dimension + 1
             end
end  Check dimension;






integer procedure  Identifier;
begin   integer  n;
        last nlp:= nlp; read identifier; Identifier:= n:= look up;
        if n > nlp then Ask librarian;
        if n > nlp then begin ERRORMESSAGE (204);
                              nlp:= nlp + word count + 3;
                              space[nl base - nlp + 1]:= 0
                        end
end  Identifier;


procedure  Scan codex (n); integer  n;
begin   block cell pointer:= space[nl base - block cell pointer]  %  d13;
next0:  next symbol; if  last symbol = minus then next symbol;
        if letter last symbol then Identifier else unsigned integerx (0);
        if last symbol = comma then goto next0;
        if last symbol = unquote then next symbol
end  Scan codex;


procedure  Ask librarian;
begin   comment if the current identifier occurs in the library
                then this procedure will add a new namecell to
                the name list and increase nlp;
end  Ask librarian;






main program of prescan 1:
        if  !  text in memory
        then begin NEWPAGE;
                   PRINTTEXT ("input tape for prescan1")
             end;
        runnumber:= 200; init;
        block cell pointer:= next block cell pointer:= 0;
        dp0:= instruct counter;
        instruct counter:= instruct counter + top of display;
        space[nl base - nlp] := -1;
        next symbol; entrance block;
        Program; Static addressing;
        output
end prescan1;












procedure translate;
begin

integer last lnc, lnc, last lncr, macro, parameter, state,
        stack0, stack1, b, ret level, max depth,
        ret max depth, max depth isr, max display length,
        max proc level, ecount, controlled variable, increment,
        l0, l1, l2, l3, l4, l5, number of switchx elements,
        switchx identifier, switchx list count, sword,
        address of constant, sum of maxima;

Boolean in switchx declaration, in codex body, ifx statement forbidden,
        complicated, complex stepx element;


procedure  Arithexp;
begin   integer  future1, future2;
        if last symbol = ifx
        then begin  future1:= future2:= 0;
                    next symbol; Boolexp; Macro2 (COJU, future1);
                    if last symbol  !=  thenx then ERRORMESSAGE (300)
                                          else next symbol;
                    Simple arithexp;
                    if last symbol = elsex
                    then begin  Macro2 (JU, future2);
                                Substitute (future1);
                                next symbol; Arithexp;
                                Substitute (future2)
                         end
                    else ERRORMESSAGE (301)
             end
        else Simple arithexp
end  Arithexp;


procedure  Simple arithexp;
begin   if last symbol = minus then begin  next symbol; Term;
                                           Macro (NEG)
                                    end
                                    else begin if last symbol = plus
                                               then next symbol;
                                               Term
                                         end;
        Next term
end  Simple arithexp;






procedure  Next term;
begin   if last symbol = plus  then begin  Macro (STACK);
                                           next symbol; Term;
                                           Macro (ADD); Next term
                                    end
        else
        if last symbol = minus then begin Macro (STACK);
                                          next symbol; Term;
                                          Macro (SUB); Next term
                                    end
end  Next term;


procedure  Term;  begin  Factor; Next factor end  Term;


procedure  Next factor;
begin   if last symbol = mul then begin  Macro (STACK);
                                         next symbol; Factor;
                                         Macro (MUL); Next factor
                                  end
        else
        if last symbol = div then begin  Macro (STACK);
                                         next symbol; Factor;
                                         Macro (DIV); Next factor
                                  end
        else
        if last symbol = idi then begin  Macro (STACK);
                                         next symbol; Factor;
                                         Macro (IDI); Next factor
                                  end
end  Next factor;


procedure  Factor;  begin  Primary; Next primary end  Factor;


procedure  Next primary;
begin   if last symbol = ttp then begin  Macro (STACK);
                                         next symbol; Primary;
                                         Macro (TTP); Next primary
                                  end
end  Next primary;






procedure  Primary;
begin   integer n;
        if last symbol = open then begin  next symbol; Arithexp;
                                          if last symbol = close
                                          then next symbol
                                          else ERRORMESSAGE (302)
                                   end
        else
        if digit last symbol  then begin  Unsigned number;
                                          Arithconstant
                                   end
        else
        if letter last symbol then begin  n:= Identifier;
                                          Subscripted variable (n);
                                          Function designator (n);
                                         Arithname (n)
                                   end
        else
        begin  ERRORMESSAGE (303);
               if last symbol = ifx  |  last symbol = plus  |
                  last symbol = minus
               then Arithexp
        end
end  Primary;


procedure  Arithname (n); integer n;
begin   if Nonarithmetic (n) then  ERRORMESSAGE (304);
        complicated:= Formal (n)  |  Function (n);
        if Simple (n)
        then begin if Formal (n)  then Macro2 (DOS, n) else
                   if Integerx (n) then Macro2 (TIV, n)
                                  else Macro2 (TRV, n)
             end
end  Arithname;


procedure  Subscripted variable (n); integer n;
begin   if Subscrvar (n) then begin  Address description (n);
                                     if last symbol = colonequal
                                     then begin Macro (STACK);
                                                Macro (STAA)
                                          end
                                     else Evaluation of (n)
                            end
end  Subscripted variable;






procedure  Address description (n); integer  n;
begin   if last symbol = sub
        then begin  next symbol; dimension:= Subscript list;
                    Check dimension (n);
                    if Formal (n)        then Macro2 (DOS, n) else
                    if Designational (n) then Macro2 (TSWE, n)
                                         else Macro2 (TAK, n)
             end
        else ERRORMESSAGE (305)
end  Address description;


procedure  Evaluation of (n); integer  n;
begin   if  Designational(n)
        then begin  if Formal (n) then  Macro (TFSL)
                                  else  Macro (TSL)
             end
        else
        if Booleanx (n) then Macro (TSB)  else
        if Stringx (n)  then Macro (TSST) else
        if Formal (n)  then Macro (TFSU) else
        if Integerx (n) then Macro (TSI)  else Macro (TSR)
end  Evaluation of;


integer procedure  Subscript list;
begin   Arithexp;
        if last symbol = comma
        then begin  Macro (STACK); next symbol;
                    Subscript list:= Subscript list + 1
             end
        else begin  if last symbol = bus
                    then next symbol
                    else ERRORMESSAGE (306);
                    Subscript list:= 1
             end
end  Subscript list;






procedure  Boolexp;
begin   integer future1, future2;
        if last symbol = ifx
        then begin  future1:= future2:= 0;
                    next symbol; Boolexp; Macro2 (COJU, future1);
                    if last symbol  !=  thenx then ERRORMESSAGE (307)
                                          else next symbol;
                    Simple booleanx;
                    if last symbol = elsex
                    then begin Macro2 (JU, future2);
                               Substitute (future1);
                               next symbol; Boolexp;
                               Substitute (future2)
                         end
                    else ERRORMESSAGE (308)
             end
             else Simple booleanx
end  Boolexp;


procedure  Simple booleanx;
begin   Implication; Next implication end  Simple booleanx;


procedure  Next implication;
begin   if last symbol = qvl then begin Macro (STAB);
                                        next symbol; Implication;
                                        Macro (QVL); Next implication
                                  end
end  Next implication;


procedure  Implication;  begin  Boolterm; Next boolterm end  Implication;


procedure  Next boolterm;
begin   if last symbol = imp then begin  Macro (STAB);
                                         next symbol; Boolterm;
                                         Macro (IMP); Next boolterm
                                  end
end  Next boolterm;


procedure  Boolterm;  begin  Boolfac; Next boolfac end  Boolterm;


procedure  Next boolfac;
begin   if last symbol = or  then begin  Macro (STAB);
                                         next symbol; Boolfac;
                                         Macro (OR); Next boolfac
                                  end
end  Next boolfac;


procedure  Boolfac;  begin  Boolsec; Next boolsec end  Boolfac;






procedure  Next boolsec;
begin   if last symbol = and then begin  Macro (STAB);
                                         next symbol; Boolsec;
                                         Macro (AND); Next boolsec
                                  end
end  Next boolsec;


procedure  Boolsec;
begin   if last symbol = non then begin  next symbol; Boolprim;
                                         Macro (NON)
                                  end
                             else Boolprim
end  Boolsec;


procedure  Boolprim;
begin   integer type, n;
        if last symbol = open
        then begin next symbol; Arboolexp (type);
                   if last symbol = close then next symbol
                                          else ERRORMESSAGE (309);
                   if type = ar   then Rest of relation else
                   if type = arbo then begin if arithoperator last symbol
                                             then Rest of relation
                                             else Relation
                                       end
             end
        else
        if letter last symbol then begin n:= Identifier;
                                         Subscripted variable (n);
                                         Boolprimrest (n)
                                   end
        else
        if digit last symbol  |  last symbol = plus  |  last symbol = minus
        then begin Simple arithexp; Rest of relation end
        else
        if last symbol = truex  |  last symbol = falsex
        then begin Macro2 (TBC, last symbol); next symbol end
        else ERRORMESSAGE (310)
end  Boolprim;






Boolean procedure  Relation;
begin   integer relmacro;
        if relatoperator last symbol
        then begin relmacro:= Relatmacro; Macro (STACK);
                   next symbol; Simple arithexp;
                   Macro (relmacro); Relation:= truex
             end
       else Relation:= falsex
end  Relation;


procedure  Rest of relation;
begin   Rest of arithexp;
        if  !  Relation then  ERRORMESSAGE (311)
end  Rest of relation;


procedure  Boolprimrest (n); integer  n;
begin   Function designator (n);
        if Arithmetic (n)  |  arithoperator last symbol
            |  relatoperator last symbol
        then begin  Arithname (n); Rest of relation end
        else Boolname (n)
end  Boolprimrest;


procedure  Boolname (n); integer  n;
begin   if Nonboolean (n) then ERRORMESSAGE (312);
        if Simple (n)     then begin if Formal (n) then Macro2 (DOS, n)
                                                   else Macro2 (TBV, n)
                               end
end  Boolname;


procedure  Arboolexp (type); integer  type;
begin   integer future1, future2;
        if last symbol = ifx
        then begin  future1:= future2:= 0;
                    next symbol; Boolexp; Macro2 (COJU, future1);
                    if last symbol  !=  thenx then ERRORMESSAGE (313)
                                          else next symbol;
                    Simple arboolexp (type);
                    if last symbol = elsex
                    then
                    begin  Macro2 (JU, future2); Substitute (future1);
                           next symbol;
                           if type = bo then Boolexp else
                           if type = ar then Arithexp
                                        else Arboolexp (type);
                           Substitute (future2)
                    end
                    else ERRORMESSAGE (314)
             end
        else Simple arboolexp (type)
end Arboolexp;






procedure  Simple arboolexp (type); integer  type;
begin   integer n;
        if last symbol = open
        then begin  next symbol; Arboolexp (type);
                    if last symbol = close then next symbol
                                           else ERRORMESSAGE (315);
                    if type = bo  |
                       type = arbo  &  booloperator last symbol
                    then begin Rest of boolexp; type:= bo end
                    else if type = ar  |
                            arithoperator last symbol  |
                            relatoperator last symbol
                         then Rest of arboolexp (type)
             end
        else
        if letter last symbol
        then begin  n:= Identifier; Subscripted variable (n);
                    Arboolrest (type, n)
             end
        else
        if digit last symbol  |  last symbol = plus  |  last symbol = minus
        then begin Simple arithexp; Rest of arboolexp (type) end
        else
        if last symbol = non  |  last symbol = truex  |  last symbol = falsex
        then begin Simple booleanx; type:= bo end
        else
        begin ERRORMESSAGE (316); type:= arbo end
end  Simple arboolexp;


procedure  Rest of arithexp;
begin   Next primary; Next factor; Next term end Rest of arithexp;


procedure  Rest of boolexp;
begin   Next boolsec; Next boolfac; Next boolterm; Next implication
end Rest of boolexp;


procedure  Rest of arboolexp (type); integer  type;
begin   Rest of arithexp;
        if Relation
        then begin  Rest of boolexp; type:= bo end else type:= ar
end  Rest of arboolexp;






procedure  Arboolrest (type, n); integer  type, n;
begin   Function designator (n);
        if Booleanx (n)  |  booloperator last symbol
        then begin  Boolname (n); Rest of boolexp; type:= bo end
        else
        if  Arithmetic (n)  |  arithoperator last symbol  |
            relatoperator last symbol
        then begin  Arithname (n); Rest of arboolexp (type) end
        else begin  if Stringx (n)  |  Designational (n)
                    then ERRORMESSAGE (317);
                    Macro2 (DOS, n); type:= arbo
             end
end  Arboolrest;


procedure  Stringexp;
begin   integer  futurel, future2;
        if last symbol = ifx
        then begin  futurel:= future2:= 0;
                    next symbol; Boolexp; Macro2 ( COJU, futurel);
                    if last symbol  !=  thenx then ERRORMESSAGE (318)
                                          else next symbol;
                    Simple stringexp;
                    if last symbol = elsex
                    then begin  Macro2 (JU, future2);
                                Substitute (futurel);
                                next symbol; Stringexp;
                                Substitute (future2)
                         end
                    else ERRORMESSAGE (319)
             end
        else Simple stringexp
end  Stringexp;


procedure  Simple stringexp;
begin   integer  future, n;
        if last symbol = open
        then begin  next symbol; Stringexp;
                    if last symbol = close then next symbol
                                           else ERRORMESSAGE (320)
             end
        else
        if  letter last symbol
        then begin  n:= Identifier; Subscripted variable (n);
                    Stringname (n)
             end
        else
        if last symbol = quote
        then begin  Macro (TCST); future:= 0; Macro2 (JU, future);
                    Constant stringx; Substitute (future)
             end
        else ERRORMESSAGE (321)
end  Simple stringexp;






procedure  Stringname (n); integer  n;
begin   if Nonstring (n) then ERRORMESSAGE (322);
        Function designator (n);
        if Simple (n) then begin  if Formal (n) then Macro2 (DOS, n)
                                                else Macro2 (TSTV, n)
                           end
end  Stringname;


procedure  Desigexp;
begin   integer  futurel, future2;
        if last symbol = ifx
        then begin  futurel:= future2:= 0;
                    next symbol; Boolexp; Macro2 (COJU, futurel);
                    if last symbol  !=  thenx then ERRORMESSAGE (323)
                                          else next symbol;
                    Simple desigexp;
                    if last symbol = elsex
                    then begin Macro2 (JU, future2);
                               Substitute (futurel);
                               next symbol; Desigexp;
                               Substitute (future2)
                         end
                    else ERRORMESSAGE (324)
             end
        else Simple desigexp
end  Desigexp;


procedure  Simple desigexp;
begin   integer n;
        if  last symbol = open
        then begin  next symbol; Desigexp;
                    if last symbol = close then next symbol
                                           else ERRORMESSAGE (325)
             end
        else
        if letter last symbol then begin n:= Identifier;
                                         Subscripted variable (n);
                                         Designame (n)
                                   end
        else
        if  digit last symbol then begin Unsigned number;
                                         if in name list
                                         then Macro2 (TLV, integerx labelx)
                                         else ERRORMESSAGE (326)
                                   end
        else ERRORMESSAGE (327)
end  Simple desigexp;






procedure  Designame (n); integer  n;
begin   if Nondesignational (n) then ERRORMESSAGE (328);
        if Simple (n)
        then begin  if Formal (n) then Macro2 (DOS, n)
                                  else Macro2 (TLV, n)
             end
end  Designame;


procedure  Ardesexp (type); integer  type;
begin   Exp (type);
        if type = bo  |  type = st then ERRORMESSAGE (329);
        if type = un             then type:= intlab else
        if type = nondes         then type:= ar
end  Ardesexp;


procedure  Nondesexp (type); integer  type;
begin   Exp (type);
        if type = des    then ERRORMESSAGE (330);
        if type = un     then type:= nondes else
        if type = intlab then type:= ar
end  Nondesexp;


procedure  Exp (type); integer  type;
begin   integer  future1, future2;
        if last symbol = ifx
        then begin  future1:= future2:= 0;
                    next symbol; Boolexp; Macro2 (COJU, future1);
                    if last symbol  !=  thenx then ERRORMESSAGE (331)
                                          else next symbol;
                    Simplexp (type);
                    if last symbol = elsex
                    then
                    begin  Macro2 (JU, future2);
                           Substitute (future1); next symbol;
                           if type = ar     then Arithexp        else
                           if type = bo     then Boolexp         else
                           if type = st     then Stringexp       else
                           if type = des    then Desigexp        else
                           if type = intlab then Ardesexp (type) else
                           if type = nondes then Nondesexp (type)
                                            else Exp (type);
                           Substitute (future2)
                    end
                    else ERRORMESSAGE (332)
             end
        else  Simplexp (type)
end Exp;






procedure  Simplexp (type); integer  type;
begin   integer n;
        if last symbol = open
        then begin  next symbol; Exp (type);
                    if last symbol = close then next symbol
                                           else ERRORMESSAGE (333);
                    if type = bo  |  (type = nondes  |  type = un)  &
                       booloperator last symbol
                    then begin Rest of boolexp; type:= bo end
                    else
                    if type  !=  st  &  type  !=  des  &  operator last symbol
                    then  Rest of arboolexp (type)
             end
        else
        if letter last symbol
        then begin  n:= Identifier; Subscripted variable (n);
                    Exprest (type, n)
             end
        else
        if digit last symbol
        then begin  Unsigned number; Arithconstant;
                    if in name list  &  (  !  operator last symbol)
                    then begin  Macro2 (TLV, integerx labelx);
                                type:= intlab
                         end
                    else Rest of arboolexp (type)
             end
        else
        if last symbol = plus  |  last symbol = minus
        then Simple arboolexp (type)
        else
        if last symbol = non  |  last symbol = truex  |  last symbol = falsex
        then begin Simple booleanx; type:= bo end
        else
        if last symbol = quote then begin Simple stringexp; type:= st end
        else
        begin  ERRORMESSAGE (334); type:= un end
end  Simplexp;






procedure  Exprest (type, n); integer type, n;
begin   if Designational (n) then begin Designame (n); type:= des end
        else
        if Stringx (n)        then begin Stringname (n); type:= st end
        else
        begin  Function designator (n);
               if Booleanx (n)  |  booloperator last symbol
               then begin Boolname (n); Rest of boolexp; type:= bo end
               else
               if Arithmetic (n)  |  arithoperator last symbol  |
                  relatoperator last symbol
               then begin Arithname (n); Rest of arboolexp (type) end
               else begin if Simple (n) then  Macro2 (DOS, n);
                          type:= if Unknown (n) then un else nondes
                    end
       end
end  Exprest;


procedure  Assignstat (n); integer  n;
begin   Subscripted variable (n);
        if last symbol = colonequal then Distribute on type (n)
                                    else ERRORMESSAGE (335)
end  Assignstat;


integer procedure  Distribute on type (n); integer  n;
begin   if Integerx (n)
        then begin  Intassign (n); Distribute on type:= in end
        else
        if  Realx (n)
        then begin  Realassign (n); Distribute on type:= re end
        else
        if Booleanx (n)
        then begin  Boolassign (n); Distribute on type:= bo end
        else
        if Stringx (n)
        then begin  Stringassign (n); Distribute on type:= st end
        else Distribute on type:= if Arithmetic (n) then Arassign (n)
                                                    else Unassign (n)
end  Distribute on type;


procedure  Prepare (n); integer  n;
begin   if Function (n)
        then begin  if Formal (n) then ERRORMESSAGE (336)
                    else
                    if  Outside declaration (n) then ERRORMESSAGE (337)
                    else n:= Local position (n)
             end
        else if Simple (n)  &  Formal (n) then Macro2 (DOS2, n);
        next symbol
end  Prepare;






Boolean procedure  Intassign (n); integer  n;
begin   integer m;  Boolean  rounded;
        if  Noninteger (n) then  ERRORMESSAGE (338);
        Prepare (n); rounded:= falsex;
        if letter last symbol
        then begin  m:= Identifier; Subscripted variable (m);
                    if last symbol = colonequal
                    then rounded:= Intassign (m)
                    else begin  Function designator (m);
                                Arithname (m); Rest of arithexp
                         end
             end
        else Arithexp;
        if Subscrvar (n)
        then begin   if Formal (n) then Macro (STFSU)
                     else
                     if rounded    then Macro (SSTSI)
                                   else Macro (STSI)
             end
        else if Formal (n) then Macro2 (DOS3, n)
        else if rounded    then Macro2 (SSTI, n)
                           else Macro2 (STI, n);
        Intassign:= Formal (n)  ->   rounded
end Intassign;


procedure  Realassign (n); integer  n;
begin   integer  m;
        if Nonreal (n) then ERRORMESSAGE (339);
        Prepare (n);
        if letter last symbol
        then begin m:= Identifier; Subscripted variable (m);
                   if last symbol = colonequal
                   then Realassign (m)
                   else begin Function designator (m);
                              Arithname (m); Rest of arithexp
                        end
             end
        else Arithexp;
        if  Subscrvar (n)
        then begin if Formal (n) then Macro (STFSU)
                                 else Macro (STSR)
             end
        else if Formal (n) then Macro2 (DOS3, n)
                           else Macro2 (STR, n)
end  Realassign;






procedure  Boolassign (n); integer  n;
begin   integer m;
        if Nonboolean (n) then  ERRORMESSAGE (340);
        Prepare (n);
        if letter last symbol
        then begin m:= Identifier; Subscripted variable (m);
                   if last symbol = colonequal
                   then Boolassign (m)
                   else begin Boolprimrest (m); Rest of boolexp end
             end
        else Boolexp;
        if  Subscrvar (n) then Macro (STSB)
                          else if Formal (n) then Macro2 (DOS3, n)
                                             else Macro2 (STB, n)
end  Boolassign;


procedure  Stringassign (n); integer  n;
begin   integer m;
        if  Nonstring (n) then  ERRORMESSAGE (341);
        Prepare (n);
        if letter last symbol
        then begin m:= Identifier; Subscripted variable (m);
                   if last symbol = colonequal
                   then Stringassign (m)
                   else Stringname (m)
             end
        else Stringexp;
        if Subscrvar (n) then Macro (STSST)
                         else if Formal (n) then Macro2 (DOS3, n)
                                            else Macro2 (STST, n)
end  Stringassign;


integer procedure  Arassign (n); integer  n;
begin   integer type, m;
        if  Nonarithmetic (n) then  ERRORMESSAGE (342);
        Prepare (n); type:= ar;
        if letter last symbol
        then begin m:= Identifier; Subscripted variable (m);
                   if last symbol = colonequal
                   then begin if Nonarithmetic (m)
                              then ERRORMESSAGE (343);
                              type:= Distribute on type (m)
                        end
                   else begin Function designator (m);
                              Arithname (m); Rest of arithexp
                        end
             end
        else Arithexp;
        if Subscrvar (n) then Macro (STFSU) else Macro2 (DOS3, n);
        Arassign:= type
end  Arassign;






integer procedure  Unassign (n); integer  n;
begin   integer  type, m;
        if  Nontype (n) then  ERRORMESSAGE (344);
        Prepare (n);
        if letter last symbol
        then begin m:= Identifier; Subscripted variable (m);
                   if Nontype (m) then ERRORMESSAGE (345);
                   if last symbol = colonequal
                   then type:= Distribute on type (m)
                   else Exprest (type, m)
             end
        else Nondesexp (type);
        if Subscrvar (n)
        then begin if type = bo then Macro (STSB)
                   else
                   if type = st then Macro (STSST)
                                else Macro (STFSU)
             end
        else Macro2 (DOS3, n);
        Unassign:= type
end  Unassign;


procedure  Function designator (n); integer  n;
begin   if Proc (n)
        then begin if Nonfunction (n) then ERRORMESSAGE (346);
                   Procedurex call (n)
             end
end  Function designator;


procedure  Procstat (n); integer  n;
begin   if  Proc (n)
        then begin Procedurex call (n);
                   if   !  (In library (n)  |  Function (n))
                   then last lnc:= - n;
                   if  Formal (n)  |  (Function (n)  &  Stringx (n))
                   then  Macro (REJST)
             end
        else ERRORMESSAGE (347)
end  Procstat;






procedure  Procedurex call (n); integer  n;
begin   integer  number of parameters;
        if Operator like (n)
        then Process operator (n)
        else begin number of parameters:= List length (n);
                   if number of parameters  !=  0
                   then Parameter list (n, number of parameters)
                   else if Formal (n)
                        then Macro2 (DOS, n)
                        else if In library(n) then Macro2 (ISUBJ, n)
                                              else Macro2 (SUBJ, n)
             end
end Procedurecall;


integer procedure  Ordinal number (n); integer  n;
begin   Ordinal number:= if Formal (n) then 15
                         else
                         if Subscrvar (n)
                         then (if Arithmetic (n)
                               then (if Realx (n) then 8 else 9)
                               else if Booleanx (n)
                                    then 10 else 11)
                         else
                         if Function (n)
                         then (if Arithmetic (n)
                               then (if Realx (n) then 24 else 25)
                                else if Booleanx (n) then 26 else 27)
                         else
                         if Proc (n) then 30
                         else
                         if Arithmetic(n)
                         then (if Realx (n) then 0 else 1)
                         else if Booleanx (n)
                              then 2
                              else if Stringx (n) then 3 else 14
end  Ordinal number;






procedure  Parameter list (n, number of parameters);
integer  n, number of parameters;
begin integer  count, m, f, apd, type, future;
      Boolean  simple identifier;
      integer  array  descriptor list[1 : number of parameters];
      count:= future:= 0; f:= n;
      if last symbol = open
      then
      begin
      next: count:= count + 1; next symbol;
           Actual parameter (apd, simple identifier, type, future);
           if count  <=  number of parameters
           then
           begin descriptor list[count]:= apd;
                 if   !  Formal (n)
                 then
                 begin f:= Next formal identifier (f);
                       if simple identifier
                       then
                       begin if Subscrvar (f)
                             then
                             begin if Nonsubscrvar (type)
                                   then  ERRORMESSAGE (348);
                                   Check type (f, type);
                                   Check list length (f, type)
                             end
                             else
                             if Proc (f)
                             then
                             begin if Nonproc (type)
                                   then ERRORMESSAGE (349);
                                   Check list length (f, type);
                                   if Function (f)
                                   then begin if Nonfunction (type)
                                              then ERRORMESSAGE (350);
                                              Check type (f, type)
                                        end
                             end
                             else
                             if Simple (f)
                             then
                             begin if Nonsimple (type)
                                   then ERRORMESSAGE (351);
                                   Check type (f, type)
                             end
                       end
                       else
                       begin if Subscrvar (f)  |  Proc (f)
                             then ERRORMESSAGE (352);
                             if Assigned tox (f)  &  Nonassignable (apd)
                             then ERRORMESSAGE (353);
                             if Arithmetic(f)  &
                                (type = bo  |  type = st  |  type = des)
                             then ERRORMESSAGE (354) else
                             if Booleanx (f)  &
                                type  !=  bo  &  type  !=  nondes  &  type  !=  un
                             then ERRORMESSAGE (355) else
                             if Stringx (f)  &
                                type  !=  st  &  type  !=  nondes  &  type  !=  un
                             then ERRORMESSAGE (356) else
                             if Designational (f)  &
                                type  !=  des  &  type  !=  un
                             then ERRORMESSAGE (357) else
                             if Arbost (f)  &  type = des
                             then ERRORMESSAGE (358)
                       end
                 end
           end
           else ERRORMESSAGE (359);
           if last symbol = comma then goto next;
           if last symbol = close
           then begin next symbol;
                      if count < number of parameters
                      then ERRORMESSAGE (360)
                end
           else ERRORMESSAGE (361)
      end
      else ERRORMESSAGE (362);
      if future  !=  0 then  Substitute (future);
      if Formal (n) then  Macro2 (DOS, n) else  if In library (n)
                                                then Macro2 (ISUBJ, n)
                                                else Macro2 (SUBJ, n);
      m:= 0;
next apd: if m < count  &  m < number of parameters
      then begin m:= m + 1; apd:= descriptor list[m];
                 Macro2 (CODEx, apd); goto next apd
           end
end  Parameter list;






procedure  Actual parameter (apd, simple identifier, type, future);
integer  apd, type, future; Boolean  simple identifier;
begin integer  n, beginx address;
      beginx address:= Order counter + (if future = 0 then 1 else 0);
      simple identifier:= falsex;
      if letter last symbol
      then
      begin n:= Identifier;
            if last symbol = comma  |  last symbol = close
            then
            begin type:= n; simple identifier:= truex;
                  if Proc (n)  &   !  Formal (n)
                  then
                  begin if future = 0 then Macro2 (JU, future);
                        Macro (TFD);
                         if In library (n) then Macro2 (LJU1, n)
                                           else Macro2 (JU1, n);
                         apd:= d20  * Ordinal number (n) + beginx address
                  end
                  else if Subscrvar (n)  &  Designational (n)  &
                           !  Formal (n)
                       then begin if future = 0
                                  then Macro2 (JU, future);
                                  Macro2 (TSWE, n);
                                  apd:= 12  * d20 + beginx address
                            end
                       else apd:= d20  * Ordinal number (n) +
                                  Address (n) +
                                  (if Dynamic (n) then d18 else 0)
            end
            else
            begin Start implicit subroutine (future);
                  if Subscrvar (n) then Address description (n);
                  if (last symbol = comma  |  last symbol = close)  &
                     (  !  Designational (n))
                  then
                  begin if Unknown (n) then Macro (SAS);
                        Macro2 (EXITSV, -2  * dimension);
                        apd:= d20  * (if Booleanx (n) then 18 else
                                     if Stringx (n)  then 19 else
                                     if Formal (n)  then 32 else
                                     if Realx (n)    then 16 else 17)
                              + Ordercounter;
                        type:= if Arithmetic (n) then ar else
                               if Booleanx (n)    then bo else
                               if Stringx (n)     then st else
                               if Arbost (n)     then nondes else un;
                        Macro2 (SUBJ, -beginx address);
                        if Booleanx (n) then Macro (TASB)  else
                        if Stringx (n)  then Macro (TASST) else
                        if Formal (n)  then Macro (TASU)  else
                        if Integerx (n) then Macro (TASI)
                                       else Macro (TASR);
                        Macro (DECS); Macro2 (SUBJ, -beginx address);
                        Macro (FAD)
                  end

                  else
                  begin if Subscrvar (n) then Evaluation of (n);
                        Exprest (type, n); Macro (EXITIS);
                        apd:= mask[type] + beginx address
                  end
            end
      end
      else
      if digit last symbol
      then begin Unsigned number;
                 if (last symbol = comma  |  last symbol = close)  &
                    (  !  in name list)
                 then begin type:= ar; apd:= Number descriptor end
                 else begin Start implicit subroutine (future);
                            Arithconstant;
                            if in name list  &  (  !  operator last symbol)
                            then begin Macro2 (TLV, integerx labelx);
                                       type:= intlab
                                 end
                            else Rest of arboolexp (type);
                            Macro (EXITIS);
                            apd:= mask[type] + beginx address
                      end
           end
      else
      if last symbol = plus
      then
      begin next symbol;
            if digit last symbol
            then begin Unsigned number;
                       if last symbol = comma  |  last symbol = close
                       then begin type:= ar; apd:= Number descriptor end
                       else begin Start implicit subroutine (future);
                                  Arithconstant;
                                  Rest of arboolexp (type);
                                  Macro (EXITIS);
                                  apd:= mask[type] + beginx address
                            end
                 end
            else begin Start implicit subroutine (future);
                       Arboolexp (type);
                       Macro (EXITIS); apd:= mask[type] + beginx address
                 end
      end
      else
      if last symbol = minus
      then
      begin next symbol;
            if digit last symbol
            then begin Unsigned number;
                       if (last symbol = comma  |  last symbol = close)  &
                           small
                       then
                       begin type:= ar;
                             apd:= d20  * 13 + valuex of constant
                       end
                       else
                       begin Start implicit subroutine (future);
                             Arithconstant; Next primary; Next factor;
                             Macro (NEG); Rest of arboolexp (type);
                             Macro (EXITIS);
                             apd:= mask[type] + beginx address
                       end
                 end
            else begin Start implicit subroutine (future);
                       Term; Macro (NEG);
                       Rest of arboolexp (type);
                       Macro (EXITIS); apd:= mask[type] + beginx address
                 end
      end
      else
      if last symbol = truex  |  last symbol = falsex
      then
      begin type:= bo; n:= last symbol; next symbol;
            if last symbol = comma  |  last symbol = close
            then apd:= d20  * 6 + (if n = truex then 0 else 1)
            else begin Start implicit subroutine (future);
                       Macro2 (TBC, n);
                       Rest of boolexp;
                       Macro (EXITIS);
                       apd:= mask[type] + beginx address
                 end
      end
      else begin Start implicit subroutine (future); Exp (type);
                 Macro (EXITIS); apd:= mask[type] + beginx address
           end
end  Actual parameter;






procedure  Start implicit subroutine (future); integer  future;
begin   if future = 0 then  Macro2 (JU, future);
        Macro (ENTRIS)
end  Start implicit subroutine;


integer procedure Number descriptor;
begin   Number descriptor:=
        if small then d20  * 7 + valuex of constant
                 else d20  * (if  realx number then 4 else 5)
                      + address of constant
end  Number descriptor;


procedure  Process operator (n); integer  n;
begin   integer  count;
        count:= 0;
        if last symbol = open
        then begin
             next: next symbol; Arithexp; count:= count + 1;
                   if last symbol = comma
                   then begin Macro (STACK); goto next end;
                   if last symbol = close
                   then next symbol
                   else ERRORMESSAGE (361)
             end;
        if count  !=  List length (n) then ERRORMESSAGE (363);
        Macro (Operator macro (n))
end  Process operator;


Boolean procedure  Nonassignable (apd); integer  apd;
begin   integer  rank;
        rank:= apd  %  d20;
        Nonassignable:= (rank  !=  15)  &  (rank - rank  %  16  * 16) > 3
end  Nonassignable;


procedure  Line;
begin   if lnc  !=  last lnc then Line1 end  Line;


procedure  Line1;
begin   if wanted then begin last lnc:= lnc; Macro2 (LNC, lnc) end
end  Line1;






procedure Statement;
begin   ifx statement forbidden:= falsex; Stat end  Statement;


procedure  Unconditional statement;
begin   ifx statement forbidden:= truex; Stat end  Unconditional statement;


procedure  Stat;
begin   integer n, save lnc;
        if letter last symbol
        then begin save lnc:= line counter;
                   n:= Identifier;
                   if Designational (n)
                   then begin Labelx declaration (n); Stat end
                   else begin lnc:= save lnc; Line;
                              if Subscrvar (n)  |  last symbol = colonequal
                              then Assignstat (n)
                              else Procstat (n)
                        end
             end
        else
        if digit last symbol
        then begin Unsigned number;
                   if in name list
                   then begin Labelx declaration (integerx labelx); Stat end
                   else ERRORMESSAGE (364)
             end
        else begin if last symbol = gotox
                   then begin lnc:= line counter; Line; Gotostat end
                   else
                   if last symbol = beginx
                   then begin save lnc:= line counter; next symbol;
                              if declarator last symbol
                              then begin lnc:= save lnc; Line; Block end
                              else Compound tail;
                              next symbol
                        end
                   else
                   if last symbol = ifx
                   then begin if ifx statement forbidden
                              then ERRORMESSAGE (365);
                              lnc:= line counter; Line; Ifstat
                        end
                   else
                   if last symbol = forx
                   then begin lnc:= line counter; Line; Forstat;
                              if last symbol = elsex
                              then ERRORMESSAGE (366)
                        end
             end
end Stat;







procedure Gotostat;
begin   integer n;
        next symbol;
        if letter last symbol
        then begin n:= Identifier; Subscripted variable (n);
                   if local labelx (n)
                   then begin Test forx count (n); Macro2 (JU, n) end
                   else begin Designame (n); Macro (JUA) end
             end
        else begin Desigexp; Macro (JUA) end
end Gotostat;


procedure Compound tail;
begin   Statement;
        if last symbol  !=  semicolon  &  last symbol  !=  endx
        then begin ERRORMESSAGE (367);
                   skip rest of statement (Statement)
             end;
        if last symbol = semi colon
        then begin next symbol; Compound tail end
end Compound tail;


procedure  Ifstat;
begin   integer  future1, future2, save lnc, last lnc1;
        future1:= future2:= 0; save lnc:= line counter;
        next symbol; Boolexp; Macro2 (COJU, future1);
        if last symbol = thenx then next symbol else ERRORMESSAGE (368);
        Unconditional statement;
        if last symbol = elsex
        then begin Macro2 (JU, future2); Substitute (future1);
                   last lnc1:= last lnc; last lnc:= save lnc;
                   next symbol; Statement; Substitute (future2);
                   if last lnc > last lnc1 then last lnc:= last lnc1
             end
        else begin Substitute (future1);
                   if last lnc > save lnc then last lnc:= save lnc
             end
end  Ifstat;


procedure Forstat;
begin   integer  future, save lnc;
        save lnc:= line counter;
        l0:= 0; next symbol; Forx list;
        future:= 0; Macro2 (JU, future); if l0  !=  0 then Substitute(l0);
        if last symbol = dox then next symbol else ERRORMESSAGE (369);
        Increase status (increment); forx count:= forx count + 1;
        Statement;
        Increase status (- increment); forx count:= forx count - 1;
        if last lnc < 0  |  lnc  !=  save lnc
        then begin lnc:= save lnc; Line1 end;
        Macro2 (LJU,status); Substitute (future)
end  Forstat;






procedure  Store preparation;
begin   if Subscrvar (controlled variable) then Macro2 (SUBJ, - 12)
        else
        if  Formal (controlled variable)
        then Macro2 (DOS2, controlled variable)
end  Store preparation;


procedure  Store macro;
begin   if  Subscrvar (controlled variable)
        then begin if Formal (controlled variable)  then Macro (STFSU)
                   else
                   if Integerx (controlled variable) then Macro (STSI)
                                                    else Macro (STSR);
                   Macro2 (DECB, 2)
             end
        else if Formal (controlled variable)
             then Macro2 (DOS3, controlled variable)
             else if Integerx (controlled variable)
                  then Macro2 (STI, controlled variable)
                  else Macro2 (STR, controlled variable)
end  Store macro;


procedure Take macro;
begin   if Subscrvar (controlled variable)
        then Macro2 (SUBJ, - l1)
        else Arithname (controlled variable)
end  Take macro;


procedure Forx list;
begin if letter last symbol
      then
      begin controlled variable:= Identifier;
            if Nonarithmetic (controlled variable)
            then ERRORMESSAGE (370);
            if Subscrvar (controlled variable)
            then
            begin l3:= 0; Macro2 (JU, l3);
                  l4:= Order counter;
                  Address description (controlled variable);
                  Macro2 (EXITSV, 1 - 2  * dimension);
                  l1:= Order counter;
                  Macro2 (SUBJ, - l4);
                  if Formal (controlled variable)  then Macro (TSCVU)
                  else
                  if Integerx (controlled variable) then Macro (TISCV)
                                                   else Macro (TRSCV);
                  l2:= Order counter;
                  Macro2 (SUBJ, - l4); Macro (FADCV);
                  Substitute (l3)
            end
            else if Function (controlled variable)
                 then ERRORMESSAGE (371);
            if last symbol  !=  colonequal then ERRORMESSAGE (372);




      list: l3:= Order counter;
            Macro2 (TSIC, 0); Macro2 (SSTI, status);
            l4:= Order counter;
            Store preparation;
            next symbol; Arithexp;
            if last symbol = comma  |  last symbol = dox
            then begin Store macro; Macro2 (JU, l0);
                       Substitute (l3)
                 end
            else
            if last symbol = whilex
            then begin Store macro;
                       next symbol; Boolexp;
                       Macro2 (YCOJU, l0); Subst2 (l4, l3)
                 end
            else
            if last symbol = stepx
            then begin l5:= 0; Macro2 (JU, l5); l4:= Order counter;
                       next symbol; complicated:= falsex; Arithexp;
                       complex stepx element:=
                       complicated  |  Order counter> l4 + 1;
                       if complex stepx element then  Macro (EXIT);
                       Substitute (l3);
                       Store preparation; Take macro; Macro (STACK);
                       if complex stepx element then  Macro2 (SUBJ, - l4)
                                               else  Macro2 (DOx, l4);
                       Macro (ADD);
                       Substitute (15);
                       Store macro;
                       if Subscrvar (controlled variable)  |
                          Formal (controlled variable)
                       then Take macro;
                       Macro (STACK);
                       if last symbol = untilx
                       then begin next symbol; Arithexp end
                       else ERRORMESSAGE (373);
                       Macro (TEST1);
                       if complex stepx element then Macro2 (SUBJ, - l4)
                                               else Macro2 (DOx, l4);
                       Macro (TEST2); Macro2 (YCOJU, l0)
                 end
            else ERRORMESSAGE (374);
            if last symbol = comma then goto list
     end
     else ERRORMESSAGE (375)
end  Forx list;






procedure  Switchx declaration;
begin integer  m;
      next symbol;
      if letter last symbol
      then
      begin switchx identifier:= Identifier;
            number of switchx elements:= List length (switchx identifier);
            if last symbol = colonequal
            then
            begin integer array
                  sword list[1 : number of switchx elements];
                  switchx list count:= 0; in switchx declaration:= truex;
            next: switchx list count:= switchx list count + 1;
                  next symbol;
                  if letter last symbol
                  then
                  begin m:= Identifier;
                        if Nondesignational (m) then ERRORMESSAGE (376);
                        if Subscrvar (m)
                        then
                        begin sword:= -45613055 + Order counter;
                              Subscripted variable (m); Macro (EXIT)
                        end
                        else
                        sword:= (if Formal (m)
                                 then -33685503
                                 else 4718592 + (if Dynamic (m)
                                                 then function digit
                                                 else 0)) +
                                 Address (m)
                  end
                  else
                  if digit last symbol
                  then
                  begin Unsigned number;
                        if  in name list
                        then  sword:= 4718592 +
                                      (if Dynamic (integerx labelx)
                                       then function digit
                                       else 0) +
                                      Address (integerx labelx)
                        else ERRORMESSAGE (377)
                  end
                  else
                  begin sword:= - 45613055 + Order counter;
                        Desigexp; Macro (EXIT)
                  end;




                  if switchx list count > number of switchx elements
                  then ERRORMESSAGE (378);
                  sword list[switchx list count]:= sword;
                  if last symbol= comma then goto next;
                  if switchx list count < number of switchx elements
                  then ERRORMESSAGE (379);
                  Mark position in name list (switchx identifier);
                  in switchx declaration:= falsex;
                  Macro2 (CODEx, number of switchx elements);
                  m:= 0;
            next sword: if m < switchx list count  &
                           m < number of switchx elements
                  then begin m:= m + 1; sword:= sword list[m];
                             Macro2 (CODEx, sword); goto next sword
                       end
            end
            else ERRORMESSAGE (380)
      end
      else ERRORMESSAGE (381)
end  Switchx declaration;


procedure Arrayx declaration;
begin   integer  n, count;
        next symbol; lnc:= line counter; Line;
        n:= Identifier; dimension:= List length (n); count:= 1;
next:   if last symbol = comma then begin next symbol; Identifier;
                                          count:= count + 1; goto next
                                    end;
        if last symbol = sub   then begin in arrayx declaration:= truex;
                                          Bound pair list;
                                          in arrayx declaration:= falsex
                                    end
                               else ERRORMESSAGE (382);
        Macro2 (TNA, count); Macro2 (TDA, dimension);
        Macro2 (TAA, n); Macro (arr decla macro);
        if last symbol = comma then Arrayx declaration
end  Arrayx declaration;


procedure  Bound pair list;
begin   next symbol; Arithexp; Macro (STACK);
        if last symbol = colon then begin next symbol; Arithexp;
                                          Macro (STACK)
                                    end
                               else ERRORMESSAGE (383);
        if last symbol = comma then Bound pair list
                               else if last symbol = bus
                                    then next symbol
                                    else ERRORMESSAGE (384)
end  Bound pair list;






procedure  Procedurex declaration;
begin integer  n, f, count, save lnc;
      next symbol; f:= n:= Identifier;
      Skip parameter list; skip valuex list; skip specification list;
      if  !  In library (n) then Mark position in name list (n);
      if in codex (n)
      then Translate codex
      else begin if Function (n) then Set inside declaration (n, truex);
                 entrance block;
                 Macro2 (DPTR, display level);
                 Macro2 (INCRB, top of display);
                 for count:= List length (n) step - 1 until 1 do
                 begin f:= Next formal identifier(f);
                       if In valuex list (f)
                       then
                       begin if Subscrvar (f)
                             then Macro (CEN)
                             else
                             begin if Arithmetic (f)
                                   then begin if Integerx (f)
                                              then Macro (CIV)
                                              else Macro (CRV)
                                        end
                                   else if Booleanx (f) then Macro (CBV)
                                   else if Stringx (f)  then Macro (CSTV)
                                                       else Macro (CLV)
                             end
                       end
                       else if Assigned tox (f) then Macro (CLPN)
                                               else Macro (CEN)
                 end;
                 Macro2 (TDL, display level);
                 Macro2 (ENTRPB, local space);
                 Labelx list; f:= n;
                 for  count:= List length (n) step - 1 until 1 do
                 begin f:= Next formal identifier (f);
                       if In valuex list (f)  &  Subscrvar (f)
                       then begin Macro2 (TAA, f);
                                  if Integerx (f) then Macro (TIAV)
                                                 else Macro (TAV)
                            end
                 end;




                 save lnc:= last lnc; last lnc:= - line counter;
                 Save and restore lnc (SLNC, n);
                 if last symbol = beginx
                 then begin next symbol; if declarator last symbol
                                         then Declaration list;
                            Compound tail; next symbol
                      end
                 else Statement;
                 lnc:= last lnc:= save lnc;
                 if Function (n)
                 then begin Set inside declaration (n, falsex);
                            f:= Local position (n);
                            if Arithmetic (f) then Arithname (f) else
                            if Booleanx (f) then Boolname (f)
                            else begin Stringname(f); Macro (LOS) end
                      end;
                 Save and restore lnc (RLNC, n);
                 if use of counter stack then Macro (EXITPC)
                                         else Macro (EXITP);
                 exit block
          end
end  Procedurex declaration;


procedure Save and restore lnc (macro, n); integer  macro, n;
begin  if wanted  &  Function (n) then Macro2 (macro, Local position1 (n))
end  Save and restore lnc;


procedure Block;
begin   entrance block;
        Macro2 (TBL, display level); Macro2 (ENTRB, local space);
        Labelx list; Declaration list; Compound tail;
        if use of counter stack then Macro2 (EXITC, display level)
                                else Macro2 (EXITB, display level);
        exit block
end  Block;






procedure  Declaration list;
begin   integer future, arr dec;
        future:= arr dec:= 0;
next:   if type declarator last symbol then skip type declaration
        else
        if arr declarator last symbol
        then begin  if future  !=  0
                    then begin Substitute (future);
                               future:= 0
                         end;
                    arr dec:= 1; Arrayx declaration
             end
        else
        begin if future = 0 then Macro2 (JU, future);
              if last symbol = switchx then Switchx declaration
                                      else Procedurex declaration
        end;
        if last symbol = semicolon then next symbol
                                   else ERRORMESSAGE (385);
        if declarator last symbol  then goto next;
        if future  !=  0  then Substitute (future);
        if arr dec  !=  0 then Macro2 (SWP, display level)
end  Declaration list;


procedure Labelx list;
begin   integer  n, count;
        count:= Number of local labels;
        if count > 0
        then begin Macro2 (DECB, 2  * count);
                   Macro2 (LAD, display level);
                   n:= 0; for count:= count step - 1 until 1 do
                   begin next: n:= Next local labelx (n);
                               if Super local (n) then goto next;
                               if count = 1 then Macro2 (LAST, n)
                                            else Macro2 (NIL, n)
                   end
             end
end  Labelx list;






procedure  Program;
begin   integer  n;
        if letter last symbol
        then begin n:= Identifier;
                   if last symbol = colon
                   then Labelx declaration (n);
                   Program
             end
        else
        if digit last symbol
        then begin Unsigned number;
                   if in name list  &  last symbol = colon
                   then Labelx declaration (integerx labelx);
                   Program
             end
        else
        if last symbol = beginx
        then begin next symbol;
                   if declarator last symbol then Block
                                             else Compound tail;
                   Macro (ENDx)
             end
        else begin next symbol; Program end
end  Program;


procedure  Labelx declaration (n); integer  n;
begin   last lnc:= - line counter;
        if Subscrvar (n)       then begin ERRORMESSAGE (388);
                                          Subscripted variable (n)
                                    end
                               else Mark position in name list (n);
        if last symbol = colon then next symbol else ERRORMESSAGE (389)
end  Labelx declaration;


procedure  Substitute (address); integer  address;
begin   Subst2 (Order counter, address) end  Substitute;


procedure Subst2 (address1, address2);
value  address1, address2; integer  address1, address2;
begin   integer  instruction, instruct part, address part;
        address2:= abs (address2);
        instruction:= space[prog base + address2];
        instruct part:= instruction  %  d15  * d15 -
                        (if instruction < 0 then 32767 else 0);
        address part:= instruction - instruct part;
        space[prog base + address2]:= instruct part + address1;
        if address part = 0
        then begin if instruct part = endx of list
                   then space[prog base + address2]:=
                        - space[prog base + address2]
             end
        else Subst2 (address1, address part)
end  Subst2;






integer procedure  Order counter;
begin   Macro (EMPTY); Order counter:= instruct counter
end  Order counter;


procedure  Macro (macro number); integer  macro number;
begin  Macro2 (macro number, parameter) end  Macro;


procedure  Macro2 (macro number, metaparameter);
integer  macro number, metaparameter;
begin   macro:= if macro number < 512 then macro list[macro number]
                                      else macro number;
        parameter:= metaparameter;
        if state = 0
        then begin if macro = STACK then state:= 1
                   else
                   if Simple arithmetic take macro then Load (3)
                   else
                   Produce (macro, parameter)
             end
        else
        if state = 1
        then begin Load (2);
                   if  !  Simple arithmetic take macro
                   then begin Produce (STACK, parameter); Unload end
             end
        else
        if state = 2
        then begin if Optimizable operator then Optimize
                   else
                   begin Produce (STACK, parameter); state:= 3;
                         Macro2 (macro, parameter)
                   end
             end
        else
        if state = 3
        then begin if macro = NEG then Optimize
                   else
                   begin Unload; Macro2 (macro, parameter) end
             end;
        if Forward jumping macro  &  metaparameter  <=  0
        then Assign (metaparameter)
end  Macro2;






procedure  Load (state i); integer  state i;
begin  stack0:= macro; stack1:= parameter; state:= state i end  Load;


procedure  Unload;
begin   Produce (stack0, stack1); state:= 0 end  Unload;


procedure  Optimize;
begin  stack0:= tabel[5  * Opt number (macro) + Opt number (stack0)];
       Unload
end  Optimize;


procedure  Assign (metaparameter); integer  metaparameter;
begin  metaparameter:= - (instruct counter - 1) end  Assign;


procedure  Produce (macro, parameter); integer  macro, parameter;
begin   integer  number, par number, entry, count;
        if macro = EMPTY then
        else
        if macro = CODEx
        then begin space[prog base + instruct counter]:= parameter;
                   instruct counter:= instruct counter + 1;
                   test pointers
             end
        else begin number:= Instruct number (macro);
                   par number:= Par part (macro);
                   entry:= Instruct part (macro) - 1;
                   if par number > 0
                   then Process parameter (macro, parameter);
                   Process stack pointer (macro);
                   for count:= 1 step 1 until number do
                   Produce (CODEx, instruct list[entry + count] +
                                  (if count = par number
                                   then parameter else 0))
             end
end  Produce;






procedure  Process stack pointer (macro); integer  macro;
begin   if  !  in codex body
        then
        begin integer  reaction;
              reaction:= B reaction (macro);
              if reaction < 9
              then begin b:= b + reaction - 4;
                         if b > max depth then max depth:= b
                   end
              else
              if reaction = 10 then b:= 0
              else
              if reaction = 11 then b:= b - 2  * (dimension - 1)
              else
              if reaction = 12
              then begin if ecount = 0
                         then
                         begin ret level:= b;
                               ret max depth:= max depth;
                               b:= 0; max depth:= max depth isr
                         end;
                         ecount:= ecount + 1
                   end
              else
              if reaction = 13
              then begin if macro = EXITSV
                         then
                         begin if b > max depth isr
                               then max depth isr:= b;
                               b:= b - 2  * (dimension - 1)
                         end;
                         if ecount = 1
                         then
                         begin if max depth > max depth isr
                               then max depth isr:= max depth;
                               b:= ret level;
                               max depth:= ret max depth
                         end;
                         if ecount > 0 then ecount:= ecount - 1
                   end
              else
              if reaction = 14
              then begin b:= display level + top of display;
                         if b > max display length
                         then max display length:= b;
                         ret max depth:= max depth
                   end
              else
              if reaction = 15
              then begin if b > max proc level
                         then max proc level:= b;
                         b:= 0; max depth:= ret max depth
                   end
        end
end  Process stack pointer;






procedure  Process parameter (macro, parameter);
integer  macro, parameter;
begin    if Valuex like (macro)
         then
         begin if macro = TBC
               then parameter:= if parameter = truex then 0 else 1
               else
               if macro = SWP then parameter:= d9  * parameter
               else
               if macro  !=  EXITSV then parameter:= abs (parameter)
         end
         else
         begin if macro = JU  |  macro = SUBJ  |  macro = NIL  |  macro = LAST
               then begin if parameter  <=  0
                          then parameter:= - parameter
                          else parameter:= Program address (parameter)
                    end
               else parameter:= Address (parameter) +
                                (if Dynamic (parameter)
                                 then (if macro = TLV  |  macro = TAA
                                       then function digit
                                       else if macro = STST
                                            then function letter
                                            else c variant)
                                 else 0)
         end
end Process parameter;


Boolean procedure  Simple arithmetic take macro;
begin   Simple arithmetic take macro:= bit stringx (d1, d0, macro) = 1
end  Simple arithmetic take macro;


Boolean procedure  Optimizable operator;
begin   Optimizable operator:= bit stringx (d2, d1, macro) = 1
end  Optimizable operator;


Boolean procedure  Forward jumping macro;
begin   Forward jumping macro:= bit stringx (d3, d2, macro) = 1
end  Forward jumping macro;


Boolean procedure  Valuex like (macro); integer macro;
begin   Valuex like:= bit stringx (d4, d3, macro) = 1 end  Valuex like;


integer procedure  Opt number (macro); integer macro;
begin   Opt number:= bit stringx (d8, d4, macro) end  Opt number;


integer procedure  Instruct number (macro); integer macro;
begin   Instruct number:= bit stringx (d10, d8, macro)
end  Instruct number;






integer procedure  Par part (macro); integer  macro;
begin   Par part:= bit stringx (d12, d10, macro) end Par part;


integer procedure  Instruct part (macro); integer  macro;
begin   Instruct part:= bit stringx (d21, d12, macro) end  Instruct part;


integer procedure  B reaction (macro); integer  macro;
begin   B reaction:= macro  %  d21 end  B reaction;


integer procedure  Codex bits (n); integer  n;
begin   Codex bits:= space[nl base - n]  %  d19 end  Codex bits;


integer procedure  Character (n); integer  n;
begin   Character:= bit stringx (d24, d19, space[nl base - n])
end  Character;


Boolean procedure  Arithmetic (n); integer  n;
begin   integer i;
        i:= type bits (n);
        Arithmetic:= Character (n)  !=  24  &  (i < 2  |  i = 4)
end  Arithmetic;


Boolean procedure  Realx (n); integer  n;
begin   Realx:= Character (n)  !=  24  &  type bits (n) = 0 end  Realx;


Boolean procedure  Integerx (n); integer  n;
begin   Integerx:= type bits (n) = 1 end  Integerx;


Boolean procedure  Booleanx (n); integer  n;
begin   Booleanx:= type bits (n) = 2 end  Booleanx;


Boolean procedure  Stringx (n); integer  n;
begin   Stringx:= type bits (n) = 3 end  Stringx;


Boolean procedure  Designational (n); integer  n;
begin   Designational:= type bits (n) = 6 end  Designational;


Boolean procedure  Arbost (n); integer  n;
begin   Arbost:= Character (n)  !=  24  &  type bits (n) < 6 end  Arbost;


Boolean procedure  Unknown (n); integer  n;
begin   Unknown:= type bits (n) = 7 end  Unknown;






Boolean procedure  Nonarithmetic (n); integer  n;
begin   integer i;
        i:= type bits (n);
        Nonarithmetic:= Character (n) = 24  |  i = 2  |  i = 3  |  i = 6
end  Nonarithmetic;


Boolean procedure  Nonreal (n); integer  n;
begin   Nonreal:= Nonarithmetic (n)  |  type bits (n) = 1 end  Nonreal;


Boolean procedure  Noninteger (n); integer  n;
begin   Noninteger:= Nonarithmetic (n)  |  type bits (n) = 0
end  Noninteger;


Boolean procedure  Nonboolean (n); integer  n;
begin   integer i;
        i:= type bits (n); Nonboolean:= i  !=  2  &  i  !=  5  &  i  !=  7
end  Nonboolean;


Boolean procedure  Nonstring (n); integer  n;
begin   integer i;
        i:= type bits (n); Nonstring:= i  !=  3  &  i  !=  5  &  i  !=  7
end  Nonstring;


Boolean procedure  Nondesignational (n); integer  n;
begin   Nondesignational:= type bits (n) < 6 end  Nondesignational;


Boolean procedure  Nontype (n); integer  n;
begin   Nontype:= type bits (n) = 6  |  (Proc (n)  &  Nonfunction (n))
end  Nontype;


Boolean procedure  Simple (n); integer  n;
begin   Simple:= Codex bits (n) = 127  |  Simple1 (n) end  Simple;


Boolean procedure  Simple1 (n); integer  n;
begin   Simple1 := Character (n)  %  d3 = 0 end  Simple1;


Boolean procedure  Subscrvar (n); integer  n;
begin   Subscrvar:= Character (n)  %  d3 = 1 end  Subscrvar;


Boolean procedure  Proc (n); integer  n;
begin   Proc:= Character (n)  %  d3 > 1  &  Codex bits (n)  !=  127 end  Proc;


Boolean procedure  Function (n); integer  n;
begin   Function:= Character (n)  %  d3 = 2 end  Function;






Boolean procedure  Nonsimple (n); integer  n;
begin   Nonsimple:=  !  (Simple (n)  |  (if Proc (n)
                                     then (Formal (n)  |  Function (n))  &
                                           List length (n) < 1
                                     else falsex ))
end  Nonsirnple;


Boolean procedure  Nonsubscrvar (n); integer  n;
begin   Nonsubscrvar:= Simple1 (n)  |  Proc (n) end  Nonsubscrvar;


Boolean procedure  Nonproc (n); integer  n;
begin   Nonproc:=  !  (Character (n)  %  d3  >=  2  |
                     (Formal (n)  &  Simple1 (n)  &   !  Assigned tox (n)))
end  Nonproc;


Boolean procedure  Nonfunction (n); integer  n;
begin   Nonfunction:=  !  (Function (n)  |  Formal (n)) end  Nonfunction;


Boolean procedure  Formal (n); integer  n;
begin   Formal:= Codex bits (n) > 95 end  Formal;


Boolean procedure  In valuex list (n); integer  n;
begin   In valuex list:= Codex bits (n) > 63  &   !  Formal (n)
end  In valuex list;


Boolean procedure  Assigned tox (n); integer  n;
begin   Assigned tox:= bit stringx (d19, d18, space[nl base - n]) = 1
end  Assigned tox;


Boolean procedure  Dynamic (n); integer  n;
begin   Dynamic:= Codex bits (n) > 63  |  Assigned tox (n) end  Dynamic;


Boolean procedure  In library (n); integer  n;
begin   In library:= space[nl base - n - 1] > d25 end  In library;


Boolean procedure  Id1 (k, n); integer  k, n;
begin   Id1:= bit stringx (2  * k, k, space[nl base - n - 1]) = 1 end  Id1;


Boolean procedure  Operator like (n); integer  n;
begin   Operator like:= Id1 (d23, n) end  Operator like;


Boolean procedure  Outside declaration (n); integer  n;
begin   Outside declaration:= Id1 (d22, n) end  Outside declaration;






Boolean procedure  Ass tox function designator (n); integer  n;
begin   Ass tox function designator:= Id1 (d21, n)
end  Ass tox function designator;


Boolean procedure  Declared (n); integer  n;
begin   Declared:= Id1 (d19, n) end  Declared;


Boolean procedure  Super local (n); integer  n;
begin   Super local:= Id1 (d18, n) end  Super local;


procedure  Change (k, n); integer  k, n;
begin   integer  i, j;
        i:= space[nl base - n - 1]; j:= i - i  %  (2  * k)  * (2  * k);
        space[nl base - n - 1]:= i + (if j < k then k else -k)
end  Change;


integer procedure  Local position (n); integer  n;
begin   if   !  Ass tox function designator (n) then  Change (d21, n);
        Local position:= Local position1 (n)
end  Local position;


integer procedure  Local position1 (n); integer  n;
begin   Local position1:= n + 2 end  Local position1;


procedure  Set inside declaration (n, bool); integer  n; Boolean  bool;
begin  Change (d22, n);
       if   !  (bool  |  Ass tox function designator (n))
       then  ERRORMESSAGE (390)
end  Set inside declaration;






procedure  Mark position in name list (n); integer n;
begin   integer  address;
        if Declared (n)
        then ERRORMESSAGE (391)
        else begin address:= Program address (n);
                   if address  !=  0 then Substitute (address);
                   Change (d19, n)
             end
end  Mark position in name list;


integer procedure  Program address (n); integer  n;
begin   integer  word, head, m;
        m:= if Codex bits (n) = 6 then n + 1 else n;
        word:= space[nl base - m]; head:= word  %  d18  * d18;
        if  !  Declared (n)
        then space[nl base - m]:= head + Order counter;
        Program address:= word - head
end  Program address;

integer procedure  Address (n); integer  n;
begin   integer  word, tail, level;
        word:= Codex bits (n);
        if word > 13  &  word < 25
        then tail:= Program address (n)
        else begin word:= space[nl base - n];
                   tail:= word - word  %  d18  * d18;
                   if Dynamic (n)
                   then begin level:= tail  %  d9;
                              if level = proc level  &
                                  !  in switchx declaration
                              then tail:= tail + d9  * (63 - level)
                        end
             end;
        Address:= tail
end  Address;


integer procedure  List length (n); integer  n;
begin   List length:= bit stringx (d18, d0, space[nl base - n - 1]) - 1
end  List length;


procedure  Test forx count (n); integer  n;
begin   if space[nl base - n - 1]  %  d20 > forx count
        then ERRORMESSAGE (392)
end  Test forx count;






procedure  Check dimension (n); integer  n;
begin   integer  i;
        i:= if Codex bits (n) = 14 then 1 else List length (n);
        if i  >=  0  &  i  !=  dimension then ERRORMESSAGE (393)
end  Check dimension;


procedure  Check list length (f, n); integer  f, n;
begin   integer  i, j;
        i:= List length (f);
        j:= if Codex bits (n) = 14 then 1 else List length (n);
        if i  >=  0  &  j  >=  0  &  i  !=  j then ERRORMESSAGE (394)
end Check list length;


procedure  Check type (f, n); integer  f, n;
begin   if (Designational (f)  &  Nondesignational (n))  |
           (Arbost (f)         &  Nontype (n))           |
           (Arithmetic (f)     &  Nonarithmetic (n))     |
           (Booleanx (f)        &  Nonboolean (n))        |
           (Stringx (f)         &  Nonstring (n))
        then ERRORMESSAGE (395)
end Check type;


integer procedure  Number of local labels;
begin   Number of local labels:=
          bit stringx (d13, d0, space[nl base - block cell pointer - 3])
end  Number of local labels;


integer procedure  Next local labelx (n); integer  n;
begin   Next local labelx:=
          if n = 0 then space[nl base - block cell pointer - 3]  %  d13
                   else next identifier (n)
end  Next local labelx;


integer procedure  Next formal identifier (n); integer  n;
begin   Next formal identifier:=
          next identifier (n + (if Formal (n)  |  In library (n)  |
                                   In valuex list (n)
                                then 2
                                else if Function (n) then 9 else 8))
end  Next formal identifier;


procedure  Increase status (increment); integer  increment;
begin   space[nl base - block cell pointer - 2]:=
        space[nl base - block cell pointer - 2] + increment
end  Increase status;


integer procedure  Identifier;
begin   read identifier; Identifier:= look up end  Identifier;






procedure  Skip parameter list;
begin   if last symbol = open
        then begin next symbol; skip type declaration;
                   if last symbol = close then next symbol
             end;
        if lastsymbol = semicolon then next symbol
end  Skip parameter list;


procedure  Translate codex;
begin   integer  macro, parameter;
        if last symbol = quote
        then begin in codex body:= truex;
             next: next symbol;
                   if digit last symbol
                   then
                   begin macro:= unsigned integerx (0);
                         if macro < 512 then macro:= macro list[macro];
                         if Par part (macro) > 0
                         then
                         begin if last symbol = comma
                               then next symbol
                               else ERRORMESSAGE (396);
                               if letter last symbol
                               then parameter:= Identifier
                               else
                               if digit last symbol
                               then parameter:= unsigned integerx (0)
                               else
                               if last symbol = minus
                               then
                               begin next symbol;
                                     if digit last symbol
                                     then parameter:=
                                          - unsigned integerx (0)
                                     else ERRORMESSAGE (397)
                               end
                               else ERRORMESSAGE (398);
                               Macro2 (macro, parameter)
                        end
                        else Macro (macro)
               end
               else ERRORMESSAGE (399);
               if last symbol = comma then goto next;
               if last symbol = unquote then next symbol
                                        else ERRORMESSAGE (400);
               in codex body:= falsex
        end
        else ERRORMESSAGE (401);
        entrance block; exit block
end Translate codex;






procedure  Unsigned number;
begin   integer  p;
        unsigned number;
        if  !  small
        then begin p:= 0;
             next: if p = dp0 then goto found;
                   if space[prog base + p]  !=  valuex of constant  |
                      space[prog base + p + 1]  !=  decimal exponent
                   then begin p:= p + 2; goto next end;
             found: address of constant:= p
             end
end  Unsigned number;


procedure  Arithconstant;
begin   if small then Macro2 (TSIC, valuex of constant)
        else
        if realx number then Macro2 (TRC, address of constant)
                       else Macro2 (TIC, address of constant)
end  Arithconstant;


integer procedure  Operator macro (n); integer  n;
begin   Operator macro:= space[nl base - n - 2] end  Operator macro;


procedure  Constant stringx;
begin   integer word, count;
        quote counter:= 1;
next0:  word:= count:= 0;
next1:  next symbol;
        if last symbol  !=  unquote
        then begin word:= d8  * word + last symbol;
                   count:= count + 1;
                   if count = 3
                   then begin Macro2(CODEx, word); goto next0 end;
                   goto next1
             end;
next2:  word:= d8  * word + 255; count:= count + 1;
        if count < 3 then goto next2;
        Macro2 (CODEx, word); quote counter:= 0; next symbol
end  Constant stringx;


integer procedure  Relatmacro;
begin   Relatmacro:=  if last symbol = les then LES else
                      if last symbol = mst then MST else
                      if last symbol = mor then MOR else
                      if last symbol = lst then LST else
                      if last symbol = equ then EQU else UQU
end  Relatmacro;






main program of translate scan:
        if  !  text in memory
        then begin NEWPAGE;
                   PRINTTEXT ("input tape for translate scan")
             end;
        start:= instruct counter; last nlp:= nlp;
        runnumber:= 300; init; increment:= d13;
        state:= b:= max depth:= max depth isr:=
        max display length:= max proc level:= ecount:= 0;
        in switchx declaration:= in codex body:= falsex;
        next block cell pointer:= 0;
        entrance block; next symbol;
        Program;
        sum of maxima:= max depth + max depth isr +
                        max display length + max proc level;
        Macro2 (CODEx, sum of maxima);
        output
end translate;






procedure  output;
begin   integer  i, k, apostrophe, instruct number, par, address;

        procedure  pucar (n); integer  n;
        begin  integer i;
               for i:= 1 step 1 until n do PUNLCR
        end  pucar;

        procedure  tabspace (n); integer  n;
        begin  integer i, k;
               k:= n  %  8;
               for i:= 1 step 1 until k do PUSYM (118);
               PUSPACE (n - k  * 8)
        end  tabspace;

        procedure  absfixp (k); integer  k;
        begin  ABSFIXP (4, 0, k); pucar (2) end  absfixp;

        procedure  punch (bool); Boolean  bool;
        begin  if bool then PUTEXT (" true")
                       else PUTEXT ("false");
               pucar (2)
        end  punch;

        procedure  punch octal (n);  value n;  integer  n;
        begin  integer  i, k;
               Boolean  minussign;
               minussign:= n < 0; n:= abs (n);
               PUSYM (if minussign then minus else plus);
               PUSYM (apostrophe);
               for i:= d24, d21, d18, d15, d12, d9, d6, d3, d0 do
               begin k:= n  %  i; n:= n - k  * i; PUSYM (k) end;
               PUSYM (apostrophe)
        end  punch octal;

        apostrophe:= 120;
        PUNLCR;
        if runnumber = 100
        then
        begin tabspace (22); PUTEXT ("prescan0"); pucar (2);
              PUTEXT ("erroneous"); PUSPACE (14);
              punch (erroneous); PUTEXT ("text length");
              PUSPACE (12);
              absfixp (if text in memory then text pointer + 1 else 0);
              PUTEXT ("namelist"); pucar(2);
              for i:= 0 step 1 until nlp - 1 do
              begin tabspace (7); ABSFIXP (4, 0, i); PUSPACE(5);
                    punch octal (space[nl base - i]); PUNLCR
              end;
              STOPCODE;
              PUNLCR; PUTEXT ("dp0"); pucar (2);
              PUTEXT ("start"); pucar (2);
              PUTEXT ("program"); pucar (2);




              for i:= prog base step 1 until instruct counter - 1 do
              begin tabspace (7); ABSFIXP (4, 0, i);
                    FIXP (16, 0, space[i]); PUNLCR
              end;
              RUNOUT; STOPCODE
        end
        else if runnumber = 200
        then
        begin tabspace (38); PUTEXT ("prescan1"); pucar (2);
              tabspace (39); punch (erroneous); tabspace (39);
              absfixp (if text in memory then text pointer + 1 else 0);
              pucar (2);
              for i:= 0 step 1 until nlp - 1 do
              begin tabspace (34); punch octal (space[nl base - i]);
                    PUNLCR
              end;
              STOPCODE; pucar (7);
              for i:= prog base step 1 until instruct counter - 1 do
              begin tabspace (32); FIXP (13, 0, space[i]); PUNLCR end;
              RUNOUT; STOPCODE
        end
        else
        begin tabspace (54); PUTEXT ("translate"); pucar (2);
              tabspace (55); punch (erroneous); tabspace (55);
              absfixp (if text in memory then text pointer + 1 else 0);
              pucar (2);
              for i:= 0 step 1 until nlp - 1 do
              begin tabspace (50); punch octal (space[nl base - i]);
                    PUSPACE (2); ABSFIXP (4, 0, i); PUNLCR
              end;
              STOPCODE; PUNLCR;
              tabspace (55); absfixp (dp0);
              tabspace (55); absfixp (start); pucar (2);
              for i:= prog base step 1 until start - 1 do
              begin tabspace (48); FIXP(13, 0, space[i]);
                    PUSPACE (2); ABSFIXP (4, 0, i); PUNLCR
              end;
              PUNLCR;
              for i:= start step 1 until instruct counter - 1 do
              begin k:= space[i]; par:= k  %  32768;
                    address:= k - par  * 32768;
                    instruct number:= par  %  10;
                    par:= par - instruct number  * 10;
                    tabspace (48); ABSFIXP (3, 0, instruct number);
                    ABSFIXP (1, 0, par); ABSFIXP (5, 0, address);
                    PUSPACE (2); ABSFIXP (4, 0, i) ; PUNLCR
              end
       end
end  output;






main program:
        for n:= 0 step 1 until endx of memory do space[n]:= 0;
        instruct counter:= prog base:= nlp:= 0;
        text base:= endx of memory  %  3;
        nl base:= endx of memory;


        prescan0;
        if  !  derroneous
        then begin  prescan1;
                    translate
             end;

endrun:
end
end
