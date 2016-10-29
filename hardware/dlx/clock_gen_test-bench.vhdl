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
--  $RCSfile: clock_gen_test-bench.vhdl,v $  $Revision: 2.1 $  $Date: 1993/10/31 20:29:43 $
--
--------------------------------------------------------------------------
--
--  Architecture for test bench for clock generator
--


architecture bench of clock_gen_test is

  component clock_gen
    port (phi1, phi2 : out bit;
      	  reset : out bit);
  end component;
  
  for cg : clock_gen
    use entity work.clock_gen(behaviour)
    	generic map (Tpw => 8 ns, Tps => 2 ns);
 
  signal p1, p2, reset : bit;
  
begin

  cg : clock_gen
    port map (p1, p2, reset);
    
end bench;
