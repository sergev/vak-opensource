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
--  $RCSfile: clock_gen.vhdl,v $  $Revision: 2.1 $  $Date: 1993/10/31 20:20:50 $
--
--------------------------------------------------------------------------
--
--  Entity declaration for clock generator
--


entity clock_gen is

  generic (Tpw : Time;		          -- clock pulse width
      	   Tps : Time;		          -- clock pulse separation
           tag : string := "";
           origin_x, origin_y : real := 0.0);
     					  
  port (phi1, phi2 : out bit;             -- two-phase non-overlapping clocks
      	reset : out bit);		  -- synchronous reset
	
end clock_gen;
