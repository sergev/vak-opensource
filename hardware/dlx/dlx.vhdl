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
--  $RCSfile: dlx.vhdl,v $  $Revision: 2.1 $  $Date: 1993/10/31 22:37:14 $
--
--------------------------------------------------------------------------
--
--  Entity specification for DLX processor
--


use work.dlx_types.all,
    work.mem_types.all;

entity dlx is
  generic (Tpd_clk_out : Time;		  -- clock to output propagation delay
           debug : boolean := false;      -- controls debug trace writes
           tag : string := "";
           origin_x, origin_y : real := 0.0);
  port (phi1, phi2 : in bit;		  -- 2-phase non-overlapping clocks
      	reset : in bit;			  -- synchronous reset input
      	a : out dlx_address;		  -- address bus output
	d : inout dlx_word_bus bus;	  -- bidirectional data bus
	halt : out bit;			  -- halt indicator
	width : out mem_width;		  -- byte/haldword/word indicator
	write_enable : out bit;		  -- selects read or write cycle
	mem_enable : out bit;		  -- starts memory cycle
	ifetch : out bit;		  -- indicates instruction fetch
	ready : in bit);		  -- status from memory system
end dlx;
