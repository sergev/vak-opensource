--------------------------------------------------------------------------------
--
--   AM2901 Benchmark
--
-- Source: AMD data book
--
-- VHDL Benchmark author Indraneel Ghosh
--                       University Of California, Irvine, CA 92717
--
-- Developed on Jan 1, 1992
--
-- Verification Information:
--
--                  Verified     By whom?           Date         Simulator
--                  --------   ------------        --------     ------------
--  Syntax            yes  Champaka Ramachandran  Sept 17, 92     ZYCAD
--  Functionality     yes  Champaka Ramachandran  Sept 17, 92     ZYCAD
--------------------------------------------------------------------------------

use work.types.all;  -- some MVL7 functions
use work.MVL7_functions.all;  -- some MVL7 functions
use work.synthesis_types.all; -- some data types ( hints for synthesis)

entity a2901 is 
	port (
		I : in MVL7_vector(8 downto 0); 
		Aadd, Badd : in integer range 15 downto 0;
		D : in MVL7_vector(3 downto 0);
		Y : out MVL7_vector(3 downto 0);
		RAM0, RAM3, Q0, Q3 : inout MVL7; 
		CLK : in clock; 
 		C0 : in MVL7;  
		OEbar : in MVL7;
		C4, Gbar, Pbar, OVR, F3, F30 : out MVL7);
end a2901;

architecture a2901 of a2901 is 

        signal RAM : Memory(15 downto 0);
        signal RE,S : MVL7_vector(3 downto 0);
	signal A, B : MVL7_vector(3 downto 0);
	signal Q : MVL7_vector(3 downto 0);
	signal F : MVL7_vector(3 downto 0);

begin

-------------------------------------------------------------------------

alu_inputs : block
begin

A <= RAM(Aadd);
B <= RAM(Badd);

-- SELECT THE SOURCE OPERANDS FOR ALU. SELECTED OPERANDS ARE "RE" AND "S".

with I(2 downto 0) select
  RE <= A when "000" | "001",
       "0000" when "010" | "011" | "100",
       D when others;

with I(2 downto 0) select
  S <= A when "100" | "101",
       B when "001" | "011",
       "0000" when "111",
       Q when others;

end block alu_inputs;

-------------------------------------------------------------------------

alu : block

signal R_ext,S_ext,result,temp_p,temp_g : MVL7_vector(4 downto 0);

begin

--   TO FACILITATE COMPUTATION OF CARRY-OUT "C4", WE EXTEND THE CHOSEN 
--   ALU OPERANDS "RE" AND "S" (4 BIT OPERANDS) BY 1 BIT IN THE MSB POSITION. 

--   THUS THE EXTENDED OPERANDS "R_EXT" AND "S_EXT" (5 BIT OPERANDS) ARE
--   FORMED AND ARE USED IN THE ALU OPERATION. THE EXTRA BIT IS SET TO '0' 
--   INITIALLY. THE ALU'S EXTENDED OUTPUT ( 5 BITS LONG) IS "result".

R_ext <= '0' & not(RE) when I(5 downto 3) = "001" else
         '0' & RE;
S_ext <= '0' & not(S) when I(5 downto 3) = "010" else
         '0' & S;

-- SELECT THE FUNCTION FOR ALU.

--   IN THE ADD/SUBTRACT OPERATIONS, THE CARRY-INPUT "C0" (1 BIT) IS EXTENDED
--   BY 4 BITS ( ALL '0') IN THE MORE SIGNIFICANT BITS TO MATCH ITS LENGTH TO
--   THAT OF "R_ext" AND "S_ext". THEN, THESE THREE OPERANDS ARE ADDED.

--   ADD/SUBTRACT OPERATIONS ARE DONE ON 2'S COMPLEMENT OPERANDS.

with I(5 downto 3) select
   result <= R_ext + S_ext + ("0000" & C0) when "000" | "001" | "010",
             R_ext or S_ext when "011",
             R_ext and S_ext when "100",
             not(R_ext) and S_ext when "101",
             R_ext xor S_ext when "110",
             not( R_ext xor S_ext) when others;

-- EVALUATE OTHER ALU OUTPUTS.

--  FROM EXTENDED OUTPUT "result" ( 5 BITS), WE OBTAIN THE NORMAL ALU OUTPUT,
--  "F" (4 BITS) BY LEAVING OUT THE MSB ( WHICH CORRESPONDS TO CARRY-OUT
--  "C4"). 

--  TO FACILITATE COMPUTATION OF CARRY LOOKAHEAD TERMS "Pbar" AND "Gbar", WE
--  COMPUTE INTERMEDIATE TERMS "temp_p" AND "temp_g".

F <= result(3 downto 0);
OVR <= not (R_ext(3) xor S_ext(3)) and
                           ( R_ext(3) xor result(3) );
C4 <= result(4);
temp_p <= R_ext or S_ext;          -- R or S may get
temp_g <= R_ext and S_ext;         -- inverted (sub)
Pbar <= not(temp_p(0) and temp_p(1) and temp_p(2) and temp_p(3));
Gbar <= not( temp_g(3) or 
             (temp_p(3) and temp_g(2)) or 
             (temp_p(3) and temp_p(2) and temp_g(1)) or
             (temp_p(3) and temp_p(2) and temp_p(1) and temp_g(0))
           );
F3 <= result(3);
F30 <= not(result(3) or result(2) or result(1) or result(0));

end block alu;

-------------------------------------------------------------------------

mem : block ( (not clk'stable) and (clk = '1') )

begin

-- WRITE TO RAM WITH/WITHOUT SHIFTING. RAM DESTINATIONS ARE 
-- ADDRESSED BY "Badd".
        
RAM(Badd) <= guarded F when ((not(I(8)) and I(7)) = '1') else
             RAM3 & F(3 downto 1) when ((I(8) and not(I(7))) = '1') else
             F(2 downto 0) & RAM0 when ((I(8) and I(7)) = '1') else
             RAM(Badd);

end block mem;

-------------------------------------------------------------------------

Q_reg : block ( (not clk'stable) and (clk = '1') )

begin

-- WRITE TO Q REGISTER WITH/WITHOUT SHIFTING. 
        
Q <= guarded F when (I(8 downto 6) = "000")  else
             Q3 & Q(3 downto 1) when (I(8 downto 6) = "100") else
             Q(2 downto 0) & Q0 when (I(8 downto 6) = "110") else
             Q;

end block Q_reg;

-------------------------------------------------------------------------

output_and_shifter : block

begin

-- GENERATE DATA OUTPUT "Y" 
   
Y <= A when (( I(8 downto 6) = "010") and ( OEbar = '0')) else
     F when (not(( I(8 downto 6) = "010")) and ( OEbar = '0')) else
     "ZZZZ";

-- GENERATE BIDIRECTIONAL SHIFTER SIGNALS.

RAM0 <= F(0) when ( I(8) = '1') and ( I(7) = '0' ) else 
       'Z';             
RAM3 <= F(3) when ( I(8) = '1') and ( I(7) = '1' ) else
         'Z';           

Q3 <= Q(3) when ( I(8) = '1') and ( I(7) = '1') else
          'Z';         
Q0 <= Q(0) when ( I(8) = '1') and ( I(7) = '0') else
          'Z';         

end block output_and_shifter;

-------------------------------------------------------------------------

end a2901;








