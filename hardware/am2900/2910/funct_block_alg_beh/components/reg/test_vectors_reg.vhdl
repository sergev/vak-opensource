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
	component creg
	   port (
      RLD_BAR : in MVL7;
         load : in MVL7;
         decr : in MVL7;
          clk : in clock;
            D : in MVL7_VECTOR(11 downto 0);
	   RE : inout MVL7_VECTOR(11 downto 0);
    Rzero_bar : out MVL7
		 );
	end component;
									
signal RLD_BAR : MVL7;
signal load : MVL7;
signal decr : MVL7;
signal clk : clock;
signal D : MVL7_VECTOR(11 downto 0);
signal RE : MVL7_VECTOR(11 downto 0);
signal Rzero_bar : MVL7;

for all : creg use entity work.reg(reg);

begin

CREG1 : creg port map(
      RLD_BAR,
         load,
         decr,
          clk,
            D,
	   RE,
    Rzero_bar 
		     );

process

begin

--------------------

clk <= '0';

wait for 1 ns;

RLD_BAR <= '1';

load <= '1';

decr <= '0';

D <= "000000000000";

wait for 4 ns;

clk <= '1';

wait for 4 ns;

assert (Rzero_bar = '0')
report
"Assert 0 : < Rzero_bar /= 0 >"
severity warning;

assert (RE = "000000000000")
report
"Assert 1 : < RE /= 000000000000 >" 
severity warning;

wait for 1 ns;

--------------------

clk <= '0';

wait for 1 ns;

RLD_BAR <= '1';

load <= '1';

decr <= '0';

D <= "111111111111";

wait for 4 ns;

clk <= '1';

wait for 4 ns;

assert (Rzero_bar = '1')
report
"Assert 2 : < Rzero_bar /= 1 >"
severity warning;

assert (RE = "111111111111")
report
"Assert 3 : < RE /= 111111111111 >" 
severity warning;

wait for 1 ns;

--------------------

clk <= '0';

wait for 1 ns;

RLD_BAR <= '0';

load <= '0';

decr <= '0';

D <= "000000000000";

wait for 4 ns;

clk <= '1';

wait for 4 ns;

assert (Rzero_bar = '0')
report
"Assert 4 : < Rzero_bar /= 0 >"
severity warning;

assert (RE = "000000000000")
report
"Assert 5 : < RE /= 000000000000 >" 
severity warning;

wait for 1 ns;

--------------------

clk <= '0';

wait for 1 ns;

RLD_BAR <= '1';

load <= '0';

decr <= '0';

D <= "111111111111";

wait for 4 ns;

clk <= '1';

wait for 4 ns;

assert (Rzero_bar = '0')
report
"Assert 6 : < Rzero_bar /= 0 >"
severity warning;

assert (RE = "000000000000")
report
"Assert 7 : < RE /= 000000000000 >" 
severity warning;

wait for 1 ns;

--------------------

clk <= '0';

wait for 1 ns;

RLD_BAR <= '0';

load <= '0';

decr <= '0';

D <= "111111111111";

wait for 4 ns;

clk <= '1';

wait for 4 ns;

assert (Rzero_bar = '1')
report
"Assert 8 : < Rzero_bar /= 1 >"
severity warning;

assert (RE = "111111111111")
report
"Assert 9 : < RE /= 111111111111 >" 
severity warning;

wait for 1 ns;

--------------------

clk <= '0';

wait for 1 ns;

RLD_BAR <= '1';

load <= '0';

decr <= '0';

D <= "000000000000";

wait for 4 ns;

clk <= '1';

wait for 4 ns;

assert (Rzero_bar = '1')
report
"Assert 10 : < Rzero_bar /= 1 >"
severity warning;

assert (RE = "111111111111")
report
"Assert 11 : < RE /= 111111111111 >" 
severity warning;

wait for 1 ns;

--------------------
--------------------

clk <= '0';

wait for 1 ns;

RLD_BAR <= '0';

D <= "111111111111";

wait for 4 ns;

clk <= '1';

wait for 4 ns;

wait for 1 ns;

--------------------

clk <= '0';

wait for 1 ns;

decr <= '1';

RLD_BAR <= '1';

wait for 4 ns;

clk <= '1';

wait for 4 ns;

assert (RE = "111111111110")
report
"Assert 12 : < RE /= 111111111110 >" 
severity warning;

wait for 1 ns;

--------------------
--------------------

clk <= '0';

wait for 1 ns;

RLD_BAR <= '0';

D <= "111111111110";

wait for 4 ns;

clk <= '1';

wait for 4 ns;

wait for 1 ns;

--------------------

clk <= '0';

wait for 1 ns;

decr <= '1';

RLD_BAR <= '1';

wait for 4 ns;

clk <= '1';

wait for 4 ns;

assert (RE = "111111111101")
report
"Assert 13 : < RE /= 111111111101 >" 
severity warning;

wait for 1 ns;

--------------------
--------------------

clk <= '0';

wait for 1 ns;

RLD_BAR <= '0';

D <= "111111111100";

wait for 4 ns;

clk <= '1';

wait for 4 ns;

wait for 1 ns;

--------------------

clk <= '0';

wait for 1 ns;

decr <= '1';

RLD_BAR <= '1';

wait for 4 ns;

clk <= '1';

wait for 4 ns;

assert (RE = "111111111011")
report
"Assert 14 : < RE /= 111111111011 >" 
severity warning;

wait for 1 ns;

--------------------
--------------------

clk <= '0';

wait for 1 ns;

RLD_BAR <= '0';

D <= "111111111000";

wait for 4 ns;

clk <= '1';

wait for 4 ns;

wait for 1 ns;

--------------------

clk <= '0';

wait for 1 ns;

decr <= '1';

RLD_BAR <= '1';

wait for 4 ns;

clk <= '1';

wait for 4 ns;

assert (RE = "111111110111")
report
"Assert 15 : < RE /= 111111110111 >" 
severity warning;

wait for 1 ns;

--------------------
--------------------

clk <= '0';

wait for 1 ns;

RLD_BAR <= '0';

D <= "111111110000";

wait for 4 ns;

clk <= '1';

wait for 4 ns;

wait for 1 ns;

--------------------
 
clk <= '0';

wait for 1 ns;

decr <= '1';

RLD_BAR <= '1';

wait for 4 ns;

clk <= '1';

wait for 4 ns;

assert (RE = "111111101111")
report
"Assert 16 : < RE /= 111111101111 >" 
severity warning;

wait for 1 ns;

--------------------
--------------------

clk <= '0';

wait for 1 ns;

RLD_BAR <= '0';

D <= "111111100000";

wait for 4 ns;

clk <= '1';

wait for 4 ns;

wait for 1 ns;

--------------------
 
clk <= '0';

wait for 1 ns;

decr <= '1';

RLD_BAR <= '1';

wait for 4 ns;

clk <= '1';

wait for 4 ns;

assert (RE = "111111011111")
report
"Assert 17 : < RE /= 111111011111 >" 
severity warning;

wait for 1 ns;

--------------------
--------------------

clk <= '0';

wait for 1 ns;

RLD_BAR <= '0';

D <= "111111000000";

wait for 4 ns;

clk <= '1';

wait for 4 ns;

wait for 1 ns;

--------------------
 
clk <= '0';

wait for 1 ns;

decr <= '1';

RLD_BAR <= '1';

wait for 4 ns;

clk <= '1';

wait for 4 ns;

assert (RE = "111110111111")
report
"Assert 18 : < RE /= 111110111111 >" 
severity warning;

wait for 1 ns;

--------------------
--------------------

clk <= '0';

wait for 1 ns;

RLD_BAR <= '0';

D <= "111110000000";

wait for 4 ns;

clk <= '1';

wait for 4 ns;

wait for 1 ns;

--------------------
 
clk <= '0';

wait for 1 ns;

decr <= '1';

RLD_BAR <= '1';

wait for 4 ns;

clk <= '1';

wait for 4 ns;

assert (RE = "111101111111")
report
"Assert 19 : < RE /= 111101111111 >" 
severity warning;

wait for 1 ns;

--------------------
--------------------

clk <= '0';

wait for 1 ns;

RLD_BAR <= '0';

D <= "111100000000";

wait for 4 ns;

clk <= '1';

wait for 4 ns;

wait for 1 ns;

--------------------
 
clk <= '0';

wait for 1 ns;

decr <= '1';

RLD_BAR <= '1';

wait for 4 ns;

clk <= '1';

wait for 4 ns;

assert (RE = "111011111111")
report
"Assert 20 : < RE /= 111011111111 >" 
severity warning;

wait for 1 ns;

--------------------
--------------------

clk <= '0';

wait for 1 ns;

RLD_BAR <= '0';

D <= "111000000000";

wait for 4 ns;

clk <= '1';

wait for 4 ns;

wait for 1 ns;

--------------------
 
clk <= '0';

wait for 1 ns;

decr <= '1';

RLD_BAR <= '1';

wait for 4 ns;

clk <= '1';

wait for 4 ns;

assert (RE = "110111111111")
report
"Assert 21 : < RE /= 110111111111 >" 
severity warning;

wait for 1 ns;

--------------------
--------------------

clk <= '0';

wait for 1 ns;

RLD_BAR <= '0';

D <= "110000000000";

wait for 4 ns;

clk <= '1';

wait for 4 ns;

wait for 1 ns;

--------------------
 
clk <= '0';

wait for 1 ns;

decr <= '1';

RLD_BAR <= '1';

wait for 4 ns;

clk <= '1';

wait for 4 ns;

assert (RE = "101111111111")
report
"Assert 22 : < RE /= 101111111111 >" 
severity warning;

wait for 1 ns;

--------------------
--------------------

clk <= '0';

wait for 1 ns;

RLD_BAR <= '0';

D <= "100000000000";

wait for 4 ns;

clk <= '1';

wait for 4 ns;

wait for 1 ns;

--------------------
 
clk <= '0';

wait for 1 ns;

decr <= '1';

RLD_BAR <= '1';

wait for 4 ns;

clk <= '1';

wait for 4 ns;

assert (RE = "011111111111")
report
"Assert 23 : < RE /= 011111111111 >" 
severity warning;

wait for 1 ns;

--------------------
--------------------

end process;

end AA;


