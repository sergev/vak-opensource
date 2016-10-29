--------------------------------------------------------------------------------
--
--   AM2901 Benchmark -- mem test vectors
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

 component mem_inst
  port (
	    RAM : inout Memory(15 downto 0);
            F : in MVL7_vector(3 downto 0);
            clk :  in clock; 
            I :  in MVL7_vector(8 downto 0);
            RAM0, RAM3 : in MVL7;
            Aadd, Badd : in integer range 15 downto 0
       );
 end component;

 signal RAM : Memory(15 downto 0);
 signal F :  MVL7_vector(3 downto 0);
 signal clk :   clock; 
 signal I :   MVL7_vector(8 downto 0);
 signal RAM0, RAM3 :  MVL7;
 signal Aadd, Badd :  integer range 15 downto 0;

 for all : mem_inst use entity work.mem(mem);

begin

mem_inst1 : mem_inst port map(
	    RAM,
            F ,
            clk,
            I ,
            RAM0, RAM3 ,
            Aadd, Badd 
                             );

process

begin

----------------------------------------------------------

F <= "0111";         --#1
I <= "010000000";
RAM0 <= 'Z';
RAM3 <= 'Z';
Aadd <= 0;           -- Load F into RAM(Badd) where Badd = 0
Badd <= 0;

wait for 1 ns;

clk <= '1';
 
wait for 1 ns;

clk <= '0';

wait for 1 ns;

assert (RAM(Aadd) = "0111")
report
"Assert 1 : < RAM(Aadd) /= '0111'> "
severity warning;

assert(RAM(Badd) = "0111")
report
"Assert 2 : < RAM(Badd) /= '0111'> "
severity warning;

wait for 1 ns;

----------------------------------------------------------

F <= "1001";         --#2
I <= "011000000";
RAM0 <= 'Z';
RAM3 <= 'Z';
Aadd <= 0;           -- Load F into RAM(Badd) where Badd = 1
Badd <= 1;

wait for 1 ns;

clk <= '1';
 
wait for 1 ns;

clk <= '0';

wait for 1 ns;

assert (RAM(Aadd) = "0111")
report
"Assert a1 : < RAM(Aadd) /= '0111'> "
severity warning;

assert(RAM(Badd) = "1001")
report
"Assert a2 : < RAM(Badd) /= '1001'> "
severity warning;

wait for 1 ns;

----------------------------------------------------------

F <= "0000";         --#3
I <= "000000000";
RAM0 <= 'Z';
RAM3 <= 'Z';
Aadd <= 1;             --  Do nothing
Badd <= 0;

wait for 1 ns;

clk <= '1';
 
wait for 1 ns;

clk <= '0';

wait for 1 ns;

assert (RAM(Aadd) = "1001")
report
"Assert b1 : < RAM(Aadd) /= '1001'> "
severity warning;

assert(RAM(Badd) = "0111")
report
"Assert b2 : < RAM(Badd) /= '0111'> "
severity warning;

wait for 1 ns;

----------------------------------------------------------

F <= "1111";         --#4
I <= "001000000";    
RAM0 <= 'Z';
RAM3 <= 'Z';         --         Do nothing
Aadd <= 0;
Badd <= 1;

wait for 1 ns;

clk <= '1';
 
wait for 1 ns;

clk <= '0';

wait for 1 ns;

assert (RAM(Aadd) = "0111")
report
"Assert c1 : < RAM(Aadd) /= '0111'> "
severity warning;

assert(RAM(Badd) = "1001")
report
"Assert c2 : < RAM(Badd) /= '1001'> "
severity warning;

wait for 1 ns;

----------------------------------------------------------

F <= "1001";         --#5
I <= "100000000";
RAM0 <= 'Z';
RAM3 <= '0';
Aadd <= 1;           -- Down shift F and load that into RAM(Badd)
Badd <= 2;           --  with input 0.  Badd = 2.

wait for 1 ns;

clk <= '1';
 
wait for 1 ns;

clk <= '0';

wait for 1 ns;

assert (RAM(Aadd) = "1001")
report
"Assert d1 : < RAM(Aadd) /= '1001'> "
severity warning;

assert(RAM(Badd) = "0100")
report
"Assert d2 : < RAM(Badd) /= '0100'> "
severity warning;

wait for 1 ns;

----------------------------------------------------------

F <= "0111";         --#6
I <= "100000000";
RAM0 <= 'Z';
RAM3 <= '1';
Aadd <= 0;          -- Down shift F and load that into RAM(Badd)
Badd <= 3;          --  with input 1.  Badd = 3

wait for 1 ns;

clk <= '1';
 
wait for 1 ns;

clk <= '0';

wait for 1 ns;

assert (RAM(Aadd) = "0111")
report
"Assert e1 : < RAM(Aadd) /= '0111'> "
severity warning;

assert(RAM(Badd) = "1011")
report
"Assert e2 : < RAM(Badd) /= '1011'> "
severity warning;

wait for 1 ns;

----------------------------------------------------------

F <= "1101";         --#7
I <= "101000000";
RAM0 <= 'Z';
RAM3 <= '0';
Aadd <= 2;       -- Down shift F and load that into RAM(Badd)
Badd <= 4;       --  with input 0.  Badd = 4

wait for 1 ns;

clk <= '1';
 
wait for 1 ns;

clk <= '0';

wait for 1 ns;

assert (RAM(Aadd) = "0100")
report
"Assert f1 : < RAM(Aadd) /= '0100'> "
severity warning;

assert(RAM(Badd) = "0110")
report
"Assert f2 : < RAM(Badd) /= '0110'> "
severity warning;

wait for 1 ns;

----------------------------------------------------------

F <= "1101";         --#8
I <= "101000000";
RAM0 <= 'Z';
RAM3 <= '1';      -- Down shift F and load that into RAM(Badd)
Aadd <= 3;        --  with input 1.  Badd = 5
Badd <= 5;

wait for 1 ns;

clk <= '1';
 
wait for 1 ns;

clk <= '0';

wait for 1 ns;

assert (RAM(Aadd) = "1011")
report
"Assert g1 : < RAM(Aadd) /= '1011'> "
severity warning;

assert(RAM(Badd) = "1110")
report
"Assert g2 : < RAM(Badd) /= '1110'> "
severity warning;

wait for 1 ns;

----------------------------------------------------------

F <= "0110";         --#9
I <= "110000000";
RAM0 <= '0';
RAM3 <= 'Z';        -- Up shift F and load that into RAM(Badd)
Aadd <= 4;          --  with input 0.  Badd = 2
Badd <= 2;

wait for 1 ns;

clk <= '1';
 
wait for 1 ns;

clk <= '0';

wait for 1 ns;

assert (RAM(Aadd) = "0110")
report
"Assert h1 : < RAM(Aadd) /= '0110'> "
severity warning;

assert(RAM(Badd) = "1100")
report
"Assert h2 : < RAM(Badd) /= '1100'> "
severity warning;

wait for 1 ns;

----------------------------------------------------------

F <= "0000";         --#10
I <= "110000000";
RAM0 <= '1';
RAM3 <= 'Z';    -- Up shift F and load that into RAM(Badd)
Aadd <= 2;      --  with input 1.  Badd = 6
Badd <= 6;

wait for 1 ns;

clk <= '1';
 
wait for 1 ns;

clk <= '0';

wait for 1 ns;

assert (RAM(Aadd) = "1100")
report
"Assert i1 : < RAM(Aadd) /= '1100'> "
severity warning;

assert(RAM(Badd) = "0001")
report
"Assert i2 : < RAM(Badd) /= '0001'> "
severity warning;

wait for 1 ns;

----------------------------------------------------------

F <= "1101";         --#11
I <= "111000000";
RAM0 <= '0';
RAM3 <= 'Z';
Aadd <= 5;    -- Up shift F and load that into RAM(Badd)
Badd <= 7;    --  with input 0.  Badd = 7

wait for 1 ns;

clk <= '1';
 
wait for 1 ns;

clk <= '0';

wait for 1 ns;

assert (RAM(Aadd) = "1110")
report
"Assert j1 : < RAM(Aadd) /= '1110'> "
severity warning;

assert(RAM(Badd) = "1010")
report
"Assert j2 : < RAM(Badd) /= '1010'> "
severity warning;

wait for 1 ns;

----------------------------------------------------------

F <= "0010";         --#12
I <= "111000000";
RAM0 <= '1';
RAM3 <= 'Z';
Aadd <= 6;       -- Up shift F and load that into RAM(Badd)
Badd <= 8;       --  with input 1.  Badd = 8

wait for 1 ns;

clk <= '1';
 
wait for 1 ns;

clk <= '0';

wait for 1 ns;

assert (RAM(Aadd) = "0001")
report
"Assert k1 : < RAM(Aadd) /= '0001'> "
severity warning;

assert(RAM(Badd) = "0101")
report
"Assert k2 : < RAM(Badd) /= '0101'> "
severity warning;

wait for 1 ns;

----------------------------------------------------------


end process;

end A;