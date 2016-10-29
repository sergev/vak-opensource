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
--  $RCSfile: bv_arithmetic-body.vhdl,v $  $Revision: 2.1 $  $Date: 1993/11/02 22:50:39 $
--
--------------------------------------------------------------------------
--
--  Bit vector arithmetic package body.
--  
--  Does arithmetic and logical operations on bit vectors, treating them
--  as either unsigned or signed (2's complement) integers.  Leftmost bit
--  is most significant or sign bit, rightmost bit is least significant
--  bit.  Dyadic operations need the two arguments to be of the same
--  length, however their index ranges and directions may differ.  Results
--  must be of the same length as the operands.
--  
--------------------------------------------------------------------------

package body bv_arithmetic is
  
  ----------------------------------------------------------------
  --  Type conversions
  ----------------------------------------------------------------

  ----------------------------------------------------------------
  -- bv_to_natural
  --
  -- Convert bit vector encoded unsigned integer to natural.
  ----------------------------------------------------------------

  function bv_to_natural(bv : in bit_vector) return natural is

    variable result : natural := 0;

  begin
    for index in bv'range loop
      result := result * 2 + bit'pos(bv(index));
    end loop;
    return result;
  end bv_to_natural;


  ----------------------------------------------------------------
  -- natural_to_bv
  --
  -- Convert natural to bit vector encoded unsigned integer.
  -- (length is used as the size of the result.)
  ----------------------------------------------------------------

  function natural_to_bv(nat : in natural;
      	      	      	 length : in natural) return bit_vector is

    variable temp : natural := nat;
    variable result : bit_vector(0 to length-1);

  begin
    for index in result'reverse_range loop
      result(index) := bit'val(temp rem 2);
      temp := temp / 2;
    end loop;
    return result;
  end natural_to_bv;


  ----------------------------------------------------------------
  -- bv_to_integer
  --
  -- Convert bit vector encoded signed integer to integer
  ----------------------------------------------------------------

  function bv_to_integer(bv : in bit_vector) return integer is

    variable temp : bit_vector(bv'range);
    variable result : integer := 0;

  begin
    if bv(bv'left) = '1' then	  -- negative number
      temp := not bv;
    else
      temp := bv;
    end if;
    for index in bv'range loop	  -- sign bit of temp = '0'
      result := result * 2 + bit'pos(temp(index));
    end loop;
    if bv(bv'left) = '1' then
      result := (-result) - 1;
    end if;
    return result;
  end bv_to_integer;


  ----------------------------------------------------------------
  -- integer_to_bv
  --
  -- Convert integer to bit vector encoded signed integer.
  -- (length is used as the size of the result.)
  ----------------------------------------------------------------

  function integer_to_bv(int : in integer;
      	      	      	 length : in natural) return bit_vector is

    variable temp : integer;
    variable result : bit_vector(0 to length-1);

  begin
    if int < 0 then
      temp := -(int+1);
    else
      temp := int;
    end if;
    for index in result'reverse_range loop
      result(index) := bit'val(temp rem 2);
      temp := temp / 2;
    end loop;
    if int < 0 then
      result := not result;
      result(result'left) := '1';
    end if;
    return result;
  end integer_to_bv;


  ----------------------------------------------------------------
  --  Arithmetic operations
  ----------------------------------------------------------------

  ----------------------------------------------------------------
  -- bv_add
  --
  -- Signed addition with overflow detection
  ----------------------------------------------------------------

  procedure bv_add (bv1, bv2 : in bit_vector;
      	       	    bv_result : out bit_vector;
		    overflow : out boolean) is

    alias op1 : bit_vector(1 to bv1'length) is bv1;
    alias op2 : bit_vector(1 to bv2'length) is bv2;  
    variable result : bit_vector(1 to bv_result'length);
    variable carry_in : bit;
    variable carry_out : bit := '0';

  begin
    assert bv1'length = bv2'length and bv1'length = bv_result'length
      report "bv_add: operands of different lengths"
      severity failure;
    for index in result'reverse_range loop
      carry_in := carry_out;  -- of previous bit
      result(index) := op1(index) xor op2(index) xor carry_in;
      carry_out := (op1(index) and op2(index))
      	      	   or (carry_in and (op1(index) xor op2(index)));
    end loop;
    bv_result := result;
    overflow := carry_out /= carry_in;
  end bv_add;


  ----------------------------------------------------------------
  -- "+"
  --
  -- Signed addition without overflow detection
  ----------------------------------------------------------------

  function "+" (bv1, bv2 : in bit_vector) return bit_vector is

    alias op1 : bit_vector(1 to bv1'length) is bv1;
    alias op2 : bit_vector(1 to bv2'length) is bv2;  
    variable result : bit_vector(1 to bv1'length);
    variable carry_in : bit;
    variable carry_out : bit := '0';

  begin
    assert bv1'length = bv2'length
      --  use concatenation to work around Synthesia MINT code gen bug
      report '"' & '+' & '"' & ": operands of different lengths"
      severity failure;
    for index in result'reverse_range loop
      carry_in := carry_out;  -- of previous bit
      result(index) := op1(index) xor op2(index) xor carry_in;
      carry_out := (op1(index) and op2(index))
      	      	   or (carry_in and (op1(index) xor op2(index)));
    end loop;
    return result;
  end "+";


  ----------------------------------------------------------------
  -- bv_sub
  --
  -- Signed subtraction with overflow detection
  ----------------------------------------------------------------

  procedure bv_sub (bv1, bv2 : in bit_vector;
      	       	    bv_result : out bit_vector;
		    overflow : out boolean) is

    -- subtraction implemented by adding ((not bv2) + 1), ie -bv2

    alias op1 : bit_vector(1 to bv1'length) is bv1;
    alias op2 : bit_vector(1 to bv2'length) is bv2;  
    variable result : bit_vector(1 to bv_result'length);
    variable carry_in : bit;
    variable carry_out : bit := '1';

  begin
    assert bv1'length = bv2'length and bv1'length = bv_result'length
      report "bv_sub: operands of different lengths"
      severity failure;
    for index in result'reverse_range loop
      carry_in := carry_out;  -- of previous bit
      result(index) := op1(index) xor (not op2(index)) xor carry_in;
      carry_out := (op1(index) and (not op2(index)))
      	      	   or (carry_in and (op1(index) xor (not op2(index))));
    end loop;
    bv_result := result;
    overflow := carry_out /= carry_in;
  end bv_sub;


  ----------------------------------------------------------------
  -- "-"
  --
  -- Signed subtraction without overflow detection
  ----------------------------------------------------------------

  function "-" (bv1, bv2 : in bit_vector) return bit_vector is

    -- subtraction implemented by adding ((not bv2) + 1), ie -bv2

    alias op1 : bit_vector(1 to bv1'length) is bv1;
    alias op2 : bit_vector(1 to bv2'length) is bv2;  
    variable result : bit_vector(1 to bv1'length);
    variable carry_in : bit;
    variable carry_out : bit := '1';

  begin
    assert bv1'length = bv2'length
      --  use concatenation to work around Synthesia MINT code gen bug
      report '"' & '-' & '"' & ": operands of different lengths"
      severity failure;
    for index in result'reverse_range loop
      carry_in := carry_out;  -- of previous bit
      result(index) := op1(index) xor (not op2(index)) xor carry_in;
      carry_out := (op1(index) and (not op2(index)))
      	      	   or (carry_in and (op1(index) xor (not op2(index))));
    end loop;
    return result;
  end "-";


  ----------------------------------------------------------------
  -- bv_addu
  --
  -- Unsigned addition with overflow detection
  ----------------------------------------------------------------

  procedure bv_addu (bv1, bv2 : in bit_vector;
      	       	     bv_result : out bit_vector;
		     overflow : out boolean) is

    alias op1 : bit_vector(1 to bv1'length) is bv1;
    alias op2 : bit_vector(1 to bv2'length) is bv2;  
    variable result : bit_vector(1 to bv_result'length);
    variable carry : bit := '0';

  begin
    assert bv1'length = bv2'length and bv1'length = bv_result'length
      report "bv_addu: operands of different lengths"
      severity failure;
    for index in result'reverse_range loop
      result(index) := op1(index) xor op2(index) xor carry;
      carry := (op1(index) and op2(index))
               or (carry and (op1(index) xor op2(index)));
    end loop;
    bv_result := result;
    overflow := carry = '1';
  end bv_addu;


  ----------------------------------------------------------------
  -- bv_addu
  --
  -- Unsigned addition without overflow detection
  ----------------------------------------------------------------

  procedure bv_addu (bv1, bv2 : in bit_vector;
      	       	     bv_result : out bit_vector) is

    alias op1 : bit_vector(1 to bv1'length) is bv1;
    alias op2 : bit_vector(1 to bv2'length) is bv2;  
    variable result : bit_vector(1 to bv_result'length);
    variable carry : bit := '0';

  begin
    assert bv1'length = bv2'length and bv1'length = bv_result'length
      report "bv_addu: operands of different lengths"
      severity failure;
    for index in result'reverse_range loop
      result(index) := op1(index) xor op2(index) xor carry;
      carry := (op1(index) and op2(index))
               or (carry and (op1(index) xor op2(index)));
    end loop;
    bv_result := result;
  end bv_addu;


  ----------------------------------------------------------------
  -- bv_subu
  --
  -- Unsigned subtraction with overflow detection
  ----------------------------------------------------------------

  procedure bv_subu (bv1, bv2 : in bit_vector;
      	       	     bv_result : out bit_vector;
		     overflow : out boolean) is

    alias op1 : bit_vector(1 to bv1'length) is bv1;
    alias op2 : bit_vector(1 to bv2'length) is bv2;  
    variable result : bit_vector(1 to bv_result'length);
    variable borrow : bit := '0';

  begin
    assert bv1'length = bv2'length and bv1'length = bv_result'length
      report "bv_subu: operands of different lengths"
      severity failure;
    for index in result'reverse_range loop
      result(index) := op1(index) xor op2(index) xor borrow;
      borrow := (not op1(index) and op2(index))
      	      	or (borrow and not (op1(index) xor op2(index)));
    end loop;
    bv_result := result;
    overflow := borrow = '1';
  end bv_subu;


  ----------------------------------------------------------------
  -- bv_subu
  --
  -- Unsigned subtraction without overflow detection
  ----------------------------------------------------------------

  procedure bv_subu (bv1, bv2 : in bit_vector;
      	       	     bv_result : out bit_vector) is

    alias op1 : bit_vector(1 to bv1'length) is bv1;
    alias op2 : bit_vector(1 to bv2'length) is bv2;  
    variable result : bit_vector(1 to bv_result'length);
    variable borrow : bit := '0';

  begin
    assert bv1'length = bv2'length and bv1'length = bv_result'length
      report "bv_subu: operands of different lengths"
      severity failure;
    for index in result'reverse_range loop
      result(index) := op1(index) xor op2(index) xor borrow;
      borrow := (not op1(index) and op2(index))
      	      	or (borrow and not (op1(index) xor op2(index)));
    end loop;
    bv_result := result;
  end bv_subu;


  ----------------------------------------------------------------
  -- bv_neg
  --
  -- Signed negation with overflow detection
  ----------------------------------------------------------------

  procedure bv_neg (bv : in bit_vector;
                    bv_result : out bit_vector;
                    overflow : out boolean) is

    CONSTANT zero : bit_vector(bv'range) := (others => '0');

  begin
    bv_sub( zero, bv, bv_result, overflow );
  end bv_neg;


  ----------------------------------------------------------------
  -- "-"
  --
  -- Signed negation without overflow detection
  ----------------------------------------------------------------

  function "-" (bv : in bit_vector) return bit_vector is

    CONSTANT zero : bit_vector(bv'range) := (others => '0');

  begin
    return zero - bv;
  end "-";


  ----------------------------------------------------------------
  -- bv_mult
  --
  -- Signed multiplication with overflow detection
  ----------------------------------------------------------------

  procedure bv_mult (bv1, bv2 : in bit_vector;
      	       	     bv_result : out bit_vector;
		     overflow : out boolean) is

    variable    negative_result  : boolean;
    variable    op1              : bit_vector(bv1'range) := bv1;
    variable    op2              : bit_vector(bv2'range) := bv2;
    variable    multu_result     : bit_vector(bv1'range);
    variable    multu_overflow   : boolean;
    --  constant    abs_min_int      : bit_vector(bv1'range)
    --                                 := (bv1'left => '1', others => '0');
    --  causes Synthesia MINT code generator to prang.  Work around:
    variable    abs_min_int      : bit_vector(bv1'range) := (others => '0');

  begin
    assert bv1'length = bv2'length and bv1'length = bv_result'length
      report "bv_mult: operands of different lengths"
      severity failure;
    abs_min_int(bv1'left) := '1';        -- Synthesia work around
    negative_result := (op1(op1'left) = '1') xor (op2(op2'left) = '1');
    if (op1(op1'left) = '1') then
      op1 := - bv1;
    end if;
    if (op2(op2'left) = '1') then
      op2 := - bv2;
    end if;
    bv_multu(op1, op2, multu_result, multu_overflow);
    if (negative_result) then
      overflow := multu_overflow or (multu_result > abs_min_int);
      bv_result := - multu_result;
    else
      overflow := multu_overflow or (multu_result(multu_result'left) = '1');
      bv_result := multu_result;
    end if;
  end bv_mult;


  ----------------------------------------------------------------
  -- "*"
  --
  -- Signed multiplication without overflow detection
  ----------------------------------------------------------------

  function "*" (bv1, bv2 : in bit_vector) return bit_vector is

    variable    negative_result  : boolean;
    variable    op1              : bit_vector(bv1'range) := bv1;
    variable    op2              : bit_vector(bv2'range) := bv2;
    variable    result           : bit_vector(bv1'range);
    
  begin
    assert bv1'length = bv2'length
      --  use concatenation to work around Synthesia MINT code gen bug
      report '"' & '*' & '"' & ": operands of different lengths"
      severity failure;
    negative_result := (op1(op1'left) = '1') xor (op2(op2'left) = '1');
    if (op1(op1'left) = '1') then
      op1 := - bv1;
    end if;
    if (op2(op2'left) = '1') then
      op2 := - bv2;
    end if;
    bv_multu(op1, op2, result);
    if (negative_result) then
      result := - result;
    end if;
    return result;
  end "*";


  ----------------------------------------------------------------
  -- bv_multu
  --
  -- Unsigned multiplication with overflow detection
  ----------------------------------------------------------------

  procedure bv_multu (bv1, bv2 : in bit_vector;
      	       	      bv_result : out bit_vector;
		      overflow : out boolean) is

    --  Based on shift&add multiplier in Appendix A of Hennessy & Patterson

    constant    bv_length        : natural := bv1'length;
    constant    accum_length     : natural := bv_length * 2;
    constant    zero             : bit_vector(accum_length-1 downto bv_length)
                                   := (others => '0');
    variable    accum            : bit_vector(accum_length-1 downto 0);
    variable    addu_overflow    : boolean;
    variable    carry            : bit;

  begin
    assert bv1'length = bv2'length and bv1'length = bv_result'length
      report "bv_multu: operands of different lengths"
      severity failure;
    accum(bv_length-1 downto 0) := bv1;
    accum(accum_length-1 downto bv_length) := zero;
    for count in 1 to bv_length loop
      if (accum(0) = '1') then
        bv_addu( accum(accum_length-1 downto bv_length), bv2,
                 accum(accum_length-1 downto bv_length), addu_overflow);
        carry := bit'val(boolean'pos(addu_overflow));
      else
        carry := '0';
      end if;
      accum := carry & accum(accum_length-1 downto 1);
    end loop;
    bv_result := accum(bv_length-1 downto 0);
    overflow := accum(accum_length-1 downto bv_length) /= zero;
  end bv_multu;


  ----------------------------------------------------------------
  --  bv_multu
  --
  -- Unsigned multiplication without overflow detection
  ----------------------------------------------------------------

  procedure bv_multu (bv1, bv2 : in bit_vector;
      	       	      bv_result : out bit_vector) is

    -- Use bv_multu with overflow detection, but ignore overflow flag

    variable    tmp_overflow : boolean;

  begin
    -- following procedure asserts bv1'length = bv2'length
    bv_multu(bv1, bv2, bv_result, tmp_overflow);
  end bv_multu;


  ----------------------------------------------------------------
  -- bv_div
  --
  -- Signed division with divide by zero and overflow detection
  ----------------------------------------------------------------

  procedure bv_div (bv1, bv2 : in bit_vector;
      	       	    bv_result : out bit_vector;
		    div_by_zero : out boolean;
                    overflow : out boolean) is

    --  Need overflow, in case divide b"10...0" (min_int) by -1
    --  Don't use bv_to_int, in case size bigger than host machine!

    variable    negative_result  : boolean;
    variable    op1              : bit_vector(bv1'range) := bv1;
    variable    op2              : bit_vector(bv2'range) := bv2;
    variable    divu_result      : bit_vector(bv1'range);

  begin
    assert bv1'length = bv2'length
      report "bv_div: operands of different lengths"
      severity failure;
    negative_result := (op1(op1'left) = '1') xor (op2(op2'left) = '1');
    if (op1(op1'left) = '1') then
      op1 := - bv1;
    end if;
    if (op2(op2'left) = '1') then
      op2 := - bv2;
    end if;
    bv_divu(op1, op2, divu_result, div_by_zero);
    if (negative_result) then
      overflow := false;
      bv_result := - divu_result;
    else
      overflow := divu_result(divu_result'left) = '1';
      bv_result := divu_result;
    end if;
  end bv_div;


  ----------------------------------------------------------------
  -- "/"
  --
  -- Signed division without divide by zero and overflow detection
  ----------------------------------------------------------------

  function "/" (bv1, bv2 : in bit_vector) return bit_vector is

    variable    negative_result  : boolean;
    variable    op1              : bit_vector(bv1'range) := bv1;
    variable    op2              : bit_vector(bv2'range) := bv2;
    variable    result           : bit_vector(bv1'range);

  begin
    assert bv1'length = bv2'length
      --  use concatenation to work around Synthesia MINT code gen bug
      report '"' & '/' & '"' & ": operands of different lengths"
      severity failure;
    negative_result := (op1(op1'left) = '1') xor (op2(op2'left) = '1');
    if (op1(op1'left) = '1') then
      op1 := - bv1;
    end if;
    if (op2(op2'left) = '1') then
      op2 := - bv2;
    end if;
    bv_divu(op1, op2, result);
    if (negative_result) then
      result := - result;
    end if;
    return result;
  end "/";


  ----------------------------------------------------------------
  -- bv_divu
  --
  -- Unsigned division with divide by zero detection
  ----------------------------------------------------------------

  procedure bv_divu (bv1, bv2 : in bit_vector;
      	       	     bv_result : out bit_vector;
		     div_by_zero : out boolean) is

    --  based on algorithm in Sun Sparc architecture manual

    constant    len              : natural := bv1'length;
    variable    zero, one,
                big_value        : bit_vector(len-1 downto 0)
				     := (others => '0');
    variable    dividend         : bit_vector(bv1'length-1 downto 0) := bv1;
    variable    divisor          : bit_vector(bv2'length-1 downto 0) := bv2;
    variable    quotient         : bit_vector(len-1 downto 0);  --  unsigned
    variable    remainder        : bit_vector(len-1 downto 0);  --  signed
    variable    shifted_divisor,
                shifted_1        : bit_vector(len-1 downto 0);
    variable    log_quotient     : natural;
    variable    ignore_overflow  : boolean;

  begin
    assert bv1'length = bv2'length
      report "bv_divu: operands of different lengths"
      severity failure;
    one(0) := '1';
    big_value(len-2) := '1';
    --  
    --  check for zero divisor
    --  
    if (divisor = zero) then
      div_by_zero := true;
      return;
    end if;
    --  
    --  estimate log of quotient
    --
    log_quotient := 0;
    shifted_divisor := divisor;
    loop 
      exit when (log_quotient >= len)
                or (shifted_divisor > big_value)
                or (shifted_divisor >= dividend);
      log_quotient := log_quotient + 1;
      shifted_divisor := bv_sll(shifted_divisor, 1);
    end loop;
    --
    --  perform division
    --
    remainder := dividend;
    quotient := zero;
    shifted_divisor := bv_sll(divisor, log_quotient);
    shifted_1 := bv_sll(one, log_quotient);
    for iter in log_quotient downto 0 loop
      if bv_ge(remainder, zero) then
        bv_sub(remainder, shifted_divisor, remainder, ignore_overflow);
        bv_addu(quotient, shifted_1, quotient, ignore_overflow);
      else
        bv_add(remainder, shifted_divisor, remainder, ignore_overflow);
        bv_subu(quotient, shifted_1, quotient, ignore_overflow);
      end if;
      shifted_divisor := '0' & shifted_divisor(len-1 downto 1);
      shifted_1 := '0' & shifted_1(len-1 downto 1);
    end loop;
    if (bv_lt(remainder, zero)) then
      bv_add(remainder, divisor, remainder, ignore_overflow);
      bv_subu(quotient, one, quotient, ignore_overflow);
    end if;
    bv_result := quotient;
  end bv_divu;


  ----------------------------------------------------------------
  -- bv_divu
  --
  -- Unsigned division without divide by zero detection
  ----------------------------------------------------------------

  procedure bv_divu (bv1, bv2 : in bit_vector;
      	       	     bv_result : out bit_vector) is

    -- Use bv_divu with divide by zero detection,
    -- but ignore div_by_zero flag

    variable tmp_div_by_zero : boolean;

  begin
    -- following procedure asserts bv1'length = bv2'length
    bv_divu(bv1, bv2, bv_result, tmp_div_by_zero);
  end bv_divu;


  ----------------------------------------------------------------
  --  Logical operators
  --  (Provided for VHDL-87, built in for VHDL-93)
  ----------------------------------------------------------------

  ----------------------------------------------------------------
  -- bv_sll
  --
  -- Shift left logical (fill with '0' bits)
  ----------------------------------------------------------------

  function bv_sll (bv : in bit_vector;
       	      	   shift_count : in natural) return bit_vector is

    constant bv_length : natural := bv'length;
    constant actual_shift_count : natural := shift_count mod bv_length;
    alias bv_norm : bit_vector(1 to bv_length) is bv;
    variable result : bit_vector(1 to bv_length) := (others => '0');

  begin
    result(1 to bv_length - actual_shift_count)
      := bv_norm(actual_shift_count + 1 to bv_length);
    return result;
  end bv_sll;


  ----------------------------------------------------------------
  -- bv_srl
  --
  -- Shift right logical (fill with '0' bits)
  ----------------------------------------------------------------

  function bv_srl (bv : in bit_vector;
      	      	   shift_count : in natural) return bit_vector is

    constant bv_length : natural := bv'length;
    constant actual_shift_count : natural := shift_count mod bv_length;
    alias bv_norm : bit_vector(1 to bv_length) is bv;
    variable result : bit_vector(1 to bv_length) := (others => '0');

  begin
    result(actual_shift_count + 1 to bv_length)
      := bv_norm(1 to bv_length - actual_shift_count);
    return result;
  end bv_srl;


  ----------------------------------------------------------------
  -- bv_sra
  --
  -- Shift right arithmetic (fill with copy of sign bit)
  ----------------------------------------------------------------


  function bv_sra (bv : in bit_vector;
      	      	   shift_count : in natural) return bit_vector is

    constant bv_length : natural := bv'length;
    constant actual_shift_count : natural := shift_count mod bv_length;
    alias bv_norm : bit_vector(1 to bv_length) is bv;
    variable result : bit_vector(1 to bv_length) := (others => bv(bv'left));

  begin
    result(actual_shift_count + 1 to bv_length)
      := bv_norm(1 to bv_length - actual_shift_count);
    return result;
  end bv_sra;


  ----------------------------------------------------------------
  -- bv_rol
  --
  -- Rotate left
  ----------------------------------------------------------------

  function bv_rol (bv : in bit_vector;
       	           rotate_count : in natural) return bit_vector is

    constant bv_length : natural := bv'length;
    constant actual_rotate_count : natural := rotate_count mod bv_length;
    alias bv_norm : bit_vector(1 to bv_length) is bv;
    variable result : bit_vector(1 to bv_length);

  begin
    result(1 to bv_length - actual_rotate_count)
      := bv_norm(actual_rotate_count + 1 to bv_length);
    result(bv_length - actual_rotate_count + 1 to bv_length)
      := bv_norm(1 to actual_rotate_count);
    return result;
  end bv_rol;


  ----------------------------------------------------------------
  -- bv_ror
  --
  -- Rotate right
  ----------------------------------------------------------------

  function bv_ror (bv : in bit_vector;
       	           rotate_count : in natural) return bit_vector is

    constant bv_length : natural := bv'length;
    constant actual_rotate_count : natural := rotate_count mod bv_length;
    alias bv_norm : bit_vector(1 to bv_length) is bv;
    variable result : bit_vector(1 to bv_length);

  begin
    result(actual_rotate_count + 1 to bv_length)
      := bv_norm(1 to bv_length - actual_rotate_count);
    result(1 to actual_rotate_count)
      := bv_norm(bv_length - actual_rotate_count + 1 to bv_length);
    return result;
  end bv_ror;


  ----------------------------------------------------------------
  --  Arithmetic comparison operators.
  --  Perform comparisons on bit vector encoded signed integers.
  --  (For unsigned integers, built in lexical comparison does
  --  the required operation.)
  ----------------------------------------------------------------

  ----------------------------------------------------------------
  -- bv_lt
  --
  -- Signed less than comparison
  ----------------------------------------------------------------

  function bv_lt (bv1, bv2 : in bit_vector) return boolean is

    variable tmp1 : bit_vector(bv1'range) := bv1;
    variable tmp2 : bit_vector(bv2'range) := bv2;

  begin
    assert bv1'length = bv2'length
      report "bv_lt: operands of different lengths"
      severity failure;
    tmp1(tmp1'left) := not tmp1(tmp1'left);
    tmp2(tmp2'left) := not tmp2(tmp2'left);
    return tmp1 < tmp2;
  end bv_lt;
  

  ----------------------------------------------------------------
  -- bv_le
  --
  -- Signed less than or equal comparison
  ----------------------------------------------------------------

  function bv_le (bv1, bv2 : in bit_vector) return boolean is

    variable tmp1 : bit_vector(bv1'range) := bv1;
    variable tmp2 : bit_vector(bv2'range) := bv2;

  begin
    assert bv1'length = bv2'length
      report "bv_le: operands of different lengths"
      severity failure;
    tmp1(tmp1'left) := not tmp1(tmp1'left);
    tmp2(tmp2'left) := not tmp2(tmp2'left);
    return tmp1 <= tmp2;
  end bv_le;


  ----------------------------------------------------------------
  -- bv_gt
  --
  -- Signed greater than comparison
  ----------------------------------------------------------------

  function bv_gt (bv1, bv2 : in bit_vector) return boolean is

    variable tmp1 : bit_vector(bv1'range) := bv1;
    variable tmp2 : bit_vector(bv2'range) := bv2;

  begin
    assert bv1'length = bv2'length
      report "bv_gt: operands of different lengths"
      severity failure;
    tmp1(tmp1'left) := not tmp1(tmp1'left);
    tmp2(tmp2'left) := not tmp2(tmp2'left);
    return tmp1 > tmp2;
  end bv_gt;
  

  ----------------------------------------------------------------
  -- bv_ge
  --
  -- Signed greater than or equal comparison
  ----------------------------------------------------------------

  function bv_ge (bv1, bv2 : in bit_vector) return boolean is

    variable tmp1 : bit_vector(bv1'range) := bv1;
    variable tmp2 : bit_vector(bv2'range) := bv2;

  begin
    assert bv1'length = bv2'length
      report "bv_ged: operands of different lengths"
      severity failure;
    tmp1(tmp1'left) := not tmp1(tmp1'left);
    tmp2(tmp2'left) := not tmp2(tmp2'left);
    return tmp1 >= tmp2;
  end bv_ge;
  

  ----------------------------------------------------------------
  --  Extension operators - convert a bit vector to a longer one
  ----------------------------------------------------------------

  ----------------------------------------------------------------
  -- bv_sext
  --
  -- Sign extension - replicate the sign bit of the operand into
  -- the most significant bits of the result.  Length parameter
  -- determines size of result.  If length < bv'length, result is
  -- rightmost length bits of bv.
  ----------------------------------------------------------------

  function bv_sext (bv : in bit_vector;
      	      	    length : in natural) return bit_vector is

    alias bv_norm : bit_vector(1 to bv'length) is bv;
    variable result : bit_vector(1 to length) := (others => bv(bv'left));
    variable src_length : natural := bv'length;

  begin
    if src_length > length then
      src_length := length;
    end if;
    result(length - src_length + 1 to length)
      := bv_norm(bv'length - src_length + 1 to bv'length);
    return result;
  end bv_sext;


  ----------------------------------------------------------------
  -- bv_zext
  --
  -- Zero extension - replicate zero bits into the most significant
  -- bits of the result.  Length parameter determines size of result.
  -- If length < bv'length, result is rightmost length bits of bv.
  ----------------------------------------------------------------

  function bv_zext (bv : in bit_vector;
      	      	    length : in natural) return bit_vector is

    alias bv_norm : bit_vector(1 to bv'length) is bv;
    variable result : bit_vector(1 to length) := (others => '0');
    variable src_length : natural := bv'length;

  begin
    if src_length > length then
      src_length := length;
    end if;
    result(length - src_length + 1 to length)
      := bv_norm(bv'length - src_length + 1 to bv'length);
    return result;
  end bv_zext;


end bv_arithmetic;
