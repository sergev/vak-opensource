/*
 * Example of /bin/echo written in XPL.
 *
 * Compile with:
 * xpl echo.xpl && cc echo.c -o echo -lxpl
 */
if argc < 2 then do;
    output = 'No args.';
    return 1;
end;

declare i fixed;
declare s character;
do i = 1 to argc - 1;
    if i > 1 then
        s = s || ' ';
    s = s || argv(i);
end;
output = s;

return 0;
eof
