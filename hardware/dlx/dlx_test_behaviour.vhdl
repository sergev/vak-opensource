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
--  $RCSfile: dlx_test_behaviour.vhdl,v $  $Revision: 2.2 $  $Date: 1993/11/02 17:59:08 $
--
--------------------------------------------------------------------------
--
--  Configuration of test bench for DLX, using architecture behaviour
--


configuration dlx_test_behaviour of dlx_test is

  for bench

    for cg : clock_gen
      use entity work.clock_gen(behaviour)
        generic map (Tpw => 8 ns, Tps => 2 ns);
    end for;
 
    for mem : memory
      use entity work.memory(behaviour)
        generic map (mem_size => 65536,
                     Tac1 => 95 ns, Tacb => 35 ns, Tpd_clk_out => 2 ns);
    end for;
 
    for bus_monitor : dlx_bus_monitor
      use entity work.dlx_bus_monitor(behaviour)
      generic map (enable => true, verbose => false, tag => "bus monitor");
    end for;

    for proc : dlx
      use entity work.dlx(behaviour)
        generic map (Tpd_clk_out => 2 ns, debug => true, tag => "proc");
    end for;

  end for;

end dlx_test_behaviour;
