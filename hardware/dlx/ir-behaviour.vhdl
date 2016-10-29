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
--  $RCSfile: ir-behaviour.vhdl,v $  $Revision: 2.1 $  $Date: 1993/11/02 18:56:39 $
--
--------------------------------------------------------------------------
--
--  Behavioural architecture of instruction register.
--


use work.dlx_instr.all;

architecture behaviour of ir is

begin

  reg: process (d, latch_en, immed_sel1, immed_sel2, 
      	      	immed_unsigned1, immed_unsigned2, immed_en1, immed_en2)

    variable latched_instr : dlx_word;
    
    use work.bv_arithmetic.bv_zext, work.bv_arithmetic.bv_sext;

  begin
    if latch_en = '1' then
      latched_instr := d;
      ir_out <= latched_instr after Tpd;
    end if;
    --
    if immed_en1 = '1' then
      if immed_sel1 = immed_size_16 then
      	if immed_unsigned1 = '1' then
	  immed_q1 <= bv_zext(latched_instr(16 to 31), 32) after Tpd;
	else
	  immed_q1 <= bv_sext(latched_instr(16 to 31), 32) after Tpd;
	end if;
      else -- immed_sel1 = immed_size_26
      	if immed_unsigned1 = '1' then
	  immed_q1 <= bv_zext(latched_instr(6 to 31), 32) after Tpd;
	else
	  immed_q1 <= bv_sext(latched_instr(6 to 31), 32) after Tpd;
	end if;
      end if;
    else
      immed_q1 <= null after Tpd;
    end if;
    --
    if immed_en2 = '1' then
      if immed_sel2 = immed_size_16 then
      	if immed_unsigned2 = '1' then
	  immed_q2 <= bv_zext(latched_instr(16 to 31), 32) after Tpd;
	else
	  immed_q2 <= bv_sext(latched_instr(16 to 31), 32) after Tpd;
	end if;
      else -- immed_sel2 = immed_size_26
      	if immed_unsigned2 = '1' then
	  immed_q2 <= bv_zext(latched_instr(6 to 31), 32) after Tpd;
	else
	  immed_q2 <= bv_sext(latched_instr(6 to 31), 32) after Tpd;
	end if;
      end if;
    else
      immed_q2 <= null after Tpd;
    end if;
  end process reg;

end behaviour;
