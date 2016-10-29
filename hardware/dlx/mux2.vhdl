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
--  $RCSfile: mux2.vhdl,v $  $Revision: 2.1 $  $Date: 1993/11/02 19:01:03 $
--
--------------------------------------------------------------------------
--
--  Entity declaration for two-input multiplexor.
--


use work.dlx_types.all;

entity mux2 is
  generic (Tpd : Time;
           tag : string := "";
           origin_x, origin_y : real := 0.0);
  port (i0, i1 : in dlx_word;
      	y : out dlx_word;
	sel : in bit);
end mux2;
