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
--  $RCSfile: memory.vhdl,v $  $Revision: 2.1 $  $Date: 1993/10/31 21:09:12 $
--
--------------------------------------------------------------------------
--
--  Entity declaration for memory model
--


use work.dlx_types.all, work.mem_types.all;

entity memory is

  generic (mem_size : positive; 	-- size in bytes (multiple of 4)
           Tac1 : Time;                 -- access time 1st cycle (read or write)
           Tacb : Time;                 -- access time burst (read or write)
	   Tpd_clk_out : Time;		-- clock to output delay
           tag : string := "";
           origin_x, origin_y : real := 0.0);

  port (phi1, phi2 : in bit;		-- 2-phase non-overlapping clocks
      	a : in dlx_address;		-- byte address: a(0) is lsb
	d : inout dlx_word_bus bus;     -- bidirectional data bus: d(0) is msb
	width : in mem_width;		-- byte/haldword/word indicator
	write_enable : in bit;		-- selects read or write cycle
        burst : in bit := '0';          -- indicates more to come in burst
                                        --   can be left open
                                        --   for non-burst client
	mem_enable : in bit;		-- starts memory cycle
	ready : out bit);		-- status from memory system

end memory;
