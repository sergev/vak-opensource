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
--  $RCSfile: reg_1_out.vhdl,v $  $Revision: 2.1 $  $Date: 1993/11/02 19:04:11 $
--
--------------------------------------------------------------------------
--
--  Entity declaration for register with one tri-state output.
--


use work.dlx_types.all;

entity reg_1_out is
  generic (Tpd : Time;
           tag : string := "";
           origin_x, origin_y : real := 0.0);
  port (d : in dlx_word;
	q : out dlx_word_bus bus;
	latch_en : in bit;
	out_en : in bit);
end reg_1_out;
