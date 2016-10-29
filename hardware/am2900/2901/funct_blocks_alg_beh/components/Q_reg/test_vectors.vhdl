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
use work.synthesis_types.all;

entity E is
end;

architecture A of E is

 component Q_reg_inst
  port (
           F : in MVL7_vector(3 downto 0);
           clk : in clock;
           I :  in MVL7_vector(8 downto 0); 
           Q0, Q3 : in MVL7;
           Q : inout MVL7_vector(3 downto 0)   
       );
 end component;

     signal F :  MVL7_vector(3 downto 0);
     signal clk :  clock;
     signal I :   MVL7_vector(8 downto 0); 
     signal Q0, Q3 :  MVL7;
     signal Q : MVL7_vector(3 downto 0);

 for all : Q_reg_inst use entity work.Q_reg(Q_reg);

begin

Q_reg_inst1 : Q_reg_inst port map(
                               F,
                               clk,
                               I,
                               Q0, Q3,
                               Q
                             );

process

begin

---------------------------------------------------------------------

I <= "000000000";    --#1
F <= "0111";       
Q0 <= 'Z';
Q3 <= 'Z';           --             load F into Q

wait for 1 ns;

clk <= '1';
 
wait for 1 ns;

clk <= '0';

wait for 1 ns;

assert (Q = "0111")
report
"Assert 1 : < Q /= '0111'> "
severity warning;

wait for 1 ns;

---------------------------------------------------------------------

I <= "001000000";    --#2
F <= "0000";
Q0 <= 'Z';           --        do nothing 
Q3 <= 'Z';

wait for 1 ns;

clk <= '1';
 
wait for 1 ns;

clk <= '0';

wait for 1 ns;

assert (Q = "0111")
report
"Assert 2 : < Q /= '0111'> "
severity warning;

wait for 1 ns;

---------------------------------------------------------------------

I <= "010000000";    --#3
F <= "0000";
Q0 <= 'Z';           --           do nothing
Q3 <= 'Z';

wait for 1 ns;

clk <= '1';
 
wait for 1 ns;

clk <= '0';

wait for 1 ns;

assert (Q = "0111")
report
"Assert 3 : < Q /= '0111'> "
severity warning;

wait for 1 ns;

---------------------------------------------------------------------

I <= "011000000";    --#4
F <= "0000";
Q0 <= 'Z';           --            do nothing
Q3 <= 'Z';

wait for 1 ns;

clk <= '1';
 
wait for 1 ns;

clk <= '0';

wait for 1 ns;

assert (Q = "0111")
report
"Assert 4 : < Q /= '0111'> "
severity warning;

wait for 1 ns;

---------------------------------------------------------------------

I <= "100000000";    --#5
F <= "0000";
Q0 <= 'Z';          
Q3 <= '1';           --         down shift Q with input 1

wait for 1 ns;

clk <= '1';
 
wait for 1 ns;

clk <= '0';

wait for 1 ns;

assert (Q = "1011")
report
"Assert 5 : < Q /= '1011'> "
severity warning;

wait for 1 ns;

---------------------------------------------------------------------

I <= "100000000";     --#6
F <= "0000";
Q0 <= 'Z';
Q3 <= '0';          --         down shift Q with input 0

wait for 1 ns;

clk <= '1';
 
wait for 1 ns;

clk <= '0';

wait for 1 ns;

assert (Q = "0101")
report
"Assert 6 : < Q /= '0101'> "
severity warning;

wait for 1 ns;

--------------------------------------------------------------------

I <= "101000000";    --#7
F <= "0000";
Q0 <= 'Z';
Q3 <= 'Z';           --      do nothing

wait for 1 ns;

clk <= '1';
 
wait for 1 ns;

clk <= '0';

wait for 1 ns;

assert (Q = "0101")
report
"Assert 7 : < Q /= '0101'> "
severity warning;

wait for 1 ns;

---------------------------------------------------------------------

I <= "110000000";    --#8
F <= "0000";
Q0 <= '1';           --       Up shift Q with input 1
Q3 <= 'Z';           

wait for 1 ns;

clk <= '1';
 
wait for 1 ns;

clk <= '0';

wait for 1 ns;

assert (Q = "1011")
report
"Assert 8 : < Q /= '1011'> "
severity warning;

wait for 1 ns;

---------------------------------------------------------------------

I <= "110000000";    --#9
F <= "0000";
Q0 <= '0';           --      Up shift Q with input 0
Q3 <= 'Z';

wait for 1 ns;

clk <= '1';
 
wait for 1 ns;

clk <= '0';

wait for 1 ns;

assert (Q = "0110")
report
"Assert 9 : < Q /= '0110'> "
severity warning;

wait for 1 ns;

---------------------------------------------------------------------

I <= "111000000";    --#10
F <= "0000";
Q0 <= 'Z';
Q3 <= 'Z';           --            do nothing

wait for 1 ns;

clk <= '1';
 
wait for 1 ns;

clk <= '0';

wait for 1 ns;

assert (Q = "0110")
report
"Assert 10 : < Q /= '0110'> "
severity warning;

wait for 1 ns;

---------------------------------------------------------------------

end process;

end A;

