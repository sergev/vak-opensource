--------------------------------------------------------------------------------
--
--   AM2901 Benchmark - test patterns for output shifter
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

entity E is
end;

architecture A of E is

 component output_and_shifter_inst
  port (
         I :  in MVL7_vector(8 downto 0);
         A, F, Q : in MVL7_vector(3 downto 0);
         OEbar : in MVL7;
         Y :  out MVL7_vector(3 downto 0);
         RAM0, RAM3, Q0, Q3 : out MVL7
       );
 end component;

 signal I :   MVL7_vector(8 downto 0);
 signal A, F, Q :  MVL7_vector(3 downto 0);
 signal OEbar : MVL7;
 signal Y :   MVL7_vector(3 downto 0);
 signal RAM0, RAM3, Q0, Q3 :  MVL7;

 for all : output_and_shifter_inst use entity 
                          work.output_and_shifter(output_and_shifter);

begin

output_and_shifter_inst1 : output_and_shifter_inst port map(
                                                            I,
                                                            A, F, Q,
                                                            OEbar,
                                                            Y ,
                                                            RAM0, RAM3, Q0, Q3
                                                            );

process

begin

-------------------------------------------------------------------------

F <= "0101";             --#1						
A <= "1110";
Q <= "0000";           -- Y gets F.
I <= "000000000";
OEBAR <= '0';

wait for 1 ns;

assert (Y = "0101")
report
"Assert a1 : < Y /= '0101'> "
severity warning;

assert (RAM0 = 'Z')
report
"Assert a1 : < RAM0 /= 'Z'> "
severity warning;

assert (RAM3 = 'Z')
report
"Assert a1 : < RAM3 /= 'Z'> "
severity warning;

assert (Q0 = 'Z')
report
"Assert a1 : < Q0 /= 'Z'> "
severity warning;

assert (Q3 = 'Z')
report
"Assert a1 : < Q3 /= 'Z'> "
severity warning;

wait for 1 ns;

-------------------------------------------------------------------------

F <= "0101";             --#2
A <= "0111";
Q <= "0000";            -- Y gets tri-stated.
I <= "000000000";
OEBAR <= '1';

wait for 1 ns;

assert (Y = "ZZZZ")
report
"Assert b1 : < Y /= 'ZZZZ'> "
severity warning;

assert (RAM0 = 'Z')
report
"Assert b1 : < RAM0 /= 'Z'> "
severity warning;

assert (RAM3 = 'Z')
report
"Assert b1 : < RAM3 /= 'Z'> "
severity warning;

assert (Q0 = 'Z')
report
"Assert b1 : < Q0 /= 'Z'> "
severity warning;

assert (Q3 = 'Z')
report
"Assert b1 : < Q3 /= 'Z'> "
severity warning;

wait for 1 ns;

-------------------------------------------------------------------------

F <= "1110";             --#3
A <= "0110";
Q <= "0000";           -- Y gets F.
I <= "001000000";
OEBAR <= '0';

wait for 1 ns;

assert (Y = "1110")
report
"Assert c1 : < Y /= '1110'> "
severity warning;

assert (RAM0 = 'Z')
report
"Assert c1 : < RAM0 /= 'Z'> "
severity warning;

assert (RAM3 = 'Z')
report
"Assert c1 : < RAM3 /= 'Z'> "
severity warning;

assert (Q0 = 'Z')
report
"Assert c1 : < Q0 /= 'Z'> "
severity warning;

assert (Q3 = 'Z')
report
"Assert c1 : < Q3 /= 'Z'> "
severity warning;

wait for 1 ns;

-------------------------------------------------------------------------

F <= "1110";             --#4
A <= "0110";
Q <= "0000";           -- Y gets A.
I <= "010000000";
OEBAR <= '0';

wait for 1 ns;

assert (Y = "0110")
report
"Assert d1 : < Y /= '0110'> "
severity warning;

assert (RAM0 = 'Z')
report
"Assert d1 : < RAM0 /= 'Z'> "
severity warning;

assert (RAM3 = 'Z')
report
"Assert d1 : < RAM3 /= 'Z'> "
severity warning;

assert (Q0 = 'Z')
report
"Assert d1 : < Q0 /= 'Z'> "
severity warning;

assert (Q3 = 'Z')
report
"Assert d1 : < Q3 /= 'Z'> "
severity warning;

wait for 1 ns;

-------------------------------------------------------------------------

F <= "1110";             --#5						
A <= "0111";
Q <= "0000";           -- Y gets F.
I <= "011000000";
OEBAR <= '0';

wait for 1 ns;

assert (Y = "1110")
report
"Assert e1 : < Y /= '1110'> "
severity warning;

assert (RAM0 = 'Z')
report
"Assert e1 : < RAM0 /= 'Z'> "
severity warning;

assert (RAM3 = 'Z')
report
"Assert e1 : < RAM3 /= 'Z'> "
severity warning;

assert (Q0 = 'Z')
report
"Assert e1 : < Q0 /= 'Z'> "
severity warning;

assert (Q3 = 'Z')
report
"Assert e1 : < Q3 /= 'Z'> "
severity warning;

wait for 1 ns;

-------------------------------------------------------------------------

F <= "1001";             --#6
A <= "1000";
Q <= "0100";           -- Y gets F. RAM0 gets F(0). Q0 gets Q(0)
I <= "100000000";
OEBAR <= '0';

wait for 1 ns;

assert (Y = "1001")
report
"Assert f1 : < Y /= '1001'> "
severity warning;

assert (RAM0 = '1')
report
"Assert f1 : < RAM0 /= '1'> "
severity warning;

assert (RAM3 = 'Z')
report
"Assert f1 : < RAM3 /= 'Z'> "
severity warning;

assert (Q0 = '0')
report
"Assert f1 : < Q0 /= '0'> "
severity warning;

assert (Q3 = 'Z')
report
"Assert f1 : < Q3 /= 'Z'> "
severity warning;

wait for 1 ns;

-------------------------------------------------------------------------

F <= "0110";             --#7
A <= "1110";
Q <= "1101";    -- Y gets F. RAM0 gets F(0). Q0 gets Q(0)
I <= "101000000";
OEBAR <= '0';

wait for 1 ns;

assert (Y = "0110")
report
"Assert g1 : < Y /= '0110'> "
severity warning;

assert (RAM0 = '0')
report
"Assert g1 : < RAM0 /= '0'> "
severity warning;

assert (RAM3 = 'Z')
report
"Assert g1 : < RAM3 /= 'Z'> "
severity warning;

assert (Q0 = '1')
report
"Assert g1 : < Q0 /= '1'> "
severity warning;

assert (Q3 = 'Z')
report
"Assert g1 : < Q3 /= 'Z'> "
severity warning;

wait for 1 ns;

-------------------------------------------------------------------------

F <= "0100";             --#8						
A <= "0010";
Q <= "1000";         -- Y gets F. RAM3 gets F(3). Q3 gets Q(3)
I <= "110000000";
OEBAR <= '0';

wait for 1 ns;

assert (Y = "0100")
report
"Assert h1 : < Y /= '0100'> "
severity warning;

assert (RAM0 = 'Z')
report
"Assert h1 : < RAM0 /= 'Z'> "
severity warning;

assert (RAM3 = '0')
report
"Assert h1 : < RAM3 /= '0'> "
severity warning;

assert (Q0 = 'Z')
report
"Assert h1 : < Q0 /= 'Z'> "
severity warning;

assert (Q3 = '1')
report
"Assert h1 : < Q3 /= '1'> "
severity warning;

wait for 1 ns;

-------------------------------------------------------------------------

F <= "0001";             --#9
A <= "1100";
Q <= "1000";        -- Y gets F. RAM3 gets F(3). Q3 gets Q(3)
I <= "111000000";
OEBAR <= '0';

wait for 1 ns;

assert (Y = "0001")
report
"Assert i1 : < Y /= '0001'> "
severity warning;

assert (RAM0 = 'Z')
report
"Assert i1 : < RAM0 /= 'Z'> "
severity warning;

assert (RAM3 = '0')
report
"Assert i1 : < RAM3 /= '0'> "
severity warning;

assert (Q0 = 'Z')
report
"Assert i1 : < Q0 /= 'Z'> "
severity warning;

assert (Q3 = '1')
report
"Assert i1 : < Q3 /= '1'> "
severity warning;

wait for 1 ns;

-------------------------------------------------------------------------
end process;

end A;