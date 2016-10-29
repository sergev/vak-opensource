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

--library ZYCAD;
use work.TYPES.all;
use work.MVL7_functions.all;

entity alu is
   port (
         RE, S : in MVL7_vector(3 downto 0);
         I : in MVL7_vector(8 downto 0);
         C0 : in MVL7;
         C4, OVR, F30, F3, Pbar, Gbar :  out MVL7;
         F : out MVL7_vector(3 downto 0) 
        );
end alu;

architecture alu of alu is 

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

end alu;

------------------------------
