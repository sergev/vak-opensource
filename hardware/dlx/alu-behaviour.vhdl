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
--  $RCSfile: alu-behaviour.vhdl,v $  $Revision: 2.1 $  $Date: 1993/11/02 18:52:07 $
--
--------------------------------------------------------------------------
--
--  Behavioural architecture of ALU.
--


architecture behaviour of alu is

begin

  alu_op: process (s1, s2, latch_en, func)

    use work.bv_arithmetic.all;

    variable stored_s1, stored_s2 : dlx_word;
    variable temp_result : dlx_word;
    variable temp_overflow : boolean;

  begin
    if latch_en = '1' then
      stored_s1 := s1;
      stored_s2 := s2;
    end if;
    case func is
      when alu_pass_s1 =>
        temp_result := stored_s1;
      when alu_pass_s2 =>
        temp_result := stored_s2;
      when alu_and =>
        temp_result := stored_s1 and stored_s2;
      when alu_or =>
        temp_result := stored_s1 or stored_s2;
      when alu_xor =>
        temp_result := stored_s1 xor stored_s2;
      when alu_sll =>
        temp_result := bv_sll(stored_s1, bv_to_natural(stored_s2(27 to 31)));
      when alu_srl =>
        temp_result := bv_srl(stored_s1, bv_to_natural(stored_s2(27 to 31)));
      when alu_sra =>
        temp_result := bv_sra(stored_s1, bv_to_natural(stored_s2(27 to 31)));
      when alu_add =>
        bv_add(stored_s1, stored_s2, temp_result, temp_overflow);
      when alu_addu =>
        bv_addu(stored_s1, stored_s2, temp_result, temp_overflow);
      when alu_sub =>
        bv_sub(stored_s1, stored_s2, temp_result, temp_overflow);
      when alu_subu =>
        bv_subu(stored_s1, stored_s2, temp_result, temp_overflow);
    end case;
    result <= temp_result after Tpd;
    zero <= bit'val(boolean'pos(temp_result = dlx_word'(X"0000_0000"))) after Tpd;
    negative <= temp_result(0) after Tpd;
    overflow <= bit'val(boolean'pos(temp_overflow)) after Tpd;
  end process alu_op;
  
end behaviour;
