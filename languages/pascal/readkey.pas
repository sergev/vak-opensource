program read_key;
uses
    crt, sysutils;
var
    k: char;
begin
    while true do begin
        writeln('');
        write('Press key: ');
        k := readkey;

        if k = chr(0) then begin
            {k := readkey;}
            writeln('Special function: ', ord(k));

        end else if k = chr(&33) then begin
            writeln('Quit.');
            break;

        end else if k = ' ' then begin
            writeln('Space');

        end else if (ord(k) > ord(' ')) and (ord(k) <= ord('~')) then begin
            writeln('Printable: ', k);

        end else begin
            writeln('Decimal: ', ord(k));
        end
    end
end.
