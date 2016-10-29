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
--  $RCSfile: alu_types.vhdl,v $  $Revision: 2.1 $  $Date: 1993/11/02 18:48:41 $
--
--------------------------------------------------------------------------
--
--  Package defining types for ALU.
--


package alu_types is
  
  type alu_func is (alu_add, alu_addu, alu_sub, alu_subu, 
      	      	    alu_and, alu_or, alu_xor,
      	      	    alu_sll, alu_srl, alu_sra, 
		    alu_pass_s1, alu_pass_s2);

end alu_types;
