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
--  $RCSfile: reg_file.vhdl,v $  $Revision: 2.1 $  $Date: 1993/11/02 19:16:43 $
--
--------------------------------------------------------------------------
--
--  Entity declaration for register file.
--


use work.dlx_types.all,
    work.dlx_instr.all;

entity reg_file is
  generic (Tac : Time;
           tag : string := "";
           origin_x, origin_y : real := 0.0);
  port (a1 : in dlx_reg_addr;
  	q1 : out dlx_word;
	a2 : in dlx_reg_addr;
	q2 : out dlx_word;
	a3 : in dlx_reg_addr;
	d3 : in dlx_word;
	write_en : in bit);
end reg_file;
