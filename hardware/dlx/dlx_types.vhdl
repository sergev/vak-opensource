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
--  $RCSfile: dlx_types.vhdl,v $  $Revision: 2.1 $  $Date: 1993/10/31 20:35:20 $
--
--------------------------------------------------------------------------
--
--  Package specification for types used in dlx model
--


package dlx_types is
  
  subtype dlx_word is bit_vector(0 to 31);              -- bit 0 is msb
  subtype dlx_halfword is bit_vector(0 to 15);          -- bit 0 is msb
  subtype dlx_byte is bit_vector(0 to 7);               -- bit 0 is msb
  
  type dlx_word_array is array (positive range <>) of dlx_word;
  function resolve_dlx_word (values : in dlx_word_array) return dlx_word;
  subtype dlx_word_bus is resolve_dlx_word dlx_word;
  
  subtype dlx_address is bit_vector(31 downto 0);       -- bit 0 is lsb
  
end dlx_types;
