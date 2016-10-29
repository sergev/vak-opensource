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
--  $RCSfile: ir.vhdl,v $  $Revision: 2.1 $  $Date: 1993/11/02 18:55:16 $
--
--------------------------------------------------------------------------
--
--  Entity declaration for instruction register.
--


use work.dlx_types.all,
    work.dlx_instr.immed_size;

entity ir is
  generic (Tpd : Time;
           tag : string := "";
           origin_x, origin_y : real := 0.0);
  port (d : in dlx_word;
	immed_q1, immed_q2 : out dlx_word_bus bus;
	ir_out : out dlx_word;
	latch_en : in bit;
	immed_sel1, immed_sel2 : in immed_size;
	immed_unsigned1, immed_unsigned2 : in bit;
	immed_en1, immed_en2 : in bit);
end ir;
