--------------------------------------------------------------------------
--
--  Copyright (C) 1993, Peter J. Ashenden
--  Mail:	Dept. Computer Science
--		University of Adelaide, SA 5005, Australia
--  e-mail:	petera@cs.adelaide.edu.au
--
--  This program is free software; you can redistribute it and/or modify
--  it under the terms of the GNU General Public License as published by
--  the Free Software Foundation; either version 1, or (at your option)
--  any later version.
--
--  This program is distributed in the hope that it will be useful,
--  but WITHOUT ANY WARRANTY; without even the implied warranty of
--  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
--  GNU General Public License for more details.
--
--  You should have received a copy of the GNU General Public License
--  along with this program; if not, write to the Free Software
--  Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
--
--------------------------------------------------------------------------
--
--  $RCSfile: images_test-bench.vhdl,v $  $Revision: 1.1 $  $Date: 1993/10/25 20:47:40 $
--
--------------------------------------------------------------------------
--
--  Architecture declaration for test bench for images package
--

use std.textio.all;

use work.images.all;

architecture bench of images_test is

begin

  process

    variable L : line;

  begin
    ----------------------------------------------------------------
    -- Test binary image of a bit vector
    ----------------------------------------------------------------
    write(L, image(b"01"));
    writeline(output, L);
    write(L, image(b"01011010"));
    writeline(output, L);

    ----------------------------------------------------------------
    -- Test octal image of a bit vector
    ----------------------------------------------------------------
    write(L, image_octal(o"05"));
    writeline(output, L);
    write(L, image_octal(o"370"));
    writeline(output, L);
    write(L, image_octal(b"1"));
    writeline(output, L);
    write(L, image_octal(b"11000"));
    writeline(output, L);

    ----------------------------------------------------------------
    -- Test hex image of a bit vector
    ----------------------------------------------------------------
    write(L, image_hex(x"0C"));
    writeline(output, L);
    write(L, image_hex(x"F1"));
    writeline(output, L);
    write(L, image_hex("1"));
    writeline(output, L);
    write(L, image_hex(b"1001000"));
    writeline(output, L);

    ----------------------------------------------------------------
    -- Add further tests here ...
    ----------------------------------------------------------------


    ----------------------------------------------------------------
    -- end of tests
    ----------------------------------------------------------------
    wait;
  end process;

end bench;


