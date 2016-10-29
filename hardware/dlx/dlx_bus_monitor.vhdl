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
--  $RCSfile: dlx_bus_monitor.vhdl,v $  $Revision: 2.1 $  $Date: 1993/10/31 22:35:51 $
--
--------------------------------------------------------------------------
--
--  Entity declaration of DLX bus monitor
--


use work.dlx_types.all,
    work.mem_types.all;

entity dlx_bus_monitor is
  generic (enable : boolean := true;      -- enable monitoring
           verbose : boolean := true;     -- verbose vs brief info
           tag : string := "");
  port (phi1, phi2 : in bit;		  -- 2-phase non-overlapping clocks
      	reset : in bit;			  -- synchronous reset
      	a : in dlx_address;		  -- address bus
	d : in dlx_word;	  	  -- data bus
	halt : in bit;			  -- halt indicator
	width : in mem_width;		  -- byte/haldword/word indicator
	write_enable : in bit;		  -- selects read or write cycle
        burst : in bit := '0';            -- indicates more to come in burst
	mem_enable : in bit;		  -- starts memory cycle
	ifetch : in bit;		  -- indicates instruction fetch
	ready : in bit);		  -- status from memory system
end dlx_bus_monitor;
