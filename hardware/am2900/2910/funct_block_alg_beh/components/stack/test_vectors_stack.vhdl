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
	component cstack
	   port (
          clk : in clock;
	  pop : in MVL7;
         push : in MVL7;
        clear : in MVL7;
	  uPC : in MVL7_VECTOR(11 downto 0);
           sp : inout INTEGER range 0 to 5;
     reg_file : inout MEMORY_12_BIT(5 downto 0);
     FULL_BAR : out MVL7
		);
	end component;
									
signal clk : clock;
signal pop : MVL7;
signal push : MVL7;
signal clear : MVL7;
signal uPC : MVL7_VECTOR(11 downto 0);
signal sp : INTEGER range 0 to 5;
signal reg_file : MEMORY_12_BIT(5 downto 0);
signal FULL_BAR : MVL7;

for all : cstack use entity work.stack(stack);

begin

	CSTACK1 : cstack port map(
          clk,
	  pop,
         push,
        clear,
	  uPC,
           sp,
     reg_file,
     FULL_BAR
				);

process

begin

--------------------

clk <= '0';

wait for 1 ns;

pop <= '0';

push <= '0';

clear <= '1';

uPC <= "000000000000";

wait for 4 ns;

clk <= '1';

wait for 4 ns;

assert (sp = 0)
report
"Assert 0 : < sp /= 0 >"
severity warning;

assert (FULL_BAR = '1')
report
"Assert 0a : < FULL_BAR /= 1 >"
severity warning;

wait for 1 ns;

--------------------

clk <= '0';

wait for 1 ns;

pop <= '0';

push <= '1';

clear <= '0';

uPC <= "000000000001";

wait for 4 ns;

clk <= '1';

wait for 4 ns;

assert (sp = 1)
report
"Assert 1 : < sp /= 1 >"
severity warning;

assert (FULL_BAR = '1')
report
"Assert 1a : < FULL_BAR /= 1 >"
severity warning;

assert (reg_file(sp) = "000000000001" )
report
"Assert 1b : < reg_file(sp) /= 000000000001 >"
severity warning;

wait for 1 ns;

--------------------


clk <= '0';

wait for 1 ns;

pop <= '0';

push <= '1';

clear <= '0';

uPC <= "000000000010";

wait for 4 ns;

clk <= '1';

wait for 4 ns;

assert (sp = 2)
report
"Assert 2 : < sp /= 2 >"
severity warning;

assert (FULL_BAR = '1')
report
"Assert 2a : < FULL_BAR /= 1 >"
severity warning;

assert (reg_file(sp) = "000000000010" )
report
"Assert 2b : < reg_file(sp) /= 000000000010 >"
severity warning;

wait for 1 ns;

--------------------


clk <= '0';

wait for 1 ns;

pop <= '0';

push <= '1';

clear <= '0';

uPC <= "000000000100";

wait for 4 ns;

clk <= '1';

wait for 4 ns;

assert (sp = 3)
report
"Assert 3 : < sp /= 3 >"
severity warning;

assert (FULL_BAR = '1')
report
"Assert 3a : < FULL_BAR /= 1 >"
severity warning;

assert (reg_file(sp) = "000000000100" )
report
"Assert 3b : < reg_file(sp) /= 000000000100 >"
severity warning;

wait for 1 ns;

--------------------


clk <= '0';

wait for 1 ns;

pop <= '0';

push <= '1';

clear <= '0';

uPC <= "000000001000";

wait for 4 ns;

clk <= '1';

wait for 4 ns;

assert (sp = 4)
report 
"Assert 4 : < sp /= 4 >"
severity warning;

assert (FULL_BAR = '1')
report
"Assert 4a : < FULL_BAR /= 1 >"
severity warning;

assert (reg_file(sp) = "000000001000" )
report
"Assert 4b : < reg_file(sp) /= 000000001000 >"
severity warning;

wait for 1 ns;

--------------------


clk <= '0';

wait for 1 ns;

pop <= '0';

push <= '1';

clear <= '0';

uPC <= "111111111111";

wait for 4 ns;

clk <= '1';

wait for 4 ns;

assert (sp = 5)
report
"Assert 5 : < sp /= 5 >"
severity warning;

assert (FULL_BAR = '0')
report
"Assert 5a : < FULL_BAR /= 0 >"
severity warning;

assert (reg_file(sp) = "111111111111" )
report
"Assert 5b : < reg_file(sp) /= 111111111111 >"
severity warning;

wait for 1 ns;

--------------------


clk <= '0';

wait for 1 ns;

pop <= '0';

push <= '1';

clear <= '0';

uPC <= "000000010000";

wait for 4 ns;

clk <= '1';

wait for 4 ns;

assert (sp = 5)
report
"Assert 6 : < sp /= 5 >"
severity warning;

assert (FULL_BAR = '0')
report
"Assert 6a : < FULL_BAR /= 0 >"
severity warning;

assert (reg_file(sp) = "000000010000" )
report
"Assert 6b : < reg_file(sp) /= 000000010000 >"
severity warning;

wait for 1 ns;

--------------------


clk <= '0';

wait for 1 ns;

pop <= '1';

push <= '0';

clear <= '0';

wait for 4 ns;

clk <= '1';

wait for 4 ns;

assert (sp = 4)
report
"Assert 7 : < sp /= 4 >"
severity warning;

assert (FULL_BAR = '1')
report
"Assert 7a : < FULL_BAR /= 1 >"
severity warning;

assert (reg_file(sp) = "000000001000" )
report
"Assert 7b : < reg_file(sp) /= 000000001000 >"
severity warning;

wait for 1 ns;

--------------------

clk <= '0';

wait for 1 ns;

pop <= '1';

push <= '0';

clear <= '0';

wait for 4 ns;

clk <= '1';

wait for 4 ns;

assert (sp = 3)
report
"Assert 8 : < sp /= 3 >"
severity warning;

assert (FULL_BAR = '1')
report
"Assert 8a : < FULL_BAR /= 1 >"
severity warning;

assert (reg_file(sp) = "000000000100" )
report
"Assert 8b : < reg_file(sp) /= 000000000100 >"
severity warning;

wait for 1 ns;

--------------------

clk <= '0';

wait for 1 ns;

pop <= '1';

push <= '0';

clear <= '0';

wait for 4 ns;

clk <= '1';

wait for 4 ns;

assert (sp = 2)
report
"Assert 9 : < sp /= 2 >"
severity warning;

assert (FULL_BAR = '1')
report
"Assert 9a : < FULL_BAR /= 1 >"
severity warning;

assert (reg_file(sp) = "000000000010" )
report
"Assert 9b : < reg_file(sp) /= 000000000010 >"
severity warning;

wait for 1 ns;

--------------------

clk <= '0';

wait for 1 ns;

pop <= '1';

push <= '0';

clear <= '0';

wait for 4 ns;

clk <= '1';

wait for 4 ns;

assert (sp = 1)
report
"Assert 10 : < sp /= 1 >"
severity warning;

assert (FULL_BAR = '1')
report
"Assert 10a : < FULL_BAR /= 1 >"
severity warning;

assert (reg_file(sp) = "000000000001" )
report
"Assert 10b : < reg_file(sp) /= 000000000001 >"
severity warning;

wait for 1 ns;

--------------------

clk <= '0';

wait for 1 ns;

pop <= '1';

push <= '0';

clear <= '0';

wait for 4 ns;

clk <= '1';

wait for 4 ns;

assert (sp = 0)
report
"Assert 11 : < sp /= 0 >"
severity warning;

assert (FULL_BAR = '1')
report
"Assert 11a : < FULL_BAR /= 1 >"
severity warning;

wait for 1 ns;

--------------------

clk <= '0';

wait for 1 ns;

pop <= '1';

push <= '0';

clear <= '0';

wait for 4 ns;

clk <= '1';

wait for 4 ns;

assert (sp = 0)
report
"Assert 12 : < sp /= 0 >"
severity warning;

assert (FULL_BAR = '1')
report
"Assert 12a : < FULL_BAR /= 1 >"
severity warning;

wait for 1 ns;

--------------------

clk <= '0';

wait for 1 ns;

pop <= '0';

push <= '1';

clear <= '0';

uPC <= "111111111110";

wait for 4 ns;

clk <= '1';

wait for 4 ns;

assert (sp = 1)
report
"Assert 13 : < sp /= 1 >"
severity warning;

assert (FULL_BAR = '1')
report
"Assert 13a : < FULL_BAR /= 1 >"
severity warning;

assert (reg_file(sp) = "111111111110" )
report
"Assert 13b : < reg_file(sp) /= 111111111110 >"
severity warning;

wait for 1 ns;

--------------------


clk <= '0';

wait for 1 ns;

pop <= '0';

push <= '1';

clear <= '0';

uPC <= "111111111101";

wait for 4 ns;

clk <= '1';

wait for 4 ns;

assert (sp = 2)
report
"Assert A2 : < sp /= 2 >"
severity warning;

assert (FULL_BAR = '1')
report
"Assert A2a : < FULL_BAR /= 1 >"
severity warning;

assert (reg_file(sp) = "111111111101" )
report
"Assert A2b : < reg_file(sp) /= 111111111101 >"
severity warning;

wait for 1 ns;

--------------------


clk <= '0';

wait for 1 ns;

pop <= '0';

push <= '1';

clear <= '0';

uPC <= "111111111011";

wait for 4 ns;

clk <= '1';

wait for 4 ns;

assert (sp = 3)
report
"Assert A3 : < sp /= 3 >"
severity warning;

assert (FULL_BAR = '1')
report
"Assert A3a : < FULL_BAR /= 1 >"
severity warning;

assert (reg_file(sp) = "111111111011" )
report
"Assert A3b : < reg_file(sp) /= 111111111011 >"
severity warning;

wait for 1 ns;

--------------------


clk <= '0';

wait for 1 ns;

pop <= '0';

push <= '1';

clear <= '0';

uPC <= "111111110111";

wait for 4 ns;

clk <= '1';

wait for 4 ns;

assert (sp = 4)
report 
"Assert A4 : < sp /= 4 >"
severity warning;

assert (FULL_BAR = '1')
report
"Assert A4a : < FULL_BAR /= 1 >"
severity warning;

assert (reg_file(sp) = "111111110111" )
report
"Assert A4b : < reg_file(sp) /= 111111110111 >"
severity warning;

wait for 1 ns;

--------------------


clk <= '0';

wait for 1 ns;

pop <= '0';

push <= '1';

clear <= '0';

uPC <= "000000000000";

wait for 4 ns;

clk <= '1';

wait for 4 ns;

assert (sp = 5)
report
"Assert A5 : < sp /= 5 >"
severity warning;

assert (FULL_BAR = '0')
report
"Assert A5a : < FULL_BAR /= 0 >"
severity warning;

assert (reg_file(sp) = "000000000000" )
report
"Assert A5b : < reg_file(sp) /= 000000000000 >"
severity warning;

wait for 1 ns;

--------------------


clk <= '0';

wait for 1 ns;

pop <= '0';

push <= '1';

clear <= '0';

uPC <= "111111101111";

wait for 4 ns;

clk <= '1';

wait for 4 ns;

assert (sp = 5)
report
"Assert A6 : < sp /= 5 >"
severity warning;

assert (FULL_BAR = '0')
report
"Assert A6a : < FULL_BAR /= 0 >"
severity warning;

assert (reg_file(sp) = "111111101111" )
report
"Assert A6b : < reg_file(sp) /= 111111101111 >"
severity warning;

wait for 1 ns;

--------------------


clk <= '0';

wait for 1 ns;

pop <= '1';

push <= '0';

clear <= '0';

wait for 4 ns;

clk <= '1';

wait for 4 ns;

assert (sp = 4)
report
"Assert A7 : < sp /= 4 >"
severity warning;

assert (FULL_BAR = '1')
report
"Assert A7a : < FULL_BAR /= 1 >"
severity warning;

assert (reg_file(sp) = "111111110111" )
report
"Assert A7b : < reg_file(sp) /= 111111110111 >"
severity warning;

wait for 1 ns;

--------------------

clk <= '0';

wait for 1 ns;

pop <= '1';

push <= '0';

clear <= '0';

wait for 4 ns;

clk <= '1';

wait for 4 ns;

assert (sp = 3)
report
"Assert A8 : < sp /= 3 >"
severity warning;

assert (FULL_BAR = '1')
report
"Assert A8a : < FULL_BAR /= 1 >"
severity warning;

assert (reg_file(sp) = "111111111011" )
report
"Assert A8b : < reg_file(sp) /= 111111111011 >"
severity warning;

wait for 1 ns;

--------------------

clk <= '0';

wait for 1 ns;

pop <= '1';

push <= '0';

clear <= '0';

wait for 4 ns;

clk <= '1';

wait for 4 ns;

assert (sp = 2)
report
"Assert A9 : < sp /= 2 >"
severity warning;

assert (FULL_BAR = '1')
report
"Assert A9a : < FULL_BAR /= 1 >"
severity warning;

assert (reg_file(sp) = "111111111101" )
report
"Assert A9b : < reg_file(sp) /= 111111111101 >"
severity warning;

wait for 1 ns;

--------------------

clk <= '0';

wait for 1 ns;

pop <= '1';

push <= '0';

clear <= '0';

wait for 4 ns;

clk <= '1';

wait for 4 ns;

assert (sp = 1)
report
"Assert A10 : < sp /= 1 >"
severity warning;

assert (FULL_BAR = '1')
report
"Assert A10a : < FULL_BAR /= 1 >"
severity warning;

assert (reg_file(sp) = "111111111110" )
report
"Assert A10b : < reg_file(sp) /= 111111111110 >"
severity warning;

wait for 1 ns;

--------------------

clk <= '0';

wait for 1 ns;

pop <= '1';

push <= '0';

clear <= '0';

wait for 4 ns;

clk <= '1';

wait for 4 ns;

assert (sp = 0)
report
"Assert A11 : < sp /= 0 >"
severity warning;

assert (FULL_BAR = '1')
report
"Assert A11a : < FULL_BAR /= 1 >"
severity warning;

wait for 1 ns;

--------------------

clk <= '0';

wait for 1 ns;

pop <= '1';

push <= '0';

clear <= '0';

wait for 4 ns;

clk <= '1';

wait for 4 ns;

assert (sp = 0)
report
"Assert A12 : < sp /= 0 >"
severity warning;

assert (FULL_BAR = '1')
report
"Assert A12a : < FULL_BAR /= 1 >"
severity warning;

wait for 1 ns;

--------------------

end process;

end AA;

