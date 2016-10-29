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
use work.types.all;
use work.MVL7_functions.all;

entity alu_inputs is
      port (
            Aadd, Badd : in integer; 
	    Q, D : in MVL7_vector(3 downto 0);
            I : in MVL7_vector(8 downto 0); 
            RE, S : out MVL7_vector(3 downto 0)
           );
end alu_inputs;

architecture alu_inputs of alu_inputs is

  type Memory is array (integer range <>) of MVL7_vector(3 downto 0);
  signal RAM : Memory(15 downto 0) := ( 
					("0000"),
					("0000"),
					("0000"),
					("0000"),
					("0000"),
					("0000"),
					("0000"),
					("0000"),
					("0000"),
					("0000"),
					("0000"),
					("0000"),
					("0000"),
					("0000"),
					("0100"),
					("1000"));
  signal A, B : MVL7_vector(3 downto 0);

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

end alu_inputs;