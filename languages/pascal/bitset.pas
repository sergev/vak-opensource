program hello;
type
    bitset = set of 0..31;
    word = record case integer of
            0: (l: longword);
            1: (m: bitset)
           end;
var
    v: word;
    i: integer;
begin
    { Test bitmasks }
    for i:=0 to 31 do begin
        v.m := [1, i];
        writeln('v = ', v.l);
    end
end.
