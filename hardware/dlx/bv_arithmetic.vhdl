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
--  $RCSfile: bv_arithmetic.vhdl,v $  $Revision: 2.1 $  $Date: 1993/10/31 20:10:41 $
--
--------------------------------------------------------------------------
--
--  Bit vector arithmetic package specification.
--  
--  Does arithmetic and logical operations on bit vectors, treating them
--  as either unsigned or signed (2's complement) integers.  Leftmost bit
--  is most significant or sign bit, rightmost bit is least significant
--  bit.  Dyadic operations need the two arguments to be of the same
--  length, however their index ranges and directions may differ.  Results
--  must be of the same length as the operands.
--  
--------------------------------------------------------------------------

use std.textio.line;

package bv_arithmetic is
  
  ----------------------------------------------------------------
  --  Type conversions
  ----------------------------------------------------------------

  -- Convert bit vector encoded unsigned integer to natural.

  function bv_to_natural (bv : in bit_vector) return natural;
  
  -- Convert natural to bit vector encoded unsigned integer.
  -- (length is used as the size of the result.)

  function natural_to_bv (nat : in natural;
      	      	      	  length : in natural) return bit_vector;

  -- Convert bit vector encoded signed integer to integer

  function bv_to_integer (bv : in bit_vector) return integer;
  
  -- Convert integer to bit vector encoded signed integer.
  -- (length is used as the size of the result.)

  function integer_to_bv (int : in integer;
      	      	      	  length : in natural) return bit_vector;

  ----------------------------------------------------------------
  --  Arithmetic operations
  ----------------------------------------------------------------

  -- Signed addition with overflow detection

  procedure bv_add (bv1, bv2 : in bit_vector;
      	       	    bv_result : out bit_vector;
		    overflow : out boolean);

  -- Signed addition without overflow detection

  function "+" (bv1, bv2 : in bit_vector) return bit_vector;

  -- Signed subtraction with overflow detection

  procedure bv_sub (bv1, bv2 : in bit_vector;
      	       	    bv_result : out bit_vector;
		    overflow : out boolean);

  -- Signed subtraction without overflow detection

  function "-" (bv1, bv2 : in bit_vector) return bit_vector;

  -- Unsigned addition with overflow detection

  procedure bv_addu (bv1, bv2 : in bit_vector;
      	       	     bv_result : out bit_vector;
		     overflow : out boolean);

  -- Unsigned addition without overflow detection

  procedure bv_addu (bv1, bv2 : in bit_vector;
      	       	     bv_result : out bit_vector);

  -- Unsigned subtraction with overflow detection

  procedure bv_subu (bv1, bv2 : in bit_vector;
      	       	     bv_result : out bit_vector;
		     overflow : out boolean);

  -- Unsigned subtraction without overflow detection

  procedure bv_subu (bv1, bv2 : in bit_vector;
      	       	     bv_result : out bit_vector);

  -- Signed negation with overflow detection

  procedure bv_neg (bv : in bit_vector;
                    bv_result : out bit_vector;
                    overflow : out boolean);

  -- Signed negation without overflow detection

  function "-" (bv : in bit_vector) return bit_vector;

  -- Signed multiplication with overflow detection

  procedure bv_mult (bv1, bv2 : in bit_vector;
      	       	     bv_result : out bit_vector;
		     overflow : out boolean);

  -- Signed multiplication without overflow detection

  function "*" (bv1, bv2 : in bit_vector) return bit_vector;

  -- Unsigned multiplication with overflow detection

  procedure bv_multu (bv1, bv2 : in bit_vector;
      	       	      bv_result : out bit_vector;
		      overflow : out boolean);

  -- Unsigned multiplication without overflow detection

  procedure bv_multu (bv1, bv2 : in bit_vector;
      	       	      bv_result : out bit_vector);

  -- Signed division with divide by zero and overflow detection

  procedure bv_div (bv1, bv2 : in bit_vector;
      	       	    bv_result : out bit_vector;
		    div_by_zero : out boolean;
                    overflow : out boolean);

  -- Signed division without divide by zero and overflow detection

  function "/" (bv1, bv2 : in bit_vector) return bit_vector;

  -- Unsigned division with divide by zero detection

  procedure bv_divu (bv1, bv2 : in bit_vector;
      	       	     bv_result : out bit_vector;
		     div_by_zero : out boolean);

  -- Unsigned division without divide by zero detection

  procedure bv_divu (bv1, bv2 : in bit_vector;
      	       	     bv_result : out bit_vector);

  ----------------------------------------------------------------
  --  Logical operators
  --  (Provided for VHDL-87, built in for VHDL-93)
  ----------------------------------------------------------------

  -- Shift left logical (fill with '0' bits)

  function bv_sll (bv : in bit_vector;
       	      	   shift_count : in natural) return bit_vector;

  -- Shift right logical (fill with '0' bits)

  function bv_srl (bv : in bit_vector;
      	      	   shift_count : in natural) return bit_vector;

  -- Shift right arithmetic (fill with copy of sign bit)

  function bv_sra (bv : in bit_vector;
      	      	   shift_count : in natural) return bit_vector;

  -- Rotate left

  function bv_rol (bv : in bit_vector;
       	           rotate_count : in natural) return bit_vector;

  -- Rotate right

  function bv_ror (bv : in bit_vector;
      	      	   rotate_count : in natural) return bit_vector;

  ----------------------------------------------------------------
  --  Arithmetic comparison operators.
  --  Perform comparisons on bit vector encoded signed integers.
  --  (For unsigned integers, built in lexical comparison does
  --  the required operation.)
  ----------------------------------------------------------------

  -- Signed less than comparison

  function bv_lt (bv1, bv2 : in bit_vector) return boolean;
  
  -- Signed less than or equal comparison

  function bv_le (bv1, bv2 : in bit_vector) return boolean;
  
  -- Signed greater than comparison

  function bv_gt (bv1, bv2 : in bit_vector) return boolean;
  
  -- Signed greater than or equal comparison

  function bv_ge (bv1, bv2 : in bit_vector) return boolean;
  
  ----------------------------------------------------------------
  --  Extension operators - convert a bit vector to a longer one
  ----------------------------------------------------------------

  -- Sign extension - replicate the sign bit of the operand into
  -- the most significant bits of the result.  Length parameter
  -- determines size of result.  If length < bv'length, result is
  -- rightmost length bits of bv.

  function bv_sext (bv : in bit_vector;
      	      	    length : in natural) return bit_vector;
  
  -- Zero extension - replicate zero bits into the most significant
  -- bits of the result.  Length parameter determines size of result.
  -- If length < bv'length, result is rightmost length bits of bv.

  function bv_zext (bv : in bit_vector;
      	      	    length : in natural) return bit_vector;

end bv_arithmetic;
