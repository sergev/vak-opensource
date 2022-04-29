 /*

                             9 9   b e e r s   p r o g r a m


                                                 Jay Moseley
                                                 September 2008

 */

declare print_file literally '0';            /* sysprint output */

declare count fixed initial(99),
        lyric character,
        bottles(2) character initial(' bottles', ' bottle'),
        of_beer character initial(' of beer'),
        take character initial('Take one down, pass it around, '),
        wall character initial('on the wall');

print_line:
   procedure (output_text);
      declare output_text character;

      output(print_file) = ' ' || output_text;

   end  print_line;

do while count > 1;
  lyric = count;
  if count > 0 then lyric = lyric || bottles(0);
               else lyric = lyric || bottles(1);
  lyric = lyric || of_beer || ' ' || wall || ', ' || count;
  if count > 0 then lyric = lyric || bottles(0);
               else lyric = lyric || bottles(1);
  lyric = lyric || of_beer || '.';
    call print_line(lyric);
  count = count - 1;
  lyric = take || count;
  if count > 1 then lyric = lyric || bottles(0);
               else lyric = lyric || bottles(1);
  lyric = lyric || of_beer || ' ' || wall || '.';
    call print_line(lyric);
  call print_line(' ');
end;

lyric = '1' || bottles(1) || of_beer || ' ' || wall || ', ';
lyric = lyric || '1' || bottles(1) || of_beer || '.';
  call print_line(lyric);
lyric = take || 'no more' || bottles(0) || of_beer || ' ' || wall || '!';
  call print_line(lyric);
call print_line(' ');

eof eof eof eof eof eof eof eof eof eof  foe foe foe foe foe foe foe foe foe foe
