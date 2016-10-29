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
--  $RCSfile: dlx_test_rtl.vhdl,v $  $Revision: 2.1 $  $Date: 1993/11/02 20:20:00 $
--
--------------------------------------------------------------------------
--
--  Configuration of DLX test bench using register transfer level
--  architecture of DLX processor.
--


configuration dlx_test_rtl of dlx_test is

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
      generic map (enable => true, verbose => true, tag => "bus monitor");
    end for;

    for proc : dlx
      use entity work.dlx(rtl)
        generic map (Tpd_clk_out => 2 ns, debug => true, tag => "proc");
      for rtl
      	for all : alu
	  use entity work.alu(behaviour)
            generic map (Tpd => 4 ns);
	end for;
	for all : reg_file
	  use entity work.reg_file(behaviour)
            generic map (Tac => 4 ns);
	end for;
      	for all : latch
	  use entity work.latch(behaviour)
            generic map (Tpd => 2 ns);
	end for;
      	for all : reg_1_out
	  use entity work.reg_1_out(behaviour)
            generic map (Tpd => 2 ns);
	end for;
      	for all : reg_2_out
	  use entity work.reg_2_out(behaviour)
            generic map (Tpd => 2 ns);
	end for;
      	for all : reg_2_1_out
	  use entity work.reg_2_1_out(behaviour)
            generic map (Tpd => 2 ns);
	end for;
      	for all : reg_3_out
	  use entity work.reg_3_out(behaviour)
            generic map (Tpd => 2 ns);
	end for;
      	for all : mux2
	  use entity work.mux2(behaviour)
            generic map (Tpd => 1 ns);
	end for;
      	for all : ir
	  use entity work.ir(behaviour)
            generic map (Tpd => 2 ns);
	end for;
      	for the_controller : controller
	  use entity work.controller(behaviour)
            generic map (Tpd_clk_ctrl => 2 ns, Tpd_clk_const => 4 ns,
                         debug => true);
	end for;
      end for;  -- rtl of dlx
    end for;  -- proc : dlx
    
  end for;  -- bench of dlx_test

end dlx_test_rtl;
