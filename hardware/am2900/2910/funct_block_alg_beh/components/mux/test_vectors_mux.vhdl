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
	component cmux
	   port (
	R_sel : in MVL7;
	D_sel : in MVL7;
      uPC_sel : in MVL7;
    stack_sel : in MVL7;
       OEBAR : in MVL7;
	   sp : in INTEGER range 0 to 5;
            D : in MVL7_VECTOR(11 downto 0);
           RE : in MVL7_VECTOR(11 downto 0);
          uPC : in MVL7_VECTOR(11 downto 0);
            Y : out MVL7_VECTOR(11 downto 0)
		);
	end component;
									
    signal R_sel :  MVL7;
    signal D_sel :  MVL7;
  signal uPC_sel :  MVL7;
signal stack_sel :  MVL7;
    signal OEBAR :  MVL7;
       signal sp :  INTEGER range 0 to 5;
        signal D :  MVL7_VECTOR(11 downto 0);
       signal RE :  MVL7_VECTOR(11 downto 0);
      signal uPC :  MVL7_VECTOR(11 downto 0);
        signal Y :  MVL7_VECTOR(11 downto 0);

for all : cmux use entity work.multiplexer(multiplexer);

begin

CMUX1 : cmux port map(
	R_sel,
	D_sel,
      uPC_sel,
    stack_sel,
        OEBAR,
	   sp,
            D,
           RE,
          uPC,
            Y
);

process

begin

---------------------

R_sel <= '1';
 
D_sel <= '0';

uPC_sel <= '0';

stack_sel <= '0';

OEBAR <= '0';

sp <= 2;

RE <= "000000000000";

D <= "111111111111";

uPC <= "111111111111";

wait for 1 ns;

assert (Y = "000000000000")
report
"Assert 0 : < Y /= 000000000000 >" 
severity warning;

wait for 1 ns;

---------------------

R_sel <= '1';
 
D_sel <= '0';

uPC_sel <= '0';

stack_sel <= '0';

OEBAR <= '0';

sp <= 1;

RE <= "111111111111";

D <= "000000000000";

uPC <= "000000000000";

wait for 1 ns;

assert (Y = "111111111111")
report
"Assert 1 : < Y /= 111111111111 >" 
severity warning;

wait for 1 ns;

---------------------

R_sel <= '0';
 
D_sel <= '1';

uPC_sel <= '0';

stack_sel <= '0';

OEBAR <= '0';

sp <= 2;

RE <= "111111111111";

D <= "000000000000";

uPC <= "111111111111";

wait for 1 ns;

assert (Y = "000000000000")
report
"Assert 2 : < Y /= 000000000000 >" 
severity warning;

wait for 1 ns;

---------------------

R_sel <= '0';
 
D_sel <= '1';

uPC_sel <= '0';

stack_sel <= '0';

OEBAR <= '0';

sp <= 1;

RE <= "000000000000";

D <= "111111111111";

uPC <= "000000000000";

wait for 1 ns;

assert (Y = "111111111111")
report
"Assert 3 : < Y /= 111111111111 >" 
severity warning;

wait for 1 ns;

---------------------

R_sel <= '0';
 
D_sel <= '0';

uPC_sel <= '1';

stack_sel <= '0';

OEBAR <= '0';

sp <= 2;

RE <= "111111111111";

D <= "111111111111";

uPC <= "000000000000";

wait for 1 ns;

assert (Y = "000000000000")
report
"Assert 4 : < Y /= 000000000000 >" 
severity warning;

wait for 1 ns;

---------------------

R_sel <= '0';
 
D_sel <= '0';

uPC_sel <= '1';

stack_sel <= '0';

OEBAR <= '0';

sp <= 1;

RE <= "000000000000";

D <= "000000000000";

uPC <= "111111111111";

wait for 1 ns;

assert (Y = "111111111111")
report
"Assert 5 : < Y /= 111111111111 >" 
severity warning;

wait for 1 ns;

---------------------

R_sel <= '0';
 
D_sel <= '0';

uPC_sel <= '1';

stack_sel <= '0';

OEBAR <= '0';

sp <= 1;

RE <= "111111111111";

D <= "111111111111";

uPC <= "000000000000";

wait for 1 ns;

assert (Y = "000000000000")
report
"Assert 6 : < Y /= 000000000000 >" 
severity warning;

wait for 1 ns;

---------------------

R_sel <= '0';
 
D_sel <= '0';

uPC_sel <= '1';

stack_sel <= '0';

OEBAR <= '0';

sp <= 2;

RE <= "000000000000";

D <= "000000000000";

uPC <= "111111111111";

wait for 1 ns;

assert (Y = "111111111111")
report
"Assert 7 : < Y /= 111111111111 >" 
severity warning;

wait for 1 ns;

---------------------

R_sel <= '0';
 
D_sel <= '1';

uPC_sel <= '0';

stack_sel <= '0';

OEBAR <= '1';

sp <= 2;

RE <= "111111111111";

D <= "000000000000";

uPC <= "111111111111";

wait for 1 ns;

assert (Y = "ZZZZZZZZZZZZ")
report
"Assert 8 : < Y /= ZZZZZZZZZZZZ >" 
severity warning;

wait for 1 ns;

---------------------

R_sel <= '0';
 
D_sel <= '1';

uPC_sel <= '0';

stack_sel <= '0';

OEBAR <= '1';

sp <= 1;

RE <= "000000000000";

D <= "111111111111";

uPC <= "000000000000";

wait for 1 ns;

assert (Y = "ZZZZZZZZZZZZ")
report
"Assert 9 : < Y /= ZZZZZZZZZZZZ >" 
severity warning;

wait for 1 ns;

---------------------

end process;

end AA;
