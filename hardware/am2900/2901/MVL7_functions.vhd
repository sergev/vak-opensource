--***************************************************************************
--              VHDL BIT_VECTOR Operations for MVL7 type
--                                                                         
--         Copyright (c) 1990   UCI CADLAB, Dept. of ICS                  
--         Author :  Sanjiv Narayan (narayan@ics.uci.edu)                
--
--         Updated 7/8/91 by Sanjiv and Frank 
-- 
--         updated by Champaka Ramachandran 16/10/92
--
--***************************************************************************
use work.TYPES.all;

package MVL7_functions is
   function SHL( v2 :  MVL7_VECTOR ;  fill :  MVL7 ) return MVL7_VECTOR;
   function SHL0( v2 :  MVL7_VECTOR ;  dist :  INTEGER ) return MVL7_VECTOR;
   function SHL1( v2 :  MVL7_VECTOR ;  dist :  INTEGER ) return MVL7_VECTOR;
   function SHR( v2 :  MVL7_VECTOR ;  fill :  MVL7 ) return MVL7_VECTOR;
   function SHR0( v2 :  MVL7_VECTOR ;  dist :  INTEGER ) return MVL7_VECTOR;
   function SHR1( v2 :  MVL7_VECTOR ;  dist :  INTEGER ) return MVL7_VECTOR;
   function ROTR( v2 :  MVL7_VECTOR ;  dist :  INTEGER ) return MVL7_VECTOR;
   function ROTL( v2 :  MVL7_VECTOR ;  dist :  INTEGER ) return MVL7_VECTOR;
   function I2B( Number :  integer ;  len :  integer ) return MVL7_VECTOR;
   function B2I( v2 :  MVL7_VECTOR ) return integer;
   function COMP( v2 :  MVL7_VECTOR ) return MVL7_VECTOR;
   function TWOs_COMP( v2 :  MVL7_VECTOR ) return MVL7_VECTOR;
   function ODD_PARITY( v1 :  MVL7_VECTOR ) return MVL7;
   function EVEN_PARITY( v1 :  MVL7_VECTOR ) return MVL7;
   function REVERSE( v2 :  MVL7_VECTOR ) return MVL7_VECTOR;
   function SUM( v2 :  MVL7_VECTOR ) return integer;
   function PAD( v :  MVL7_VECTOR ;  width :  integer ) return MVL7_VECTOR;
   function DEC( x :  MVL7_VECTOR ) return MVL7_VECTOR;
   function INC( x :  MVL7_VECTOR ) return MVL7_VECTOR;
   function CARRY_ADD( x1 :  MVL7_VECTOR ;  x2 :  MVL7_VECTOR ) return MVL7_VECTOR;
   function "+" ( x1 :  MVL7_VECTOR ;  x2 :  MVL7_VECTOR ) return MVL7_VECTOR;
   function "-"( x1 :  MVL7_VECTOR ;  x2 :  MVL7_VECTOR ) return MVL7_VECTOR;
   function "*" ( x1 :  MVL7_VECTOR ;  x2 :  MVL7_VECTOR ) return MVL7_VECTOR;

-- truth table for "WiredOr" function
   constant tbl_WIREDOR: MVL7_TABLE := 
--  -----------------------------------------------
--  |  X    0    1    Z    W    L    H |          |
--  -----------------------------------------------
    (('X', 'X', '1', 'X', 'X', 'L', 'H'),  -- | X |
     ('X', '0', '1', '0', '0', 'L', 'H'),  -- | 0 |
     ('1', '1', '1', '1', '1', '1', '1'),  -- | 1 |
     ('X', '0', '1', 'Z', 'W', 'L', 'H'),  -- | Z |
     ('X', '0', '1', 'W', 'W', 'W', 'W'),  -- | W |
     ('L', 'L', '1', 'L', 'W', 'L', 'W'),  -- | L |
     ('H', 'H', '1', 'H', 'W', 'W', 'H')); -- | H |

  function WiredOr (V: MVL7_VECTOR) return MVL7;

end;

package body MVL7_functions is

--***************************************************************************
   function SHL( v2 :  MVL7_VECTOR ;  fill :  MVL7 ) return MVL7_VECTOR is
      variable v1: MVL7_VECTOR (v2'high downto v2'low);
      variable shift_val: MVL7_VECTOR (v1'high downto v1'low);
      variable I: integer;
   begin
      v1 := v2;
      for I in v1'high downto (v1'low + 1) loop
         shift_val(I) := v1(I - 1);
      end loop ;
      shift_val(v1'low) := fill;
      return shift_val;
   end;

--***************************************************************************
   function SHL0( v2 :  MVL7_VECTOR ;  dist :  INTEGER ) return MVL7_VECTOR is
      variable v1: MVL7_VECTOR (v2'high downto v2'low);
      variable I: INTEGER;
   begin
      v1 := v2;
      for I in 1 to dist loop
         v1 := SHL(v1,'0');
      end loop ;
      return v1;
   end;

--***************************************************************************
   function SHL1( v2 :  MVL7_VECTOR ;  dist :  INTEGER ) return MVL7_VECTOR is
      variable v1: MVL7_VECTOR (v2'high downto v2'low);
      variable I: INTEGER;
   begin
      v1 := v2;
      for I in 1 to dist loop
         v1 := SHL(v1,'1');
      end loop ;
      return v1;
   end;

--***************************************************************************
   function SHR( v2 :  MVL7_VECTOR ;  fill :  MVL7 ) return MVL7_VECTOR is
      variable v1: MVL7_VECTOR (v2'high downto v2'low);
      variable shift_val: MVL7_VECTOR (v1'high downto v1'low);
   begin
      v1 := v2;
      for I in v1'low to (v1'high - 1) loop
         shift_val(I) := v1(I + 1);
      end loop ;
      shift_val(v1'high) := fill;
      return shift_val;
   end;

--***************************************************************************
   function SHR0( v2 :  MVL7_VECTOR ;  dist :  INTEGER ) return MVL7_VECTOR is
      variable v1: MVL7_VECTOR (v2'high downto v2'low);
      variable I: INTEGER;
   begin
      v1 := v2;
      for I in 1 to dist loop
         v1 := SHR(v1,'0');
      end loop ;
      return v1;
   end;

--***************************************************************************
   function SHR1( v2 :  MVL7_VECTOR ;  dist :  INTEGER ) return MVL7_VECTOR is
      variable v1: MVL7_VECTOR (v2'high downto v2'low);
      variable I: INTEGER;
   begin
      v1 := v2;
      for I in 1 to dist loop
         v1 := SHR(v1,'1');
      end loop ;
      return v1;
   end;

--***************************************************************************
   function ROTR( v2 :  MVL7_VECTOR ;  dist :  INTEGER ) return MVL7_VECTOR is
      variable v1: MVL7_VECTOR (v2'high downto v2'low);
      variable I: INTEGER;
   begin
      v1 := v2;
      for i in 1 to dist loop
         v1 := SHR(v1,v1(v1'low));
      end loop ;
      return v1;
   end;

--***************************************************************************
   function ROTL( v2 :  MVL7_VECTOR ;  dist :  INTEGER ) return MVL7_VECTOR is
      variable v1: MVL7_VECTOR (v2'high downto v2'low);
      variable I: INTEGER;
   begin
      v1 := v2;
      for i in 1 to dist loop
         v1 := SHL(v1,v1(v1'high));
      end loop ;
      return v1;
   end;

--***************************************************************************
   function I2B( Number :  integer ;  len :  integer ) return MVL7_VECTOR is
      variable temp: MVL7_VECTOR (len - 1 downto 0);
      variable NUM: integer:=0;
      variable QUOTIENT: integer:=0;
   begin
      QUOTIENT := Number;
      for I in 0 to len - 1 loop
         NUM := 0;
         while QUOTIENT > 1 loop
            QUOTIENT := QUOTIENT - 2;
            NUM := NUM + 1;
         end loop ;
         case QUOTIENT is
            when 1 =>
               temp(I) := '1';
            when 0 =>
               temp(I) := '0';
            when others =>
               null;
         end case;
         QUOTIENT := NUM;
      end loop ;
      return temp;
   end;

--***************************************************************************
   function B2I( v2 :  MVL7_VECTOR ) return integer is
      variable v1: MVL7_VECTOR (v2'high downto v2'low);
      variable SUM: integer:=0;
   begin
      v1 := v2;
      for N in v1'low to v1'high loop
         if v1(N) = '1' then
            SUM := SUM + (2 ** (N - v1'low));
         end if;
      end loop ;
      return SUM;
   end;

--***************************************************************************
   function COMP( v2 :  MVL7_VECTOR ) return MVL7_VECTOR is
      variable v1: MVL7_VECTOR (v2'high downto v2'low);
      variable temp: MVL7_VECTOR (v1'high downto v1'low);
      variable I: INTEGER;
   begin
      v1 := v2;
      for I in v1'low to v1'high loop
         if v1(I) = '0' then
            temp(i) := '1';
         else
            temp(i) := '0';
         end if;
      end loop ;
      return temp;
   end;

--***************************************************************************
   function TWOs_COMP( v2 :  MVL7_VECTOR ) return MVL7_VECTOR is
      variable v1: MVL7_VECTOR (v2'high downto v2'low);
      variable temp: MVL7_VECTOR (v1'high downto v1'low);
   begin
      v1 := v2;
      temp := comp(v1);
      temp := INC(temp);
      return temp;
   end;

--***************************************************************************
   function "-" ( x1 :  MVL7_VECTOR ;  x2 :  MVL7_VECTOR ) return MVL7_VECTOR is
      variable v1: MVL7_VECTOR (x1'high - x1'low downto 0);
      variable v2: MVL7_VECTOR (x2'high - x2'low downto 0);
      variable SUM: MVL7_VECTOR (v1'high downto v1'low);
   begin
      v1 := x1;
      v2 := x2;
      assert v1'length = v2'length 
         report "MVL7 vector -: operands of unequal lengths" severity FAILURE;
      SUM := I2B(B2I(v1) - B2I(v2),SUM'length);
      return (SUM);
   end;

--***************************************************************************
   function DEC( x :  MVL7_VECTOR ) return MVL7_VECTOR is
      variable v: MVL7_VECTOR (x'high downto x'low);
   begin
      v := x;
      return I2B(B2I(v) - 1,v'length);
   end;

--***************************************************************************
   function CARRY_ADD( x1: MVL7_VECTOR ; x2: MVL7_VECTOR ) return MVL7_VECTOR is
      variable v1: MVL7_VECTOR (x1'high - x1'low downto 0);
      variable v2: MVL7_VECTOR (x2'high - x2'low downto 0);
      variable SUM: MVL7_VECTOR (x1'high - x1'low + 1 downto 0);
      -- + 1 is for carry
   begin
      v1 := x1;
      v2 := x2;
      assert v1'length = v2'length 
         report "MVL7vector carry add:operands of unequal lengths" 
         severity FAILURE;
      SUM := I2B(B2I(v1) + B2I(v2),SUM'length);
      return (SUM);
   end;

--***************************************************************************
   function "+" ( x1 :  MVL7_VECTOR ;  x2 :  MVL7_VECTOR ) return MVL7_VECTOR is
      variable v1: MVL7_VECTOR (x1'high - x1'low downto 0);
      variable v2: MVL7_VECTOR (x2'high - x2'low downto 0);
      variable SUM: MVL7_VECTOR (v1'high downto v1'low);
   begin
      v1 := x1;
      v2 := x2;
      assert v1'length = v2'length 
         report "MVL7 vector +: operands of unequal lengths" severity FAILURE;
      SUM := I2B(B2I(v1) + B2I(v2),SUM'length);
      return (SUM);
   end;

--***************************************************************************
   function INC( x :  MVL7_VECTOR ) return MVL7_VECTOR is
      variable v: MVL7_VECTOR (x'high downto x'low);
   begin
      v := x;
      return I2B(B2I(v) + 1,v'length);
   end;

--***************************************************************************
   function ODD_PARITY( v1 :  MVL7_VECTOR ) return MVL7 is
   begin
      if ((SUM(v1) mod 2) = 1) then
         return '0';
      else
         return '1';
      end if;
   end;

--***************************************************************************
   function EVEN_PARITY( v1 :  MVL7_VECTOR ) return MVL7 is
   begin
      if ((SUM(v1) mod 2) = 1) then
         return '1';
      else
         return '0';
      end if;
   end;

--***************************************************************************
   function REVERSE( v2 :  MVL7_VECTOR ) return MVL7_VECTOR is
      variable v1: MVL7_VECTOR (v2'high downto v2'low);
      variable temp: MVL7_VECTOR (v1'high downto v1'low);
   begin
      v1 := v2;
      for I in v1'high downto v1'low loop
         temp(I) := v1(v1'high - I + v1'low);
      end loop ;
      return temp;
   end;

--***************************************************************************
   function SUM( v2 :  MVL7_VECTOR ) return integer is
      variable v1: MVL7_VECTOR (v2'high downto v2'low);
      variable count: integer:=0;
   begin
      v1 := v2;
      for I in v1'high downto v1'low loop
         if (v1(I) = '1') then
            count := count + 1;
         end if;
      end loop ;
      return count;
   end;

--***************************************************************************
   function PAD( v :  MVL7_VECTOR ;  width :  integer ) return MVL7_VECTOR is
   begin
      return I2B(B2I(v),width);
   end;

--***************************************************************************
   function "*" ( x1 :  MVL7_VECTOR ;  x2 :  MVL7_VECTOR ) return MVL7_VECTOR is
      variable v1: MVL7_VECTOR (x1'high - x1'low downto 0);
      variable v2: MVL7_VECTOR (x2'high - x2'low downto 0);
      variable PROD: MVL7_VECTOR (v1'high downto v1'low);
   begin
      v1 := x1;
      v2 := x2;
      assert v1'length = v2'length 
         report "MVL7 vector MUL: operands of unequal lengths" severity FAILURE;
      PROD := I2B(B2I(v1) * B2I(v2),PROD'length);
      return (PROD);
   end;

--***************************************************************************

function WiredOr (V: MVL7_VECTOR) return MVL7 is
  variable result: MVL7;
begin
  result := 'Z';
  for i in V'range loop
    result := tbl_WIREDOr(result, V(i));
    exit when result = '1';
  end loop;
  return result;
end WiredOr;
--***************************************************************************
end;