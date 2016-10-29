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
--  $RCSfile: reg_file-behaviour.vhdl,v $  $Revision: 2.1 $  $Date: 1993/11/02 19:16:52 $
--
--------------------------------------------------------------------------
--
--  Behavioural architecture of register file.
--


architecture behaviour of reg_file is
  
begin

  reg: process (a1, a2, a3, d3, write_en)

    use work.bv_arithmetic.bv_to_natural;
    
    constant all_zeros : dlx_word := X"0000_0000";

    type register_array is array (reg_index range 1 to 31) of dlx_word;
    
    variable register_file : register_array;
    variable reg_index1, reg_index2, reg_index3 : reg_index;

  begin
    -- do write first if enabled
    --
    if write_en = '1' then
      reg_index3 := bv_to_natural(a3);
      if reg_index3 /= 0 then
      	register_file(reg_index3) := d3;
      end if;
    end if;
    --
    -- read port 1
    --
    reg_index1 := bv_to_natural(a1);
    if reg_index1 /= 0 then
      q1 <= register_file(reg_index1) after Tac;
    else
      q1 <= all_zeros after Tac;
    end if;
    --
    -- read port 2
    --
    reg_index2 := bv_to_natural(a2);
    if reg_index2 /= 0 then
      q2 <= register_file(reg_index2) after Tac;
    else
      q2 <= all_zeros after Tac;
    end if;
  end process reg;

end behaviour;
