--------------------------------------------------------------------------------
--
--   AMD 2910 Benchmark (Functional blocks) (Algorithmic Behaviour of Funct blocks)
--
-- Source:  AMD data book
--
-- VHDL Benchmark author Indraneel Ghosh
--                       University Of California, Irvine, CA 92717
--
-- Developed on Feb 19, 1992
--
-- Verification Information:
--
--                  Verified     By whom?           Date         Simulator
--                  --------   ------------        --------     ------------
--  Syntax            yes   Champaka Ramachandran  Sept17, 92     ZYCAD
--  Functionality     yes   Champaka Ramachandran  Sept17, 92     ZYCAD
--------------------------------------------------------------------------------

--library ZYCAD;
use work.types.all;
use work.MVL7_functions.all;
use work.synthesis_types.all;

entity E is
end;

architecture AA of E is
	component cupc
	   port (
          clk : in clock;
	   CI : in MVL7;
	clear : in MVL7;
       Y_temp : in MVL7_VECTOR(11 downto 0);
	  uPC : inout MVL7_VECTOR(11 downto 0)
		);
	end component;							

signal clk : clock;
signal CI : MVL7;
signal clear : MVL7;
signal Y_temp : MVL7_VECTOR(11 downto 0);
signal uPC : MVL7_VECTOR(11 downto 0);

for all : cupc use entity work.upc(upc);

begin

CUPC1 : cupc port map(
		          clk,
			   CI,
			clear,
		       Y_temp,
			  uPC
		     );

process

begin

----------------------

clk <= '0';

wait for 1 ns;

CI <= '0';

clear <= '0';

Y_temp <= "000000000000";

wait for 4 ns;

clk <= '1';

wait for 4 ns;

assert (uPC = "000000000000")
report
"Assert 0 : < uPC = 000000000000 >"
severity warning;

wait for 1 ns;

----------------------

clk <= '0';

wait for 1 ns;

CI <= '0';

clear <= '0';

Y_temp <= "111111111111";

wait for 4 ns;

clk <= '1';

wait for 4 ns;

assert (uPC = "111111111111")
report
"Assert 1 : < uPC = 111111111111 >"
severity warning;

wait for 1 ns;

----------------------

clk <= '0';

wait for 1 ns;

CI <= '0';

clear <= '1';

Y_temp <= "111111111111";

wait for 4 ns;

clk <= '1';

wait for 4 ns;

assert (uPC = "000000000000")
report
"Assert 2 : < uPC = 000000000000 >"
severity warning;

wait for 1 ns;

----------------------

clk <= '0';

wait for 1 ns;

CI <= '1';

clear <= '0';

Y_temp <= "000000000000";

wait for 4 ns;

clk <= '1';

wait for 4 ns;

assert (uPC = "000000000001")
report
"Assert 3 : < uPC = 000000000001 >"
severity warning;

wait for 1 ns;

----------------------

clk <= '0';

wait for 1 ns;

CI <= '1';

clear <= '0';

Y_temp <= "000000000001";

wait for 4 ns;

clk <= '1';

wait for 4 ns;

assert (uPC = "000000000010")
report
"Assert 4 : < uPC = 000000000010 >"
severity warning;

wait for 1 ns;

----------------------

clk <= '0';

wait for 1 ns;

CI <= '1';

clear <= '0';

Y_temp <= "000000000011";

wait for 4 ns;

clk <= '1';

wait for 4 ns;

assert (uPC = "000000000100")
report
"Assert 5 : < uPC = 000000000100 >"
severity warning;

wait for 1 ns;

----------------------

clk <= '0';

wait for 1 ns;

CI <= '1';

clear <= '0';

Y_temp <= "000000000111";

wait for 4 ns;

clk <= '1';

wait for 4 ns;

assert (uPC = "000000001000")
report
"Assert 6 : < uPC = 000000001000 >"
severity warning;

wait for 1 ns;

----------------------

clk <= '0';

wait for 1 ns;

CI <= '1';

clear <= '0';

Y_temp <= "000000001111";

wait for 4 ns;

clk <= '1';

wait for 4 ns;

assert (uPC = "000000010000")
report
"Assert 7 : < uPC = 000000010000 >"
severity warning;

wait for 1 ns;

----------------------

clk <= '0';

wait for 1 ns;

CI <= '1';

clear <= '0';

Y_temp <= "000000011111";

wait for 4 ns;

clk <= '1';

wait for 4 ns;

assert (uPC = "000000100000")
report
"Assert 8 : < uPC = 000000100000 >"
severity warning;

wait for 1 ns;

----------------------

clk <= '0';

wait for 1 ns;

CI <= '1';

clear <= '0';

Y_temp <= "000000111111";

wait for 4 ns;

clk <= '1';

wait for 4 ns;

assert (uPC = "000001000000")
report
"Assert 9 : < uPC = 000001000000 >"
severity warning;

wait for 1 ns;

----------------------

clk <= '0';

wait for 1 ns;

CI <= '1';

clear <= '0';

Y_temp <= "000001111111";

wait for 4 ns;

clk <= '1';

wait for 4 ns;

assert (uPC = "000010000000")
report
"Assert 10 : < uPC = 000010000000 >"
severity warning;

wait for 1 ns;

----------------------

clk <= '0';

wait for 1 ns;

CI <= '1';

clear <= '0';

Y_temp <= "000011111111";

wait for 4 ns;

clk <= '1';

wait for 4 ns;

assert (uPC = "000100000000")
report
"Assert 11 : < uPC = 000100000000 >"
severity warning;

wait for 1 ns;

----------------------

clk <= '0';

wait for 1 ns;

CI <= '1';

clear <= '0';

Y_temp <= "000111111111";

wait for 4 ns;

clk <= '1';

wait for 4 ns;

assert (uPC = "001000000000")
report
"Assert 12 : < uPC = 001000000000 >"
severity warning;

wait for 1 ns;

----------------------

clk <= '0';

wait for 1 ns;

CI <= '1';

clear <= '0';

Y_temp <= "001111111111";

wait for 4 ns;

clk <= '1';

wait for 4 ns;

assert (uPC = "010000000000")
report
"Assert 13 : < uPC = 010000000000 >"
severity warning;

wait for 1 ns;

----------------------

clk <= '0';

wait for 1 ns;

CI <= '1';

clear <= '0';

Y_temp <= "011111111111";

wait for 4 ns;

clk <= '1';

wait for 4 ns;

assert (uPC = "100000000000")
report
"Assert 14 : < uPC = 100000000000 >"
severity warning;

wait for 1 ns;

----------------------

clk <= '0';

wait for 1 ns;

CI <= '1';

clear <= '0';

Y_temp <= "111111111111";

wait for 4 ns;

clk <= '1';

wait for 4 ns;

assert (uPC = "000000000000")
report
"Assert 15 : < uPC = 000000000000 >"
severity warning;

wait for 1 ns;

----------------------
end process;

end AA;
