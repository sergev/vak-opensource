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
--  $RCSfile: images.vhdl,v $  $Revision: 1.1 $  $Date: 1993/10/25 20:45:45 $
--
--------------------------------------------------------------------------
--
--  Images package specification.
--
--  Functions that return the string image of values.
--  Each image is a correctly formed literal according to the
--  rules of VHDL-93.
--
--------------------------------------------------------------------------

package images is


  -- Image of bit vector as binary bit string literal
  -- (in the format B"...")
  -- Length of result is bv'length + 3

  function image (bv : in bit_vector) return string;


  -- Image of bit vector as octal bit string literal
  -- (in the format O"...")
  -- Length of result is (bv'length+2)/3 + 3

  function image_octal (bv : in bit_vector) return string;


  -- Image of bit vector as hex bit string literal
  -- (in the format X"...")
  -- Length of result is (bv'length+3)/4 + 3

  function image_hex (bv : in bit_vector) return string;


end images;

