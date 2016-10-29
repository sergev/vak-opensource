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
--  $RCSfile: reg_3_out-behaviour.vhdl,v $  $Revision: 2.1 $  $Date: 1993/11/02 19:14:21 $
--
--------------------------------------------------------------------------
--
--  Behavioural architecture of register with three tri-state outputs.
--


architecture behaviour of reg_3_out is

begin

  reg: process (d, latch_en, out_en1, out_en2, out_en3)

    variable latched_value : dlx_word;

  begin
    if latch_en = '1' then
      latched_value := d;
    end if;
    if out_en1 = '1' then
      q1 <= latched_value after Tpd;
    else
      q1 <= null after Tpd;
    end if;
    if out_en2 = '1' then
      q2 <= latched_value after Tpd;
    else
      q2 <= null after Tpd;
    end if;
    if out_en3 = '1' then
      q3 <= latched_value after Tpd;
    else
      q3 <= null after Tpd;
    end if;
  end process reg;

end behaviour;
