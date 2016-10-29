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
--  $RCSfile: latch.vhdl,v $  $Revision: 2.1 $  $Date: 1993/11/02 18:58:13 $
--
--------------------------------------------------------------------------
--
--  Entity declaration for transparent latch.
--


use work.dlx_types.all;

entity latch is
  generic (Tpd : Time;
           tag : string := "";
           origin_x, origin_y : real := 0.0);
  port (d : in dlx_word;
      	q : out dlx_word;
	latch_en : in bit);
end latch;
