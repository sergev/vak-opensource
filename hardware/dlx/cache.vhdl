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
--  $RCSfile: cache.vhdl,v $  $Revision: 2.1 $  $Date: 1993/11/02 21:42:34 $
--
--------------------------------------------------------------------------
--
--  Entity declaration for cache.
--


use work.dlx_types.all,
    work.mem_types.all,
    work.cache_types.all;

entity cache is
  generic (cache_size : positive;        --  in bytes, power of 2
           line_size : positive;         --  in bytes, power of 2
           associativity : positive;     --  1 = direct mapped
           write_strategy : strategy_type;  --  write_through or copy_back
           Tpd_clk_out : Time;		  -- clock to output propagation delay
           tag : string := "";
           origin_x, origin_y : real := 0.0);
  port (phi1, phi2 : in bit;		  -- 2-phase non-overlapping clocks
      	reset : in bit;			  -- synchronous reset input
        --  connections to CPU
	cpu_enable : in bit;		  -- starts memory cycle
	cpu_width : in mem_width;	  -- byte/halfword/word indicator
	cpu_write : in bit;		  -- selects read or write cycle
	cpu_ready : out bit;		  -- status from memory system
      	cpu_a : in dlx_address;		  -- address bus output
	cpu_d : inout dlx_word_bus bus;	  -- bidirectional data bus
        --  connections to memory
	mem_enable : out bit;		  -- starts memory cycle
	mem_width : out mem_width;	  -- byte/halfword/word indicator
	mem_write : out bit;		  -- selects read or write cycle
        mem_burst : out bit;              --  tell memory to burst txfer
	mem_ready : in bit;		  -- status from memory system
      	mem_a : out dlx_address;	  -- address bus output
	mem_d : inout dlx_word_bus bus);  -- bidirectional data bus
end cache;
