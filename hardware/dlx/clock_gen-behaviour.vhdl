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
--  $RCSfile: clock_gen-behaviour.vhdl,v $  $Revision: 2.1 $  $Date: 1993/10/31 20:29:05 $
--
--------------------------------------------------------------------------
--
--  Behavioural architecture body for clock generator
--


architecture behaviour of clock_gen is

  constant clock_period : Time := 2*(Tpw+Tps);

begin

  reset_driver: 
    reset <= '1', '0' after 2*clock_period + Tpw+Tps;

  clock_driver : process
  begin
    phi1 <= '1', '0' after Tpw;
    phi2 <= '1' after Tpw+Tps, '0' after Tpw+Tps+Tpw;
    wait for clock_period;
  end process clock_driver;
  
end behaviour;
