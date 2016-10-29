--------------------------------------------------------------------------------
--
--   AMD 2910 Benchmark
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

use work.types.all;
use work.MVL7_functions.all;	--some binary functions
use work.synthesis_types.all;	--hints for synthesis

entity E is
end;

architecture AA of E is
	 component am_func2910
	  port (
		I : in MVL7_VECTOR(3 downto 0);
		CCEN_BAR : in MVL7;
		CC_BAR : in MVL7;
		RLD_BAR : in MVL7;
		CI : in MVL7;
		OEbar : in MVL7;
		clk : in clock;
		D : in MVL7_VECTOR(11 downto 0);
		Y : out MVL7_VECTOR(11 downto 0);
		PL_BAR : out MVL7;
		VECT_BAR : out MVL7;
		MAP_BAR : out MVL7;
		FULL_BAR : out MVL7
	  );
	 end component;

		signal I : MVL7_VECTOR(3 downto 0);
		signal CCEN_BAR : MVL7;
		signal CC_BAR : MVL7;
		signal RLD_BAR : MVL7;
		signal CI : MVL7;
		signal OEbar : MVL7;
		signal clk : clock;
		signal D : MVL7_VECTOR(11 downto 0);
		signal Y : MVL7_VECTOR(11 downto 0);
		signal PL_BAR : MVL7;
		signal VECT_BAR : MVL7;
		signal MAP_BAR : MVL7;
		signal FULL_BAR : MVL7;

for all : am_func2910 use entity work.AM2910(AM2910);

begin

AM_FUNC1 : am_func2910 port map(
		I,
		CCEN_BAR,
		CC_BAR,
		RLD_BAR,
		CI,
		OEbar,
		clk,
		D,
		Y,
		PL_BAR,
		VECT_BAR,
		MAP_BAR,
		FULL_BAR 
           );

process

begin

-- **************************
-- *	                 *
-- * TEST VECTORS FOR STACK *
-- *       	         *
-- **************************
-- *********** PAIRS TESTING PUSH AND POP ************
-- ******* PUSH ( I = 1) & POP ( I = 8 ) & OTHER CONDITIONS ***********  
-- ** LOAD R **
--------------------------

clk <= '1'; --	 Cycle No: 0

wait for 1 ns;

I <= "0000";
CCEN_BAR <= '0';
CC_BAR <= '0';
RLD_BAR <= '0';
CI <= '0';
OEbar <= '0';
D <= "000000000000";

wait for 4 ns;

clk <= '0';

wait for 4 ns;

wait for 1 ns;

-- ** Y = D **
--------------------------

clk <= '1'; --	 Cycle No: 1

wait for 1 ns;

I <= "0010";
CCEN_BAR <= '0';
CC_BAR <= '0';
RLD_BAR <= '1';
CI <= '0';
OEbar <= '0';
D <= "000000000000";

wait for 4 ns;

clk <= '0';

wait for 4 ns;

wait for 1 ns;

-- ** PUSH **
--------------------------

clk <= '1'; --	 Cycle No: 2

wait for 1 ns;

I <= "0001";
CCEN_BAR <= '0';
CC_BAR <= '0';
RLD_BAR <= '1';
CI <= '0';
OEbar <= '0';

wait for 4 ns;

clk <= '0';

wait for 4 ns;

wait for 1 ns;

-- ** Y = TOP OF STACK **
--------------------------

clk <= '1'; --	 Cycle No: 3

wait for 1 ns;

I <= "1101";
CCEN_BAR <= '0';
CC_BAR <= '1';
RLD_BAR <= '1';
CI <= '0';
OEbar <= '0';

wait for 4 ns;

clk <= '0';

wait for 4 ns;

assert (Y = "000000000000")
report
"Assert 0 : < Y /= 000000000000 >" -- 	Vector No: 0
severity warning;

assert (FULL_BAR = '1')
report
"Assert 1 : < FULL_BAR /= 1 >"
severity warning;

wait for 1 ns;

-- ** Y = D **
--------------------------

clk <= '1'; --	 Cycle No: 4

wait for 1 ns;

I <= "0010";
CCEN_BAR <= '0';
CC_BAR <= '0';
RLD_BAR <= '1';
CI <= '0';
OEbar <= '0';
D <= "111111111111";

wait for 4 ns;

clk <= '0';

wait for 4 ns;

wait for 1 ns;

-- ** PUSH **
--------------------------

clk <= '1'; --	 Cycle No: 5

wait for 1 ns;

I <= "0001";
CCEN_BAR <= '0';
CC_BAR <= '0';
RLD_BAR <= '1';
CI <= '0';
OEbar <= '0';

wait for 4 ns;

clk <= '0';

wait for 4 ns;

wait for 1 ns;

-- ** Y = TOP OF STACK **
--------------------------

clk <= '1'; --	 Cycle No: 6

wait for 1 ns;

I <= "1101";
CCEN_BAR <= '0';
CC_BAR <= '1';
RLD_BAR <= '1';
CI <= '0';
OEbar <= '0';

wait for 4 ns;

clk <= '0';

wait for 4 ns;

assert (Y = "111111111111")
report
"Assert 2 : < Y /= 111111111111 >" -- 	Vector No: 1
severity warning;

assert (FULL_BAR = '1')
report
"Assert 3 : < FULL_BAR /= 1 >"
severity warning;

wait for 1 ns;

-- **************************************************
-- ** LOAD R **
--------------------------

clk <= '1'; --	 Cycle No: 7

wait for 1 ns;

I <= "1110";
CCEN_BAR <= '0';
CC_BAR <= '0';
RLD_BAR <= '0';
CI <= '0';
OEbar <= '0';
D <= "000000000000";

wait for 4 ns;

clk <= '0';

wait for 4 ns;

wait for 1 ns;

-- ** Y = TOP OF STACK
--------------------------

clk <= '1'; --	 Cycle No: 8

wait for 1 ns;

I <= "1101";
CCEN_BAR <= '0';
CC_BAR <= '1';
RLD_BAR <= '1';
CI <= '0';
OEbar <= '0';

wait for 4 ns;

clk <= '0';

wait for 4 ns;

assert (Y = "111111111111")
report
"Assert 4 : < Y /= 111111111111 >" -- 	Vector No: 2
severity warning;

assert (FULL_BAR = '1')
report
"Assert 5 : < FULL_BAR /= 1 >"
severity warning;

wait for 1 ns;

-- ** POP **
--------------------------

clk <= '1'; --	 Cycle No: 9

wait for 1 ns;

I <= "1000";
CCEN_BAR <= '0';
CC_BAR <= '0';
RLD_BAR <= '1';
CI <= '0';
OEbar <= '0';

wait for 4 ns;

clk <= '0';

wait for 4 ns;

assert (FULL_BAR = '1')
report
"Assert 6 : < FULL_BAR /= 1 >" -- 	Vector No: 3
severity warning;

wait for 1 ns;

-- ** Y = TOP OF STACK
--------------------------

clk <= '1'; --	 Cycle No: 10

wait for 1 ns;

I <= "1101";
CCEN_BAR <= '0';
CC_BAR <= '1';
RLD_BAR <= '1';
CI <= '0';
OEbar <= '0';

wait for 4 ns;

clk <= '0';

wait for 4 ns;

assert (Y = "000000000000")
report
"Assert 7 : < Y /= 000000000000 >" -- 	Vector No: 4
severity warning;

assert (FULL_BAR = '1')
report
"Assert 8 : < FULL_BAR /= 1 >"
severity warning;

wait for 1 ns;

-- **************************************************
-- **************************************************
-- ******* PUSH ( I = 1) & POP ( I = 8 ) & OTHER CONDITIONS ***********  
-- ** LOAD R **
--------------------------

clk <= '1'; --	 Cycle No: 11

wait for 1 ns;

I <= "0000";
CCEN_BAR <= '0';
CC_BAR <= '0';
RLD_BAR <= '0';
CI <= '0';
OEbar <= '0';
D <= "111111111111";

wait for 4 ns;

clk <= '0';

wait for 4 ns;

wait for 1 ns;

-- ** Y = D **
--------------------------

clk <= '1'; --	 Cycle No: 12

wait for 1 ns;

I <= "0010";
CCEN_BAR <= '0';
CC_BAR <= '0';
RLD_BAR <= '1';
CI <= '0';
OEbar <= '0';
D <= "000000000000";

wait for 4 ns;

clk <= '0';

wait for 4 ns;

wait for 1 ns;

-- ** PUSH **
--------------------------

clk <= '1'; --	 Cycle No: 13

wait for 1 ns;

I <= "0001";
CCEN_BAR <= '0';
CC_BAR <= '0';
RLD_BAR <= '1';
CI <= '0';
OEbar <= '0';

wait for 4 ns;

clk <= '0';

wait for 4 ns;

wait for 1 ns;

-- ** Y = TOP OF STACK **
--------------------------

clk <= '1'; --	 Cycle No: 14

wait for 1 ns;

I <= "1101";
CCEN_BAR <= '0';
CC_BAR <= '1';
RLD_BAR <= '1';
CI <= '0';
OEbar <= '0';

wait for 4 ns;

clk <= '0';

wait for 4 ns;

assert (Y = "000000000000")
report
"Assert 9 : < Y /= 000000000000 >" -- 	Vector No: 5
severity warning;

assert (FULL_BAR = '1')
report
"Assert 10 : < FULL_BAR /= 1 >"
severity warning;

wait for 1 ns;

-- ** Y = D **
--------------------------

clk <= '1'; --	 Cycle No: 15

wait for 1 ns;

I <= "0010";
CCEN_BAR <= '0';
CC_BAR <= '0';
RLD_BAR <= '1';
CI <= '0';
OEbar <= '0';
D <= "111111111111";

wait for 4 ns;

clk <= '0';

wait for 4 ns;

wait for 1 ns;

-- ** PUSH **
--------------------------

clk <= '1'; --	 Cycle No: 16

wait for 1 ns;

I <= "0001";
CCEN_BAR <= '0';
CC_BAR <= '0';
RLD_BAR <= '1';
CI <= '0';
OEbar <= '0';

wait for 4 ns;

clk <= '0';

wait for 4 ns;

wait for 1 ns;

-- ** Y = TOP OF STACK **
--------------------------

clk <= '1'; --	 Cycle No: 17

wait for 1 ns;

I <= "1101";
CCEN_BAR <= '0';
CC_BAR <= '1';
RLD_BAR <= '1';
CI <= '0';
OEbar <= '0';

wait for 4 ns;

clk <= '0';

wait for 4 ns;

assert (Y = "111111111111")
report
"Assert 11 : < Y /= 111111111111 >" -- 	Vector No: 6
severity warning;

assert (FULL_BAR = '1')
report
"Assert 12 : < FULL_BAR /= 1 >"
severity warning;

wait for 1 ns;

-- **************************************************
-- ** LOAD R **
--------------------------

clk <= '1'; --	 Cycle No: 18

wait for 1 ns;

I <= "1110";
CCEN_BAR <= '0';
CC_BAR <= '0';
RLD_BAR <= '0';
CI <= '0';
OEbar <= '0';
D <= "000000000000";

wait for 4 ns;

clk <= '0';

wait for 4 ns;

wait for 1 ns;

-- ** Y = TOP OF STACK
--------------------------

clk <= '1'; --	 Cycle No: 19

wait for 1 ns;

I <= "1101";
CCEN_BAR <= '0';
CC_BAR <= '1';
RLD_BAR <= '1';
CI <= '0';
OEbar <= '0';

wait for 4 ns;

clk <= '0';

wait for 4 ns;

assert (Y = "111111111111")
report
"Assert 13 : < Y /= 111111111111 >" -- 	Vector No: 7
severity warning;

assert (FULL_BAR = '1')
report
"Assert 14 : < FULL_BAR /= 1 >"
severity warning;

wait for 1 ns;

-- ** POP **
--------------------------

clk <= '1'; --	 Cycle No: 20

wait for 1 ns;

I <= "1000";
CCEN_BAR <= '0';
CC_BAR <= '1';
RLD_BAR <= '1';
CI <= '0';
OEbar <= '0';

wait for 4 ns;

clk <= '0';

wait for 4 ns;

assert (FULL_BAR = '1')
report
"Assert 15 : < FULL_BAR /= 1 >" -- 	Vector No: 8
severity warning;

wait for 1 ns;

-- ** Y = TOP OF STACK
--------------------------

clk <= '1'; --	 Cycle No: 21

wait for 1 ns;

I <= "1101";
CCEN_BAR <= '0';
CC_BAR <= '1';
RLD_BAR <= '1';
CI <= '0';
OEbar <= '0';

wait for 4 ns;

clk <= '0';

wait for 4 ns;

assert (Y = "000000000000")
report
"Assert 16 : < Y /= 000000000000 >" -- 	Vector No: 9
severity warning;

assert (FULL_BAR = '1')
report
"Assert 17 : < FULL_BAR /= 1 >"
severity warning;

wait for 1 ns;

-- **************************************************
-- **************************************************
-- ******* PUSH ( I = 1) & POP ( I = 8 ) & OTHER CONDITIONS ***********  
-- ** LOAD R **
--------------------------

clk <= '1'; --	 Cycle No: 22

wait for 1 ns;

I <= "0000";
CCEN_BAR <= '0';
CC_BAR <= '0';
RLD_BAR <= '0';
CI <= '0';
OEbar <= '0';
D <= "000000000000";

wait for 4 ns;

clk <= '0';

wait for 4 ns;

wait for 1 ns;

-- ** Y = D **
--------------------------

clk <= '1'; --	 Cycle No: 23

wait for 1 ns;

I <= "0010";
CCEN_BAR <= '0';
CC_BAR <= '0';
RLD_BAR <= '1';
CI <= '0';
OEbar <= '0';
D <= "000000000000";

wait for 4 ns;

clk <= '0';

wait for 4 ns;

wait for 1 ns;

-- ** PUSH **
--------------------------

clk <= '1'; --	 Cycle No: 24

wait for 1 ns;

I <= "0001";
CCEN_BAR <= '1';
CC_BAR <= '0';
RLD_BAR <= '1';
CI <= '0';
OEbar <= '0';

wait for 4 ns;

clk <= '0';

wait for 4 ns;

wait for 1 ns;

-- ** Y = TOP OF STACK **
--------------------------

clk <= '1'; --	 Cycle No: 25

wait for 1 ns;

I <= "1101";
CCEN_BAR <= '0';
CC_BAR <= '1';
RLD_BAR <= '1';
CI <= '0';
OEbar <= '0';

wait for 4 ns;

clk <= '0';

wait for 4 ns;

assert (Y = "000000000000")
report
"Assert 18 : < Y /= 000000000000 >" -- 	Vector No: 10
severity warning;

assert (FULL_BAR = '1')
report
"Assert 19 : < FULL_BAR /= 1 >"
severity warning;

wait for 1 ns;

-- ** Y = D **
--------------------------

clk <= '1'; --	 Cycle No: 26

wait for 1 ns;

I <= "0010";
CCEN_BAR <= '0';
CC_BAR <= '0';
RLD_BAR <= '1';
CI <= '0';
OEbar <= '0';
D <= "111111111111";

wait for 4 ns;

clk <= '0';

wait for 4 ns;

wait for 1 ns;

-- ** PUSH **
--------------------------

clk <= '1'; --	 Cycle No: 27

wait for 1 ns;

I <= "0001";
CCEN_BAR <= '1';
CC_BAR <= '0';
RLD_BAR <= '1';
CI <= '0';
OEbar <= '0';

wait for 4 ns;

clk <= '0';

wait for 4 ns;

wait for 1 ns;

-- ** Y = TOP OF STACK **
--------------------------

clk <= '1'; --	 Cycle No: 28

wait for 1 ns;

I <= "1101";
CCEN_BAR <= '0';
CC_BAR <= '1';
RLD_BAR <= '1';
CI <= '0';
OEbar <= '0';

wait for 4 ns;

clk <= '0';

wait for 4 ns;

assert (Y = "111111111111")
report
"Assert 20 : < Y /= 111111111111 >" -- 	Vector No: 11
severity warning;

assert (FULL_BAR = '1')
report
"Assert 21 : < FULL_BAR /= 1 >"
severity warning;

wait for 1 ns;

-- **************************************************
-- ** LOAD R **
--------------------------

clk <= '1'; --	 Cycle No: 29

wait for 1 ns;

I <= "1110";
CCEN_BAR <= '0';
CC_BAR <= '0';
RLD_BAR <= '0';
CI <= '0';
OEbar <= '0';
D <= "000000000000";

wait for 4 ns;

clk <= '0';

wait for 4 ns;

wait for 1 ns;

-- ** Y = TOP OF STACK
--------------------------

clk <= '1'; --	 Cycle No: 30

wait for 1 ns;

I <= "1101";
CCEN_BAR <= '0';
CC_BAR <= '1';
RLD_BAR <= '1';
CI <= '0';
OEbar <= '0';

wait for 4 ns;

clk <= '0';

wait for 4 ns;

assert (Y = "111111111111")
report
"Assert 22 : < Y /= 111111111111 >" -- 	Vector No: 12
severity warning;

assert (FULL_BAR = '1')
report
"Assert 23 : < FULL_BAR /= 1 >"
severity warning;

wait for 1 ns;

-- ** POP **
--------------------------

clk <= '1'; --	 Cycle No: 31

wait for 1 ns;

I <= "1000";
CCEN_BAR <= '1';
CC_BAR <= '1';
RLD_BAR <= '1';
CI <= '0';
OEbar <= '0';

wait for 4 ns;

clk <= '0';

wait for 4 ns;

assert (FULL_BAR = '1')
report
"Assert 24 : < FULL_BAR /= 1 >" -- 	Vector No: 13
severity warning;

wait for 1 ns;

-- ** Y = TOP OF STACK
--------------------------

clk <= '1'; --	 Cycle No: 32

wait for 1 ns;

I <= "1101";
CCEN_BAR <= '0';
CC_BAR <= '1';
RLD_BAR <= '1';
CI <= '0';
OEbar <= '0';

wait for 4 ns;

clk <= '0';

wait for 4 ns;

assert (Y = "000000000000")
report
"Assert 25 : < Y /= 000000000000 >" -- 	Vector No: 14
severity warning;

assert (FULL_BAR = '1')
report
"Assert 26 : < FULL_BAR /= 1 >"
severity warning;

wait for 1 ns;

-- **************************************************
-- **************************************************
-- ******* PUSH ( I = 1) & POP ( I = 8 ) & OTHER CONDITIONS ***********  
-- ** LOAD R **
--------------------------

clk <= '1'; --	 Cycle No: 33

wait for 1 ns;

I <= "0000";
CCEN_BAR <= '0';
CC_BAR <= '0';
RLD_BAR <= '0';
CI <= '0';
OEbar <= '0';
D <= "111111111111";

wait for 4 ns;

clk <= '0';

wait for 4 ns;

wait for 1 ns;

-- ** Y = D **
--------------------------

clk <= '1'; --	 Cycle No: 34

wait for 1 ns;

I <= "0010";
CCEN_BAR <= '0';
CC_BAR <= '0';
RLD_BAR <= '1';
CI <= '0';
OEbar <= '0';
D <= "000000000000";

wait for 4 ns;

clk <= '0';

wait for 4 ns;

wait for 1 ns;

-- ** PUSH **
--------------------------

clk <= '1'; --	 Cycle No: 35

wait for 1 ns;

I <= "0001";
CCEN_BAR <= '1';
CC_BAR <= '0';
RLD_BAR <= '1';
CI <= '0';
OEbar <= '0';

wait for 4 ns;

clk <= '0';

wait for 4 ns;

wait for 1 ns;

-- ** Y = TOP OF STACK **
--------------------------

clk <= '1'; --	 Cycle No: 36

wait for 1 ns;

I <= "1101";
CCEN_BAR <= '0';
CC_BAR <= '1';
RLD_BAR <= '1';
CI <= '0';
OEbar <= '0';

wait for 4 ns;

clk <= '0';

wait for 4 ns;

assert (Y = "000000000000")
report
"Assert 27 : < Y /= 000000000000 >" -- 	Vector No: 15
severity warning;

assert (FULL_BAR = '1')
report
"Assert 28 : < FULL_BAR /= 1 >"
severity warning;

wait for 1 ns;

-- ** Y = D **
--------------------------

clk <= '1'; --	 Cycle No: 37

wait for 1 ns;

I <= "0010";
CCEN_BAR <= '0';
CC_BAR <= '0';
RLD_BAR <= '1';
CI <= '0';
OEbar <= '0';
D <= "111111111111";

wait for 4 ns;

clk <= '0';

wait for 4 ns;

wait for 1 ns;

-- ** PUSH **
--------------------------

clk <= '1'; --	 Cycle No: 38

wait for 1 ns;

I <= "0001";
CCEN_BAR <= '1';
CC_BAR <= '0';
RLD_BAR <= '1';
CI <= '0';
OEbar <= '0';

wait for 4 ns;

clk <= '0';

wait for 4 ns;

wait for 1 ns;

-- ** Y = TOP OF STACK **
--------------------------

clk <= '1'; --	 Cycle No: 39

wait for 1 ns;

I <= "1101";
CCEN_BAR <= '0';
CC_BAR <= '1';
RLD_BAR <= '1';
CI <= '0';
OEbar <= '0';

wait for 4 ns;

clk <= '0';

wait for 4 ns;

assert (Y = "111111111111")
report
"Assert 29 : < Y /= 111111111111 >" -- 	Vector No: 16
severity warning;

assert (FULL_BAR = '1')
report
"Assert 30 : < FULL_BAR /= 1 >"
severity warning;

wait for 1 ns;

-- **************************************************
-- ** LOAD R **
--------------------------

clk <= '1'; --	 Cycle No: 40

wait for 1 ns;

I <= "1110";
CCEN_BAR <= '0';
CC_BAR <= '0';
RLD_BAR <= '0';
CI <= '0';
OEbar <= '0';
D <= "000000000000";

wait for 4 ns;

clk <= '0';

wait for 4 ns;

wait for 1 ns;

-- ** Y = TOP OF STACK
--------------------------

clk <= '1'; --	 Cycle No: 41

wait for 1 ns;

I <= "1101";
CCEN_BAR <= '0';
CC_BAR <= '1';
RLD_BAR <= '1';
CI <= '0';
OEbar <= '0';

wait for 4 ns;

clk <= '0';

wait for 4 ns;

assert (Y = "111111111111")
report
"Assert 31 : < Y /= 111111111111 >" -- 	Vector No: 17
severity warning;

assert (FULL_BAR = '1')
report
"Assert 32 : < FULL_BAR /= 1 >"
severity warning;

wait for 1 ns;

-- ** POP **
--------------------------

clk <= '1'; --	 Cycle No: 42

wait for 1 ns;

I <= "1000";
CCEN_BAR <= '1';
CC_BAR <= '0';
RLD_BAR <= '1';
CI <= '0';
OEbar <= '0';

wait for 4 ns;

clk <= '0';

wait for 4 ns;

assert (FULL_BAR = '1')
report
"Assert 33 : < FULL_BAR /= 1 >" -- 	Vector No: 18
severity warning;

wait for 1 ns;

-- ** Y = TOP OF STACK
--------------------------

clk <= '1'; --	 Cycle No: 43

wait for 1 ns;

I <= "1101";
CCEN_BAR <= '0';
CC_BAR <= '1';
RLD_BAR <= '1';
CI <= '0';
OEbar <= '0';

wait for 4 ns;

clk <= '0';

wait for 4 ns;

assert (Y = "000000000000")
report
"Assert 34 : < Y /= 000000000000 >" -- 	Vector No: 19
severity warning;

assert (FULL_BAR = '1')
report
"Assert 35 : < FULL_BAR /= 1 >"
severity warning;

wait for 1 ns;

-- **************************************************
-- **************************************************
-- ******* PUSH ( I = 1) & POP ( I = 10 ) & OTHER CONDITIONS ***********  
-- ** LOAD R **
--------------------------

clk <= '1'; --	 Cycle No: 44

wait for 1 ns;

I <= "0000";
CCEN_BAR <= '0';
CC_BAR <= '0';
RLD_BAR <= '0';
CI <= '0';
OEbar <= '0';
D <= "000000000000";

wait for 4 ns;

clk <= '0';

wait for 4 ns;

wait for 1 ns;

-- ** Y = D **
--------------------------

clk <= '1'; --	 Cycle No: 45

wait for 1 ns;

I <= "0010";
CCEN_BAR <= '0';
CC_BAR <= '0';
RLD_BAR <= '1';
CI <= '0';
OEbar <= '0';
D <= "000000000000";

wait for 4 ns;

clk <= '0';

wait for 4 ns;

wait for 1 ns;

-- ** PUSH **
--------------------------

clk <= '1'; --	 Cycle No: 46

wait for 1 ns;

I <= "0001";
CCEN_BAR <= '1';
CC_BAR <= '1';
RLD_BAR <= '1';
CI <= '0';
OEbar <= '0';

wait for 4 ns;

clk <= '0';

wait for 4 ns;

wait for 1 ns;

-- ** Y = TOP OF STACK **
--------------------------

clk <= '1'; --	 Cycle No: 47

wait for 1 ns;

I <= "1101";
CCEN_BAR <= '0';
CC_BAR <= '1';
RLD_BAR <= '1';
CI <= '0';
OEbar <= '0';

wait for 4 ns;

clk <= '0';

wait for 4 ns;

assert (Y = "000000000000")
report
"Assert 36 : < Y /= 000000000000 >" -- 	Vector No: 20
severity warning;

assert (FULL_BAR = '1')
report
"Assert 37 : < FULL_BAR /= 1 >"
severity warning;

wait for 1 ns;

-- ** Y = D **
--------------------------

clk <= '1'; --	 Cycle No: 48

wait for 1 ns;

I <= "0010";
CCEN_BAR <= '0';
CC_BAR <= '0';
RLD_BAR <= '1';
CI <= '0';
OEbar <= '0';
D <= "111111111111";

wait for 4 ns;

clk <= '0';

wait for 4 ns;

wait for 1 ns;

-- ** PUSH **
--------------------------

clk <= '1'; --	 Cycle No: 49

wait for 1 ns;

I <= "0001";
CCEN_BAR <= '1';
CC_BAR <= '1';
RLD_BAR <= '1';
CI <= '0';
OEbar <= '0';

wait for 4 ns;

clk <= '0';

wait for 4 ns;

wait for 1 ns;

-- ** Y = TOP OF STACK **
--------------------------

clk <= '1'; --	 Cycle No: 50

wait for 1 ns;

I <= "1101";
CCEN_BAR <= '0';
CC_BAR <= '1';
RLD_BAR <= '1';
CI <= '0';
OEbar <= '0';

wait for 4 ns;

clk <= '0';

wait for 4 ns;

assert (Y = "111111111111")
report
"Assert 38 : < Y /= 111111111111 >" -- 	Vector No: 21
severity warning;

assert (FULL_BAR = '1')
report
"Assert 39 : < FULL_BAR /= 1 >"
severity warning;

wait for 1 ns;

-- **************************************************
-- ** LOAD R **
--------------------------

clk <= '1'; --	 Cycle No: 51

wait for 1 ns;

I <= "1110";
CCEN_BAR <= '0';
CC_BAR <= '0';
RLD_BAR <= '0';
CI <= '0';
OEbar <= '0';
D <= "000000000000";

wait for 4 ns;

clk <= '0';

wait for 4 ns;

wait for 1 ns;

-- ** Y = TOP OF STACK
--------------------------

clk <= '1'; --	 Cycle No: 52

wait for 1 ns;

I <= "1101";
CCEN_BAR <= '0';
CC_BAR <= '1';
RLD_BAR <= '1';
CI <= '0';
OEbar <= '0';

wait for 4 ns;

clk <= '0';

wait for 4 ns;

assert (Y = "111111111111")
report
"Assert 40 : < Y /= 111111111111 >" -- 	Vector No: 22
severity warning;

assert (FULL_BAR = '1')
report
"Assert 41 : < FULL_BAR /= 1 >"
severity warning;

wait for 1 ns;

-- ** POP **
--------------------------

clk <= '1'; --	 Cycle No: 53

wait for 1 ns;

I <= "1010";
CCEN_BAR <= '0';
CC_BAR <= '0';
RLD_BAR <= '1';
CI <= '0';
OEbar <= '0';

wait for 4 ns;

clk <= '0';

wait for 4 ns;

assert (FULL_BAR = '1')
report
"Assert 42 : < FULL_BAR /= 1 >" -- 	Vector No: 23
severity warning;

wait for 1 ns;

-- ** Y = TOP OF STACK
--------------------------

clk <= '1'; --	 Cycle No: 54

wait for 1 ns;

I <= "1101";
CCEN_BAR <= '0';
CC_BAR <= '1';
RLD_BAR <= '1';
CI <= '0';
OEbar <= '0';

wait for 4 ns;

clk <= '0';

wait for 4 ns;

assert (Y = "000000000000")
report
"Assert 43 : < Y /= 000000000000 >" -- 	Vector No: 24
severity warning;

assert (FULL_BAR = '1')
report
"Assert 44 : < FULL_BAR /= 1 >"
severity warning;

wait for 1 ns;

-- **************************************************
-- **************************************************
-- ******* PUSH ( I = 1) & POP ( I = 10 ) & OTHER CONDITIONS *********** 
-- ** LOAD R **
--------------------------

clk <= '1'; --	 Cycle No: 55

wait for 1 ns;

I <= "0000";
CCEN_BAR <= '0';
CC_BAR <= '0';
RLD_BAR <= '0';
CI <= '0';
OEbar <= '0';
D <= "111111111111";

wait for 4 ns;

clk <= '0';

wait for 4 ns;

wait for 1 ns;

-- ** Y = D **
--------------------------

clk <= '1'; --	 Cycle No: 56

wait for 1 ns;

I <= "0010";
CCEN_BAR <= '0';
CC_BAR <= '0';
RLD_BAR <= '1';
CI <= '0';
OEbar <= '0';
D <= "000000000000";

wait for 4 ns;

clk <= '0';

wait for 4 ns;

wait for 1 ns;

-- ** PUSH **
--------------------------

clk <= '1'; --	 Cycle No: 57

wait for 1 ns;

I <= "0001";
CCEN_BAR <= '1';
CC_BAR <= '1';
RLD_BAR <= '1';
CI <= '0';
OEbar <= '0';

wait for 4 ns;

clk <= '0';

wait for 4 ns;

wait for 1 ns;

-- ** Y = TOP OF STACK **
--------------------------

clk <= '1'; --	 Cycle No: 58

wait for 1 ns;

I <= "1101";
CCEN_BAR <= '0';
CC_BAR <= '1';
RLD_BAR <= '1';
CI <= '0';
OEbar <= '0';

wait for 4 ns;

clk <= '0';

wait for 4 ns;

assert (Y = "000000000000")
report
"Assert 45 : < Y /= 000000000000 >" -- 	Vector No: 25
severity warning;

assert (FULL_BAR = '1')
report
"Assert 46 : < FULL_BAR /= 1 >"
severity warning;

wait for 1 ns;

-- ** Y = D **
--------------------------

clk <= '1'; --	 Cycle No: 59

wait for 1 ns;

I <= "0010";
CCEN_BAR <= '0';
CC_BAR <= '0';
RLD_BAR <= '1';
CI <= '0';
OEbar <= '0';
D <= "111111111111";

wait for 4 ns;

clk <= '0';

wait for 4 ns;

wait for 1 ns;

-- ** PUSH **
--------------------------

clk <= '1'; --	 Cycle No: 60

wait for 1 ns;

I <= "0001";
CCEN_BAR <= '1';
CC_BAR <= '1';
RLD_BAR <= '1';
CI <= '0';
OEbar <= '0';

wait for 4 ns;

clk <= '0';

wait for 4 ns;

wait for 1 ns;

-- ** Y = TOP OF STACK **
--------------------------

clk <= '1'; --	 Cycle No: 61

wait for 1 ns;

I <= "1101";
CCEN_BAR <= '0';
CC_BAR <= '1';
RLD_BAR <= '1';
CI <= '0';
OEbar <= '0';

wait for 4 ns;

clk <= '0';

wait for 4 ns;

assert (Y = "111111111111")
report
"Assert 47 : < Y /= 111111111111 >" -- 	Vector No: 26
severity warning;

assert (FULL_BAR = '1')
report
"Assert 48 : < FULL_BAR /= 1 >"
severity warning;

wait for 1 ns;

-- **************************************************
-- ** LOAD R **
--------------------------

clk <= '1'; --	 Cycle No: 62

wait for 1 ns;

I <= "1110";
CCEN_BAR <= '0';
CC_BAR <= '0';
RLD_BAR <= '0';
CI <= '0';
OEbar <= '0';
D <= "111111111111";

wait for 4 ns;

clk <= '0';

wait for 4 ns;

wait for 1 ns;

-- ** Y = TOP OF STACK
--------------------------

clk <= '1'; --	 Cycle No: 63

wait for 1 ns;

I <= "1101";
CCEN_BAR <= '0';
CC_BAR <= '1';
RLD_BAR <= '1';
CI <= '0';
OEbar <= '0';

wait for 4 ns;

clk <= '0';

wait for 4 ns;

assert (Y = "111111111111")
report
"Assert 49 : < Y /= 111111111111 >" -- 	Vector No: 27
severity warning;

assert (FULL_BAR = '1')
report
"Assert 50 : < FULL_BAR /= 1 >"
severity warning;

wait for 1 ns;

-- ** POP **
--------------------------

clk <= '1'; --	 Cycle No: 64

wait for 1 ns;

I <= "1010";
CCEN_BAR <= '0';
CC_BAR <= '0';
RLD_BAR <= '1';
CI <= '0';
OEbar <= '0';

wait for 4 ns;

clk <= '0';

wait for 4 ns;

assert (FULL_BAR = '1')
report
"Assert 51 : < FULL_BAR /= 1 >" -- 	Vector No: 28
severity warning;

wait for 1 ns;

-- ** Y = TOP OF STACK
--------------------------

clk <= '1'; --	 Cycle No: 65

wait for 1 ns;

I <= "1101";
CCEN_BAR <= '0';
CC_BAR <= '1';
RLD_BAR <= '1';
CI <= '0';
OEbar <= '0';

wait for 4 ns;

clk <= '0';

wait for 4 ns;

assert (Y = "000000000000")
report
"Assert 52 : < Y /= 000000000000 >" -- 	Vector No: 29
severity warning;

assert (FULL_BAR = '1')
report
"Assert 53 : < FULL_BAR /= 1 >"
severity warning;

wait for 1 ns;

-- **************************************************
-- **************************************************
-- ******* PUSH ( I = 4) & POP ( I = 10 ) & OTHER CONDITIONS *********** 
-- ** LOAD R **
--------------------------

clk <= '1'; --	 Cycle No: 66

wait for 1 ns;

I <= "0000";
CCEN_BAR <= '0';
CC_BAR <= '0';
RLD_BAR <= '0';
CI <= '0';
OEbar <= '0';
D <= "000000000000";

wait for 4 ns;

clk <= '0';

wait for 4 ns;

wait for 1 ns;

-- ** Y = D **
--------------------------

clk <= '1'; --	 Cycle No: 67

wait for 1 ns;

I <= "0010";
CCEN_BAR <= '0';
CC_BAR <= '0';
RLD_BAR <= '1';
CI <= '0';
OEbar <= '0';
D <= "000000000000";

wait for 4 ns;

clk <= '0';

wait for 4 ns;

wait for 1 ns;

-- ** PUSH **
--------------------------

clk <= '1'; --	 Cycle No: 68

wait for 1 ns;

I <= "0100";
CCEN_BAR <= '0';
CC_BAR <= '0';
RLD_BAR <= '1';
CI <= '0';
OEbar <= '0';

wait for 4 ns;

clk <= '0';

wait for 4 ns;

wait for 1 ns;

-- ** Y = TOP OF STACK **
--------------------------

clk <= '1'; --	 Cycle No: 69

wait for 1 ns;

I <= "1101";
CCEN_BAR <= '0';
CC_BAR <= '1';
RLD_BAR <= '1';
CI <= '0';
OEbar <= '0';

wait for 4 ns;

clk <= '0';

wait for 4 ns;

assert (Y = "000000000000")
report
"Assert 54 : < Y /= 000000000000 >" -- 	Vector No: 30
severity warning;

assert (FULL_BAR = '1')
report
"Assert 55 : < FULL_BAR /= 1 >"
severity warning;

wait for 1 ns;

-- ** Y = D **
--------------------------

clk <= '1'; --	 Cycle No: 70

wait for 1 ns;

I <= "0010";
CCEN_BAR <= '0';
CC_BAR <= '0';
RLD_BAR <= '1';
CI <= '0';
OEbar <= '0';
D <= "111111111111";

wait for 4 ns;

clk <= '0';

wait for 4 ns;

wait for 1 ns;

-- ** PUSH **
--------------------------

clk <= '1'; --	 Cycle No: 71

wait for 1 ns;

I <= "0100";
CCEN_BAR <= '0';
CC_BAR <= '0';
RLD_BAR <= '1';
CI <= '0';
OEbar <= '0';

wait for 4 ns;

clk <= '0';

wait for 4 ns;

wait for 1 ns;

-- ** Y = TOP OF STACK **
--------------------------

clk <= '1'; --	 Cycle No: 72

wait for 1 ns;

I <= "1101";
CCEN_BAR <= '0';
CC_BAR <= '1';
RLD_BAR <= '1';
CI <= '0';
OEbar <= '0';

wait for 4 ns;

clk <= '0';

wait for 4 ns;

assert (Y = "111111111111")
report
"Assert 56 : < Y /= 111111111111 >" -- 	Vector No: 31
severity warning;

assert (FULL_BAR = '1')
report
"Assert 57 : < FULL_BAR /= 1 >"
severity warning;

wait for 1 ns;

-- **************************************************
-- ** LOAD R **
--------------------------

clk <= '1'; --	 Cycle No: 73

wait for 1 ns;

I <= "1110";
CCEN_BAR <= '0';
CC_BAR <= '0';
RLD_BAR <= '0';
CI <= '0';
OEbar <= '0';
D <= "000000000000";

wait for 4 ns;

clk <= '0';

wait for 4 ns;

wait for 1 ns;

-- ** Y = TOP OF STACK
--------------------------

clk <= '1'; --	 Cycle No: 74

wait for 1 ns;

I <= "1101";
CCEN_BAR <= '0';
CC_BAR <= '1';
RLD_BAR <= '1';
CI <= '0';
OEbar <= '0';

wait for 4 ns;

clk <= '0';

wait for 4 ns;

assert (Y = "111111111111")
report
"Assert 58 : < Y /= 111111111111 >" -- 	Vector No: 32
severity warning;

assert (FULL_BAR = '1')
report
"Assert 59 : < FULL_BAR /= 1 >"
severity warning;

wait for 1 ns;

-- ** POP **
--------------------------

clk <= '1'; --	 Cycle No: 75

wait for 1 ns;

I <= "1010";
CCEN_BAR <= '1';
CC_BAR <= '0';
RLD_BAR <= '1';
CI <= '0';
OEbar <= '0';

wait for 4 ns;

clk <= '0';

wait for 4 ns;

assert (FULL_BAR = '1')
report
"Assert 60 : < FULL_BAR /= 1 >" -- 	Vector No: 33
severity warning;

wait for 1 ns;

-- ** Y = TOP OF STACK
--------------------------

clk <= '1'; --	 Cycle No: 76

wait for 1 ns;

I <= "1101";
CCEN_BAR <= '0';
CC_BAR <= '1';
RLD_BAR <= '1';
CI <= '0';
OEbar <= '0';

wait for 4 ns;

clk <= '0';

wait for 4 ns;

assert (Y = "000000000000")
report
"Assert 61 : < Y /= 000000000000 >" -- 	Vector No: 34
severity warning;

assert (FULL_BAR = '1')
report
"Assert 62 : < FULL_BAR /= 1 >"
severity warning;

wait for 1 ns;

-- **************************************************
-- **************************************************
-- ******* PUSH ( I = 4) & POP ( I = 10 ) & OTHER CONDITIONS *********** 
-- ** LOAD R **
--------------------------

clk <= '1'; --	 Cycle No: 77

wait for 1 ns;

I <= "0000";
CCEN_BAR <= '0';
CC_BAR <= '0';
RLD_BAR <= '0';
CI <= '0';
OEbar <= '0';
D <= "111111111111";

wait for 4 ns;

clk <= '0';

wait for 4 ns;

wait for 1 ns;

-- ** Y = D **
--------------------------

clk <= '1'; --	 Cycle No: 78

wait for 1 ns;

I <= "0010";
CCEN_BAR <= '0';
CC_BAR <= '0';
RLD_BAR <= '1';
CI <= '0';
OEbar <= '0';
D <= "000000000000";

wait for 4 ns;

clk <= '0';

wait for 4 ns;

wait for 1 ns;

-- ** PUSH **
--------------------------

clk <= '1'; --	 Cycle No: 79

wait for 1 ns;

I <= "0100";
CCEN_BAR <= '0';
CC_BAR <= '0';
RLD_BAR <= '1';
CI <= '0';
OEbar <= '0';

wait for 4 ns;

clk <= '0';

wait for 4 ns;

wait for 1 ns;

-- ** Y = TOP OF STACK **
--------------------------

clk <= '1'; --	 Cycle No: 80

wait for 1 ns;

I <= "1101";
CCEN_BAR <= '0';
CC_BAR <= '1';
RLD_BAR <= '1';
CI <= '0';
OEbar <= '0';

wait for 4 ns;

clk <= '0';

wait for 4 ns;

assert (Y = "000000000000")
report
"Assert 63 : < Y /= 000000000000 >" -- 	Vector No: 35
severity warning;

assert (FULL_BAR = '1')
report
"Assert 64 : < FULL_BAR /= 1 >"
severity warning;

wait for 1 ns;

-- ** Y = D **
--------------------------

clk <= '1'; --	 Cycle No: 81

wait for 1 ns;

I <= "0010";
CCEN_BAR <= '0';
CC_BAR <= '0';
RLD_BAR <= '1';
CI <= '0';
OEbar <= '0';
D <= "111111111111";

wait for 4 ns;

clk <= '0';

wait for 4 ns;

wait for 1 ns;

-- ** PUSH **
--------------------------

clk <= '1'; --	 Cycle No: 82

wait for 1 ns;

I <= "0100";
CCEN_BAR <= '0';
CC_BAR <= '0';
RLD_BAR <= '1';
CI <= '0';
OEbar <= '0';

wait for 4 ns;

clk <= '0';

wait for 4 ns;

wait for 1 ns;

-- ** Y = TOP OF STACK **
--------------------------

clk <= '1'; --	 Cycle No: 83

wait for 1 ns;

I <= "1101";
CCEN_BAR <= '0';
CC_BAR <= '1';
RLD_BAR <= '1';
CI <= '0';
OEbar <= '0';

wait for 4 ns;

clk <= '0';

wait for 4 ns;

assert (Y = "111111111111")
report
"Assert 65 : < Y /= 111111111111 >" -- 	Vector No: 36
severity warning;

assert (FULL_BAR = '1')
report
"Assert 66 : < FULL_BAR /= 1 >"
severity warning;

wait for 1 ns;

-- **************************************************
-- ** LOAD R **
--------------------------

clk <= '1'; --	 Cycle No: 84

wait for 1 ns;

I <= "1110";
CCEN_BAR <= '0';
CC_BAR <= '0';
RLD_BAR <= '0';
CI <= '0';
OEbar <= '0';
D <= "111111111111";

wait for 4 ns;

clk <= '0';

wait for 4 ns;

wait for 1 ns;

-- ** Y = TOP OF STACK
--------------------------

clk <= '1'; --	 Cycle No: 85

wait for 1 ns;

I <= "1101";
CCEN_BAR <= '0';
CC_BAR <= '1';
RLD_BAR <= '1';
CI <= '0';
OEbar <= '0';

wait for 4 ns;

clk <= '0';

wait for 4 ns;

assert (Y = "111111111111")
report
"Assert 67 : < Y /= 111111111111 >" -- 	Vector No: 37
severity warning;

assert (FULL_BAR = '1')
report
"Assert 68 : < FULL_BAR /= 1 >"
severity warning;

wait for 1 ns;

-- ** POP **
--------------------------

clk <= '1'; --	 Cycle No: 86

wait for 1 ns;

I <= "1010";
CCEN_BAR <= '1';
CC_BAR <= '0';
RLD_BAR <= '1';
CI <= '0';
OEbar <= '0';

wait for 4 ns;

clk <= '0';

wait for 4 ns;

assert (FULL_BAR = '1')
report
"Assert 69 : < FULL_BAR /= 1 >" -- 	Vector No: 38
severity warning;

wait for 1 ns;

-- ** Y = TOP OF STACK
--------------------------

clk <= '1'; --	 Cycle No: 87

wait for 1 ns;

I <= "1101";
CCEN_BAR <= '0';
CC_BAR <= '1';
RLD_BAR <= '1';
CI <= '0';
OEbar <= '0';

wait for 4 ns;

clk <= '0';

wait for 4 ns;

assert (Y = "000000000000")
report
"Assert 70 : < Y /= 000000000000 >" -- 	Vector No: 39
severity warning;

assert (FULL_BAR = '1')
report
"Assert 71 : < FULL_BAR /= 1 >"
severity warning;

wait for 1 ns;

-- **************************************************
-- **************************************************
-- ******* PUSH ( I = 4) & POP ( I = 10 ) & OTHER CONDITIONS *********** 
-- ** LOAD R **
--------------------------

clk <= '1'; --	 Cycle No: 88

wait for 1 ns;

I <= "0000";
CCEN_BAR <= '0';
CC_BAR <= '0';
RLD_BAR <= '0';
CI <= '0';
OEbar <= '0';
D <= "000000000000";

wait for 4 ns;

clk <= '0';

wait for 4 ns;

wait for 1 ns;

-- ** Y = D **
--------------------------

clk <= '1'; --	 Cycle No: 89

wait for 1 ns;

I <= "0010";
CCEN_BAR <= '0';
CC_BAR <= '0';
RLD_BAR <= '1';
CI <= '0';
OEbar <= '0';
D <= "000000000000";

wait for 4 ns;

clk <= '0';

wait for 4 ns;

wait for 1 ns;

-- ** PUSH **
--------------------------

clk <= '1'; --	 Cycle No: 90

wait for 1 ns;

I <= "0100";
CCEN_BAR <= '0';
CC_BAR <= '1';
RLD_BAR <= '1';
CI <= '0';
OEbar <= '0';

wait for 4 ns;

clk <= '0';

wait for 4 ns;

wait for 1 ns;

-- ** Y = TOP OF STACK **
--------------------------

clk <= '1'; --	 Cycle No: 91

wait for 1 ns;

I <= "1101";
CCEN_BAR <= '0';
CC_BAR <= '1';
RLD_BAR <= '1';
CI <= '0';
OEbar <= '0';

wait for 4 ns;

clk <= '0';

wait for 4 ns;

assert (Y = "000000000000")
report
"Assert 72 : < Y /= 000000000000 >" -- 	Vector No: 40
severity warning;

assert (FULL_BAR = '1')
report
"Assert 73 : < FULL_BAR /= 1 >"
severity warning;

wait for 1 ns;

-- ** Y = D **
--------------------------

clk <= '1'; --	 Cycle No: 92

wait for 1 ns;

I <= "0010";
CCEN_BAR <= '0';
CC_BAR <= '0';
RLD_BAR <= '1';
CI <= '0';
OEbar <= '0';
D <= "111111111111";

wait for 4 ns;

clk <= '0';

wait for 4 ns;

wait for 1 ns;

-- ** PUSH **
--------------------------

clk <= '1'; --	 Cycle No: 93

wait for 1 ns;

I <= "0100";
CCEN_BAR <= '0';
CC_BAR <= '1';
RLD_BAR <= '1';
CI <= '0';
OEbar <= '0';

wait for 4 ns;

clk <= '0';

wait for 4 ns;

wait for 1 ns;

-- ** Y = TOP OF STACK **
--------------------------

clk <= '1'; --	 Cycle No: 94

wait for 1 ns;

I <= "1101";
CCEN_BAR <= '0';
CC_BAR <= '1';
RLD_BAR <= '1';
CI <= '0';
OEbar <= '0';

wait for 4 ns;

clk <= '0';

wait for 4 ns;

assert (Y = "111111111111")
report
"Assert 74 : < Y /= 111111111111 >" -- 	Vector No: 41
severity warning;

assert (FULL_BAR = '1')
report
"Assert 75 : < FULL_BAR /= 1 >"
severity warning;

wait for 1 ns;

-- **************************************************
-- ** LOAD R **
--------------------------

clk <= '1'; --	 Cycle No: 95

wait for 1 ns;

I <= "1110";
CCEN_BAR <= '0';
CC_BAR <= '0';
RLD_BAR <= '0';
CI <= '0';
OEbar <= '0';
D <= "000000000000";

wait for 4 ns;

clk <= '0';

wait for 4 ns;

wait for 1 ns;

-- ** Y = TOP OF STACK
--------------------------

clk <= '1'; --	 Cycle No: 96

wait for 1 ns;

I <= "1101";
CCEN_BAR <= '0';
CC_BAR <= '1';
RLD_BAR <= '1';
CI <= '0';
OEbar <= '0';

wait for 4 ns;

clk <= '0';

wait for 4 ns;

assert (Y = "111111111111")
report
"Assert 76 : < Y /= 111111111111 >" -- 	Vector No: 42
severity warning;

assert (FULL_BAR = '1')
report
"Assert 77 : < FULL_BAR /= 1 >"
severity warning;

wait for 1 ns;

-- ** POP **
--------------------------

clk <= '1'; --	 Cycle No: 97

wait for 1 ns;

I <= "1010";
CCEN_BAR <= '1';
CC_BAR <= '1';
RLD_BAR <= '1';
CI <= '0';
OEbar <= '0';

wait for 4 ns;

clk <= '0';

wait for 4 ns;

assert (FULL_BAR = '1')
report
"Assert 78 : < FULL_BAR /= 1 >" -- 	Vector No: 43
severity warning;

wait for 1 ns;

-- ** Y = TOP OF STACK
--------------------------

clk <= '1'; --	 Cycle No: 98

wait for 1 ns;

I <= "1101";
CCEN_BAR <= '0';
CC_BAR <= '1';
RLD_BAR <= '1';
CI <= '0';
OEbar <= '0';

wait for 4 ns;

clk <= '0';

wait for 4 ns;

assert (Y = "000000000000")
report
"Assert 79 : < Y /= 000000000000 >" -- 	Vector No: 44
severity warning;

assert (FULL_BAR = '1')
report
"Assert 80 : < FULL_BAR /= 1 >"
severity warning;

wait for 1 ns;

-- **************************************************
-- **************************************************
-- ******* PUSH ( I = 4) & POP ( I = 10 ) & OTHER CONDITIONS *********** 
-- ** LOAD R **
--------------------------

clk <= '1'; --	 Cycle No: 99

wait for 1 ns;

I <= "0000";
CCEN_BAR <= '0';
CC_BAR <= '0';
RLD_BAR <= '0';
CI <= '0';
OEbar <= '0';
D <= "111111111111";

wait for 4 ns;

clk <= '0';

wait for 4 ns;

wait for 1 ns;

-- ** Y = D **
--------------------------

clk <= '1'; --	 Cycle No: 100

wait for 1 ns;

I <= "0010";
CCEN_BAR <= '0';
CC_BAR <= '0';
RLD_BAR <= '1';
CI <= '0';
OEbar <= '0';
D <= "000000000000";

wait for 4 ns;

clk <= '0';

wait for 4 ns;

wait for 1 ns;

-- ** PUSH **
--------------------------

clk <= '1'; --	 Cycle No: 101

wait for 1 ns;

I <= "0100";
CCEN_BAR <= '0';
CC_BAR <= '1';
RLD_BAR <= '1';
CI <= '0';
OEbar <= '0';

wait for 4 ns;

clk <= '0';

wait for 4 ns;

wait for 1 ns;

-- ** Y = TOP OF STACK **
--------------------------

clk <= '1'; --	 Cycle No: 102

wait for 1 ns;

I <= "1101";
CCEN_BAR <= '0';
CC_BAR <= '1';
RLD_BAR <= '1';
CI <= '0';
OEbar <= '0';

wait for 4 ns;

clk <= '0';

wait for 4 ns;

assert (Y = "000000000000")
report
"Assert 81 : < Y /= 000000000000 >" -- 	Vector No: 45
severity warning;

assert (FULL_BAR = '1')
report
"Assert 82 : < FULL_BAR /= 1 >"
severity warning;

wait for 1 ns;

-- ** Y = D **
--------------------------

clk <= '1'; --	 Cycle No: 103

wait for 1 ns;

I <= "0010";
CCEN_BAR <= '0';
CC_BAR <= '0';
RLD_BAR <= '1';
CI <= '0';
OEbar <= '0';
D <= "111111111111";

wait for 4 ns;

clk <= '0';

wait for 4 ns;

wait for 1 ns;

-- ** PUSH **
--------------------------

clk <= '1'; --	 Cycle No: 104

wait for 1 ns;

I <= "0100";
CCEN_BAR <= '0';
CC_BAR <= '1';
RLD_BAR <= '1';
CI <= '0';
OEbar <= '0';

wait for 4 ns;

clk <= '0';

wait for 4 ns;

wait for 1 ns;

-- ** Y = TOP OF STACK **
--------------------------

clk <= '1'; --	 Cycle No: 105

wait for 1 ns;

I <= "1101";
CCEN_BAR <= '0';
CC_BAR <= '1';
RLD_BAR <= '1';
CI <= '0';
OEbar <= '0';

wait for 4 ns;

clk <= '0';

wait for 4 ns;

assert (Y = "111111111111")
report
"Assert 83 : < Y /= 111111111111 >" -- 	Vector No: 46
severity warning;

assert (FULL_BAR = '1')
report
"Assert 84 : < FULL_BAR /= 1 >"
severity warning;

wait for 1 ns;

-- **************************************************
-- ** LOAD R **
--------------------------

clk <= '1'; --	 Cycle No: 106

wait for 1 ns;

I <= "1110";
CCEN_BAR <= '0';
CC_BAR <= '0';
RLD_BAR <= '0';
CI <= '0';
OEbar <= '0';
D <= "111111111111";

wait for 4 ns;

clk <= '0';

wait for 4 ns;

wait for 1 ns;

-- ** Y = TOP OF STACK
--------------------------

clk <= '1'; --	 Cycle No: 107

wait for 1 ns;

I <= "1101";
CCEN_BAR <= '0';
CC_BAR <= '1';
RLD_BAR <= '1';
CI <= '0';
OEbar <= '0';

wait for 4 ns;

clk <= '0';

wait for 4 ns;

assert (Y = "111111111111")
report
"Assert 85 : < Y /= 111111111111 >" -- 	Vector No: 47
severity warning;

assert (FULL_BAR = '1')
report
"Assert 86 : < FULL_BAR /= 1 >"
severity warning;

wait for 1 ns;

-- ** POP **
--------------------------

clk <= '1'; --	 Cycle No: 108

wait for 1 ns;

I <= "1010";
CCEN_BAR <= '1';
CC_BAR <= '1';
RLD_BAR <= '1';
CI <= '0';
OEbar <= '0';

wait for 4 ns;

clk <= '0';

wait for 4 ns;

assert (FULL_BAR = '1')
report
"Assert 87 : < FULL_BAR /= 1 >" -- 	Vector No: 48
severity warning;

wait for 1 ns;

-- ** Y = TOP OF STACK
--------------------------

clk <= '1'; --	 Cycle No: 109

wait for 1 ns;

I <= "1101";
CCEN_BAR <= '0';
CC_BAR <= '1';
RLD_BAR <= '1';
CI <= '0';
OEbar <= '0';

wait for 4 ns;

clk <= '0';

wait for 4 ns;

assert (Y = "000000000000")
report
"Assert 88 : < Y /= 000000000000 >" -- 	Vector No: 49
severity warning;

assert (FULL_BAR = '1')
report
"Assert 89 : < FULL_BAR /= 1 >"
severity warning;

wait for 1 ns;

-- **************************************************
-- **************************************************
-- ******* PUSH ( I = 4) & POP ( I = 11 ) & OTHER CONDITIONS *********** 
-- ** LOAD R **
--------------------------

clk <= '1'; --	 Cycle No: 110

wait for 1 ns;

I <= "0000";
CCEN_BAR <= '0';
CC_BAR <= '0';
RLD_BAR <= '0';
CI <= '0';
OEbar <= '0';
D <= "000000000000";

wait for 4 ns;

clk <= '0';

wait for 4 ns;

wait for 1 ns;

-- ** Y = D **
--------------------------

clk <= '1'; --	 Cycle No: 111

wait for 1 ns;

I <= "0010";
CCEN_BAR <= '0';
CC_BAR <= '0';
RLD_BAR <= '1';
CI <= '0';
OEbar <= '0';
D <= "000000000000";

wait for 4 ns;

clk <= '0';

wait for 4 ns;

wait for 1 ns;

-- ** PUSH **
--------------------------

clk <= '1'; --	 Cycle No: 112

wait for 1 ns;

I <= "0100";
CCEN_BAR <= '1';
CC_BAR <= '1';
RLD_BAR <= '1';
CI <= '0';
OEbar <= '0';

wait for 4 ns;

clk <= '0';

wait for 4 ns;

wait for 1 ns;

-- ** Y = TOP OF STACK **
--------------------------

clk <= '1'; --	 Cycle No: 113

wait for 1 ns;

I <= "1101";
CCEN_BAR <= '0';
CC_BAR <= '1';
RLD_BAR <= '1';
CI <= '0';
OEbar <= '0';

wait for 4 ns;

clk <= '0';

wait for 4 ns;

assert (Y = "000000000000")
report
"Assert 90 : < Y /= 000000000000 >" -- 	Vector No: 50
severity warning;

assert (FULL_BAR = '1')
report
"Assert 91 : < FULL_BAR /= 1 >"
severity warning;

wait for 1 ns;

-- ** Y = D **
--------------------------

clk <= '1'; --	 Cycle No: 114

wait for 1 ns;

I <= "0010";
CCEN_BAR <= '0';
CC_BAR <= '0';
RLD_BAR <= '1';
CI <= '0';
OEbar <= '0';
D <= "111111111111";

wait for 4 ns;

clk <= '0';

wait for 4 ns;

wait for 1 ns;

-- ** PUSH **
--------------------------

clk <= '1'; --	 Cycle No: 115

wait for 1 ns;

I <= "0100";
CCEN_BAR <= '1';
CC_BAR <= '1';
RLD_BAR <= '1';
CI <= '0';
OEbar <= '0';

wait for 4 ns;

clk <= '0';

wait for 4 ns;

wait for 1 ns;

-- ** Y = TOP OF STACK **
--------------------------

clk <= '1'; --	 Cycle No: 116

wait for 1 ns;

I <= "1101";
CCEN_BAR <= '0';
CC_BAR <= '1';
RLD_BAR <= '1';
CI <= '0';
OEbar <= '0';

wait for 4 ns;

clk <= '0';

wait for 4 ns;

assert (Y = "111111111111")
report
"Assert 92 : < Y /= 111111111111 >" -- 	Vector No: 51
severity warning;

assert (FULL_BAR = '1')
report
"Assert 93 : < FULL_BAR /= 1 >"
severity warning;

wait for 1 ns;

-- **************************************************
-- ** LOAD R **
--------------------------

clk <= '1'; --	 Cycle No: 117

wait for 1 ns;

I <= "1110";
CCEN_BAR <= '0';
CC_BAR <= '0';
RLD_BAR <= '0';
CI <= '0';
OEbar <= '0';
D <= "000000000000";

wait for 4 ns;

clk <= '0';

wait for 4 ns;

wait for 1 ns;

-- ** Y = TOP OF STACK
--------------------------

clk <= '1'; --	 Cycle No: 118

wait for 1 ns;

I <= "1101";
CCEN_BAR <= '0';
CC_BAR <= '1';
RLD_BAR <= '1';
CI <= '0';
OEbar <= '0';

wait for 4 ns;

clk <= '0';

wait for 4 ns;

assert (Y = "111111111111")
report
"Assert 94 : < Y /= 111111111111 >" -- 	Vector No: 52
severity warning;

assert (FULL_BAR = '1')
report
"Assert 95 : < FULL_BAR /= 1 >"
severity warning;

wait for 1 ns;

-- ** POP **
--------------------------

clk <= '1'; --	 Cycle No: 119

wait for 1 ns;

I <= "1011";
CCEN_BAR <= '0';
CC_BAR <= '0';
RLD_BAR <= '1';
CI <= '0';
OEbar <= '0';

wait for 4 ns;

clk <= '0';

wait for 4 ns;

assert (FULL_BAR = '1')
report
"Assert 96 : < FULL_BAR /= 1 >" -- 	Vector No: 53
severity warning;

wait for 1 ns;

-- ** Y = TOP OF STACK
--------------------------

clk <= '1'; --	 Cycle No: 120

wait for 1 ns;

I <= "1101";
CCEN_BAR <= '0';
CC_BAR <= '1';
RLD_BAR <= '1';
CI <= '0';
OEbar <= '0';

wait for 4 ns;

clk <= '0';

wait for 4 ns;

assert (Y = "000000000000")
report
"Assert 97 : < Y /= 000000000000 >" -- 	Vector No: 54
severity warning;

assert (FULL_BAR = '1')
report
"Assert 98 : < FULL_BAR /= 1 >"
severity warning;

wait for 1 ns;

-- **************************************************
-- **************************************************
-- ******* PUSH ( I = 4) & POP ( I = 11 ) & OTHER CONDITIONS *********** 
-- ** LOAD R **
--------------------------

clk <= '1'; --	 Cycle No: 121

wait for 1 ns;

I <= "0000";
CCEN_BAR <= '0';
CC_BAR <= '0';
RLD_BAR <= '0';
CI <= '0';
OEbar <= '0';
D <= "111111111111";

wait for 4 ns;

clk <= '0';

wait for 4 ns;

wait for 1 ns;

-- ** Y = D **
--------------------------

clk <= '1'; --	 Cycle No: 122

wait for 1 ns;

I <= "0010";
CCEN_BAR <= '0';
CC_BAR <= '0';
RLD_BAR <= '1';
CI <= '0';
OEbar <= '0';
D <= "000000000000";

wait for 4 ns;

clk <= '0';

wait for 4 ns;

wait for 1 ns;

-- ** PUSH **
--------------------------

clk <= '1'; --	 Cycle No: 123

wait for 1 ns;

I <= "0100";
CCEN_BAR <= '1';
CC_BAR <= '1';
RLD_BAR <= '1';
CI <= '0';
OEbar <= '0';

wait for 4 ns;

clk <= '0';

wait for 4 ns;

wait for 1 ns;

-- ** Y = TOP OF STACK **
--------------------------

clk <= '1'; --	 Cycle No: 124

wait for 1 ns;

I <= "1101";
CCEN_BAR <= '0';
CC_BAR <= '1';
RLD_BAR <= '1';
CI <= '0';
OEbar <= '0';

wait for 4 ns;

clk <= '0';

wait for 4 ns;

assert (Y = "000000000000")
report
"Assert 99 : < Y /= 000000000000 >" -- 	Vector No: 55
severity warning;

assert (FULL_BAR = '1')
report
"Assert 100 : < FULL_BAR /= 1 >"
severity warning;

wait for 1 ns;

-- ** Y = D **
--------------------------

clk <= '1'; --	 Cycle No: 125

wait for 1 ns;

I <= "0010";
CCEN_BAR <= '0';
CC_BAR <= '0';
RLD_BAR <= '1';
CI <= '0';
OEbar <= '0';
D <= "111111111111";

wait for 4 ns;

clk <= '0';

wait for 4 ns;

wait for 1 ns;

-- ** PUSH **
--------------------------

clk <= '1'; --	 Cycle No: 126

wait for 1 ns;

I <= "0100";
CCEN_BAR <= '1';
CC_BAR <= '1';
RLD_BAR <= '1';
CI <= '0';
OEbar <= '0';

wait for 4 ns;

clk <= '0';

wait for 4 ns;

wait for 1 ns;

-- ** Y = TOP OF STACK **
--------------------------

clk <= '1'; --	 Cycle No: 127

wait for 1 ns;

I <= "1101";
CCEN_BAR <= '0';
CC_BAR <= '1';
RLD_BAR <= '1';
CI <= '0';
OEbar <= '0';

wait for 4 ns;

clk <= '0';

wait for 4 ns;

assert (Y = "111111111111")
report
"Assert 101 : < Y /= 111111111111 >" -- 	Vector No: 56
severity warning;

assert (FULL_BAR = '1')
report
"Assert 102 : < FULL_BAR /= 1 >"
severity warning;

wait for 1 ns;

-- **************************************************
-- ** LOAD R **
--------------------------

clk <= '1'; --	 Cycle No: 128

wait for 1 ns;

I <= "1110";
CCEN_BAR <= '0';
CC_BAR <= '0';
RLD_BAR <= '0';
CI <= '0';
OEbar <= '0';
D <= "111111111111";

wait for 4 ns;

clk <= '0';

wait for 4 ns;

wait for 1 ns;

-- ** Y = TOP OF STACK
--------------------------

clk <= '1'; --	 Cycle No: 129

wait for 1 ns;

I <= "1101";
CCEN_BAR <= '0';
CC_BAR <= '1';
RLD_BAR <= '1';
CI <= '0';
OEbar <= '0';

wait for 4 ns;

clk <= '0';

wait for 4 ns;

assert (Y = "111111111111")
report
"Assert 103 : < Y /= 111111111111 >" -- 	Vector No: 57
severity warning;

assert (FULL_BAR = '1')
report
"Assert 104 : < FULL_BAR /= 1 >"
severity warning;

wait for 1 ns;

-- ** POP **
--------------------------

clk <= '1'; --	 Cycle No: 130

wait for 1 ns;

I <= "1011";
CCEN_BAR <= '0';
CC_BAR <= '0';
RLD_BAR <= '1';
CI <= '0';
OEbar <= '0';

wait for 4 ns;

clk <= '0';

wait for 4 ns;

assert (FULL_BAR = '1')
report
"Assert 105 : < FULL_BAR /= 1 >" -- 	Vector No: 58
severity warning;

wait for 1 ns;

-- ** Y = TOP OF STACK
--------------------------

clk <= '1'; --	 Cycle No: 131

wait for 1 ns;

I <= "1101";
CCEN_BAR <= '0';
CC_BAR <= '1';
RLD_BAR <= '1';
CI <= '0';
OEbar <= '0';

wait for 4 ns;

clk <= '0';

wait for 4 ns;

assert (Y = "000000000000")
report
"Assert 106 : < Y /= 000000000000 >" -- 	Vector No: 59
severity warning;

assert (FULL_BAR = '1')
report
"Assert 107 : < FULL_BAR /= 1 >"
severity warning;

wait for 1 ns;

-- **************************************************
-- **************************************************
-- ******* PUSH ( I = 4) & POP ( I = 11 ) & OTHER CONDITIONS *********** 
-- ** LOAD R **
--------------------------

clk <= '1'; --	 Cycle No: 132

wait for 1 ns;

I <= "0000";
CCEN_BAR <= '0';
CC_BAR <= '0';
RLD_BAR <= '0';
CI <= '0';
OEbar <= '0';
D <= "000000000000";

wait for 4 ns;

clk <= '0';

wait for 4 ns;

wait for 1 ns;

-- ** Y = D **
--------------------------

clk <= '1'; --	 Cycle No: 133

wait for 1 ns;

I <= "0010";
CCEN_BAR <= '0';
CC_BAR <= '0';
RLD_BAR <= '1';
CI <= '0';
OEbar <= '0';
D <= "000000000000";

wait for 4 ns;

clk <= '0';

wait for 4 ns;

wait for 1 ns;

-- ** PUSH **
--------------------------

clk <= '1'; --	 Cycle No: 134

wait for 1 ns;

I <= "0100";
CCEN_BAR <= '1';
CC_BAR <= '0';
RLD_BAR <= '1';
CI <= '0';
OEbar <= '0';

wait for 4 ns;

clk <= '0';

wait for 4 ns;

wait for 1 ns;

-- ** Y = TOP OF STACK **
--------------------------

clk <= '1'; --	 Cycle No: 135

wait for 1 ns;

I <= "1101";
CCEN_BAR <= '0';
CC_BAR <= '1';
RLD_BAR <= '1';
CI <= '0';
OEbar <= '0';

wait for 4 ns;

clk <= '0';

wait for 4 ns;

assert (Y = "000000000000")
report
"Assert 108 : < Y /= 000000000000 >" -- 	Vector No: 60
severity warning;

assert (FULL_BAR = '1')
report
"Assert 109 : < FULL_BAR /= 1 >"
severity warning;

wait for 1 ns;

-- ** Y = D **
--------------------------

clk <= '1'; --	 Cycle No: 136

wait for 1 ns;

I <= "0010";
CCEN_BAR <= '0';
CC_BAR <= '0';
RLD_BAR <= '1';
CI <= '0';
OEbar <= '0';
D <= "111111111111";

wait for 4 ns;

clk <= '0';

wait for 4 ns;

wait for 1 ns;

-- ** PUSH **
--------------------------

clk <= '1'; --	 Cycle No: 137

wait for 1 ns;

I <= "0100";
CCEN_BAR <= '1';
CC_BAR <= '0';
RLD_BAR <= '1';
CI <= '0';
OEbar <= '0';

wait for 4 ns;

clk <= '0';

wait for 4 ns;

wait for 1 ns;

-- ** Y = TOP OF STACK **
--------------------------

clk <= '1'; --	 Cycle No: 138

wait for 1 ns;

I <= "1101";
CCEN_BAR <= '0';
CC_BAR <= '1';
RLD_BAR <= '1';
CI <= '0';
OEbar <= '0';

wait for 4 ns;

clk <= '0';

wait for 4 ns;

assert (Y = "111111111111")
report
"Assert 110 : < Y /= 111111111111 >" -- 	Vector No: 61
severity warning;

assert (FULL_BAR = '1')
report
"Assert 111 : < FULL_BAR /= 1 >"
severity warning;

wait for 1 ns;

-- **************************************************
-- ** LOAD R **
--------------------------

clk <= '1'; --	 Cycle No: 139

wait for 1 ns;

I <= "1110";
CCEN_BAR <= '0';
CC_BAR <= '0';
RLD_BAR <= '0';
CI <= '0';
OEbar <= '0';
D <= "000000000000";

wait for 4 ns;

clk <= '0';

wait for 4 ns;

wait for 1 ns;

-- ** Y = TOP OF STACK
--------------------------

clk <= '1'; --	 Cycle No: 140

wait for 1 ns;

I <= "1101";
CCEN_BAR <= '0';
CC_BAR <= '1';
RLD_BAR <= '1';
CI <= '0';
OEbar <= '0';

wait for 4 ns;

clk <= '0';

wait for 4 ns;

assert (Y = "111111111111")
report
"Assert 112 : < Y /= 111111111111 >" -- 	Vector No: 62
severity warning;

assert (FULL_BAR = '1')
report
"Assert 113 : < FULL_BAR /= 1 >"
severity warning;

wait for 1 ns;

-- ** POP **
--------------------------

clk <= '1'; --	 Cycle No: 141

wait for 1 ns;

I <= "1011";
CCEN_BAR <= '1';
CC_BAR <= '0';
RLD_BAR <= '1';
CI <= '0';
OEbar <= '0';

wait for 4 ns;

clk <= '0';

wait for 4 ns;

assert (FULL_BAR = '1')
report
"Assert 114 : < FULL_BAR /= 1 >" -- 	Vector No: 63
severity warning;

wait for 1 ns;

-- ** Y = TOP OF STACK
--------------------------

clk <= '1'; --	 Cycle No: 142

wait for 1 ns;

I <= "1101";
CCEN_BAR <= '0';
CC_BAR <= '1';
RLD_BAR <= '1';
CI <= '0';
OEbar <= '0';

wait for 4 ns;

clk <= '0';

wait for 4 ns;

assert (Y = "000000000000")
report
"Assert 115 : < Y /= 000000000000 >" -- 	Vector No: 64
severity warning;

assert (FULL_BAR = '1')
report
"Assert 116 : < FULL_BAR /= 1 >"
severity warning;

wait for 1 ns;

-- **************************************************
-- **************************************************
-- ******* PUSH ( I = 4) & POP ( I = 11 ) & OTHER CONDITIONS *********** 
-- ** LOAD R **
--------------------------

clk <= '1'; --	 Cycle No: 143

wait for 1 ns;

I <= "0000";
CCEN_BAR <= '0';
CC_BAR <= '0';
RLD_BAR <= '0';
CI <= '0';
OEbar <= '0';
D <= "111111111111";

wait for 4 ns;

clk <= '0';

wait for 4 ns;

wait for 1 ns;

-- ** Y = D **
--------------------------

clk <= '1'; --	 Cycle No: 144

wait for 1 ns;

I <= "0010";
CCEN_BAR <= '0';
CC_BAR <= '0';
RLD_BAR <= '1';
CI <= '0';
OEbar <= '0';
D <= "000000000000";

wait for 4 ns;

clk <= '0';

wait for 4 ns;

wait for 1 ns;

-- ** PUSH **
--------------------------

clk <= '1'; --	 Cycle No: 145

wait for 1 ns;

I <= "0100";
CCEN_BAR <= '1';
CC_BAR <= '0';
RLD_BAR <= '1';
CI <= '0';
OEbar <= '0';

wait for 4 ns;

clk <= '0';

wait for 4 ns;

wait for 1 ns;

-- ** Y = TOP OF STACK **
--------------------------

clk <= '1'; --	 Cycle No: 146

wait for 1 ns;

I <= "1101";
CCEN_BAR <= '0';
CC_BAR <= '1';
RLD_BAR <= '1';
CI <= '0';
OEbar <= '0';

wait for 4 ns;

clk <= '0';

wait for 4 ns;

assert (Y = "000000000000")
report
"Assert 117 : < Y /= 000000000000 >" -- 	Vector No: 65
severity warning;

assert (FULL_BAR = '1')
report
"Assert 118 : < FULL_BAR /= 1 >"
severity warning;

wait for 1 ns;

-- ** Y = D **
--------------------------

clk <= '1'; --	 Cycle No: 147

wait for 1 ns;

I <= "0010";
CCEN_BAR <= '0';
CC_BAR <= '0';
RLD_BAR <= '1';
CI <= '0';
OEbar <= '0';
D <= "111111111111";

wait for 4 ns;

clk <= '0';

wait for 4 ns;

wait for 1 ns;

-- ** PUSH **
--------------------------

clk <= '1'; --	 Cycle No: 148

wait for 1 ns;

I <= "0100";
CCEN_BAR <= '1';
CC_BAR <= '0';
RLD_BAR <= '1';
CI <= '0';
OEbar <= '0';

wait for 4 ns;

clk <= '0';

wait for 4 ns;

wait for 1 ns;

-- ** Y = TOP OF STACK **
--------------------------

clk <= '1'; --	 Cycle No: 149

wait for 1 ns;

I <= "1101";
CCEN_BAR <= '0';
CC_BAR <= '1';
RLD_BAR <= '1';
CI <= '0';
OEbar <= '0';

wait for 4 ns;

clk <= '0';

wait for 4 ns;

assert (Y = "111111111111")
report
"Assert 119 : < Y /= 111111111111 >" -- 	Vector No: 66
severity warning;

assert (FULL_BAR = '1')
report
"Assert 120 : < FULL_BAR /= 1 >"
severity warning;

wait for 1 ns;

-- **************************************************
-- ** LOAD R **
--------------------------

clk <= '1'; --	 Cycle No: 150

wait for 1 ns;

I <= "1110";
CCEN_BAR <= '0';
CC_BAR <= '0';
RLD_BAR <= '0';
CI <= '0';
OEbar <= '0';
D <= "111111111111";

wait for 4 ns;

clk <= '0';

wait for 4 ns;

wait for 1 ns;

-- ** Y = TOP OF STACK
--------------------------

clk <= '1'; --	 Cycle No: 151

wait for 1 ns;

I <= "1101";
CCEN_BAR <= '0';
CC_BAR <= '1';
RLD_BAR <= '1';
CI <= '0';
OEbar <= '0';

wait for 4 ns;

clk <= '0';

wait for 4 ns;

assert (Y = "111111111111")
report
"Assert 121 : < Y /= 111111111111 >" -- 	Vector No: 67
severity warning;

assert (FULL_BAR = '1')
report
"Assert 122 : < FULL_BAR /= 1 >"
severity warning;

wait for 1 ns;

-- ** POP **
--------------------------

clk <= '1'; --	 Cycle No: 152

wait for 1 ns;

I <= "1011";
CCEN_BAR <= '1';
CC_BAR <= '0';
RLD_BAR <= '1';
CI <= '0';
OEbar <= '0';

wait for 4 ns;

clk <= '0';

wait for 4 ns;

assert (FULL_BAR = '1')
report
"Assert 123 : < FULL_BAR /= 1 >" -- 	Vector No: 68
severity warning;

wait for 1 ns;

-- ** Y = TOP OF STACK
--------------------------

clk <= '1'; --	 Cycle No: 153

wait for 1 ns;

I <= "1101";
CCEN_BAR <= '0';
CC_BAR <= '1';
RLD_BAR <= '1';
CI <= '0';
OEbar <= '0';

wait for 4 ns;

clk <= '0';

wait for 4 ns;

assert (Y = "000000000000")
report
"Assert 124 : < Y /= 000000000000 >" -- 	Vector No: 69
severity warning;

assert (FULL_BAR = '1')
report
"Assert 125 : < FULL_BAR /= 1 >"
severity warning;

wait for 1 ns;

-- **************************************************
-- **************************************************
-- ******* PUSH ( I = 5) & POP ( I = 11 ) & OTHER CONDITIONS *********** 
-- ** LOAD R **
--------------------------

clk <= '1'; --	 Cycle No: 154

wait for 1 ns;

I <= "0000";
CCEN_BAR <= '0';
CC_BAR <= '0';
RLD_BAR <= '0';
CI <= '0';
OEbar <= '0';
D <= "000000000000";

wait for 4 ns;

clk <= '0';

wait for 4 ns;

wait for 1 ns;

-- ** Y = D **
--------------------------

clk <= '1'; --	 Cycle No: 155

wait for 1 ns;

I <= "0010";
CCEN_BAR <= '0';
CC_BAR <= '0';
RLD_BAR <= '1';
CI <= '0';
OEbar <= '0';
D <= "000000000000";

wait for 4 ns;

clk <= '0';

wait for 4 ns;

wait for 1 ns;

-- ** PUSH **
--------------------------

clk <= '1'; --	 Cycle No: 156

wait for 1 ns;

I <= "0101";
CCEN_BAR <= '0';
CC_BAR <= '0';
RLD_BAR <= '1';
CI <= '0';
OEbar <= '0';

wait for 4 ns;

clk <= '0';

wait for 4 ns;

wait for 1 ns;

-- ** Y = TOP OF STACK **
--------------------------

clk <= '1'; --	 Cycle No: 157

wait for 1 ns;

I <= "1101";
CCEN_BAR <= '0';
CC_BAR <= '1';
RLD_BAR <= '1';
CI <= '0';
OEbar <= '0';

wait for 4 ns;

clk <= '0';

wait for 4 ns;

assert (Y = "000000000000")
report
"Assert 126 : < Y /= 000000000000 >" -- 	Vector No: 70
severity warning;

assert (FULL_BAR = '1')
report
"Assert 127 : < FULL_BAR /= 1 >"
severity warning;

wait for 1 ns;

-- ** Y = D **
--------------------------

clk <= '1'; --	 Cycle No: 158

wait for 1 ns;

I <= "0010";
CCEN_BAR <= '0';
CC_BAR <= '0';
RLD_BAR <= '1';
CI <= '0';
OEbar <= '0';
D <= "111111111111";

wait for 4 ns;

clk <= '0';

wait for 4 ns;

wait for 1 ns;

-- ** PUSH **
--------------------------

clk <= '1'; --	 Cycle No: 159

wait for 1 ns;

I <= "0101";
CCEN_BAR <= '0';
CC_BAR <= '0';
RLD_BAR <= '1';
CI <= '0';
OEbar <= '0';

wait for 4 ns;

clk <= '0';

wait for 4 ns;

wait for 1 ns;

-- ** Y = TOP OF STACK **
--------------------------

clk <= '1'; --	 Cycle No: 160

wait for 1 ns;

I <= "1101";
CCEN_BAR <= '0';
CC_BAR <= '1';
RLD_BAR <= '1';
CI <= '0';
OEbar <= '0';

wait for 4 ns;

clk <= '0';

wait for 4 ns;

assert (Y = "111111111111")
report
"Assert 128 : < Y /= 111111111111 >" -- 	Vector No: 71
severity warning;

assert (FULL_BAR = '1')
report
"Assert 129 : < FULL_BAR /= 1 >"
severity warning;

wait for 1 ns;

-- **************************************************
-- ** LOAD R **
--------------------------

clk <= '1'; --	 Cycle No: 161

wait for 1 ns;

I <= "1110";
CCEN_BAR <= '0';
CC_BAR <= '0';
RLD_BAR <= '0';
CI <= '0';
OEbar <= '0';
D <= "000000000000";

wait for 4 ns;

clk <= '0';

wait for 4 ns;

wait for 1 ns;

-- ** Y = TOP OF STACK
--------------------------

clk <= '1'; --	 Cycle No: 162

wait for 1 ns;

I <= "1101";
CCEN_BAR <= '0';
CC_BAR <= '1';
RLD_BAR <= '1';
CI <= '0';
OEbar <= '0';

wait for 4 ns;

clk <= '0';

wait for 4 ns;

assert (Y = "111111111111")
report
"Assert 130 : < Y /= 111111111111 >" -- 	Vector No: 72
severity warning;

assert (FULL_BAR = '1')
report
"Assert 131 : < FULL_BAR /= 1 >"
severity warning;

wait for 1 ns;

-- ** POP **
--------------------------

clk <= '1'; --	 Cycle No: 163

wait for 1 ns;

I <= "1011";
CCEN_BAR <= '1';
CC_BAR <= '1';
RLD_BAR <= '1';
CI <= '0';
OEbar <= '0';

wait for 4 ns;

clk <= '0';

wait for 4 ns;

assert (FULL_BAR = '1')
report
"Assert 132 : < FULL_BAR /= 1 >" -- 	Vector No: 73
severity warning;

wait for 1 ns;

-- ** Y = TOP OF STACK
--------------------------

clk <= '1'; --	 Cycle No: 164

wait for 1 ns;

I <= "1101";
CCEN_BAR <= '0';
CC_BAR <= '1';
RLD_BAR <= '1';
CI <= '0';
OEbar <= '0';

wait for 4 ns;

clk <= '0';

wait for 4 ns;

assert (Y = "000000000000")
report
"Assert 133 : < Y /= 000000000000 >" -- 	Vector No: 74
severity warning;

assert (FULL_BAR = '1')
report
"Assert 134 : < FULL_BAR /= 1 >"
severity warning;

wait for 1 ns;

-- **************************************************
-- **************************************************
-- ******* PUSH ( I = 5) & POP ( I = 11 ) & OTHER CONDITIONS *********** 
-- ** LOAD R **
--------------------------

clk <= '1'; --	 Cycle No: 165

wait for 1 ns;

I <= "0000";
CCEN_BAR <= '0';
CC_BAR <= '0';
RLD_BAR <= '0';
CI <= '0';
OEbar <= '0';
D <= "111111111111";

wait for 4 ns;

clk <= '0';

wait for 4 ns;

wait for 1 ns;

-- ** Y = D **
--------------------------

clk <= '1'; --	 Cycle No: 166

wait for 1 ns;

I <= "0010";
CCEN_BAR <= '0';
CC_BAR <= '0';
RLD_BAR <= '1';
CI <= '0';
OEbar <= '0';
D <= "000000000000";

wait for 4 ns;

clk <= '0';

wait for 4 ns;

wait for 1 ns;

-- ** PUSH **
--------------------------

clk <= '1'; --	 Cycle No: 167

wait for 1 ns;

I <= "0101";
CCEN_BAR <= '0';
CC_BAR <= '0';
RLD_BAR <= '1';
CI <= '0';
OEbar <= '0';

wait for 4 ns;

clk <= '0';

wait for 4 ns;

wait for 1 ns;

-- ** Y = TOP OF STACK **
--------------------------

clk <= '1'; --	 Cycle No: 168

wait for 1 ns;

I <= "1101";
CCEN_BAR <= '0';
CC_BAR <= '1';
RLD_BAR <= '1';
CI <= '0';
OEbar <= '0';

wait for 4 ns;

clk <= '0';

wait for 4 ns;

assert (Y = "000000000000")
report
"Assert 135 : < Y /= 000000000000 >" -- 	Vector No: 75
severity warning;

assert (FULL_BAR = '1')
report
"Assert 136 : < FULL_BAR /= 1 >"
severity warning;

wait for 1 ns;

-- ** Y = D **
--------------------------

clk <= '1'; --	 Cycle No: 169

wait for 1 ns;

I <= "0010";
CCEN_BAR <= '0';
CC_BAR <= '0';
RLD_BAR <= '1';
CI <= '0';
OEbar <= '0';
D <= "111111111111";

wait for 4 ns;

clk <= '0';

wait for 4 ns;

wait for 1 ns;

-- ** PUSH **
--------------------------

clk <= '1'; --	 Cycle No: 170

wait for 1 ns;

I <= "0101";
CCEN_BAR <= '0';
CC_BAR <= '0';
RLD_BAR <= '1';
CI <= '0';
OEbar <= '0';

wait for 4 ns;

clk <= '0';

wait for 4 ns;

wait for 1 ns;

-- ** Y = TOP OF STACK **
--------------------------

clk <= '1'; --	 Cycle No: 171

wait for 1 ns;

I <= "1101";
CCEN_BAR <= '0';
CC_BAR <= '1';
RLD_BAR <= '1';
CI <= '0';
OEbar <= '0';

wait for 4 ns;

clk <= '0';

wait for 4 ns;

assert (Y = "111111111111")
report
"Assert 137 : < Y /= 111111111111 >" -- 	Vector No: 76
severity warning;

assert (FULL_BAR = '1')
report
"Assert 138 : < FULL_BAR /= 1 >"
severity warning;

wait for 1 ns;

-- **************************************************
-- ** LOAD R **
--------------------------

clk <= '1'; --	 Cycle No: 172

wait for 1 ns;

I <= "1110";
CCEN_BAR <= '0';
CC_BAR <= '0';
RLD_BAR <= '0';
CI <= '0';
OEbar <= '0';
D <= "111111111111";

wait for 4 ns;

clk <= '0';

wait for 4 ns;

wait for 1 ns;

-- ** Y = TOP OF STACK
--------------------------

clk <= '1'; --	 Cycle No: 173

wait for 1 ns;

I <= "1101";
CCEN_BAR <= '0';
CC_BAR <= '1';
RLD_BAR <= '1';
CI <= '0';
OEbar <= '0';

wait for 4 ns;

clk <= '0';

wait for 4 ns;

assert (Y = "111111111111")
report
"Assert 139 : < Y /= 111111111111 >" -- 	Vector No: 77
severity warning;

assert (FULL_BAR = '1')
report
"Assert 140 : < FULL_BAR /= 1 >"
severity warning;

wait for 1 ns;

-- ** POP **
--------------------------

clk <= '1'; --	 Cycle No: 174

wait for 1 ns;

I <= "1011";
CCEN_BAR <= '1';
CC_BAR <= '1';
RLD_BAR <= '1';
CI <= '0';
OEbar <= '0';

wait for 4 ns;

clk <= '0';

wait for 4 ns;

assert (FULL_BAR = '1')
report
"Assert 141 : < FULL_BAR /= 1 >" -- 	Vector No: 78
severity warning;

wait for 1 ns;

-- ** Y = TOP OF STACK
--------------------------

clk <= '1'; --	 Cycle No: 175

wait for 1 ns;

I <= "1101";
CCEN_BAR <= '0';
CC_BAR <= '1';
RLD_BAR <= '1';
CI <= '0';
OEbar <= '0';

wait for 4 ns;

clk <= '0';

wait for 4 ns;

assert (Y = "000000000000")
report
"Assert 142 : < Y /= 000000000000 >" -- 	Vector No: 79
severity warning;

assert (FULL_BAR = '1')
report
"Assert 143 : < FULL_BAR /= 1 >"
severity warning;

wait for 1 ns;

-- **************************************************
-- **************************************************
-- ******* PUSH ( I = 5) & POP ( I = 13 ) & OTHER CONDITIONS *********** 
-- ** LOAD R **
--------------------------

clk <= '1'; --	 Cycle No: 176

wait for 1 ns;

I <= "0000";
CCEN_BAR <= '0';
CC_BAR <= '0';
RLD_BAR <= '0';
CI <= '0';
OEbar <= '0';
D <= "000000000000";

wait for 4 ns;

clk <= '0';

wait for 4 ns;

wait for 1 ns;

-- ** Y = D **
--------------------------

clk <= '1'; --	 Cycle No: 177

wait for 1 ns;

I <= "0010";
CCEN_BAR <= '0';
CC_BAR <= '0';
RLD_BAR <= '1';
CI <= '0';
OEbar <= '0';
D <= "000000000000";

wait for 4 ns;

clk <= '0';

wait for 4 ns;

wait for 1 ns;

-- ** PUSH **
--------------------------

clk <= '1'; --	 Cycle No: 178

wait for 1 ns;

I <= "0101";
CCEN_BAR <= '0';
CC_BAR <= '1';
RLD_BAR <= '1';
CI <= '0';
OEbar <= '0';

wait for 4 ns;

clk <= '0';

wait for 4 ns;

wait for 1 ns;

-- ** Y = TOP OF STACK **
--------------------------

clk <= '1'; --	 Cycle No: 179

wait for 1 ns;

I <= "1101";
CCEN_BAR <= '0';
CC_BAR <= '1';
RLD_BAR <= '1';
CI <= '0';
OEbar <= '0';

wait for 4 ns;

clk <= '0';

wait for 4 ns;

assert (Y = "000000000000")
report
"Assert 144 : < Y /= 000000000000 >" -- 	Vector No: 80
severity warning;

assert (FULL_BAR = '1')
report
"Assert 145 : < FULL_BAR /= 1 >"
severity warning;

wait for 1 ns;

-- ** Y = D **
--------------------------

clk <= '1'; --	 Cycle No: 180

wait for 1 ns;

I <= "0010";
CCEN_BAR <= '0';
CC_BAR <= '0';
RLD_BAR <= '1';
CI <= '0';
OEbar <= '0';
D <= "111111111111";

wait for 4 ns;

clk <= '0';

wait for 4 ns;

wait for 1 ns;

-- ** PUSH **
--------------------------

clk <= '1'; --	 Cycle No: 181

wait for 1 ns;

I <= "0101";
CCEN_BAR <= '0';
CC_BAR <= '1';
RLD_BAR <= '1';
CI <= '0';
OEbar <= '0';

wait for 4 ns;

clk <= '0';

wait for 4 ns;

wait for 1 ns;

-- ** Y = TOP OF STACK **
--------------------------

clk <= '1'; --	 Cycle No: 182

wait for 1 ns;

I <= "1101";
CCEN_BAR <= '0';
CC_BAR <= '1';
RLD_BAR <= '1';
CI <= '0';
OEbar <= '0';

wait for 4 ns;

clk <= '0';

wait for 4 ns;

assert (Y = "111111111111")
report
"Assert 146 : < Y /= 111111111111 >" -- 	Vector No: 81
severity warning;

assert (FULL_BAR = '1')
report
"Assert 147 : < FULL_BAR /= 1 >"
severity warning;

wait for 1 ns;

-- **************************************************
-- ** LOAD R **
--------------------------

clk <= '1'; --	 Cycle No: 183

wait for 1 ns;

I <= "1110";
CCEN_BAR <= '0';
CC_BAR <= '0';
RLD_BAR <= '0';
CI <= '0';
OEbar <= '0';
D <= "000000000000";

wait for 4 ns;

clk <= '0';

wait for 4 ns;

wait for 1 ns;

-- ** Y = TOP OF STACK
--------------------------

clk <= '1'; --	 Cycle No: 184

wait for 1 ns;

I <= "1101";
CCEN_BAR <= '0';
CC_BAR <= '1';
RLD_BAR <= '1';
CI <= '0';
OEbar <= '0';

wait for 4 ns;

clk <= '0';

wait for 4 ns;

assert (Y = "111111111111")
report
"Assert 148 : < Y /= 111111111111 >" -- 	Vector No: 82
severity warning;

assert (FULL_BAR = '1')
report
"Assert 149 : < FULL_BAR /= 1 >"
severity warning;

wait for 1 ns;

-- ** POP **
--------------------------

clk <= '1'; --	 Cycle No: 185

wait for 1 ns;

I <= "1101";
CCEN_BAR <= '0';
CC_BAR <= '0';
RLD_BAR <= '1';
CI <= '0';
OEbar <= '0';

wait for 4 ns;

clk <= '0';

wait for 4 ns;

assert (FULL_BAR = '1')
report
"Assert 150 : < FULL_BAR /= 1 >" -- 	Vector No: 83
severity warning;

wait for 1 ns;

-- ** Y = TOP OF STACK
--------------------------

clk <= '1'; --	 Cycle No: 186

wait for 1 ns;

I <= "1101";
CCEN_BAR <= '0';
CC_BAR <= '1';
RLD_BAR <= '1';
CI <= '0';
OEbar <= '0';

wait for 4 ns;

clk <= '0';

wait for 4 ns;

assert (Y = "000000000000")
report
"Assert 151 : < Y /= 000000000000 >" -- 	Vector No: 84
severity warning;

assert (FULL_BAR = '1')
report
"Assert 152 : < FULL_BAR /= 1 >"
severity warning;

wait for 1 ns;

-- **************************************************
-- **************************************************
-- ******* PUSH ( I = 5) & POP ( I = 13 ) & OTHER CONDITIONS *********** 
-- ** LOAD R **
--------------------------

clk <= '1'; --	 Cycle No: 187

wait for 1 ns;

I <= "0000";
CCEN_BAR <= '0';
CC_BAR <= '0';
RLD_BAR <= '0';
CI <= '0';
OEbar <= '0';
D <= "111111111111";

wait for 4 ns;

clk <= '0';

wait for 4 ns;

wait for 1 ns;

-- ** Y = D **
--------------------------

clk <= '1'; --	 Cycle No: 188

wait for 1 ns;

I <= "0010";
CCEN_BAR <= '0';
CC_BAR <= '0';
RLD_BAR <= '1';
CI <= '0';
OEbar <= '0';
D <= "000000000000";

wait for 4 ns;

clk <= '0';

wait for 4 ns;

wait for 1 ns;

-- ** PUSH **
--------------------------

clk <= '1'; --	 Cycle No: 189

wait for 1 ns;

I <= "0101";
CCEN_BAR <= '0';
CC_BAR <= '1';
RLD_BAR <= '1';
CI <= '0';
OEbar <= '0';

wait for 4 ns;

clk <= '0';

wait for 4 ns;

wait for 1 ns;

-- ** Y = TOP OF STACK **
--------------------------

clk <= '1'; --	 Cycle No: 190

wait for 1 ns;

I <= "1101";
CCEN_BAR <= '0';
CC_BAR <= '1';
RLD_BAR <= '1';
CI <= '0';
OEbar <= '0';

wait for 4 ns;

clk <= '0';

wait for 4 ns;

assert (Y = "000000000000")
report
"Assert 153 : < Y /= 000000000000 >" -- 	Vector No: 85
severity warning;

assert (FULL_BAR = '1')
report
"Assert 154 : < FULL_BAR /= 1 >"
severity warning;

wait for 1 ns;

-- ** Y = D **
--------------------------

clk <= '1'; --	 Cycle No: 191

wait for 1 ns;

I <= "0010";
CCEN_BAR <= '0';
CC_BAR <= '0';
RLD_BAR <= '1';
CI <= '0';
OEbar <= '0';
D <= "111111111111";

wait for 4 ns;

clk <= '0';

wait for 4 ns;

wait for 1 ns;

-- ** PUSH **
--------------------------

clk <= '1'; --	 Cycle No: 192

wait for 1 ns;

I <= "0101";
CCEN_BAR <= '0';
CC_BAR <= '1';
RLD_BAR <= '1';
CI <= '0';
OEbar <= '0';

wait for 4 ns;

clk <= '0';

wait for 4 ns;

wait for 1 ns;

-- ** Y = TOP OF STACK **
--------------------------

clk <= '1'; --	 Cycle No: 193

wait for 1 ns;

I <= "1101";
CCEN_BAR <= '0';
CC_BAR <= '1';
RLD_BAR <= '1';
CI <= '0';
OEbar <= '0';

wait for 4 ns;

clk <= '0';

wait for 4 ns;

assert (Y = "111111111111")
report
"Assert 155 : < Y /= 111111111111 >" -- 	Vector No: 86
severity warning;

assert (FULL_BAR = '1')
report
"Assert 156 : < FULL_BAR /= 1 >"
severity warning;

wait for 1 ns;

-- **************************************************
-- ** LOAD R **
--------------------------

clk <= '1'; --	 Cycle No: 194

wait for 1 ns;

I <= "1110";
CCEN_BAR <= '0';
CC_BAR <= '0';
RLD_BAR <= '0';
CI <= '0';
OEbar <= '0';
D <= "111111111111";

wait for 4 ns;

clk <= '0';

wait for 4 ns;

wait for 1 ns;

-- ** Y = TOP OF STACK
--------------------------

clk <= '1'; --	 Cycle No: 195

wait for 1 ns;

I <= "1101";
CCEN_BAR <= '0';
CC_BAR <= '1';
RLD_BAR <= '1';
CI <= '0';
OEbar <= '0';

wait for 4 ns;

clk <= '0';

wait for 4 ns;

assert (Y = "111111111111")
report
"Assert 157 : < Y /= 111111111111 >" -- 	Vector No: 87
severity warning;

assert (FULL_BAR = '1')
report
"Assert 158 : < FULL_BAR /= 1 >"
severity warning;

wait for 1 ns;

-- ** POP **
--------------------------

clk <= '1'; --	 Cycle No: 196

wait for 1 ns;

I <= "1101";
CCEN_BAR <= '0';
CC_BAR <= '0';
RLD_BAR <= '1';
CI <= '0';
OEbar <= '0';

wait for 4 ns;

clk <= '0';

wait for 4 ns;

assert (FULL_BAR = '1')
report
"Assert 159 : < FULL_BAR /= 1 >" -- 	Vector No: 88
severity warning;

wait for 1 ns;

-- ** Y = TOP OF STACK
--------------------------

clk <= '1'; --	 Cycle No: 197

wait for 1 ns;

I <= "1101";
CCEN_BAR <= '0';
CC_BAR <= '1';
RLD_BAR <= '1';
CI <= '0';
OEbar <= '0';

wait for 4 ns;

clk <= '0';

wait for 4 ns;

assert (Y = "000000000000")
report
"Assert 160 : < Y /= 000000000000 >" -- 	Vector No: 89
severity warning;

assert (FULL_BAR = '1')
report
"Assert 161 : < FULL_BAR /= 1 >"
severity warning;

wait for 1 ns;

-- **************************************************
-- **************************************************
-- ******* PUSH ( I = 5) & POP ( I = 13 ) & OTHER CONDITIONS *********** 
-- ** LOAD R **
--------------------------

clk <= '1'; --	 Cycle No: 198

wait for 1 ns;

I <= "0000";
CCEN_BAR <= '0';
CC_BAR <= '0';
RLD_BAR <= '0';
CI <= '0';
OEbar <= '0';
D <= "000000000000";

wait for 4 ns;

clk <= '0';

wait for 4 ns;

wait for 1 ns;

-- ** Y = D **
--------------------------

clk <= '1'; --	 Cycle No: 199

wait for 1 ns;

I <= "0010";
CCEN_BAR <= '0';
CC_BAR <= '0';
RLD_BAR <= '1';
CI <= '0';
OEbar <= '0';
D <= "000000000000";

wait for 4 ns;

clk <= '0';

wait for 4 ns;

wait for 1 ns;

-- ** PUSH **
--------------------------

clk <= '1'; --	 Cycle No: 200

wait for 1 ns;

I <= "0101";
CCEN_BAR <= '1';
CC_BAR <= '1';
RLD_BAR <= '1';
CI <= '0';
OEbar <= '0';

wait for 4 ns;

clk <= '0';

wait for 4 ns;

wait for 1 ns;

-- ** Y = TOP OF STACK **
--------------------------

clk <= '1'; --	 Cycle No: 201

wait for 1 ns;

I <= "1101";
CCEN_BAR <= '0';
CC_BAR <= '1';
RLD_BAR <= '1';
CI <= '0';
OEbar <= '0';

wait for 4 ns;

clk <= '0';

wait for 4 ns;

assert (Y = "000000000000")
report
"Assert 162 : < Y /= 000000000000 >" -- 	Vector No: 90
severity warning;

assert (FULL_BAR = '1')
report
"Assert 163 : < FULL_BAR /= 1 >"
severity warning;

wait for 1 ns;

-- ** Y = D **
--------------------------

clk <= '1'; --	 Cycle No: 202

wait for 1 ns;

I <= "0010";
CCEN_BAR <= '0';
CC_BAR <= '0';
RLD_BAR <= '1';
CI <= '0';
OEbar <= '0';
D <= "111111111111";

wait for 4 ns;

clk <= '0';

wait for 4 ns;

wait for 1 ns;

-- ** PUSH **
--------------------------

clk <= '1'; --	 Cycle No: 203

wait for 1 ns;

I <= "0101";
CCEN_BAR <= '1';
CC_BAR <= '1';
RLD_BAR <= '1';
CI <= '0';
OEbar <= '0';

wait for 4 ns;

clk <= '0';

wait for 4 ns;

wait for 1 ns;

-- ** Y = TOP OF STACK **
--------------------------

clk <= '1'; --	 Cycle No: 204

wait for 1 ns;

I <= "1101";
CCEN_BAR <= '0';
CC_BAR <= '1';
RLD_BAR <= '1';
CI <= '0';
OEbar <= '0';

wait for 4 ns;

clk <= '0';

wait for 4 ns;

assert (Y = "111111111111")
report
"Assert 164 : < Y /= 111111111111 >" -- 	Vector No: 91
severity warning;

assert (FULL_BAR = '1')
report
"Assert 165 : < FULL_BAR /= 1 >"
severity warning;

wait for 1 ns;

-- **************************************************
-- ** LOAD R **
--------------------------

clk <= '1'; --	 Cycle No: 205

wait for 1 ns;

I <= "1110";
CCEN_BAR <= '0';
CC_BAR <= '0';
RLD_BAR <= '0';
CI <= '0';
OEbar <= '0';
D <= "000000000000";

wait for 4 ns;

clk <= '0';

wait for 4 ns;

wait for 1 ns;

-- ** Y = TOP OF STACK
--------------------------

clk <= '1'; --	 Cycle No: 206

wait for 1 ns;

I <= "1101";
CCEN_BAR <= '0';
CC_BAR <= '1';
RLD_BAR <= '1';
CI <= '0';
OEbar <= '0';

wait for 4 ns;

clk <= '0';

wait for 4 ns;

assert (Y = "111111111111")
report
"Assert 166 : < Y /= 111111111111 >" -- 	Vector No: 92
severity warning;

assert (FULL_BAR = '1')
report
"Assert 167 : < FULL_BAR /= 1 >"
severity warning;

wait for 1 ns;

-- ** POP **
--------------------------

clk <= '1'; --	 Cycle No: 207

wait for 1 ns;

I <= "1101";
CCEN_BAR <= '1';
CC_BAR <= '0';
RLD_BAR <= '1';
CI <= '0';
OEbar <= '0';

wait for 4 ns;

clk <= '0';

wait for 4 ns;

assert (FULL_BAR = '1')
report
"Assert 168 : < FULL_BAR /= 1 >" -- 	Vector No: 93
severity warning;

wait for 1 ns;

-- ** Y = TOP OF STACK
--------------------------

clk <= '1'; --	 Cycle No: 208

wait for 1 ns;

I <= "1101";
CCEN_BAR <= '0';
CC_BAR <= '1';
RLD_BAR <= '1';
CI <= '0';
OEbar <= '0';

wait for 4 ns;

clk <= '0';

wait for 4 ns;

assert (Y = "000000000000")
report
"Assert 169 : < Y /= 000000000000 >" -- 	Vector No: 94
severity warning;

assert (FULL_BAR = '1')
report
"Assert 170 : < FULL_BAR /= 1 >"
severity warning;

wait for 1 ns;

-- **************************************************
-- **************************************************
-- ******* PUSH ( I = 5) & POP ( I = 13 ) & OTHER CONDITIONS *********** 
-- ** LOAD R **
--------------------------

clk <= '1'; --	 Cycle No: 209

wait for 1 ns;

I <= "0000";
CCEN_BAR <= '0';
CC_BAR <= '0';
RLD_BAR <= '0';
CI <= '0';
OEbar <= '0';
D <= "111111111111";

wait for 4 ns;

clk <= '0';

wait for 4 ns;

wait for 1 ns;

-- ** Y = D **
--------------------------

clk <= '1'; --	 Cycle No: 210

wait for 1 ns;

I <= "0010";
CCEN_BAR <= '0';
CC_BAR <= '0';
RLD_BAR <= '1';
CI <= '0';
OEbar <= '0';
D <= "000000000000";

wait for 4 ns;

clk <= '0';

wait for 4 ns;

wait for 1 ns;

-- ** PUSH **
--------------------------

clk <= '1'; --	 Cycle No: 211

wait for 1 ns;

I <= "0101";
CCEN_BAR <= '1';
CC_BAR <= '1';
RLD_BAR <= '1';
CI <= '0';
OEbar <= '0';

wait for 4 ns;

clk <= '0';

wait for 4 ns;

wait for 1 ns;

-- ** Y = TOP OF STACK **
--------------------------

clk <= '1'; --	 Cycle No: 212

wait for 1 ns;

I <= "1101";
CCEN_BAR <= '0';
CC_BAR <= '1';
RLD_BAR <= '1';
CI <= '0';
OEbar <= '0';

wait for 4 ns;

clk <= '0';

wait for 4 ns;

assert (Y = "000000000000")
report
"Assert 171 : < Y /= 000000000000 >" -- 	Vector No: 95
severity warning;

assert (FULL_BAR = '1')
report
"Assert 172 : < FULL_BAR /= 1 >"
severity warning;

wait for 1 ns;

-- ** Y = D **
--------------------------

clk <= '1'; --	 Cycle No: 213

wait for 1 ns;

I <= "0010";
CCEN_BAR <= '0';
CC_BAR <= '0';
RLD_BAR <= '1';
CI <= '0';
OEbar <= '0';
D <= "111111111111";

wait for 4 ns;

clk <= '0';

wait for 4 ns;

wait for 1 ns;

-- ** PUSH **
--------------------------

clk <= '1'; --	 Cycle No: 214

wait for 1 ns;

I <= "0101";
CCEN_BAR <= '1';
CC_BAR <= '1';
RLD_BAR <= '1';
CI <= '0';
OEbar <= '0';

wait for 4 ns;

clk <= '0';

wait for 4 ns;

wait for 1 ns;

-- ** Y = TOP OF STACK **
--------------------------

clk <= '1'; --	 Cycle No: 215

wait for 1 ns;

I <= "1101";
CCEN_BAR <= '0';
CC_BAR <= '1';
RLD_BAR <= '1';
CI <= '0';
OEbar <= '0';

wait for 4 ns;

clk <= '0';

wait for 4 ns;

assert (Y = "111111111111")
report
"Assert 173 : < Y /= 111111111111 >" -- 	Vector No: 96
severity warning;

assert (FULL_BAR = '1')
report
"Assert 174 : < FULL_BAR /= 1 >"
severity warning;

wait for 1 ns;

-- **************************************************
-- ** LOAD R **
--------------------------

clk <= '1'; --	 Cycle No: 216

wait for 1 ns;

I <= "1110";
CCEN_BAR <= '0';
CC_BAR <= '0';
RLD_BAR <= '0';
CI <= '0';
OEbar <= '0';
D <= "111111111111";

wait for 4 ns;

clk <= '0';

wait for 4 ns;

wait for 1 ns;

-- ** Y = TOP OF STACK
--------------------------

clk <= '1'; --	 Cycle No: 217

wait for 1 ns;

I <= "1101";
CCEN_BAR <= '0';
CC_BAR <= '1';
RLD_BAR <= '1';
CI <= '0';
OEbar <= '0';

wait for 4 ns;

clk <= '0';

wait for 4 ns;

assert (Y = "111111111111")
report
"Assert 175 : < Y /= 111111111111 >" -- 	Vector No: 97
severity warning;

assert (FULL_BAR = '1')
report
"Assert 176 : < FULL_BAR /= 1 >"
severity warning;

wait for 1 ns;

-- ** POP **
--------------------------

clk <= '1'; --	 Cycle No: 218

wait for 1 ns;

I <= "1101";
CCEN_BAR <= '1';
CC_BAR <= '0';
RLD_BAR <= '1';
CI <= '0';
OEbar <= '0';

wait for 4 ns;

clk <= '0';

wait for 4 ns;

assert (FULL_BAR = '1')
report
"Assert 177 : < FULL_BAR /= 1 >" -- 	Vector No: 98
severity warning;

wait for 1 ns;

-- ** Y = TOP OF STACK
--------------------------

clk <= '1'; --	 Cycle No: 219

wait for 1 ns;

I <= "1101";
CCEN_BAR <= '0';
CC_BAR <= '1';
RLD_BAR <= '1';
CI <= '0';
OEbar <= '0';

wait for 4 ns;

clk <= '0';

wait for 4 ns;

assert (Y = "000000000000")
report
"Assert 178 : < Y /= 000000000000 >" -- 	Vector No: 99
severity warning;

assert (FULL_BAR = '1')
report
"Assert 179 : < FULL_BAR /= 1 >"
severity warning;

wait for 1 ns;

-- **************************************************
-- **************************************************
-- ******* PUSH ( I = 5) & POP ( I = 13 ) & OTHER CONDITIONS *********** 
-- ** LOAD R **
--------------------------

clk <= '1'; --	 Cycle No: 220

wait for 1 ns;

I <= "0000";
CCEN_BAR <= '0';
CC_BAR <= '0';
RLD_BAR <= '0';
CI <= '0';
OEbar <= '0';
D <= "000000000000";

wait for 4 ns;

clk <= '0';

wait for 4 ns;

wait for 1 ns;

-- ** Y = D **
--------------------------

clk <= '1'; --	 Cycle No: 221

wait for 1 ns;

I <= "0010";
CCEN_BAR <= '0';
CC_BAR <= '0';
RLD_BAR <= '1';
CI <= '0';
OEbar <= '0';
D <= "000000000000";

wait for 4 ns;

clk <= '0';

wait for 4 ns;

wait for 1 ns;

-- ** PUSH **
--------------------------

clk <= '1'; --	 Cycle No: 222

wait for 1 ns;

I <= "0101";
CCEN_BAR <= '1';
CC_BAR <= '0';
RLD_BAR <= '1';
CI <= '0';
OEbar <= '0';

wait for 4 ns;

clk <= '0';

wait for 4 ns;

wait for 1 ns;

-- ** Y = TOP OF STACK **
--------------------------

clk <= '1'; --	 Cycle No: 223

wait for 1 ns;

I <= "1101";
CCEN_BAR <= '0';
CC_BAR <= '1';
RLD_BAR <= '1';
CI <= '0';
OEbar <= '0';

wait for 4 ns;

clk <= '0';

wait for 4 ns;

assert (Y = "000000000000")
report
"Assert 180 : < Y /= 000000000000 >" -- 	Vector No: 100
severity warning;

assert (FULL_BAR = '1')
report
"Assert 181 : < FULL_BAR /= 1 >"
severity warning;

wait for 1 ns;

-- ** Y = D **
--------------------------

clk <= '1'; --	 Cycle No: 224

wait for 1 ns;

I <= "0010";
CCEN_BAR <= '0';
CC_BAR <= '0';
RLD_BAR <= '1';
CI <= '0';
OEbar <= '0';
D <= "111111111111";

wait for 4 ns;

clk <= '0';

wait for 4 ns;

wait for 1 ns;

-- ** PUSH **
--------------------------

clk <= '1'; --	 Cycle No: 225

wait for 1 ns;

I <= "0101";
CCEN_BAR <= '1';
CC_BAR <= '0';
RLD_BAR <= '1';
CI <= '0';
OEbar <= '0';

wait for 4 ns;

clk <= '0';

wait for 4 ns;

wait for 1 ns;

-- ** Y = TOP OF STACK **
--------------------------

clk <= '1'; --	 Cycle No: 226

wait for 1 ns;

I <= "1101";
CCEN_BAR <= '0';
CC_BAR <= '1';
RLD_BAR <= '1';
CI <= '0';
OEbar <= '0';

wait for 4 ns;

clk <= '0';

wait for 4 ns;

assert (Y = "111111111111")
report
"Assert 182 : < Y /= 111111111111 >" -- 	Vector No: 101
severity warning;

assert (FULL_BAR = '1')
report
"Assert 183 : < FULL_BAR /= 1 >"
severity warning;

wait for 1 ns;

-- **************************************************
-- ** LOAD R **
--------------------------

clk <= '1'; --	 Cycle No: 227

wait for 1 ns;

I <= "1110";
CCEN_BAR <= '0';
CC_BAR <= '0';
RLD_BAR <= '0';
CI <= '0';
OEbar <= '0';
D <= "000000000000";

wait for 4 ns;

clk <= '0';

wait for 4 ns;

wait for 1 ns;

-- ** Y = TOP OF STACK
--------------------------

clk <= '1'; --	 Cycle No: 228

wait for 1 ns;

I <= "1101";
CCEN_BAR <= '0';
CC_BAR <= '1';
RLD_BAR <= '1';
CI <= '0';
OEbar <= '0';

wait for 4 ns;

clk <= '0';

wait for 4 ns;

assert (Y = "111111111111")
report
"Assert 184 : < Y /= 111111111111 >" -- 	Vector No: 102
severity warning;

assert (FULL_BAR = '1')
report
"Assert 185 : < FULL_BAR /= 1 >"
severity warning;

wait for 1 ns;

-- ** POP **
--------------------------

clk <= '1'; --	 Cycle No: 229

wait for 1 ns;

I <= "1101";
CCEN_BAR <= '1';
CC_BAR <= '1';
RLD_BAR <= '1';
CI <= '0';
OEbar <= '0';

wait for 4 ns;

clk <= '0';

wait for 4 ns;

assert (FULL_BAR = '1')
report
"Assert 186 : < FULL_BAR /= 1 >" -- 	Vector No: 103
severity warning;

wait for 1 ns;

-- ** Y = TOP OF STACK
--------------------------

clk <= '1'; --	 Cycle No: 230

wait for 1 ns;

I <= "1101";
CCEN_BAR <= '0';
CC_BAR <= '1';
RLD_BAR <= '1';
CI <= '0';
OEbar <= '0';

wait for 4 ns;

clk <= '0';

wait for 4 ns;

assert (Y = "000000000000")
report
"Assert 187 : < Y /= 000000000000 >" -- 	Vector No: 104
severity warning;

assert (FULL_BAR = '1')
report
"Assert 188 : < FULL_BAR /= 1 >"
severity warning;

wait for 1 ns;

-- **************************************************
-- **************************************************
-- ******* PUSH ( I = 5) & POP ( I = 13 ) & OTHER CONDITIONS *********** 
-- ** LOAD R **
--------------------------

clk <= '1'; --	 Cycle No: 231

wait for 1 ns;

I <= "0000";
CCEN_BAR <= '0';
CC_BAR <= '0';
RLD_BAR <= '0';
CI <= '0';
OEbar <= '0';
D <= "111111111111";

wait for 4 ns;

clk <= '0';

wait for 4 ns;

wait for 1 ns;

-- ** Y = D **
--------------------------

clk <= '1'; --	 Cycle No: 232

wait for 1 ns;

I <= "0010";
CCEN_BAR <= '0';
CC_BAR <= '0';
RLD_BAR <= '1';
CI <= '0';
OEbar <= '0';
D <= "000000000000";

wait for 4 ns;

clk <= '0';

wait for 4 ns;

wait for 1 ns;

-- ** PUSH **
--------------------------

clk <= '1'; --	 Cycle No: 233

wait for 1 ns;

I <= "0101";
CCEN_BAR <= '1';
CC_BAR <= '0';
RLD_BAR <= '1';
CI <= '0';
OEbar <= '0';

wait for 4 ns;

clk <= '0';

wait for 4 ns;

wait for 1 ns;

-- ** Y = TOP OF STACK **
--------------------------

clk <= '1'; --	 Cycle No: 234

wait for 1 ns;

I <= "1101";
CCEN_BAR <= '0';
CC_BAR <= '1';
RLD_BAR <= '1';
CI <= '0';
OEbar <= '0';

wait for 4 ns;

clk <= '0';

wait for 4 ns;

assert (Y = "000000000000")
report
"Assert 189 : < Y /= 000000000000 >" -- 	Vector No: 105
severity warning;

assert (FULL_BAR = '1')
report
"Assert 190 : < FULL_BAR /= 1 >"
severity warning;

wait for 1 ns;

-- ** Y = D **
--------------------------

clk <= '1'; --	 Cycle No: 235

wait for 1 ns;

I <= "0010";
CCEN_BAR <= '0';
CC_BAR <= '0';
RLD_BAR <= '1';
CI <= '0';
OEbar <= '0';
D <= "111111111111";

wait for 4 ns;

clk <= '0';

wait for 4 ns;

wait for 1 ns;

-- ** PUSH **
--------------------------

clk <= '1'; --	 Cycle No: 236

wait for 1 ns;

I <= "0101";
CCEN_BAR <= '1';
CC_BAR <= '0';
RLD_BAR <= '1';
CI <= '0';
OEbar <= '0';

wait for 4 ns;

clk <= '0';

wait for 4 ns;

wait for 1 ns;

-- ** Y = TOP OF STACK **
--------------------------

clk <= '1'; --	 Cycle No: 237

wait for 1 ns;

I <= "1101";
CCEN_BAR <= '0';
CC_BAR <= '1';
RLD_BAR <= '1';
CI <= '0';
OEbar <= '0';

wait for 4 ns;

clk <= '0';

wait for 4 ns;

assert (Y = "111111111111")
report
"Assert 191 : < Y /= 111111111111 >" -- 	Vector No: 106
severity warning;

assert (FULL_BAR = '1')
report
"Assert 192 : < FULL_BAR /= 1 >"
severity warning;

wait for 1 ns;

-- **************************************************
-- ** LOAD R **
--------------------------

clk <= '1'; --	 Cycle No: 238

wait for 1 ns;

I <= "1110";
CCEN_BAR <= '0';
CC_BAR <= '0';
RLD_BAR <= '0';
CI <= '0';
OEbar <= '0';
D <= "111111111111";

wait for 4 ns;

clk <= '0';

wait for 4 ns;

wait for 1 ns;

-- ** Y = TOP OF STACK
--------------------------

clk <= '1'; --	 Cycle No: 239

wait for 1 ns;

I <= "1101";
CCEN_BAR <= '0';
CC_BAR <= '1';
RLD_BAR <= '1';
CI <= '0';
OEbar <= '0';

wait for 4 ns;

clk <= '0';

wait for 4 ns;

assert (Y = "111111111111")
report
"Assert 193 : < Y /= 111111111111 >" -- 	Vector No: 107
severity warning;

assert (FULL_BAR = '1')
report
"Assert 194 : < FULL_BAR /= 1 >"
severity warning;

wait for 1 ns;

-- ** POP **
--------------------------

clk <= '1'; --	 Cycle No: 240

wait for 1 ns;

I <= "1101";
CCEN_BAR <= '1';
CC_BAR <= '1';
RLD_BAR <= '1';
CI <= '0';
OEbar <= '0';

wait for 4 ns;

clk <= '0';

wait for 4 ns;

assert (FULL_BAR = '1')
report
"Assert 195 : < FULL_BAR /= 1 >" -- 	Vector No: 108
severity warning;

wait for 1 ns;

-- ** Y = TOP OF STACK
--------------------------

clk <= '1'; --	 Cycle No: 241

wait for 1 ns;

I <= "1101";
CCEN_BAR <= '0';
CC_BAR <= '1';
RLD_BAR <= '1';
CI <= '0';
OEbar <= '0';

wait for 4 ns;

clk <= '0';

wait for 4 ns;

assert (Y = "000000000000")
report
"Assert 196 : < Y /= 000000000000 >" -- 	Vector No: 109
severity warning;

assert (FULL_BAR = '1')
report
"Assert 197 : < FULL_BAR /= 1 >"
severity warning;

wait for 1 ns;

-- **************************************************
-- **************************************************
-- ******* PUSH ( I = 5) & POP ( I = 15 ) & OTHER CONDITIONS *********** 
-- ** LOAD R **
--------------------------

clk <= '1'; --	 Cycle No: 242

wait for 1 ns;

I <= "0000";
CCEN_BAR <= '0';
CC_BAR <= '0';
RLD_BAR <= '0';
CI <= '0';
OEbar <= '0';
D <= "111111111111";

wait for 4 ns;

clk <= '0';

wait for 4 ns;

wait for 1 ns;

-- ** Y = D **
--------------------------

clk <= '1'; --	 Cycle No: 243

wait for 1 ns;

I <= "0010";
CCEN_BAR <= '0';
CC_BAR <= '0';
RLD_BAR <= '1';
CI <= '0';
OEbar <= '0';
D <= "000000000000";

wait for 4 ns;

clk <= '0';

wait for 4 ns;

wait for 1 ns;

-- ** PUSH **
--------------------------

clk <= '1'; --	 Cycle No: 244

wait for 1 ns;

I <= "0101";
CCEN_BAR <= '1';
CC_BAR <= '0';
RLD_BAR <= '1';
CI <= '0';
OEbar <= '0';

wait for 4 ns;

clk <= '0';

wait for 4 ns;

wait for 1 ns;

-- ** Y = TOP OF STACK **
--------------------------

clk <= '1'; --	 Cycle No: 245

wait for 1 ns;

I <= "1101";
CCEN_BAR <= '0';
CC_BAR <= '1';
RLD_BAR <= '1';
CI <= '0';
OEbar <= '0';

wait for 4 ns;

clk <= '0';

wait for 4 ns;

assert (Y = "000000000000")
report
"Assert 198 : < Y /= 000000000000 >" -- 	Vector No: 110
severity warning;

assert (FULL_BAR = '1')
report
"Assert 199 : < FULL_BAR /= 1 >"
severity warning;

wait for 1 ns;

-- ** Y = D **
--------------------------

clk <= '1'; --	 Cycle No: 246

wait for 1 ns;

I <= "0010";
CCEN_BAR <= '0';
CC_BAR <= '0';
RLD_BAR <= '1';
CI <= '0';
OEbar <= '0';
D <= "111111111111";

wait for 4 ns;

clk <= '0';

wait for 4 ns;

wait for 1 ns;

-- ** PUSH **
--------------------------

clk <= '1'; --	 Cycle No: 247

wait for 1 ns;

I <= "0101";
CCEN_BAR <= '1';
CC_BAR <= '0';
RLD_BAR <= '1';
CI <= '0';
OEbar <= '0';

wait for 4 ns;

clk <= '0';

wait for 4 ns;

wait for 1 ns;

-- ** Y = TOP OF STACK **
--------------------------

clk <= '1'; --	 Cycle No: 248

wait for 1 ns;

I <= "1101";
CCEN_BAR <= '0';
CC_BAR <= '1';
RLD_BAR <= '1';
CI <= '0';
OEbar <= '0';

wait for 4 ns;

clk <= '0';

wait for 4 ns;

assert (Y = "111111111111")
report
"Assert 200 : < Y /= 111111111111 >" -- 	Vector No: 111
severity warning;

assert (FULL_BAR = '1')
report
"Assert 201 : < FULL_BAR /= 1 >"
severity warning;

wait for 1 ns;

-- **************************************************
-- ** LOAD R **
--------------------------

clk <= '1'; --	 Cycle No: 249

wait for 1 ns;

I <= "1110";
CCEN_BAR <= '0';
CC_BAR <= '0';
RLD_BAR <= '0';
CI <= '0';
OEbar <= '0';
D <= "000000000000";

wait for 4 ns;

clk <= '0';

wait for 4 ns;

wait for 1 ns;

-- ** Y = TOP OF STACK
--------------------------

clk <= '1'; --	 Cycle No: 250

wait for 1 ns;

I <= "1101";
CCEN_BAR <= '0';
CC_BAR <= '1';
RLD_BAR <= '1';
CI <= '0';
OEbar <= '0';

wait for 4 ns;

clk <= '0';

wait for 4 ns;

assert (Y = "111111111111")
report
"Assert 202 : < Y /= 111111111111 >" -- 	Vector No: 112
severity warning;

assert (FULL_BAR = '1')
report
"Assert 203 : < FULL_BAR /= 1 >"
severity warning;

wait for 1 ns;

-- ** POP **
--------------------------

clk <= '1'; --	 Cycle No: 251

wait for 1 ns;

I <= "1111";
CCEN_BAR <= '0';
CC_BAR <= '0';
RLD_BAR <= '1';
CI <= '0';
OEbar <= '0';

wait for 4 ns;

clk <= '0';

wait for 4 ns;

assert (FULL_BAR = '1')
report
"Assert 204 : < FULL_BAR /= 1 >" -- 	Vector No: 113
severity warning;

wait for 1 ns;

-- ** Y = TOP OF STACK
--------------------------

clk <= '1'; --	 Cycle No: 252

wait for 1 ns;

I <= "1101";
CCEN_BAR <= '0';
CC_BAR <= '1';
RLD_BAR <= '1';
CI <= '0';
OEbar <= '0';

wait for 4 ns;

clk <= '0';

wait for 4 ns;

assert (Y = "000000000000")
report
"Assert 205 : < Y /= 000000000000 >" -- 	Vector No: 114
severity warning;

assert (FULL_BAR = '1')
report
"Assert 206 : < FULL_BAR /= 1 >"
severity warning;

wait for 1 ns;

-- **************************************************
-- **************************************************
-- ******* PUSH ( I = 5) & POP ( I = 15 ) & OTHER CONDITIONS *********** 
-- ** LOAD R **
--------------------------

clk <= '1'; --	 Cycle No: 253

wait for 1 ns;

I <= "0000";
CCEN_BAR <= '0';
CC_BAR <= '0';
RLD_BAR <= '0';
CI <= '0';
OEbar <= '0';
D <= "111111111111";

wait for 4 ns;

clk <= '0';

wait for 4 ns;

wait for 1 ns;

-- ** Y = D **
--------------------------

clk <= '1'; --	 Cycle No: 254

wait for 1 ns;

I <= "0010";
CCEN_BAR <= '0';
CC_BAR <= '0';
RLD_BAR <= '1';
CI <= '0';
OEbar <= '0';
D <= "000000000000";

wait for 4 ns;

clk <= '0';

wait for 4 ns;

wait for 1 ns;

-- ** PUSH **
--------------------------

clk <= '1'; --	 Cycle No: 255

wait for 1 ns;

I <= "0101";
CCEN_BAR <= '1';
CC_BAR <= '0';
RLD_BAR <= '1';
CI <= '0';
OEbar <= '0';

wait for 4 ns;

clk <= '0';

wait for 4 ns;

wait for 1 ns;

-- ** Y = TOP OF STACK **
--------------------------

clk <= '1'; --	 Cycle No: 256

wait for 1 ns;

I <= "1101";
CCEN_BAR <= '0';
CC_BAR <= '1';
RLD_BAR <= '1';
CI <= '0';
OEbar <= '0';

wait for 4 ns;

clk <= '0';

wait for 4 ns;

assert (Y = "000000000000")
report
"Assert 207 : < Y /= 000000000000 >" -- 	Vector No: 115
severity warning;

assert (FULL_BAR = '1')
report
"Assert 208 : < FULL_BAR /= 1 >"
severity warning;

wait for 1 ns;

-- ** Y = D **
--------------------------

clk <= '1'; --	 Cycle No: 257

wait for 1 ns;

I <= "0010";
CCEN_BAR <= '0';
CC_BAR <= '0';
RLD_BAR <= '1';
CI <= '0';
OEbar <= '0';
D <= "111111111111";

wait for 4 ns;

clk <= '0';

wait for 4 ns;

wait for 1 ns;

-- ** PUSH **
--------------------------

clk <= '1'; --	 Cycle No: 258

wait for 1 ns;

I <= "0101";
CCEN_BAR <= '1';
CC_BAR <= '0';
RLD_BAR <= '1';
CI <= '0';
OEbar <= '0';

wait for 4 ns;

clk <= '0';

wait for 4 ns;

wait for 1 ns;

-- ** Y = TOP OF STACK **
--------------------------

clk <= '1'; --	 Cycle No: 259

wait for 1 ns;

I <= "1101";
CCEN_BAR <= '0';
CC_BAR <= '1';
RLD_BAR <= '1';
CI <= '0';
OEbar <= '0';

wait for 4 ns;

clk <= '0';

wait for 4 ns;

assert (Y = "111111111111")
report
"Assert 209 : < Y /= 111111111111 >" -- 	Vector No: 116
severity warning;

assert (FULL_BAR = '1')
report
"Assert 210 : < FULL_BAR /= 1 >"
severity warning;

wait for 1 ns;

-- **************************************************
-- ** LOAD R **
--------------------------

clk <= '1'; --	 Cycle No: 260

wait for 1 ns;

I <= "1110";
CCEN_BAR <= '0';
CC_BAR <= '0';
RLD_BAR <= '0';
CI <= '0';
OEbar <= '0';
D <= "000000000000";

wait for 4 ns;

clk <= '0';

wait for 4 ns;

wait for 1 ns;

-- ** Y = TOP OF STACK
--------------------------

clk <= '1'; --	 Cycle No: 261

wait for 1 ns;

I <= "1101";
CCEN_BAR <= '0';
CC_BAR <= '1';
RLD_BAR <= '1';
CI <= '0';
OEbar <= '0';

wait for 4 ns;

clk <= '0';

wait for 4 ns;

assert (Y = "111111111111")
report
"Assert 211 : < Y /= 111111111111 >" -- 	Vector No: 117
severity warning;

assert (FULL_BAR = '1')
report
"Assert 212 : < FULL_BAR /= 1 >"
severity warning;

wait for 1 ns;

-- ** POP **
--------------------------

clk <= '1'; --	 Cycle No: 262

wait for 1 ns;

I <= "1111";
CCEN_BAR <= '0';
CC_BAR <= '1';
RLD_BAR <= '1';
CI <= '0';
OEbar <= '0';

wait for 4 ns;

clk <= '0';

wait for 4 ns;

assert (FULL_BAR = '1')
report
"Assert 213 : < FULL_BAR /= 1 >" -- 	Vector No: 118
severity warning;

wait for 1 ns;

-- ** Y = TOP OF STACK
--------------------------

clk <= '1'; --	 Cycle No: 263

wait for 1 ns;

I <= "1101";
CCEN_BAR <= '0';
CC_BAR <= '1';
RLD_BAR <= '1';
CI <= '0';
OEbar <= '0';

wait for 4 ns;

clk <= '0';

wait for 4 ns;

assert (Y = "000000000000")
report
"Assert 214 : < Y /= 000000000000 >" -- 	Vector No: 119
severity warning;

assert (FULL_BAR = '1')
report
"Assert 215 : < FULL_BAR /= 1 >"
severity warning;

wait for 1 ns;

-- **************************************************
-- **************************************************
-- ******* PUSH ( I = 5) & POP ( I = 15 ) & OTHER CONDITIONS *********** 
-- ** LOAD R **
--------------------------

clk <= '1'; --	 Cycle No: 264

wait for 1 ns;

I <= "0000";
CCEN_BAR <= '0';
CC_BAR <= '0';
RLD_BAR <= '0';
CI <= '0';
OEbar <= '0';
D <= "111111111111";

wait for 4 ns;

clk <= '0';

wait for 4 ns;

wait for 1 ns;

-- ** Y = D **
--------------------------

clk <= '1'; --	 Cycle No: 265

wait for 1 ns;

I <= "0010";
CCEN_BAR <= '0';
CC_BAR <= '0';
RLD_BAR <= '1';
CI <= '0';
OEbar <= '0';
D <= "000000000000";

wait for 4 ns;

clk <= '0';

wait for 4 ns;

wait for 1 ns;

-- ** PUSH **
--------------------------

clk <= '1'; --	 Cycle No: 266

wait for 1 ns;

I <= "0101";
CCEN_BAR <= '1';
CC_BAR <= '0';
RLD_BAR <= '1';
CI <= '0';
OEbar <= '0';

wait for 4 ns;

clk <= '0';

wait for 4 ns;

wait for 1 ns;

-- ** Y = TOP OF STACK **
--------------------------

clk <= '1'; --	 Cycle No: 267

wait for 1 ns;

I <= "1101";
CCEN_BAR <= '0';
CC_BAR <= '1';
RLD_BAR <= '1';
CI <= '0';
OEbar <= '0';

wait for 4 ns;

clk <= '0';

wait for 4 ns;

assert (Y = "000000000000")
report
"Assert 216 : < Y /= 000000000000 >" -- 	Vector No: 120
severity warning;

assert (FULL_BAR = '1')
report
"Assert 217 : < FULL_BAR /= 1 >"
severity warning;

wait for 1 ns;

-- ** Y = D **
--------------------------

clk <= '1'; --	 Cycle No: 268

wait for 1 ns;

I <= "0010";
CCEN_BAR <= '0';
CC_BAR <= '0';
RLD_BAR <= '1';
CI <= '0';
OEbar <= '0';
D <= "111111111111";

wait for 4 ns;

clk <= '0';

wait for 4 ns;

wait for 1 ns;

-- ** PUSH **
--------------------------

clk <= '1'; --	 Cycle No: 269

wait for 1 ns;

I <= "0101";
CCEN_BAR <= '1';
CC_BAR <= '0';
RLD_BAR <= '1';
CI <= '0';
OEbar <= '0';

wait for 4 ns;

clk <= '0';

wait for 4 ns;

wait for 1 ns;

-- ** Y = TOP OF STACK **
--------------------------

clk <= '1'; --	 Cycle No: 270

wait for 1 ns;

I <= "1101";
CCEN_BAR <= '0';
CC_BAR <= '1';
RLD_BAR <= '1';
CI <= '0';
OEbar <= '0';

wait for 4 ns;

clk <= '0';

wait for 4 ns;

assert (Y = "111111111111")
report
"Assert 218 : < Y /= 111111111111 >" -- 	Vector No: 121
severity warning;

assert (FULL_BAR = '1')
report
"Assert 219 : < FULL_BAR /= 1 >"
severity warning;

wait for 1 ns;

-- **************************************************
-- ** LOAD R **
--------------------------

clk <= '1'; --	 Cycle No: 271

wait for 1 ns;

I <= "1110";
CCEN_BAR <= '0';
CC_BAR <= '0';
RLD_BAR <= '0';
CI <= '0';
OEbar <= '0';
D <= "000000000000";

wait for 4 ns;

clk <= '0';

wait for 4 ns;

wait for 1 ns;

-- ** Y = TOP OF STACK
--------------------------

clk <= '1'; --	 Cycle No: 272

wait for 1 ns;

I <= "1101";
CCEN_BAR <= '0';
CC_BAR <= '1';
RLD_BAR <= '1';
CI <= '0';
OEbar <= '0';

wait for 4 ns;

clk <= '0';

wait for 4 ns;

assert (Y = "111111111111")
report
"Assert 220 : < Y /= 111111111111 >" -- 	Vector No: 122
severity warning;

assert (FULL_BAR = '1')
report
"Assert 221 : < FULL_BAR /= 1 >"
severity warning;

wait for 1 ns;

-- ** POP **
--------------------------

clk <= '1'; --	 Cycle No: 273

wait for 1 ns;

I <= "1111";
CCEN_BAR <= '1';
CC_BAR <= '1';
RLD_BAR <= '1';
CI <= '0';
OEbar <= '0';

wait for 4 ns;

clk <= '0';

wait for 4 ns;

assert (FULL_BAR = '1')
report
"Assert 222 : < FULL_BAR /= 1 >" -- 	Vector No: 123
severity warning;

wait for 1 ns;

-- ** Y = TOP OF STACK
--------------------------

clk <= '1'; --	 Cycle No: 274

wait for 1 ns;

I <= "1101";
CCEN_BAR <= '0';
CC_BAR <= '1';
RLD_BAR <= '1';
CI <= '0';
OEbar <= '0';

wait for 4 ns;

clk <= '0';

wait for 4 ns;

assert (Y = "000000000000")
report
"Assert 223 : < Y /= 000000000000 >" -- 	Vector No: 124
severity warning;

assert (FULL_BAR = '1')
report
"Assert 224 : < FULL_BAR /= 1 >"
severity warning;

wait for 1 ns;

-- **************************************************
-- **************************************************
-- ******* PUSH ( I = 5) & POP ( I = 15 ) & OTHER CONDITIONS *********** 
-- ** LOAD R **
--------------------------

clk <= '1'; --	 Cycle No: 275

wait for 1 ns;

I <= "0000";
CCEN_BAR <= '0';
CC_BAR <= '0';
RLD_BAR <= '0';
CI <= '0';
OEbar <= '0';
D <= "111111111111";

wait for 4 ns;

clk <= '0';

wait for 4 ns;

wait for 1 ns;

-- ** Y = D **
--------------------------

clk <= '1'; --	 Cycle No: 276

wait for 1 ns;

I <= "0010";
CCEN_BAR <= '0';
CC_BAR <= '0';
RLD_BAR <= '1';
CI <= '0';
OEbar <= '0';
D <= "000000000000";

wait for 4 ns;

clk <= '0';

wait for 4 ns;

wait for 1 ns;

-- ** PUSH **
--------------------------

clk <= '1'; --	 Cycle No: 277

wait for 1 ns;

I <= "0101";
CCEN_BAR <= '1';
CC_BAR <= '0';
RLD_BAR <= '1';
CI <= '0';
OEbar <= '0';

wait for 4 ns;

clk <= '0';

wait for 4 ns;

wait for 1 ns;

-- ** Y = TOP OF STACK **
--------------------------

clk <= '1'; --	 Cycle No: 278

wait for 1 ns;

I <= "1101";
CCEN_BAR <= '0';
CC_BAR <= '1';
RLD_BAR <= '1';
CI <= '0';
OEbar <= '0';

wait for 4 ns;

clk <= '0';

wait for 4 ns;

assert (Y = "000000000000")
report
"Assert 225 : < Y /= 000000000000 >" -- 	Vector No: 125
severity warning;

assert (FULL_BAR = '1')
report
"Assert 226 : < FULL_BAR /= 1 >"
severity warning;

wait for 1 ns;

-- ** Y = D **
--------------------------

clk <= '1'; --	 Cycle No: 279

wait for 1 ns;

I <= "0010";
CCEN_BAR <= '0';
CC_BAR <= '0';
RLD_BAR <= '1';
CI <= '0';
OEbar <= '0';
D <= "111111111111";

wait for 4 ns;

clk <= '0';

wait for 4 ns;

wait for 1 ns;

-- ** PUSH **
--------------------------

clk <= '1'; --	 Cycle No: 280

wait for 1 ns;

I <= "0101";
CCEN_BAR <= '1';
CC_BAR <= '0';
RLD_BAR <= '1';
CI <= '0';
OEbar <= '0';

wait for 4 ns;

clk <= '0';

wait for 4 ns;

wait for 1 ns;

-- ** Y = TOP OF STACK **
--------------------------

clk <= '1'; --	 Cycle No: 281

wait for 1 ns;

I <= "1101";
CCEN_BAR <= '0';
CC_BAR <= '1';
RLD_BAR <= '1';
CI <= '0';
OEbar <= '0';

wait for 4 ns;

clk <= '0';

wait for 4 ns;

assert (Y = "111111111111")
report
"Assert 227 : < Y /= 111111111111 >" -- 	Vector No: 126
severity warning;

assert (FULL_BAR = '1')
report
"Assert 228 : < FULL_BAR /= 1 >"
severity warning;

wait for 1 ns;

-- **************************************************
-- ** LOAD R **
--------------------------

clk <= '1'; --	 Cycle No: 282

wait for 1 ns;

I <= "1110";
CCEN_BAR <= '0';
CC_BAR <= '0';
RLD_BAR <= '0';
CI <= '0';
OEbar <= '0';
D <= "000000000000";

wait for 4 ns;

clk <= '0';

wait for 4 ns;

wait for 1 ns;

-- ** Y = TOP OF STACK
--------------------------

clk <= '1'; --	 Cycle No: 283

wait for 1 ns;

I <= "1101";
CCEN_BAR <= '0';
CC_BAR <= '1';
RLD_BAR <= '1';
CI <= '0';
OEbar <= '0';

wait for 4 ns;

clk <= '0';

wait for 4 ns;

assert (Y = "111111111111")
report
"Assert 229 : < Y /= 111111111111 >" -- 	Vector No: 127
severity warning;

assert (FULL_BAR = '1')
report
"Assert 230 : < FULL_BAR /= 1 >"
severity warning;

wait for 1 ns;

-- ** POP **
--------------------------

clk <= '1'; --	 Cycle No: 284

wait for 1 ns;

I <= "1111";
CCEN_BAR <= '1';
CC_BAR <= '0';
RLD_BAR <= '1';
CI <= '0';
OEbar <= '0';

wait for 4 ns;

clk <= '0';

wait for 4 ns;

assert (FULL_BAR = '1')
report
"Assert 231 : < FULL_BAR /= 1 >" -- 	Vector No: 128
severity warning;

wait for 1 ns;

-- ** Y = TOP OF STACK
--------------------------

clk <= '1'; --	 Cycle No: 285

wait for 1 ns;

I <= "1101";
CCEN_BAR <= '0';
CC_BAR <= '1';
RLD_BAR <= '1';
CI <= '0';
OEbar <= '0';

wait for 4 ns;

clk <= '0';

wait for 4 ns;

assert (Y = "000000000000")
report
"Assert 232 : < Y /= 000000000000 >" -- 	Vector No: 129
severity warning;

assert (FULL_BAR = '1')
report
"Assert 233 : < FULL_BAR /= 1 >"
severity warning;

wait for 1 ns;

-- **************************************************
-- **************************************************
-- ******* PUSH ( I = 5) & POP ( I = 15 ) & OTHER CONDITIONS *********** 
-- ** LOAD R **
--------------------------

clk <= '1'; --	 Cycle No: 286

wait for 1 ns;

I <= "0000";
CCEN_BAR <= '0';
CC_BAR <= '0';
RLD_BAR <= '0';
CI <= '0';
OEbar <= '0';
D <= "111111111111";

wait for 4 ns;

clk <= '0';

wait for 4 ns;

wait for 1 ns;

-- ** Y = D **
--------------------------

clk <= '1'; --	 Cycle No: 287

wait for 1 ns;

I <= "0010";
CCEN_BAR <= '0';
CC_BAR <= '0';
RLD_BAR <= '1';
CI <= '0';
OEbar <= '0';
D <= "000000000000";

wait for 4 ns;

clk <= '0';

wait for 4 ns;

wait for 1 ns;

-- ** PUSH **
--------------------------

clk <= '1'; --	 Cycle No: 288

wait for 1 ns;

I <= "0101";
CCEN_BAR <= '1';
CC_BAR <= '0';
RLD_BAR <= '1';
CI <= '0';
OEbar <= '0';

wait for 4 ns;

clk <= '0';

wait for 4 ns;

wait for 1 ns;

-- ** Y = TOP OF STACK **
--------------------------

clk <= '1'; --	 Cycle No: 289

wait for 1 ns;

I <= "1101";
CCEN_BAR <= '0';
CC_BAR <= '1';
RLD_BAR <= '1';
CI <= '0';
OEbar <= '0';

wait for 4 ns;

clk <= '0';

wait for 4 ns;

assert (Y = "000000000000")
report
"Assert 234 : < Y /= 000000000000 >" -- 	Vector No: 130
severity warning;

assert (FULL_BAR = '1')
report
"Assert 235 : < FULL_BAR /= 1 >"
severity warning;

wait for 1 ns;

-- ** Y = D **
--------------------------

clk <= '1'; --	 Cycle No: 290

wait for 1 ns;

I <= "0010";
CCEN_BAR <= '0';
CC_BAR <= '0';
RLD_BAR <= '1';
CI <= '0';
OEbar <= '0';
D <= "111111111111";

wait for 4 ns;

clk <= '0';

wait for 4 ns;

wait for 1 ns;

-- ** PUSH **
--------------------------

clk <= '1'; --	 Cycle No: 291

wait for 1 ns;

I <= "0101";
CCEN_BAR <= '1';
CC_BAR <= '0';
RLD_BAR <= '1';
CI <= '0';
OEbar <= '0';

wait for 4 ns;

clk <= '0';

wait for 4 ns;

wait for 1 ns;

-- ** Y = TOP OF STACK **
--------------------------

clk <= '1'; --	 Cycle No: 292

wait for 1 ns;

I <= "1101";
CCEN_BAR <= '0';
CC_BAR <= '1';
RLD_BAR <= '1';
CI <= '0';
OEbar <= '0';

wait for 4 ns;

clk <= '0';

wait for 4 ns;

assert (Y = "111111111111")
report
"Assert 236 : < Y /= 111111111111 >" -- 	Vector No: 131
severity warning;

assert (FULL_BAR = '1')
report
"Assert 237 : < FULL_BAR /= 1 >"
severity warning;

wait for 1 ns;

-- **************************************************
-- ** LOAD R **
--------------------------

clk <= '1'; --	 Cycle No: 293

wait for 1 ns;

I <= "1110";
CCEN_BAR <= '0';
CC_BAR <= '0';
RLD_BAR <= '0';
CI <= '0';
OEbar <= '0';
D <= "111111111111";

wait for 4 ns;

clk <= '0';

wait for 4 ns;

wait for 1 ns;

-- ** Y = TOP OF STACK
--------------------------

clk <= '1'; --	 Cycle No: 294

wait for 1 ns;

I <= "1101";
CCEN_BAR <= '0';
CC_BAR <= '1';
RLD_BAR <= '1';
CI <= '0';
OEbar <= '0';

wait for 4 ns;

clk <= '0';

wait for 4 ns;

assert (Y = "111111111111")
report
"Assert 238 : < Y /= 111111111111 >" -- 	Vector No: 132
severity warning;

assert (FULL_BAR = '1')
report
"Assert 239 : < FULL_BAR /= 1 >"
severity warning;

wait for 1 ns;

-- ** POP **
--------------------------

clk <= '1'; --	 Cycle No: 295

wait for 1 ns;

I <= "1111";
CCEN_BAR <= '0';
CC_BAR <= '0';
RLD_BAR <= '1';
CI <= '0';
OEbar <= '0';

wait for 4 ns;

clk <= '0';

wait for 4 ns;

assert (FULL_BAR = '1')
report
"Assert 240 : < FULL_BAR /= 1 >" -- 	Vector No: 133
severity warning;

wait for 1 ns;

-- ** Y = TOP OF STACK
--------------------------

clk <= '1'; --	 Cycle No: 296

wait for 1 ns;

I <= "1101";
CCEN_BAR <= '0';
CC_BAR <= '1';
RLD_BAR <= '1';
CI <= '0';
OEbar <= '0';

wait for 4 ns;

clk <= '0';

wait for 4 ns;

assert (Y = "000000000000")
report
"Assert 241 : < Y /= 000000000000 >" -- 	Vector No: 134
severity warning;

assert (FULL_BAR = '1')
report
"Assert 242 : < FULL_BAR /= 1 >"
severity warning;

wait for 1 ns;

-- **************************************************
-- **************************************************
-- ******* PUSH ( I = 5) & POP ( I = 15 ) & OTHER CONDITIONS *********** 
-- ** LOAD R **
--------------------------

clk <= '1'; --	 Cycle No: 297

wait for 1 ns;

I <= "0000";
CCEN_BAR <= '0';
CC_BAR <= '0';
RLD_BAR <= '0';
CI <= '0';
OEbar <= '0';
D <= "111111111111";

wait for 4 ns;

clk <= '0';

wait for 4 ns;

wait for 1 ns;

-- ** Y = D **
--------------------------

clk <= '1'; --	 Cycle No: 298

wait for 1 ns;

I <= "0010";
CCEN_BAR <= '0';
CC_BAR <= '0';
RLD_BAR <= '1';
CI <= '0';
OEbar <= '0';
D <= "000000000000";

wait for 4 ns;

clk <= '0';

wait for 4 ns;

wait for 1 ns;

-- ** PUSH **
--------------------------

clk <= '1'; --	 Cycle No: 299

wait for 1 ns;

I <= "0101";
CCEN_BAR <= '1';
CC_BAR <= '0';
RLD_BAR <= '1';
CI <= '0';
OEbar <= '0';

wait for 4 ns;

clk <= '0';

wait for 4 ns;

wait for 1 ns;

-- ** Y = TOP OF STACK **
--------------------------

clk <= '1'; --	 Cycle No: 300

wait for 1 ns;

I <= "1101";
CCEN_BAR <= '0';
CC_BAR <= '1';
RLD_BAR <= '1';
CI <= '0';
OEbar <= '0';

wait for 4 ns;

clk <= '0';

wait for 4 ns;

assert (Y = "000000000000")
report
"Assert 243 : < Y /= 000000000000 >" -- 	Vector No: 135
severity warning;

assert (FULL_BAR = '1')
report
"Assert 244 : < FULL_BAR /= 1 >"
severity warning;

wait for 1 ns;

-- ** Y = D **
--------------------------

clk <= '1'; --	 Cycle No: 301

wait for 1 ns;

I <= "0010";
CCEN_BAR <= '0';
CC_BAR <= '0';
RLD_BAR <= '1';
CI <= '0';
OEbar <= '0';
D <= "111111111111";

wait for 4 ns;

clk <= '0';

wait for 4 ns;

wait for 1 ns;

-- ** PUSH **
--------------------------

clk <= '1'; --	 Cycle No: 302

wait for 1 ns;

I <= "0101";
CCEN_BAR <= '1';
CC_BAR <= '0';
RLD_BAR <= '1';
CI <= '0';
OEbar <= '0';

wait for 4 ns;

clk <= '0';

wait for 4 ns;

wait for 1 ns;

-- ** Y = TOP OF STACK **
--------------------------

clk <= '1'; --	 Cycle No: 303

wait for 1 ns;

I <= "1101";
CCEN_BAR <= '0';
CC_BAR <= '1';
RLD_BAR <= '1';
CI <= '0';
OEbar <= '0';

wait for 4 ns;

clk <= '0';

wait for 4 ns;

assert (Y = "111111111111")
report
"Assert 245 : < Y /= 111111111111 >" -- 	Vector No: 136
severity warning;

assert (FULL_BAR = '1')
report
"Assert 246 : < FULL_BAR /= 1 >"
severity warning;

wait for 1 ns;

-- **************************************************
-- ** LOAD R **
--------------------------

clk <= '1'; --	 Cycle No: 304

wait for 1 ns;

I <= "1110";
CCEN_BAR <= '0';
CC_BAR <= '0';
RLD_BAR <= '0';
CI <= '0';
OEbar <= '0';
D <= "111111111111";

wait for 4 ns;

clk <= '0';

wait for 4 ns;

wait for 1 ns;

-- ** Y = TOP OF STACK
--------------------------

clk <= '1'; --	 Cycle No: 305

wait for 1 ns;

I <= "1101";
CCEN_BAR <= '0';
CC_BAR <= '1';
RLD_BAR <= '1';
CI <= '0';
OEbar <= '0';

wait for 4 ns;

clk <= '0';

wait for 4 ns;

assert (Y = "111111111111")
report
"Assert 247 : < Y /= 111111111111 >" -- 	Vector No: 137
severity warning;

assert (FULL_BAR = '1')
report
"Assert 248 : < FULL_BAR /= 1 >"
severity warning;

wait for 1 ns;

-- ** POP **
--------------------------

clk <= '1'; --	 Cycle No: 306

wait for 1 ns;

I <= "1111";
CCEN_BAR <= '1';
CC_BAR <= '1';
RLD_BAR <= '1';
CI <= '0';
OEbar <= '0';

wait for 4 ns;

clk <= '0';

wait for 4 ns;

assert (FULL_BAR = '1')
report
"Assert 249 : < FULL_BAR /= 1 >" -- 	Vector No: 138
severity warning;

wait for 1 ns;

-- ** Y = TOP OF STACK
--------------------------

clk <= '1'; --	 Cycle No: 307

wait for 1 ns;

I <= "1101";
CCEN_BAR <= '0';
CC_BAR <= '1';
RLD_BAR <= '1';
CI <= '0';
OEbar <= '0';

wait for 4 ns;

clk <= '0';

wait for 4 ns;

assert (Y = "000000000000")
report
"Assert 250 : < Y /= 000000000000 >" -- 	Vector No: 139
severity warning;

assert (FULL_BAR = '1')
report
"Assert 251 : < FULL_BAR /= 1 >"
severity warning;

wait for 1 ns;

-- **************************************************
-- **************************************************
-- ******* PUSH ( I = 5) & POP ( I = 15 ) & OTHER CONDITIONS *********** 
-- ** LOAD R **
--------------------------

clk <= '1'; --	 Cycle No: 308

wait for 1 ns;

I <= "0000";
CCEN_BAR <= '0';
CC_BAR <= '0';
RLD_BAR <= '0';
CI <= '0';
OEbar <= '0';
D <= "111111111111";

wait for 4 ns;

clk <= '0';

wait for 4 ns;

wait for 1 ns;

-- ** Y = D **
--------------------------

clk <= '1'; --	 Cycle No: 309

wait for 1 ns;

I <= "0010";
CCEN_BAR <= '0';
CC_BAR <= '0';
RLD_BAR <= '1';
CI <= '0';
OEbar <= '0';
D <= "000000000000";

wait for 4 ns;

clk <= '0';

wait for 4 ns;

wait for 1 ns;

-- ** PUSH **
--------------------------

clk <= '1'; --	 Cycle No: 310

wait for 1 ns;

I <= "0101";
CCEN_BAR <= '1';
CC_BAR <= '0';
RLD_BAR <= '1';
CI <= '0';
OEbar <= '0';

wait for 4 ns;

clk <= '0';

wait for 4 ns;

wait for 1 ns;

-- ** Y = TOP OF STACK **
--------------------------

clk <= '1'; --	 Cycle No: 311

wait for 1 ns;

I <= "1101";
CCEN_BAR <= '0';
CC_BAR <= '1';
RLD_BAR <= '1';
CI <= '0';
OEbar <= '0';

wait for 4 ns;

clk <= '0';

wait for 4 ns;

assert (Y = "000000000000")
report
"Assert 252 : < Y /= 000000000000 >" -- 	Vector No: 140
severity warning;

assert (FULL_BAR = '1')
report
"Assert 253 : < FULL_BAR /= 1 >"
severity warning;

wait for 1 ns;

-- ** Y = D **
--------------------------

clk <= '1'; --	 Cycle No: 312

wait for 1 ns;

I <= "0010";
CCEN_BAR <= '0';
CC_BAR <= '0';
RLD_BAR <= '1';
CI <= '0';
OEbar <= '0';
D <= "111111111111";

wait for 4 ns;

clk <= '0';

wait for 4 ns;

wait for 1 ns;

-- ** PUSH **
--------------------------

clk <= '1'; --	 Cycle No: 313

wait for 1 ns;

I <= "0101";
CCEN_BAR <= '1';
CC_BAR <= '0';
RLD_BAR <= '1';
CI <= '0';
OEbar <= '0';

wait for 4 ns;

clk <= '0';

wait for 4 ns;

wait for 1 ns;

-- ** Y = TOP OF STACK **
--------------------------

clk <= '1'; --	 Cycle No: 314

wait for 1 ns;

I <= "1101";
CCEN_BAR <= '0';
CC_BAR <= '1';
RLD_BAR <= '1';
CI <= '0';
OEbar <= '0';

wait for 4 ns;

clk <= '0';

wait for 4 ns;

assert (Y = "111111111111")
report
"Assert 254 : < Y /= 111111111111 >" -- 	Vector No: 141
severity warning;

assert (FULL_BAR = '1')
report
"Assert 255 : < FULL_BAR /= 1 >"
severity warning;

wait for 1 ns;

-- **************************************************
-- ** LOAD R **
--------------------------

clk <= '1'; --	 Cycle No: 315

wait for 1 ns;

I <= "1110";
CCEN_BAR <= '0';
CC_BAR <= '0';
RLD_BAR <= '0';
CI <= '0';
OEbar <= '0';
D <= "111111111111";

wait for 4 ns;

clk <= '0';

wait for 4 ns;

wait for 1 ns;

-- ** Y = TOP OF STACK
--------------------------

clk <= '1'; --	 Cycle No: 316

wait for 1 ns;

I <= "1101";
CCEN_BAR <= '0';
CC_BAR <= '1';
RLD_BAR <= '1';
CI <= '0';
OEbar <= '0';

wait for 4 ns;

clk <= '0';

wait for 4 ns;

assert (Y = "111111111111")
report
"Assert 256 : < Y /= 111111111111 >" -- 	Vector No: 142
severity warning;

assert (FULL_BAR = '1')
report
"Assert 257 : < FULL_BAR /= 1 >"
severity warning;

wait for 1 ns;

-- ** POP **
--------------------------

clk <= '1'; --	 Cycle No: 317

wait for 1 ns;

I <= "1111";
CCEN_BAR <= '1';
CC_BAR <= '0';
RLD_BAR <= '1';
CI <= '0';
OEbar <= '0';

wait for 4 ns;

clk <= '0';

wait for 4 ns;

assert (FULL_BAR = '1')
report
"Assert 258 : < FULL_BAR /= 1 >" -- 	Vector No: 143
severity warning;

wait for 1 ns;

-- ** Y = TOP OF STACK
--------------------------

clk <= '1'; --	 Cycle No: 318

wait for 1 ns;

I <= "1101";
CCEN_BAR <= '0';
CC_BAR <= '1';
RLD_BAR <= '1';
CI <= '0';
OEbar <= '0';

wait for 4 ns;

clk <= '0';

wait for 4 ns;

assert (Y = "000000000000")
report
"Assert 259 : < Y /= 000000000000 >" -- 	Vector No: 144
severity warning;

assert (FULL_BAR = '1')
report
"Assert 260 : < FULL_BAR /= 1 >"
severity warning;

wait for 1 ns;

-- **************************************************
-- **************************************************
-- *******************************************
-- ************* TEST ALL STACK **************
-- *************  POSITIONS     **************
-- *******************************************
-- ************* FIRST SET OF STUCK-AT FAULTS *************
-- ***** RESET CHIP AND LOAD R = 0 *****
--------------------------

clk <= '1'; --	 Cycle No: 319

wait for 1 ns;

I <= "0000";
CCEN_BAR <= '0';
CC_BAR <= '0';
RLD_BAR <= '0';
CI <= '0';
OEbar <= '0';
D <= "000000000000";

wait for 4 ns;

clk <= '0';

wait for 4 ns;

wait for 1 ns;

-- ************* 6 PUSHES *************
-- ***** START PUSHES *****
-- ** PUSH  1 **
--------------------------

clk <= '1'; --	 Cycle No: 320

wait for 1 ns;

I <= "0010";
CCEN_BAR <= '0';
CC_BAR <= '0';
RLD_BAR <= '1';
CI <= '0';
OEbar <= '0';
D <= "000000000000";

wait for 4 ns;

clk <= '0';

wait for 4 ns;

wait for 1 ns;

--------------------------

clk <= '1'; --	 Cycle No: 321

wait for 1 ns;

I <= "0001";
CCEN_BAR <= '1';
CC_BAR <= '1';
RLD_BAR <= '1';
CI <= '0';
OEbar <= '0';

wait for 4 ns;

clk <= '0';

wait for 4 ns;

wait for 1 ns;

--------------------------

clk <= '1'; --	 Cycle No: 322

wait for 1 ns;

I <= "1101";
CCEN_BAR <= '0';
CC_BAR <= '1';
RLD_BAR <= '1';
CI <= '0';
OEbar <= '0';

wait for 4 ns;

clk <= '0';

wait for 4 ns;

assert (Y = "000000000000")
report
"Assert 261 : < Y /= 000000000000 >" -- 	Vector No: 145
severity warning;

assert (FULL_BAR = '1')
report
"Assert 262 : < FULL_BAR /= 1 >"
severity warning;

wait for 1 ns;

-- **************************************************
-- ** PUSH  2 **
--------------------------

clk <= '1'; --	 Cycle No: 323

wait for 1 ns;

I <= "0010";
CCEN_BAR <= '0';
CC_BAR <= '0';
RLD_BAR <= '1';
CI <= '0';
OEbar <= '0';
D <= "000000000001";

wait for 4 ns;

clk <= '0';

wait for 4 ns;

wait for 1 ns;

--------------------------

clk <= '1'; --	 Cycle No: 324

wait for 1 ns;

I <= "0001";
CCEN_BAR <= '1';
CC_BAR <= '1';
RLD_BAR <= '1';
CI <= '0';
OEbar <= '0';

wait for 4 ns;

clk <= '0';

wait for 4 ns;

wait for 1 ns;

--------------------------

clk <= '1'; --	 Cycle No: 325

wait for 1 ns;

I <= "1101";
CCEN_BAR <= '0';
CC_BAR <= '1';
RLD_BAR <= '1';
CI <= '0';
OEbar <= '0';

wait for 4 ns;

clk <= '0';

wait for 4 ns;

assert (Y = "000000000001")
report
"Assert 263 : < Y /= 000000000001 >" -- 	Vector No: 146
severity warning;

assert (FULL_BAR = '1')
report
"Assert 264 : < FULL_BAR /= 1 >"
severity warning;

wait for 1 ns;

-- **************************************************
-- ** PUSH  3 **
--------------------------

clk <= '1'; --	 Cycle No: 326

wait for 1 ns;

I <= "0010";
CCEN_BAR <= '0';
CC_BAR <= '0';
RLD_BAR <= '1';
CI <= '0';
OEbar <= '0';
D <= "000000000010";

wait for 4 ns;

clk <= '0';

wait for 4 ns;

wait for 1 ns;

--------------------------

clk <= '1'; --	 Cycle No: 327

wait for 1 ns;

I <= "0001";
CCEN_BAR <= '1';
CC_BAR <= '1';
RLD_BAR <= '1';
CI <= '0';
OEbar <= '0';

wait for 4 ns;

clk <= '0';

wait for 4 ns;

wait for 1 ns;

--------------------------

clk <= '1'; --	 Cycle No: 328

wait for 1 ns;

I <= "1101";
CCEN_BAR <= '0';
CC_BAR <= '1';
RLD_BAR <= '1';
CI <= '0';
OEbar <= '0';

wait for 4 ns;

clk <= '0';

wait for 4 ns;

assert (Y = "000000000010")
report
"Assert 265 : < Y /= 000000000010 >" -- 	Vector No: 147
severity warning;

assert (FULL_BAR = '1')
report
"Assert 266 : < FULL_BAR /= 1 >"
severity warning;

wait for 1 ns;

-- **************************************************
-- ** PUSH  4 **
--------------------------

clk <= '1'; --	 Cycle No: 329

wait for 1 ns;

I <= "0010";
CCEN_BAR <= '0';
CC_BAR <= '0';
RLD_BAR <= '1';
CI <= '0';
OEbar <= '0';
D <= "000000000100";

wait for 4 ns;

clk <= '0';

wait for 4 ns;

wait for 1 ns;

--------------------------

clk <= '1'; --	 Cycle No: 330

wait for 1 ns;

I <= "0001";
CCEN_BAR <= '1';
CC_BAR <= '1';
RLD_BAR <= '1';
CI <= '0';
OEbar <= '0';

wait for 4 ns;

clk <= '0';

wait for 4 ns;

wait for 1 ns;

--------------------------

clk <= '1'; --	 Cycle No: 331

wait for 1 ns;

I <= "1101";
CCEN_BAR <= '0';
CC_BAR <= '1';
RLD_BAR <= '1';
CI <= '0';
OEbar <= '0';

wait for 4 ns;

clk <= '0';

wait for 4 ns;

assert (Y = "000000000100")
report
"Assert 267 : < Y /= 000000000100 >" -- 	Vector No: 148
severity warning;

assert (FULL_BAR = '1')
report
"Assert 268 : < FULL_BAR /= 1 >"
severity warning;

wait for 1 ns;

-- **************************************************
-- ** PUSH  5 **
--------------------------

clk <= '1'; --	 Cycle No: 332

wait for 1 ns;

I <= "0010";
CCEN_BAR <= '0';
CC_BAR <= '0';
RLD_BAR <= '1';
CI <= '0';
OEbar <= '0';
D <= "111111110111";

wait for 4 ns;

clk <= '0';

wait for 4 ns;

wait for 1 ns;

--------------------------

clk <= '1'; --	 Cycle No: 333

wait for 1 ns;

I <= "0001";
CCEN_BAR <= '1';
CC_BAR <= '1';
RLD_BAR <= '1';
CI <= '0';
OEbar <= '0';

wait for 4 ns;

clk <= '0';

wait for 4 ns;

wait for 1 ns;

--------------------------

clk <= '1'; --	 Cycle No: 334

wait for 1 ns;

I <= "1101";
CCEN_BAR <= '0';
CC_BAR <= '1';
RLD_BAR <= '1';
CI <= '0';
OEbar <= '0';

wait for 4 ns;

clk <= '0';

wait for 4 ns;

assert (Y = "111111110111")
report
"Assert 269 : < Y /= 111111110111 >" -- 	Vector No: 149
severity warning;

assert (FULL_BAR = '0')
report
"Assert 270 : < FULL_BAR /= 0 >"
severity warning;

wait for 1 ns;

-- **************************************************
-- ** PUSH  6 (OVER-WRITE TOP OF STACK) **
--------------------------

clk <= '1'; --	 Cycle No: 335

wait for 1 ns;

I <= "0010";
CCEN_BAR <= '0';
CC_BAR <= '0';
RLD_BAR <= '1';
CI <= '0';
OEbar <= '0';
D <= "000000001000";

wait for 4 ns;

clk <= '0';

wait for 4 ns;

wait for 1 ns;

--------------------------

clk <= '1'; --	 Cycle No: 336

wait for 1 ns;

I <= "0001";
CCEN_BAR <= '1';
CC_BAR <= '1';
RLD_BAR <= '1';
CI <= '0';
OEbar <= '0';

wait for 4 ns;

clk <= '0';

wait for 4 ns;

wait for 1 ns;

--------------------------

clk <= '1'; --	 Cycle No: 337

wait for 1 ns;

I <= "1101";
CCEN_BAR <= '0';
CC_BAR <= '1';
RLD_BAR <= '1';
CI <= '0';
OEbar <= '0';

wait for 4 ns;

clk <= '0';

wait for 4 ns;

assert (Y = "000000001000")
report
"Assert 271 : < Y /= 000000001000 >" -- 	Vector No: 150
severity warning;

assert (FULL_BAR = '0')
report
"Assert 272 : < FULL_BAR /= 0 >"
severity warning;

wait for 1 ns;

-- **************************************************
-- ************* 6 POPS *************
-- **** LOAD REG-CTR WITH ZERO ****
--------------------------

clk <= '1'; --	 Cycle No: 338

wait for 1 ns;

I <= "1110";
CCEN_BAR <= '1';
CC_BAR <= '1';
RLD_BAR <= '0';
CI <= '0';
OEbar <= '0';
D <= "000000000000";

wait for 4 ns;

clk <= '0';

wait for 4 ns;

wait for 1 ns;

-- **************************************************
-- ** POP  1 **
--------------------------

clk <= '1'; --	 Cycle No: 339

wait for 1 ns;

I <= "1000";
CCEN_BAR <= '0';
CC_BAR <= '1';
RLD_BAR <= '1';
CI <= '0';
OEbar <= '0';

wait for 4 ns;

clk <= '0';

wait for 4 ns;

assert (Y = "000000001000")
report
"Assert 273 : < Y /= 000000001000 >" -- 	Vector No: 151
severity warning;

assert (FULL_BAR = '0')
report
"Assert 274 : < FULL_BAR /= 0 >"
severity warning;

wait for 1 ns;

--------------------------

clk <= '1'; --	 Cycle No: 340

wait for 1 ns;

I <= "1101";
CCEN_BAR <= '0';
CC_BAR <= '1';
RLD_BAR <= '1';
CI <= '0';
OEbar <= '0';

wait for 4 ns;

clk <= '0';

wait for 4 ns;

assert (Y = "000000000100")
report
"Assert 275 : < Y /= 000000000100 >" -- 	Vector No: 152
severity warning;

assert (FULL_BAR = '1')
report
"Assert 276 : < FULL_BAR /= 1 >"
severity warning;

wait for 1 ns;

-- **************************************************
-- ** POP  2 **
--------------------------

clk <= '1'; --	 Cycle No: 341

wait for 1 ns;

I <= "1000";
CCEN_BAR <= '0';
CC_BAR <= '1';
RLD_BAR <= '1';
CI <= '0';
OEbar <= '0';

wait for 4 ns;

clk <= '0';

wait for 4 ns;

assert (Y = "000000000100")
report
"Assert 277 : < Y /= 000000000100 >" -- 	Vector No: 153
severity warning;

assert (FULL_BAR = '1')
report
"Assert 278 : < FULL_BAR /= 1 >"
severity warning;

wait for 1 ns;

--------------------------

clk <= '1'; --	 Cycle No: 342

wait for 1 ns;

I <= "1101";
CCEN_BAR <= '0';
CC_BAR <= '1';
RLD_BAR <= '1';
CI <= '0';
OEbar <= '0';

wait for 4 ns;

clk <= '0';

wait for 4 ns;

assert (Y = "000000000010")
report
"Assert 279 : < Y /= 000000000010 >" -- 	Vector No: 154
severity warning;

assert (FULL_BAR = '1')
report
"Assert 280 : < FULL_BAR /= 1 >"
severity warning;

wait for 1 ns;

-- **************************************************
-- ** POP  3 **
--------------------------

clk <= '1'; --	 Cycle No: 343

wait for 1 ns;

I <= "1000";
CCEN_BAR <= '0';
CC_BAR <= '1';
RLD_BAR <= '1';
CI <= '0';
OEbar <= '0';

wait for 4 ns;

clk <= '0';

wait for 4 ns;

assert (Y = "000000000010")
report
"Assert 281 : < Y /= 000000000010 >" -- 	Vector No: 155
severity warning;

assert (FULL_BAR = '1')
report
"Assert 282 : < FULL_BAR /= 1 >"
severity warning;

wait for 1 ns;

--------------------------

clk <= '1'; --	 Cycle No: 344

wait for 1 ns;

I <= "1101";
CCEN_BAR <= '0';
CC_BAR <= '1';
RLD_BAR <= '1';
CI <= '0';
OEbar <= '0';

wait for 4 ns;

clk <= '0';

wait for 4 ns;

assert (Y = "000000000001")
report
"Assert 283 : < Y /= 000000000001 >" -- 	Vector No: 156
severity warning;

assert (FULL_BAR = '1')
report
"Assert 284 : < FULL_BAR /= 1 >"
severity warning;

wait for 1 ns;

-- **************************************************
-- ** POP  4 **
--------------------------

clk <= '1'; --	 Cycle No: 345

wait for 1 ns;

I <= "1000";
CCEN_BAR <= '0';
CC_BAR <= '1';
RLD_BAR <= '1';
CI <= '0';
OEbar <= '0';

wait for 4 ns;

clk <= '0';

wait for 4 ns;

assert (Y = "000000000001")
report
"Assert 285 : < Y /= 000000000001 >" -- 	Vector No: 157
severity warning;

assert (FULL_BAR = '1')
report
"Assert 286 : < FULL_BAR /= 1 >"
severity warning;

wait for 1 ns;

--------------------------

clk <= '1'; --	 Cycle No: 346

wait for 1 ns;

I <= "1101";
CCEN_BAR <= '0';
CC_BAR <= '1';
RLD_BAR <= '1';
CI <= '0';
OEbar <= '0';

wait for 4 ns;

clk <= '0';

wait for 4 ns;

assert (Y = "000000000000")
report
"Assert 287 : < Y /= 000000000000 >" -- 	Vector No: 158
severity warning;

assert (FULL_BAR = '1')
report
"Assert 288 : < FULL_BAR /= 1 >"
severity warning;

wait for 1 ns;

-- **************************************************
-- ** POP  5 **
--------------------------

clk <= '1'; --	 Cycle No: 347

wait for 1 ns;

I <= "1000";
CCEN_BAR <= '0';
CC_BAR <= '1';
RLD_BAR <= '1';
CI <= '0';
OEbar <= '0';

wait for 4 ns;

clk <= '0';

wait for 4 ns;

assert (Y = "000000000000")
report
"Assert 289 : < Y /= 000000000000 >" -- 	Vector No: 159
severity warning;

assert (FULL_BAR = '1')
report
"Assert 290 : < FULL_BAR /= 1 >"
severity warning;

wait for 1 ns;

--------------------------

clk <= '1'; --	 Cycle No: 348

wait for 1 ns;

I <= "1101";
CCEN_BAR <= '0';
CC_BAR <= '1';
RLD_BAR <= '1';
CI <= '0';
OEbar <= '0';

wait for 4 ns;

clk <= '0';

wait for 4 ns;

assert (FULL_BAR = '1')
report
"Assert 291 : < FULL_BAR /= 1 >" -- 	Vector No: 160
severity warning;

wait for 1 ns;

-- **************************************************
-- ** POP  6 (POP EMPTY STACK) **
--------------------------

clk <= '1'; --	 Cycle No: 349

wait for 1 ns;

I <= "1000";
CCEN_BAR <= '0';
CC_BAR <= '1';
RLD_BAR <= '1';
CI <= '0';
OEbar <= '0';

wait for 4 ns;

clk <= '0';

wait for 4 ns;

assert (FULL_BAR = '1')
report
"Assert 292 : < FULL_BAR /= 1 >" -- 	Vector No: 161
severity warning;

wait for 1 ns;

--------------------------

clk <= '1'; --	 Cycle No: 350

wait for 1 ns;

I <= "1101";
CCEN_BAR <= '0';
CC_BAR <= '1';
RLD_BAR <= '1';
CI <= '0';
OEbar <= '0';

wait for 4 ns;

clk <= '0';

wait for 4 ns;

assert (FULL_BAR = '1')
report
"Assert 293 : < FULL_BAR /= 1 >" -- 	Vector No: 162
severity warning;

wait for 1 ns;

-- **************************************************
-- ************* SECOND SET OF STUCK-AT FAULTS *************
-- ***** RESET CHIP AND LOAD R = 0 *****
--------------------------

clk <= '1'; --	 Cycle No: 351

wait for 1 ns;

I <= "0000";
CCEN_BAR <= '0';
CC_BAR <= '0';
RLD_BAR <= '0';
CI <= '1';
OEbar <= '0';
D <= "000000000000";

wait for 4 ns;

clk <= '0';

wait for 4 ns;

wait for 1 ns;

-- ************* 6 PUSHES *************
-- ***** START PUSHES *****
-- ** PUSH  1 **
--------------------------

clk <= '1'; --	 Cycle No: 352

wait for 1 ns;

I <= "0010";
CCEN_BAR <= '0';
CC_BAR <= '0';
RLD_BAR <= '1';
CI <= '0';
OEbar <= '0';
D <= "111111111111";

wait for 4 ns;

clk <= '0';

wait for 4 ns;

wait for 1 ns;

--------------------------

clk <= '1'; --	 Cycle No: 353

wait for 1 ns;

I <= "0001";
CCEN_BAR <= '1';
CC_BAR <= '1';
RLD_BAR <= '1';
CI <= '0';
OEbar <= '0';

wait for 4 ns;

clk <= '0';

wait for 4 ns;

wait for 1 ns;

--------------------------

clk <= '1'; --	 Cycle No: 354

wait for 1 ns;

I <= "1101";
CCEN_BAR <= '0';
CC_BAR <= '1';
RLD_BAR <= '1';
CI <= '0';
OEbar <= '0';

wait for 4 ns;

clk <= '0';

wait for 4 ns;

assert (Y = "111111111111")
report
"Assert 294 : < Y /= 111111111111 >" -- 	Vector No: 163
severity warning;

assert (FULL_BAR = '1')
report
"Assert 295 : < FULL_BAR /= 1 >"
severity warning;

wait for 1 ns;

-- **************************************************
-- ** PUSH  2 **
--------------------------

clk <= '1'; --	 Cycle No: 355

wait for 1 ns;

I <= "0010";
CCEN_BAR <= '0';
CC_BAR <= '0';
RLD_BAR <= '1';
CI <= '0';
OEbar <= '0';
D <= "111111111110";

wait for 4 ns;

clk <= '0';

wait for 4 ns;

wait for 1 ns;

--------------------------

clk <= '1'; --	 Cycle No: 356

wait for 1 ns;

I <= "0001";
CCEN_BAR <= '1';
CC_BAR <= '1';
RLD_BAR <= '1';
CI <= '0';
OEbar <= '0';

wait for 4 ns;

clk <= '0';

wait for 4 ns;

wait for 1 ns;

--------------------------

clk <= '1'; --	 Cycle No: 357

wait for 1 ns;

I <= "1101";
CCEN_BAR <= '0';
CC_BAR <= '1';
RLD_BAR <= '1';
CI <= '0';
OEbar <= '0';

wait for 4 ns;

clk <= '0';

wait for 4 ns;

assert (Y = "111111111110")
report
"Assert 296 : < Y /= 111111111110 >" -- 	Vector No: 164
severity warning;

assert (FULL_BAR = '1')
report
"Assert 297 : < FULL_BAR /= 1 >"
severity warning;

wait for 1 ns;

-- **************************************************
-- ** PUSH  3 **
--------------------------

clk <= '1'; --	 Cycle No: 358

wait for 1 ns;

I <= "0010";
CCEN_BAR <= '0';
CC_BAR <= '0';
RLD_BAR <= '1';
CI <= '0';
OEbar <= '0';
D <= "111111111101";

wait for 4 ns;

clk <= '0';

wait for 4 ns;

wait for 1 ns;

--------------------------

clk <= '1'; --	 Cycle No: 359

wait for 1 ns;

I <= "0001";
CCEN_BAR <= '1';
CC_BAR <= '1';
RLD_BAR <= '1';
CI <= '0';
OEbar <= '0';

wait for 4 ns;

clk <= '0';

wait for 4 ns;

wait for 1 ns;

--------------------------

clk <= '1'; --	 Cycle No: 360

wait for 1 ns;

I <= "1101";
CCEN_BAR <= '0';
CC_BAR <= '1';
RLD_BAR <= '1';
CI <= '0';
OEbar <= '0';

wait for 4 ns;

clk <= '0';

wait for 4 ns;

assert (Y = "111111111101")
report
"Assert 298 : < Y /= 111111111101 >" -- 	Vector No: 165
severity warning;

assert (FULL_BAR = '1')
report
"Assert 299 : < FULL_BAR /= 1 >"
severity warning;

wait for 1 ns;

-- **************************************************
-- ** PUSH  4 **
--------------------------

clk <= '1'; --	 Cycle No: 361

wait for 1 ns;

I <= "0010";
CCEN_BAR <= '0';
CC_BAR <= '0';
RLD_BAR <= '1';
CI <= '0';
OEbar <= '0';
D <= "111111111011";

wait for 4 ns;

clk <= '0';

wait for 4 ns;

wait for 1 ns;

--------------------------

clk <= '1'; --	 Cycle No: 362

wait for 1 ns;

I <= "0001";
CCEN_BAR <= '1';
CC_BAR <= '1';
RLD_BAR <= '1';
CI <= '0';
OEbar <= '0';

wait for 4 ns;

clk <= '0';

wait for 4 ns;

wait for 1 ns;

--------------------------

clk <= '1'; --	 Cycle No: 363

wait for 1 ns;

I <= "1101";
CCEN_BAR <= '0';
CC_BAR <= '1';
RLD_BAR <= '1';
CI <= '0';
OEbar <= '0';

wait for 4 ns;

clk <= '0';

wait for 4 ns;

assert (Y = "111111111011")
report
"Assert 300 : < Y /= 111111111011 >" -- 	Vector No: 166
severity warning;

assert (FULL_BAR = '1')
report
"Assert 301 : < FULL_BAR /= 1 >"
severity warning;

wait for 1 ns;

-- **************************************************
-- ** PUSH  5 **
--------------------------

clk <= '1'; --	 Cycle No: 364

wait for 1 ns;

I <= "0010";
CCEN_BAR <= '0';
CC_BAR <= '0';
RLD_BAR <= '1';
CI <= '0';
OEbar <= '0';
D <= "000000001000";

wait for 4 ns;

clk <= '0';

wait for 4 ns;

wait for 1 ns;

--------------------------

clk <= '1'; --	 Cycle No: 365

wait for 1 ns;

I <= "0001";
CCEN_BAR <= '1';
CC_BAR <= '1';
RLD_BAR <= '1';
CI <= '0';
OEbar <= '0';

wait for 4 ns;

clk <= '0';

wait for 4 ns;

wait for 1 ns;

--------------------------

clk <= '1'; --	 Cycle No: 366

wait for 1 ns;

I <= "1101";
CCEN_BAR <= '0';
CC_BAR <= '1';
RLD_BAR <= '1';
CI <= '0';
OEbar <= '0';

wait for 4 ns;

clk <= '0';

wait for 4 ns;

assert (Y = "000000001000")
report
"Assert 302 : < Y /= 000000001000 >" -- 	Vector No: 167
severity warning;

assert (FULL_BAR = '0')
report
"Assert 303 : < FULL_BAR /= 0 >"
severity warning;

wait for 1 ns;

-- **************************************************
-- ** PUSH  6 (OVER-WRITE TOP OF STACK) **
--------------------------

clk <= '1'; --	 Cycle No: 367

wait for 1 ns;

I <= "0010";
CCEN_BAR <= '0';
CC_BAR <= '0';
RLD_BAR <= '1';
CI <= '0';
OEbar <= '0';
D <= "111111110111";

wait for 4 ns;

clk <= '0';

wait for 4 ns;

wait for 1 ns;

--------------------------

clk <= '1'; --	 Cycle No: 368

wait for 1 ns;

I <= "0001";
CCEN_BAR <= '1';
CC_BAR <= '1';
RLD_BAR <= '1';
CI <= '0';
OEbar <= '0';

wait for 4 ns;

clk <= '0';

wait for 4 ns;

wait for 1 ns;

--------------------------

clk <= '1'; --	 Cycle No: 369

wait for 1 ns;

I <= "1101";
CCEN_BAR <= '0';
CC_BAR <= '1';
RLD_BAR <= '1';
CI <= '0';
OEbar <= '0';

wait for 4 ns;

clk <= '0';

wait for 4 ns;

assert (Y = "111111110111")
report
"Assert 304 : < Y /= 111111110111 >" -- 	Vector No: 168
severity warning;

assert (FULL_BAR = '0')
report
"Assert 305 : < FULL_BAR /= 0 >"
severity warning;

wait for 1 ns;

-- **************************************************
-- ************* 6 POPS *************
-- **** LOAD REG-CTR WITH ZERO ****
--------------------------

clk <= '1'; --	 Cycle No: 370

wait for 1 ns;

I <= "1110";
CCEN_BAR <= '1';
CC_BAR <= '1';
RLD_BAR <= '0';
CI <= '0';
OEbar <= '0';
D <= "000000000000";

wait for 4 ns;

clk <= '0';

wait for 4 ns;

wait for 1 ns;

-- **************************************************
-- ** POP  1 **
--------------------------

clk <= '1'; --	 Cycle No: 371

wait for 1 ns;

I <= "1000";
CCEN_BAR <= '0';
CC_BAR <= '1';
RLD_BAR <= '1';
CI <= '0';
OEbar <= '0';

wait for 4 ns;

clk <= '0';

wait for 4 ns;

assert (Y = "111111110111")
report
"Assert 306 : < Y /= 111111110111 >" -- 	Vector No: 169
severity warning;

assert (FULL_BAR = '0')
report
"Assert 307 : < FULL_BAR /= 0 >"
severity warning;

wait for 1 ns;

--------------------------

clk <= '1'; --	 Cycle No: 372

wait for 1 ns;

I <= "1101";
CCEN_BAR <= '0';
CC_BAR <= '1';
RLD_BAR <= '1';
CI <= '0';
OEbar <= '0';

wait for 4 ns;

clk <= '0';

wait for 4 ns;

assert (Y = "111111111011")
report
"Assert 308 : < Y /= 111111111011 >" -- 	Vector No: 170
severity warning;

assert (FULL_BAR = '1')
report
"Assert 309 : < FULL_BAR /= 1 >"
severity warning;

wait for 1 ns;

-- **************************************************
-- ** POP  2 **
--------------------------

clk <= '1'; --	 Cycle No: 373

wait for 1 ns;

I <= "1000";
CCEN_BAR <= '0';
CC_BAR <= '1';
RLD_BAR <= '1';
CI <= '0';
OEbar <= '0';

wait for 4 ns;

clk <= '0';

wait for 4 ns;

assert (Y = "111111111011")
report
"Assert 310 : < Y /= 111111111011 >" -- 	Vector No: 171
severity warning;

assert (FULL_BAR = '1')
report
"Assert 311 : < FULL_BAR /= 1 >"
severity warning;

wait for 1 ns;

--------------------------

clk <= '1'; --	 Cycle No: 374

wait for 1 ns;

I <= "1101";
CCEN_BAR <= '0';
CC_BAR <= '1';
RLD_BAR <= '1';
CI <= '0';
OEbar <= '0';

wait for 4 ns;

clk <= '0';

wait for 4 ns;

assert (Y = "111111111101")
report
"Assert 312 : < Y /= 111111111101 >" -- 	Vector No: 172
severity warning;

assert (FULL_BAR = '1')
report
"Assert 313 : < FULL_BAR /= 1 >"
severity warning;

wait for 1 ns;

-- **************************************************
-- ** POP  3 **
--------------------------

clk <= '1'; --	 Cycle No: 375

wait for 1 ns;

I <= "1000";
CCEN_BAR <= '0';
CC_BAR <= '1';
RLD_BAR <= '1';
CI <= '0';
OEbar <= '0';

wait for 4 ns;

clk <= '0';

wait for 4 ns;

assert (Y = "111111111101")
report
"Assert 314 : < Y /= 111111111101 >" -- 	Vector No: 173
severity warning;

assert (FULL_BAR = '1')
report
"Assert 315 : < FULL_BAR /= 1 >"
severity warning;

wait for 1 ns;

--------------------------

clk <= '1'; --	 Cycle No: 376

wait for 1 ns;

I <= "1101";
CCEN_BAR <= '0';
CC_BAR <= '1';
RLD_BAR <= '1';
CI <= '0';
OEbar <= '0';

wait for 4 ns;

clk <= '0';

wait for 4 ns;

assert (Y = "111111111110")
report
"Assert 316 : < Y /= 111111111110 >" -- 	Vector No: 174
severity warning;

assert (FULL_BAR = '1')
report
"Assert 317 : < FULL_BAR /= 1 >"
severity warning;

wait for 1 ns;

-- **************************************************
-- ** POP  4 **
--------------------------

clk <= '1'; --	 Cycle No: 377

wait for 1 ns;

I <= "1000";
CCEN_BAR <= '0';
CC_BAR <= '1';
RLD_BAR <= '1';
CI <= '0';
OEbar <= '0';

wait for 4 ns;

clk <= '0';

wait for 4 ns;

assert (Y = "111111111110")
report
"Assert 318 : < Y /= 111111111110 >" -- 	Vector No: 175
severity warning;

assert (FULL_BAR = '1')
report
"Assert 319 : < FULL_BAR /= 1 >"
severity warning;

wait for 1 ns;

--------------------------

clk <= '1'; --	 Cycle No: 378

wait for 1 ns;

I <= "1101";
CCEN_BAR <= '0';
CC_BAR <= '1';
RLD_BAR <= '1';
CI <= '0';
OEbar <= '0';

wait for 4 ns;

clk <= '0';

wait for 4 ns;

assert (Y = "111111111111")
report
"Assert 320 : < Y /= 111111111111 >" -- 	Vector No: 176
severity warning;

assert (FULL_BAR = '1')
report
"Assert 321 : < FULL_BAR /= 1 >"
severity warning;

wait for 1 ns;

-- **************************************************
-- ** POP  5 **
--------------------------

clk <= '1'; --	 Cycle No: 379

wait for 1 ns;

I <= "1000";
CCEN_BAR <= '0';
CC_BAR <= '1';
RLD_BAR <= '1';
CI <= '0';
OEbar <= '0';

wait for 4 ns;

clk <= '0';

wait for 4 ns;

assert (Y = "111111111111")
report
"Assert 322 : < Y /= 111111111111 >" -- 	Vector No: 177
severity warning;

assert (FULL_BAR = '1')
report
"Assert 323 : < FULL_BAR /= 1 >"
severity warning;

wait for 1 ns;

--------------------------

clk <= '1'; --	 Cycle No: 380

wait for 1 ns;

I <= "1101";
CCEN_BAR <= '0';
CC_BAR <= '1';
RLD_BAR <= '1';
CI <= '0';
OEbar <= '0';

wait for 4 ns;

clk <= '0';

wait for 4 ns;

assert (FULL_BAR = '1')
report
"Assert 324 : < FULL_BAR /= 1 >" -- 	Vector No: 178
severity warning;

wait for 1 ns;

-- **************************************************
-- ** POP  6 (POP EMPTY STACK) **
--------------------------

clk <= '1'; --	 Cycle No: 381

wait for 1 ns;

I <= "1000";
CCEN_BAR <= '0';
CC_BAR <= '1';
RLD_BAR <= '1';
CI <= '0';
OEbar <= '0';

wait for 4 ns;

clk <= '0';

wait for 4 ns;

assert (FULL_BAR = '1')
report
"Assert 325 : < FULL_BAR /= 1 >" -- 	Vector No: 179
severity warning;

wait for 1 ns;

--------------------------

clk <= '1'; --	 Cycle No: 382

wait for 1 ns;

I <= "1101";
CCEN_BAR <= '0';
CC_BAR <= '1';
RLD_BAR <= '1';
CI <= '0';
OEbar <= '0';

wait for 4 ns;

clk <= '0';

wait for 4 ns;

assert (FULL_BAR = '1')
report
"Assert 326 : < FULL_BAR /= 1 >" -- 	Vector No: 180
severity warning;

wait for 1 ns;

-- **************************************************
-- *************************************
-- *	                            *
-- * TEST VECTORS FOR REGISTER COUNTER *
-- *       	         	    *
-- *************************************
-- ******* TEST LOADING OPERATION (INSTRUCTION) ******* 
-- ** I = 4 **
--------------------------

clk <= '1'; --	 Cycle No: 383

wait for 1 ns;

I <= "0000";
CCEN_BAR <= '0';
CC_BAR <= '0';
RLD_BAR <= '0';
CI <= '0';
OEbar <= '0';
D <= "000000000000";

wait for 4 ns;

clk <= '0';

wait for 4 ns;

wait for 1 ns;

--------------------------

clk <= '1'; --	 Cycle No: 384

wait for 1 ns;

I <= "0100";
CCEN_BAR <= '0';
CC_BAR <= '0';
RLD_BAR <= '1';
CI <= '0';
OEbar <= '0';
D <= "000000000000";

wait for 4 ns;

clk <= '0';

wait for 4 ns;

wait for 1 ns;

--------------------------

clk <= '1'; --	 Cycle No: 385

wait for 1 ns;

I <= "0111";
CCEN_BAR <= '0';
CC_BAR <= '1';
RLD_BAR <= '1';
CI <= '0';
OEbar <= '0';

wait for 4 ns;

clk <= '0';

wait for 4 ns;

assert (Y = "000000000000")
report
"Assert 327 : < Y /= 000000000000 >" -- 	Vector No: 181
severity warning;

wait for 1 ns;

-- **************************************************
--------------------------

clk <= '1'; --	 Cycle No: 386

wait for 1 ns;

I <= "0000";
CCEN_BAR <= '0';
CC_BAR <= '0';
RLD_BAR <= '0';
CI <= '0';
OEbar <= '0';
D <= "000000000000";

wait for 4 ns;

clk <= '0';

wait for 4 ns;

wait for 1 ns;

--------------------------

clk <= '1'; --	 Cycle No: 387

wait for 1 ns;

I <= "0100";
CCEN_BAR <= '0';
CC_BAR <= '0';
RLD_BAR <= '1';
CI <= '0';
OEbar <= '0';
D <= "111111111111";

wait for 4 ns;

clk <= '0';

wait for 4 ns;

wait for 1 ns;

--------------------------

clk <= '1'; --	 Cycle No: 388

wait for 1 ns;

I <= "0111";
CCEN_BAR <= '0';
CC_BAR <= '1';
RLD_BAR <= '1';
CI <= '0';
OEbar <= '0';

wait for 4 ns;

clk <= '0';

wait for 4 ns;

assert (Y = "111111111111")
report
"Assert 328 : < Y /= 111111111111 >" -- 	Vector No: 182
severity warning;

wait for 1 ns;

-- **************************************************
--------------------------

clk <= '1'; --	 Cycle No: 389

wait for 1 ns;

I <= "0000";
CCEN_BAR <= '0';
CC_BAR <= '0';
RLD_BAR <= '0';
CI <= '0';
OEbar <= '0';
D <= "111111111111";

wait for 4 ns;

clk <= '0';

wait for 4 ns;

wait for 1 ns;

--------------------------

clk <= '1'; --	 Cycle No: 390

wait for 1 ns;

I <= "0100";
CCEN_BAR <= '0';
CC_BAR <= '0';
RLD_BAR <= '1';
CI <= '0';
OEbar <= '0';
D <= "000000000000";

wait for 4 ns;

clk <= '0';

wait for 4 ns;

wait for 1 ns;

--------------------------

clk <= '1'; --	 Cycle No: 391

wait for 1 ns;

I <= "0111";
CCEN_BAR <= '0';
CC_BAR <= '1';
RLD_BAR <= '1';
CI <= '0';
OEbar <= '0';

wait for 4 ns;

clk <= '0';

wait for 4 ns;

assert (Y = "000000000000")
report
"Assert 329 : < Y /= 000000000000 >" -- 	Vector No: 183
severity warning;

wait for 1 ns;

-- **************************************************
--------------------------

clk <= '1'; --	 Cycle No: 392

wait for 1 ns;

I <= "0000";
CCEN_BAR <= '0';
CC_BAR <= '0';
RLD_BAR <= '0';
CI <= '0';
OEbar <= '0';
D <= "111111111111";

wait for 4 ns;

clk <= '0';

wait for 4 ns;

wait for 1 ns;

--------------------------

clk <= '1'; --	 Cycle No: 393

wait for 1 ns;

I <= "0100";
CCEN_BAR <= '0';
CC_BAR <= '0';
RLD_BAR <= '1';
CI <= '0';
OEbar <= '0';
D <= "111111111111";

wait for 4 ns;

clk <= '0';

wait for 4 ns;

wait for 1 ns;

--------------------------

clk <= '1'; --	 Cycle No: 394

wait for 1 ns;

I <= "0111";
CCEN_BAR <= '0';
CC_BAR <= '1';
RLD_BAR <= '1';
CI <= '0';
OEbar <= '0';

wait for 4 ns;

clk <= '0';

wait for 4 ns;

assert (Y = "111111111111")
report
"Assert 330 : < Y /= 111111111111 >" -- 	Vector No: 184
severity warning;

wait for 1 ns;

-- **************************************************
-- **************************************************
--------------------------

clk <= '1'; --	 Cycle No: 395

wait for 1 ns;

I <= "0000";
CCEN_BAR <= '0';
CC_BAR <= '0';
RLD_BAR <= '0';
CI <= '0';
OEbar <= '0';
D <= "000000000000";

wait for 4 ns;

clk <= '0';

wait for 4 ns;

wait for 1 ns;

--------------------------

clk <= '1'; --	 Cycle No: 396

wait for 1 ns;

I <= "0100";
CCEN_BAR <= '1';
CC_BAR <= '0';
RLD_BAR <= '1';
CI <= '0';
OEbar <= '0';
D <= "000000000000";

wait for 4 ns;

clk <= '0';

wait for 4 ns;

wait for 1 ns;

--------------------------

clk <= '1'; --	 Cycle No: 397

wait for 1 ns;

I <= "0111";
CCEN_BAR <= '0';
CC_BAR <= '1';
RLD_BAR <= '1';
CI <= '0';
OEbar <= '0';

wait for 4 ns;

clk <= '0';

wait for 4 ns;

assert (Y = "000000000000")
report
"Assert 331 : < Y /= 000000000000 >" -- 	Vector No: 185
severity warning;

wait for 1 ns;

-- **************************************************
--------------------------

clk <= '1'; --	 Cycle No: 398

wait for 1 ns;

I <= "0000";
CCEN_BAR <= '0';
CC_BAR <= '0';
RLD_BAR <= '0';
CI <= '0';
OEbar <= '0';
D <= "000000000000";

wait for 4 ns;

clk <= '0';

wait for 4 ns;

wait for 1 ns;

--------------------------

clk <= '1'; --	 Cycle No: 399

wait for 1 ns;

I <= "0100";
CCEN_BAR <= '1';
CC_BAR <= '0';
RLD_BAR <= '1';
CI <= '0';
OEbar <= '0';
D <= "111111111111";

wait for 4 ns;

clk <= '0';

wait for 4 ns;

wait for 1 ns;

--------------------------

clk <= '1'; --	 Cycle No: 400

wait for 1 ns;

I <= "0111";
CCEN_BAR <= '0';
CC_BAR <= '1';
RLD_BAR <= '1';
CI <= '0';
OEbar <= '0';

wait for 4 ns;

clk <= '0';

wait for 4 ns;

assert (Y = "111111111111")
report
"Assert 332 : < Y /= 111111111111 >" -- 	Vector No: 186
severity warning;

wait for 1 ns;

-- **************************************************
--------------------------

clk <= '1'; --	 Cycle No: 401

wait for 1 ns;

I <= "0000";
CCEN_BAR <= '0';
CC_BAR <= '0';
RLD_BAR <= '0';
CI <= '0';
OEbar <= '0';
D <= "111111111111";

wait for 4 ns;

clk <= '0';

wait for 4 ns;

wait for 1 ns;

--------------------------

clk <= '1'; --	 Cycle No: 402

wait for 1 ns;

I <= "0100";
CCEN_BAR <= '1';
CC_BAR <= '0';
RLD_BAR <= '1';
CI <= '0';
OEbar <= '0';
D <= "000000000000";

wait for 4 ns;

clk <= '0';

wait for 4 ns;

wait for 1 ns;

--------------------------

clk <= '1'; --	 Cycle No: 403

wait for 1 ns;

I <= "0111";
CCEN_BAR <= '0';
CC_BAR <= '1';
RLD_BAR <= '1';
CI <= '0';
OEbar <= '0';

wait for 4 ns;

clk <= '0';

wait for 4 ns;

assert (Y = "000000000000")
report
"Assert 333 : < Y /= 000000000000 >" -- 	Vector No: 187
severity warning;

wait for 1 ns;

-- **************************************************
--------------------------

clk <= '1'; --	 Cycle No: 404

wait for 1 ns;

I <= "0000";
CCEN_BAR <= '0';
CC_BAR <= '0';
RLD_BAR <= '0';
CI <= '0';
OEbar <= '0';
D <= "111111111111";

wait for 4 ns;

clk <= '0';

wait for 4 ns;

wait for 1 ns;

--------------------------

clk <= '1'; --	 Cycle No: 405

wait for 1 ns;

I <= "0100";
CCEN_BAR <= '1';
CC_BAR <= '0';
RLD_BAR <= '1';
CI <= '0';
OEbar <= '0';
D <= "111111111111";

wait for 4 ns;

clk <= '0';

wait for 4 ns;

wait for 1 ns;

--------------------------

clk <= '1'; --	 Cycle No: 406

wait for 1 ns;

I <= "0111";
CCEN_BAR <= '0';
CC_BAR <= '1';
RLD_BAR <= '1';
CI <= '0';
OEbar <= '0';

wait for 4 ns;

clk <= '0';

wait for 4 ns;

assert (Y = "111111111111")
report
"Assert 334 : < Y /= 111111111111 >" -- 	Vector No: 188
severity warning;

wait for 1 ns;

-- **************************************************
-- **************************************************
--------------------------

clk <= '1'; --	 Cycle No: 407

wait for 1 ns;

I <= "0000";
CCEN_BAR <= '0';
CC_BAR <= '0';
RLD_BAR <= '0';
CI <= '0';
OEbar <= '0';
D <= "000000000000";

wait for 4 ns;

clk <= '0';

wait for 4 ns;

wait for 1 ns;

--------------------------

clk <= '1'; --	 Cycle No: 408

wait for 1 ns;

I <= "0100";
CCEN_BAR <= '1';
CC_BAR <= '1';
RLD_BAR <= '1';
CI <= '0';
OEbar <= '0';
D <= "000000000000";

wait for 4 ns;

clk <= '0';

wait for 4 ns;

wait for 1 ns;

--------------------------

clk <= '1'; --	 Cycle No: 409

wait for 1 ns;

I <= "0111";
CCEN_BAR <= '0';
CC_BAR <= '1';
RLD_BAR <= '1';
CI <= '0';
OEbar <= '0';

wait for 4 ns;

clk <= '0';

wait for 4 ns;

assert (Y = "000000000000")
report
"Assert 335 : < Y /= 000000000000 >" -- 	Vector No: 189
severity warning;

wait for 1 ns;

-- **************************************************
--------------------------

clk <= '1'; --	 Cycle No: 410

wait for 1 ns;

I <= "0000";
CCEN_BAR <= '0';
CC_BAR <= '0';
RLD_BAR <= '0';
CI <= '0';
OEbar <= '0';
D <= "000000000000";

wait for 4 ns;

clk <= '0';

wait for 4 ns;

wait for 1 ns;

--------------------------

clk <= '1'; --	 Cycle No: 411

wait for 1 ns;

I <= "0100";
CCEN_BAR <= '1';
CC_BAR <= '1';
RLD_BAR <= '1';
CI <= '0';
OEbar <= '0';
D <= "111111111111";

wait for 4 ns;

clk <= '0';

wait for 4 ns;

wait for 1 ns;

--------------------------

clk <= '1'; --	 Cycle No: 412

wait for 1 ns;

I <= "0111";
CCEN_BAR <= '0';
CC_BAR <= '1';
RLD_BAR <= '1';
CI <= '0';
OEbar <= '0';

wait for 4 ns;

clk <= '0';

wait for 4 ns;

assert (Y = "111111111111")
report
"Assert 336 : < Y /= 111111111111 >" -- 	Vector No: 190
severity warning;

wait for 1 ns;

-- **************************************************
--------------------------

clk <= '1'; --	 Cycle No: 413

wait for 1 ns;

I <= "0000";
CCEN_BAR <= '0';
CC_BAR <= '0';
RLD_BAR <= '0';
CI <= '0';
OEbar <= '0';
D <= "111111111111";

wait for 4 ns;

clk <= '0';

wait for 4 ns;

wait for 1 ns;

--------------------------

clk <= '1'; --	 Cycle No: 414

wait for 1 ns;

I <= "0100";
CCEN_BAR <= '1';
CC_BAR <= '1';
RLD_BAR <= '1';
CI <= '0';
OEbar <= '0';
D <= "000000000000";

wait for 4 ns;

clk <= '0';

wait for 4 ns;

wait for 1 ns;

--------------------------

clk <= '1'; --	 Cycle No: 415

wait for 1 ns;

I <= "0111";
CCEN_BAR <= '0';
CC_BAR <= '1';
RLD_BAR <= '1';
CI <= '0';
OEbar <= '0';

wait for 4 ns;

clk <= '0';

wait for 4 ns;

assert (Y = "000000000000")
report
"Assert 337 : < Y /= 000000000000 >" -- 	Vector No: 191
severity warning;

wait for 1 ns;

-- **************************************************
--------------------------

clk <= '1'; --	 Cycle No: 416

wait for 1 ns;

I <= "0000";
CCEN_BAR <= '0';
CC_BAR <= '0';
RLD_BAR <= '0';
CI <= '0';
OEbar <= '0';
D <= "111111111111";

wait for 4 ns;

clk <= '0';

wait for 4 ns;

wait for 1 ns;

--------------------------

clk <= '1'; --	 Cycle No: 417

wait for 1 ns;

I <= "0100";
CCEN_BAR <= '1';
CC_BAR <= '1';
RLD_BAR <= '1';
CI <= '0';
OEbar <= '0';
D <= "111111111111";

wait for 4 ns;

clk <= '0';

wait for 4 ns;

wait for 1 ns;

--------------------------

clk <= '1'; --	 Cycle No: 418

wait for 1 ns;

I <= "0111";
CCEN_BAR <= '0';
CC_BAR <= '1';
RLD_BAR <= '1';
CI <= '0';
OEbar <= '0';

wait for 4 ns;

clk <= '0';

wait for 4 ns;

assert (Y = "111111111111")
report
"Assert 338 : < Y /= 111111111111 >" -- 	Vector No: 192
severity warning;

wait for 1 ns;

-- **************************************************
-- **************************************************
-- ** I = 12 **
--------------------------

clk <= '1'; --	 Cycle No: 419

wait for 1 ns;

I <= "0000";
CCEN_BAR <= '0';
CC_BAR <= '0';
RLD_BAR <= '0';
CI <= '0';
OEbar <= '0';
D <= "000000000000";

wait for 4 ns;

clk <= '0';

wait for 4 ns;

wait for 1 ns;

--------------------------

clk <= '1'; --	 Cycle No: 420

wait for 1 ns;

I <= "1100";
CCEN_BAR <= '0';
CC_BAR <= '0';
RLD_BAR <= '1';
CI <= '0';
OEbar <= '0';
D <= "000000000000";

wait for 4 ns;

clk <= '0';

wait for 4 ns;

wait for 1 ns;

--------------------------

clk <= '1'; --	 Cycle No: 421

wait for 1 ns;

I <= "0111";
CCEN_BAR <= '0';
CC_BAR <= '1';
RLD_BAR <= '1';
CI <= '0';
OEbar <= '0';

wait for 4 ns;

clk <= '0';

wait for 4 ns;

assert (Y = "000000000000")
report
"Assert 339 : < Y /= 000000000000 >" -- 	Vector No: 193
severity warning;

wait for 1 ns;

-- **************************************************
--------------------------

clk <= '1'; --	 Cycle No: 422

wait for 1 ns;

I <= "0000";
CCEN_BAR <= '0';
CC_BAR <= '0';
RLD_BAR <= '0';
CI <= '0';
OEbar <= '0';
D <= "000000000000";

wait for 4 ns;

clk <= '0';

wait for 4 ns;

wait for 1 ns;

--------------------------

clk <= '1'; --	 Cycle No: 423

wait for 1 ns;

I <= "1100";
CCEN_BAR <= '0';
CC_BAR <= '0';
RLD_BAR <= '1';
CI <= '0';
OEbar <= '0';
D <= "111111111111";

wait for 4 ns;

clk <= '0';

wait for 4 ns;

wait for 1 ns;

--------------------------

clk <= '1'; --	 Cycle No: 424

wait for 1 ns;

I <= "0111";
CCEN_BAR <= '0';
CC_BAR <= '1';
RLD_BAR <= '1';
CI <= '0';
OEbar <= '0';

wait for 4 ns;

clk <= '0';

wait for 4 ns;

assert (Y = "111111111111")
report
"Assert 340 : < Y /= 111111111111 >" -- 	Vector No: 194
severity warning;

wait for 1 ns;

-- **************************************************
--------------------------

clk <= '1'; --	 Cycle No: 425

wait for 1 ns;

I <= "0000";
CCEN_BAR <= '0';
CC_BAR <= '0';
RLD_BAR <= '0';
CI <= '0';
OEbar <= '0';
D <= "111111111111";

wait for 4 ns;

clk <= '0';

wait for 4 ns;

wait for 1 ns;

--------------------------

clk <= '1'; --	 Cycle No: 426

wait for 1 ns;

I <= "1100";
CCEN_BAR <= '0';
CC_BAR <= '0';
RLD_BAR <= '1';
CI <= '0';
OEbar <= '0';
D <= "000000000000";

wait for 4 ns;

clk <= '0';

wait for 4 ns;

wait for 1 ns;

--------------------------

clk <= '1'; --	 Cycle No: 427

wait for 1 ns;

I <= "0111";
CCEN_BAR <= '0';
CC_BAR <= '1';
RLD_BAR <= '1';
CI <= '0';
OEbar <= '0';

wait for 4 ns;

clk <= '0';

wait for 4 ns;

assert (Y = "000000000000")
report
"Assert 341 : < Y /= 000000000000 >" -- 	Vector No: 195
severity warning;

wait for 1 ns;

-- **************************************************
--------------------------

clk <= '1'; --	 Cycle No: 428

wait for 1 ns;

I <= "0000";
CCEN_BAR <= '0';
CC_BAR <= '0';
RLD_BAR <= '0';
CI <= '0';
OEbar <= '0';
D <= "111111111111";

wait for 4 ns;

clk <= '0';

wait for 4 ns;

wait for 1 ns;

--------------------------

clk <= '1'; --	 Cycle No: 429

wait for 1 ns;

I <= "1100";
CCEN_BAR <= '0';
CC_BAR <= '0';
RLD_BAR <= '1';
CI <= '0';
OEbar <= '0';
D <= "111111111111";

wait for 4 ns;

clk <= '0';

wait for 4 ns;

wait for 1 ns;

--------------------------

clk <= '1'; --	 Cycle No: 430

wait for 1 ns;

I <= "0111";
CCEN_BAR <= '0';
CC_BAR <= '1';
RLD_BAR <= '1';
CI <= '0';
OEbar <= '0';

wait for 4 ns;

clk <= '0';

wait for 4 ns;

assert (Y = "111111111111")
report
"Assert 342 : < Y /= 111111111111 >" -- 	Vector No: 196
severity warning;

wait for 1 ns;

-- **************************************************
-- **************************************************
--------------------------

clk <= '1'; --	 Cycle No: 431

wait for 1 ns;

I <= "0000";
CCEN_BAR <= '0';
CC_BAR <= '0';
RLD_BAR <= '0';
CI <= '0';
OEbar <= '0';
D <= "000000000000";

wait for 4 ns;

clk <= '0';

wait for 4 ns;

wait for 1 ns;

--------------------------

clk <= '1'; --	 Cycle No: 432

wait for 1 ns;

I <= "1100";
CCEN_BAR <= '0';
CC_BAR <= '1';
RLD_BAR <= '1';
CI <= '0';
OEbar <= '0';
D <= "000000000000";

wait for 4 ns;

clk <= '0';

wait for 4 ns;

wait for 1 ns;

--------------------------

clk <= '1'; --	 Cycle No: 433

wait for 1 ns;

I <= "0111";
CCEN_BAR <= '0';
CC_BAR <= '1';
RLD_BAR <= '1';
CI <= '0';
OEbar <= '0';

wait for 4 ns;

clk <= '0';

wait for 4 ns;

assert (Y = "000000000000")
report
"Assert 343 : < Y /= 000000000000 >" -- 	Vector No: 197
severity warning;

wait for 1 ns;

-- **************************************************
--------------------------

clk <= '1'; --	 Cycle No: 434

wait for 1 ns;

I <= "0000";
CCEN_BAR <= '0';
CC_BAR <= '0';
RLD_BAR <= '0';
CI <= '0';
OEbar <= '0';
D <= "000000000000";

wait for 4 ns;

clk <= '0';

wait for 4 ns;

wait for 1 ns;

--------------------------

clk <= '1'; --	 Cycle No: 435

wait for 1 ns;

I <= "1100";
CCEN_BAR <= '0';
CC_BAR <= '1';
RLD_BAR <= '1';
CI <= '0';
OEbar <= '0';
D <= "111111111111";

wait for 4 ns;

clk <= '0';

wait for 4 ns;

wait for 1 ns;

--------------------------

clk <= '1'; --	 Cycle No: 436

wait for 1 ns;

I <= "0111";
CCEN_BAR <= '0';
CC_BAR <= '1';
RLD_BAR <= '1';
CI <= '0';
OEbar <= '0';

wait for 4 ns;

clk <= '0';

wait for 4 ns;

assert (Y = "111111111111")
report
"Assert 344 : < Y /= 111111111111 >" -- 	Vector No: 198
severity warning;

wait for 1 ns;

-- **************************************************
--------------------------

clk <= '1'; --	 Cycle No: 437

wait for 1 ns;

I <= "0000";
CCEN_BAR <= '0';
CC_BAR <= '0';
RLD_BAR <= '0';
CI <= '0';
OEbar <= '0';
D <= "111111111111";

wait for 4 ns;

clk <= '0';

wait for 4 ns;

wait for 1 ns;

--------------------------

clk <= '1'; --	 Cycle No: 438

wait for 1 ns;

I <= "1100";
CCEN_BAR <= '0';
CC_BAR <= '1';
RLD_BAR <= '1';
CI <= '0';
OEbar <= '0';
D <= "000000000000";

wait for 4 ns;

clk <= '0';

wait for 4 ns;

wait for 1 ns;

--------------------------

clk <= '1'; --	 Cycle No: 439

wait for 1 ns;

I <= "0111";
CCEN_BAR <= '0';
CC_BAR <= '1';
RLD_BAR <= '1';
CI <= '0';
OEbar <= '0';

wait for 4 ns;

clk <= '0';

wait for 4 ns;

assert (Y = "000000000000")
report
"Assert 345 : < Y /= 000000000000 >" -- 	Vector No: 199
severity warning;

wait for 1 ns;

-- **************************************************
--------------------------

clk <= '1'; --	 Cycle No: 440

wait for 1 ns;

I <= "0000";
CCEN_BAR <= '0';
CC_BAR <= '0';
RLD_BAR <= '0';
CI <= '0';
OEbar <= '0';
D <= "111111111111";

wait for 4 ns;

clk <= '0';

wait for 4 ns;

wait for 1 ns;

--------------------------

clk <= '1'; --	 Cycle No: 441

wait for 1 ns;

I <= "1100";
CCEN_BAR <= '0';
CC_BAR <= '1';
RLD_BAR <= '1';
CI <= '0';
OEbar <= '0';
D <= "111111111111";

wait for 4 ns;

clk <= '0';

wait for 4 ns;

wait for 1 ns;

--------------------------

clk <= '1'; --	 Cycle No: 442

wait for 1 ns;

I <= "0111";
CCEN_BAR <= '0';
CC_BAR <= '1';
RLD_BAR <= '1';
CI <= '0';
OEbar <= '0';

wait for 4 ns;

clk <= '0';

wait for 4 ns;

assert (Y = "111111111111")
report
"Assert 346 : < Y /= 111111111111 >" -- 	Vector No: 200
severity warning;

wait for 1 ns;

-- **************************************************
-- **************************************************
--------------------------

clk <= '1'; --	 Cycle No: 443

wait for 1 ns;

I <= "0000";
CCEN_BAR <= '0';
CC_BAR <= '0';
RLD_BAR <= '0';
CI <= '0';
OEbar <= '0';
D <= "000000000000";

wait for 4 ns;

clk <= '0';

wait for 4 ns;

wait for 1 ns;

--------------------------

clk <= '1'; --	 Cycle No: 444

wait for 1 ns;

I <= "1100";
CCEN_BAR <= '1';
CC_BAR <= '0';
RLD_BAR <= '1';
CI <= '0';
OEbar <= '0';
D <= "000000000000";

wait for 4 ns;

clk <= '0';

wait for 4 ns;

wait for 1 ns;

--------------------------

clk <= '1'; --	 Cycle No: 445

wait for 1 ns;

I <= "0111";
CCEN_BAR <= '0';
CC_BAR <= '1';
RLD_BAR <= '1';
CI <= '0';
OEbar <= '0';

wait for 4 ns;

clk <= '0';

wait for 4 ns;

assert (Y = "000000000000")
report
"Assert 347 : < Y /= 000000000000 >" -- 	Vector No: 201
severity warning;

wait for 1 ns;

-- **************************************************
--------------------------

clk <= '1'; --	 Cycle No: 446

wait for 1 ns;

I <= "0000";
CCEN_BAR <= '0';
CC_BAR <= '0';
RLD_BAR <= '0';
CI <= '0';
OEbar <= '0';
D <= "000000000000";

wait for 4 ns;

clk <= '0';

wait for 4 ns;

wait for 1 ns;

--------------------------

clk <= '1'; --	 Cycle No: 447

wait for 1 ns;

I <= "1100";
CCEN_BAR <= '1';
CC_BAR <= '0';
RLD_BAR <= '1';
CI <= '0';
OEbar <= '0';
D <= "111111111111";

wait for 4 ns;

clk <= '0';

wait for 4 ns;

wait for 1 ns;

--------------------------

clk <= '1'; --	 Cycle No: 448

wait for 1 ns;

I <= "0111";
CCEN_BAR <= '0';
CC_BAR <= '1';
RLD_BAR <= '1';
CI <= '0';
OEbar <= '0';

wait for 4 ns;

clk <= '0';

wait for 4 ns;

assert (Y = "111111111111")
report
"Assert 348 : < Y /= 111111111111 >" -- 	Vector No: 202
severity warning;

wait for 1 ns;

-- **************************************************
--------------------------

clk <= '1'; --	 Cycle No: 449

wait for 1 ns;

I <= "0000";
CCEN_BAR <= '0';
CC_BAR <= '0';
RLD_BAR <= '0';
CI <= '0';
OEbar <= '0';
D <= "111111111111";

wait for 4 ns;

clk <= '0';

wait for 4 ns;

wait for 1 ns;

--------------------------

clk <= '1'; --	 Cycle No: 450

wait for 1 ns;

I <= "1100";
CCEN_BAR <= '1';
CC_BAR <= '0';
RLD_BAR <= '1';
CI <= '0';
OEbar <= '0';
D <= "000000000000";

wait for 4 ns;

clk <= '0';

wait for 4 ns;

wait for 1 ns;

--------------------------

clk <= '1'; --	 Cycle No: 451

wait for 1 ns;

I <= "0111";
CCEN_BAR <= '0';
CC_BAR <= '1';
RLD_BAR <= '1';
CI <= '0';
OEbar <= '0';

wait for 4 ns;

clk <= '0';

wait for 4 ns;

assert (Y = "000000000000")
report
"Assert 349 : < Y /= 000000000000 >" -- 	Vector No: 203
severity warning;

wait for 1 ns;

-- **************************************************
--------------------------

clk <= '1'; --	 Cycle No: 452

wait for 1 ns;

I <= "0000";
CCEN_BAR <= '0';
CC_BAR <= '0';
RLD_BAR <= '0';
CI <= '0';
OEbar <= '0';
D <= "111111111111";

wait for 4 ns;

clk <= '0';

wait for 4 ns;

wait for 1 ns;

--------------------------

clk <= '1'; --	 Cycle No: 453

wait for 1 ns;

I <= "1100";
CCEN_BAR <= '1';
CC_BAR <= '0';
RLD_BAR <= '1';
CI <= '0';
OEbar <= '0';
D <= "111111111111";

wait for 4 ns;

clk <= '0';

wait for 4 ns;

wait for 1 ns;

--------------------------

clk <= '1'; --	 Cycle No: 454

wait for 1 ns;

I <= "0111";
CCEN_BAR <= '0';
CC_BAR <= '1';
RLD_BAR <= '1';
CI <= '0';
OEbar <= '0';

wait for 4 ns;

clk <= '0';

wait for 4 ns;

assert (Y = "111111111111")
report
"Assert 350 : < Y /= 111111111111 >" -- 	Vector No: 204
severity warning;

wait for 1 ns;

-- **************************************************
-- **************************************************
--------------------------

clk <= '1'; --	 Cycle No: 455

wait for 1 ns;

I <= "0000";
CCEN_BAR <= '0';
CC_BAR <= '0';
RLD_BAR <= '0';
CI <= '0';
OEbar <= '0';
D <= "000000000000";

wait for 4 ns;

clk <= '0';

wait for 4 ns;

wait for 1 ns;

--------------------------

clk <= '1'; --	 Cycle No: 456

wait for 1 ns;

I <= "1100";
CCEN_BAR <= '1';
CC_BAR <= '1';
RLD_BAR <= '1';
CI <= '0';
OEbar <= '0';
D <= "000000000000";

wait for 4 ns;

clk <= '0';

wait for 4 ns;

wait for 1 ns;

--------------------------

clk <= '1'; --	 Cycle No: 457

wait for 1 ns;

I <= "0111";
CCEN_BAR <= '0';
CC_BAR <= '1';
RLD_BAR <= '1';
CI <= '0';
OEbar <= '0';

wait for 4 ns;

clk <= '0';

wait for 4 ns;

assert (Y = "000000000000")
report
"Assert 351 : < Y /= 000000000000 >" -- 	Vector No: 205
severity warning;

wait for 1 ns;

-- **************************************************
--------------------------

clk <= '1'; --	 Cycle No: 458

wait for 1 ns;

I <= "0000";
CCEN_BAR <= '0';
CC_BAR <= '0';
RLD_BAR <= '0';
CI <= '0';
OEbar <= '0';
D <= "000000000000";

wait for 4 ns;

clk <= '0';

wait for 4 ns;

wait for 1 ns;

--------------------------

clk <= '1'; --	 Cycle No: 459

wait for 1 ns;

I <= "1100";
CCEN_BAR <= '1';
CC_BAR <= '1';
RLD_BAR <= '1';
CI <= '0';
OEbar <= '0';
D <= "111111111111";

wait for 4 ns;

clk <= '0';

wait for 4 ns;

wait for 1 ns;

--------------------------

clk <= '1'; --	 Cycle No: 460

wait for 1 ns;

I <= "0111";
CCEN_BAR <= '0';
CC_BAR <= '1';
RLD_BAR <= '1';
CI <= '0';
OEbar <= '0';

wait for 4 ns;

clk <= '0';

wait for 4 ns;

assert (Y = "111111111111")
report
"Assert 352 : < Y /= 111111111111 >" -- 	Vector No: 206
severity warning;

wait for 1 ns;

-- **************************************************
--------------------------

clk <= '1'; --	 Cycle No: 461

wait for 1 ns;

I <= "0000";
CCEN_BAR <= '0';
CC_BAR <= '0';
RLD_BAR <= '0';
CI <= '0';
OEbar <= '0';
D <= "111111111111";

wait for 4 ns;

clk <= '0';

wait for 4 ns;

wait for 1 ns;

--------------------------

clk <= '1'; --	 Cycle No: 462

wait for 1 ns;

I <= "1100";
CCEN_BAR <= '1';
CC_BAR <= '1';
RLD_BAR <= '1';
CI <= '0';
OEbar <= '0';
D <= "000000000000";

wait for 4 ns;

clk <= '0';

wait for 4 ns;

wait for 1 ns;

--------------------------

clk <= '1'; --	 Cycle No: 463

wait for 1 ns;

I <= "0111";
CCEN_BAR <= '0';
CC_BAR <= '1';
RLD_BAR <= '1';
CI <= '0';
OEbar <= '0';

wait for 4 ns;

clk <= '0';

wait for 4 ns;

assert (Y = "000000000000")
report
"Assert 353 : < Y /= 000000000000 >" -- 	Vector No: 207
severity warning;

wait for 1 ns;

-- **************************************************
--------------------------

clk <= '1'; --	 Cycle No: 464

wait for 1 ns;

I <= "0000";
CCEN_BAR <= '0';
CC_BAR <= '0';
RLD_BAR <= '0';
CI <= '0';
OEbar <= '0';
D <= "111111111111";

wait for 4 ns;

clk <= '0';

wait for 4 ns;

wait for 1 ns;

--------------------------

clk <= '1'; --	 Cycle No: 465

wait for 1 ns;

I <= "1100";
CCEN_BAR <= '1';
CC_BAR <= '1';
RLD_BAR <= '1';
CI <= '0';
OEbar <= '0';
D <= "111111111111";

wait for 4 ns;

clk <= '0';

wait for 4 ns;

wait for 1 ns;

--------------------------

clk <= '1'; --	 Cycle No: 466

wait for 1 ns;

I <= "0111";
CCEN_BAR <= '0';
CC_BAR <= '1';
RLD_BAR <= '1';
CI <= '0';
OEbar <= '0';

wait for 4 ns;

clk <= '0';

wait for 4 ns;

assert (Y = "111111111111")
report
"Assert 354 : < Y /= 111111111111 >" -- 	Vector No: 208
severity warning;

wait for 1 ns;

-- **************************************************
-- **************************************************
-- ******* TEST LOADING OPERATION (RLD_BAR) ******* 
--------------------------

clk <= '1'; --	 Cycle No: 467

wait for 1 ns;

I <= "0000";
CCEN_BAR <= '0';
CC_BAR <= '0';
RLD_BAR <= '0';
CI <= '0';
OEbar <= '0';
D <= "000000000000";

wait for 4 ns;

clk <= '0';

wait for 4 ns;

wait for 1 ns;

--------------------------

clk <= '1'; --	 Cycle No: 468

wait for 1 ns;

I <= "0111";
CCEN_BAR <= '0';
CC_BAR <= '1';
RLD_BAR <= '1';
CI <= '0';
OEbar <= '0';

wait for 4 ns;

clk <= '0';

wait for 4 ns;

assert (Y = "000000000000")
report
"Assert 355 : < Y /= 000000000000 >" -- 	Vector No: 209
severity warning;

wait for 1 ns;

-- **************************************************
--------------------------

clk <= '1'; --	 Cycle No: 469

wait for 1 ns;

I <= "0000";
CCEN_BAR <= '0';
CC_BAR <= '0';
RLD_BAR <= '0';
CI <= '0';
OEbar <= '0';
D <= "111111111111";

wait for 4 ns;

clk <= '0';

wait for 4 ns;

wait for 1 ns;

--------------------------

clk <= '1'; --	 Cycle No: 470

wait for 1 ns;

I <= "0111";
CCEN_BAR <= '0';
CC_BAR <= '1';
RLD_BAR <= '1';
CI <= '0';
OEbar <= '0';

wait for 4 ns;

clk <= '0';

wait for 4 ns;

assert (Y = "111111111111")
report
"Assert 356 : < Y /= 111111111111 >" -- 	Vector No: 210
severity warning;

wait for 1 ns;

-- **************************************************
-- ******* TEST DECREMENT OPERATION ********
-- ** I = 8 **
--------------------------

clk <= '1'; --	 Cycle No: 471

wait for 1 ns;

I <= "0000";
CCEN_BAR <= '0';
CC_BAR <= '0';
RLD_BAR <= '0';
CI <= '0';
OEbar <= '0';
D <= "000000000001";

wait for 4 ns;

clk <= '0';

wait for 4 ns;

wait for 1 ns;

--------------------------

clk <= '1'; --	 Cycle No: 472

wait for 1 ns;

I <= "1000";
CCEN_BAR <= '0';
CC_BAR <= '1';
RLD_BAR <= '1';
CI <= '0';
OEbar <= '0';

wait for 4 ns;

clk <= '0';

wait for 4 ns;

wait for 1 ns;

--------------------------

clk <= '1'; --	 Cycle No: 473

wait for 1 ns;

I <= "0111";
CCEN_BAR <= '0';
CC_BAR <= '1';
RLD_BAR <= '1';
CI <= '0';
OEbar <= '0';

wait for 4 ns;

clk <= '0';

wait for 4 ns;

assert (Y = "000000000000")
report
"Assert 357 : < Y /= 000000000000 >" -- 	Vector No: 211
severity warning;

wait for 1 ns;

-- **************************************************
--------------------------

clk <= '1'; --	 Cycle No: 474

wait for 1 ns;

I <= "0000";
CCEN_BAR <= '0';
CC_BAR <= '0';
RLD_BAR <= '0';
CI <= '0';
OEbar <= '0';
D <= "000000000001";

wait for 4 ns;

clk <= '0';

wait for 4 ns;

wait for 1 ns;

--------------------------

clk <= '1'; --	 Cycle No: 475

wait for 1 ns;

I <= "1000";
CCEN_BAR <= '0';
CC_BAR <= '0';
RLD_BAR <= '1';
CI <= '0';
OEbar <= '0';

wait for 4 ns;

clk <= '0';

wait for 4 ns;

wait for 1 ns;

--------------------------

clk <= '1'; --	 Cycle No: 476

wait for 1 ns;

I <= "0111";
CCEN_BAR <= '0';
CC_BAR <= '1';
RLD_BAR <= '1';
CI <= '0';
OEbar <= '0';

wait for 4 ns;

clk <= '0';

wait for 4 ns;

assert (Y = "000000000000")
report
"Assert 358 : < Y /= 000000000000 >" -- 	Vector No: 212
severity warning;

wait for 1 ns;

-- **************************************************
--------------------------

clk <= '1'; --	 Cycle No: 477

wait for 1 ns;

I <= "0000";
CCEN_BAR <= '0';
CC_BAR <= '0';
RLD_BAR <= '0';
CI <= '0';
OEbar <= '0';
D <= "000000000001";

wait for 4 ns;

clk <= '0';

wait for 4 ns;

wait for 1 ns;

--------------------------

clk <= '1'; --	 Cycle No: 478

wait for 1 ns;

I <= "1000";
CCEN_BAR <= '1';
CC_BAR <= '1';
RLD_BAR <= '1';
CI <= '0';
OEbar <= '0';

wait for 4 ns;

clk <= '0';

wait for 4 ns;

wait for 1 ns;

--------------------------

clk <= '1'; --	 Cycle No: 479

wait for 1 ns;

I <= "0111";
CCEN_BAR <= '0';
CC_BAR <= '1';
RLD_BAR <= '1';
CI <= '0';
OEbar <= '0';

wait for 4 ns;

clk <= '0';

wait for 4 ns;

assert (Y = "000000000000")
report
"Assert 359 : < Y /= 000000000000 >" -- 	Vector No: 213
severity warning;

wait for 1 ns;

-- **************************************************
--------------------------

clk <= '1'; --	 Cycle No: 480

wait for 1 ns;

I <= "0000";
CCEN_BAR <= '0';
CC_BAR <= '0';
RLD_BAR <= '0';
CI <= '0';
OEbar <= '0';
D <= "000000000001";

wait for 4 ns;

clk <= '0';

wait for 4 ns;

wait for 1 ns;

--------------------------

clk <= '1'; --	 Cycle No: 481

wait for 1 ns;

I <= "1000";
CCEN_BAR <= '1';
CC_BAR <= '0';
RLD_BAR <= '1';
CI <= '0';
OEbar <= '0';

wait for 4 ns;

clk <= '0';

wait for 4 ns;

wait for 1 ns;

--------------------------

clk <= '1'; --	 Cycle No: 482

wait for 1 ns;

I <= "0111";
CCEN_BAR <= '0';
CC_BAR <= '1';
RLD_BAR <= '1';
CI <= '0';
OEbar <= '0';

wait for 4 ns;

clk <= '0';

wait for 4 ns;

assert (Y = "000000000000")
report
"Assert 360 : < Y /= 000000000000 >" -- 	Vector No: 214
severity warning;

wait for 1 ns;

-- **************************************************
-- ** I = 9 **
--------------------------

clk <= '1'; --	 Cycle No: 483

wait for 1 ns;

I <= "0000";
CCEN_BAR <= '0';
CC_BAR <= '0';
RLD_BAR <= '0';
CI <= '0';
OEbar <= '0';
D <= "000000000001";

wait for 4 ns;

clk <= '0';

wait for 4 ns;

wait for 1 ns;

--------------------------

clk <= '1'; --	 Cycle No: 484

wait for 1 ns;

I <= "1001";
CCEN_BAR <= '0';
CC_BAR <= '1';
RLD_BAR <= '1';
CI <= '0';
OEbar <= '0';

wait for 4 ns;

clk <= '0';

wait for 4 ns;

wait for 1 ns;

--------------------------

clk <= '1'; --	 Cycle No: 485

wait for 1 ns;

I <= "0111";
CCEN_BAR <= '0';
CC_BAR <= '1';
RLD_BAR <= '1';
CI <= '0';
OEbar <= '0';

wait for 4 ns;

clk <= '0';

wait for 4 ns;

assert (Y = "000000000000")
report
"Assert 361 : < Y /= 000000000000 >" -- 	Vector No: 215
severity warning;

wait for 1 ns;

-- **************************************************
--------------------------

clk <= '1'; --	 Cycle No: 486

wait for 1 ns;

I <= "0000";
CCEN_BAR <= '0';
CC_BAR <= '0';
RLD_BAR <= '0';
CI <= '0';
OEbar <= '0';
D <= "000000000001";

wait for 4 ns;

clk <= '0';

wait for 4 ns;

wait for 1 ns;

--------------------------

clk <= '1'; --	 Cycle No: 487

wait for 1 ns;

I <= "1001";
CCEN_BAR <= '0';
CC_BAR <= '0';
RLD_BAR <= '1';
CI <= '0';
OEbar <= '0';

wait for 4 ns;

clk <= '0';

wait for 4 ns;

wait for 1 ns;

--------------------------

clk <= '1'; --	 Cycle No: 488

wait for 1 ns;

I <= "0111";
CCEN_BAR <= '0';
CC_BAR <= '1';
RLD_BAR <= '1';
CI <= '0';
OEbar <= '0';

wait for 4 ns;

clk <= '0';

wait for 4 ns;

assert (Y = "000000000000")
report
"Assert 362 : < Y /= 000000000000 >" -- 	Vector No: 216
severity warning;

wait for 1 ns;

-- **************************************************
--------------------------

clk <= '1'; --	 Cycle No: 489

wait for 1 ns;

I <= "0000";
CCEN_BAR <= '0';
CC_BAR <= '0';
RLD_BAR <= '0';
CI <= '0';
OEbar <= '0';
D <= "000000000001";

wait for 4 ns;

clk <= '0';

wait for 4 ns;

wait for 1 ns;

--------------------------

clk <= '1'; --	 Cycle No: 490

wait for 1 ns;

I <= "1001";
CCEN_BAR <= '1';
CC_BAR <= '1';
RLD_BAR <= '1';
CI <= '0';
OEbar <= '0';

wait for 4 ns;

clk <= '0';

wait for 4 ns;

wait for 1 ns;

--------------------------

clk <= '1'; --	 Cycle No: 491

wait for 1 ns;

I <= "0111";
CCEN_BAR <= '0';
CC_BAR <= '1';
RLD_BAR <= '1';
CI <= '0';
OEbar <= '0';

wait for 4 ns;

clk <= '0';

wait for 4 ns;

assert (Y = "000000000000")
report
"Assert 363 : < Y /= 000000000000 >" -- 	Vector No: 217
severity warning;

wait for 1 ns;

-- **************************************************
--------------------------

clk <= '1'; --	 Cycle No: 492

wait for 1 ns;

I <= "0000";
CCEN_BAR <= '0';
CC_BAR <= '0';
RLD_BAR <= '0';
CI <= '0';
OEbar <= '0';
D <= "000000000001";

wait for 4 ns;

clk <= '0';

wait for 4 ns;

wait for 1 ns;

--------------------------

clk <= '1'; --	 Cycle No: 493

wait for 1 ns;

I <= "1001";
CCEN_BAR <= '1';
CC_BAR <= '0';
RLD_BAR <= '1';
CI <= '0';
OEbar <= '0';

wait for 4 ns;

clk <= '0';

wait for 4 ns;

wait for 1 ns;

--------------------------

clk <= '1'; --	 Cycle No: 494

wait for 1 ns;

I <= "0111";
CCEN_BAR <= '0';
CC_BAR <= '1';
RLD_BAR <= '1';
CI <= '0';
OEbar <= '0';

wait for 4 ns;

clk <= '0';

wait for 4 ns;

assert (Y = "000000000000")
report
"Assert 364 : < Y /= 000000000000 >" -- 	Vector No: 218
severity warning;

wait for 1 ns;

-- **************************************************
-- ** I = 15 **
--------------------------

clk <= '1'; --	 Cycle No: 495

wait for 1 ns;

I <= "0000";
CCEN_BAR <= '0';
CC_BAR <= '0';
RLD_BAR <= '0';
CI <= '0';
OEbar <= '0';
D <= "000000000001";

wait for 4 ns;

clk <= '0';

wait for 4 ns;

wait for 1 ns;

--------------------------

clk <= '1'; --	 Cycle No: 496

wait for 1 ns;

I <= "1111";
CCEN_BAR <= '0';
CC_BAR <= '1';
RLD_BAR <= '1';
CI <= '0';
OEbar <= '0';

wait for 4 ns;

clk <= '0';

wait for 4 ns;

wait for 1 ns;

--------------------------

clk <= '1'; --	 Cycle No: 497

wait for 1 ns;

I <= "0111";
CCEN_BAR <= '0';
CC_BAR <= '1';
RLD_BAR <= '1';
CI <= '0';
OEbar <= '0';

wait for 4 ns;

clk <= '0';

wait for 4 ns;

assert (Y = "000000000000")
report
"Assert 365 : < Y /= 000000000000 >" -- 	Vector No: 219
severity warning;

wait for 1 ns;

-- **************************************************
--------------------------

clk <= '1'; --	 Cycle No: 498

wait for 1 ns;

I <= "0000";
CCEN_BAR <= '0';
CC_BAR <= '0';
RLD_BAR <= '0';
CI <= '0';
OEbar <= '0';
D <= "000000000001";

wait for 4 ns;

clk <= '0';

wait for 4 ns;

wait for 1 ns;

--------------------------

clk <= '1'; --	 Cycle No: 499

wait for 1 ns;

I <= "1111";
CCEN_BAR <= '0';
CC_BAR <= '0';
RLD_BAR <= '1';
CI <= '0';
OEbar <= '0';

wait for 4 ns;

clk <= '0';

wait for 4 ns;

wait for 1 ns;

--------------------------

clk <= '1'; --	 Cycle No: 500

wait for 1 ns;

I <= "0111";
CCEN_BAR <= '0';
CC_BAR <= '1';
RLD_BAR <= '1';
CI <= '0';
OEbar <= '0';

wait for 4 ns;

clk <= '0';

wait for 4 ns;

assert (Y = "000000000000")
report
"Assert 366 : < Y /= 000000000000 >" -- 	Vector No: 220
severity warning;

wait for 1 ns;

-- **************************************************
--------------------------

clk <= '1'; --	 Cycle No: 501

wait for 1 ns;

I <= "0000";
CCEN_BAR <= '0';
CC_BAR <= '0';
RLD_BAR <= '0';
CI <= '0';
OEbar <= '0';
D <= "000000000001";

wait for 4 ns;

clk <= '0';

wait for 4 ns;

wait for 1 ns;

--------------------------

clk <= '1'; --	 Cycle No: 502

wait for 1 ns;

I <= "1111";
CCEN_BAR <= '1';
CC_BAR <= '1';
RLD_BAR <= '1';
CI <= '0';
OEbar <= '0';

wait for 4 ns;

clk <= '0';

wait for 4 ns;

wait for 1 ns;

--------------------------

clk <= '1'; --	 Cycle No: 503

wait for 1 ns;

I <= "0111";
CCEN_BAR <= '0';
CC_BAR <= '1';
RLD_BAR <= '1';
CI <= '0';
OEbar <= '0';

wait for 4 ns;

clk <= '0';

wait for 4 ns;

assert (Y = "000000000000")
report
"Assert 367 : < Y /= 000000000000 >" -- 	Vector No: 221
severity warning;

wait for 1 ns;

-- **************************************************
--------------------------

clk <= '1'; --	 Cycle No: 504

wait for 1 ns;

I <= "0000";
CCEN_BAR <= '0';
CC_BAR <= '0';
RLD_BAR <= '0';
CI <= '0';
OEbar <= '0';
D <= "000000000001";

wait for 4 ns;

clk <= '0';

wait for 4 ns;

wait for 1 ns;

--------------------------

clk <= '1'; --	 Cycle No: 505

wait for 1 ns;

I <= "1111";
CCEN_BAR <= '1';
CC_BAR <= '0';
RLD_BAR <= '1';
CI <= '0';
OEbar <= '0';

wait for 4 ns;

clk <= '0';

wait for 4 ns;

wait for 1 ns;

--------------------------

clk <= '1'; --	 Cycle No: 506

wait for 1 ns;

I <= "0111";
CCEN_BAR <= '0';
CC_BAR <= '1';
RLD_BAR <= '1';
CI <= '0';
OEbar <= '0';

wait for 4 ns;

clk <= '0';

wait for 4 ns;

assert (Y = "000000000000")
report
"Assert 368 : < Y /= 000000000000 >" -- 	Vector No: 222
severity warning;

wait for 1 ns;

-- **************************************************
--------------------------

clk <= '1'; --	 Cycle No: 507

wait for 1 ns;

wait for 1 ns;

--------------------------

clk <= '1'; --	 Cycle No: 508

wait for 1 ns;

wait for 1 ns;

--------------------------

clk <= '1'; --	 Cycle No: 509

wait for 1 ns;

wait for 1 ns;

--------------------------

clk <= '1'; --	 Cycle No: 510

wait for 1 ns;

wait for 1 ns;

--------------------------

clk <= '1'; --	 Cycle No: 511

wait for 1 ns;

wait for 1 ns;

--------------------------

clk <= '1'; --	 Cycle No: 512

wait for 1 ns;

wait for 1 ns;

--------------------------

clk <= '1'; --	 Cycle No: 513

wait for 1 ns;

wait for 1 ns;

--------------------------

clk <= '1'; --	 Cycle No: 514

wait for 1 ns;

wait for 1 ns;

--------------------------

clk <= '1'; --	 Cycle No: 515

wait for 1 ns;

wait for 1 ns;

-- ******* TEST DECREMENT OPERATION BORROW RIPPLE ******* 
--------------------------

clk <= '1'; --	 Cycle No: 516

wait for 1 ns;

I <= "1110";
CCEN_BAR <= '1';
CC_BAR <= '1';
RLD_BAR <= '0';
CI <= '0';
OEbar <= '0';
D <= "111111111111";

wait for 4 ns;

clk <= '0';

wait for 4 ns;

wait for 1 ns;

--------------------------

clk <= '1'; --	 Cycle No: 517

wait for 1 ns;

I <= "1000";
CCEN_BAR <= '0';
CC_BAR <= '1';
RLD_BAR <= '1';
CI <= '0';
OEbar <= '0';

wait for 4 ns;

clk <= '0';

wait for 4 ns;

wait for 1 ns;

--------------------------

clk <= '1'; --	 Cycle No: 518

wait for 1 ns;

I <= "0111";
CCEN_BAR <= '0';
CC_BAR <= '1';
RLD_BAR <= '1';
CI <= '0';
OEbar <= '0';

wait for 4 ns;

clk <= '0';

wait for 4 ns;

assert (Y = "111111111110")
report
"Assert 369 : < Y /= 111111111110 >" -- 	Vector No: 223
severity warning;

wait for 1 ns;

-- **************************************************
--------------------------

clk <= '1'; --	 Cycle No: 519

wait for 1 ns;

I <= "1110";
CCEN_BAR <= '1';
CC_BAR <= '1';
RLD_BAR <= '0';
CI <= '0';
OEbar <= '0';
D <= "111111111110";

wait for 4 ns;

clk <= '0';

wait for 4 ns;

wait for 1 ns;

--------------------------

clk <= '1'; --	 Cycle No: 520

wait for 1 ns;

I <= "1000";
CCEN_BAR <= '0';
CC_BAR <= '1';
RLD_BAR <= '1';
CI <= '0';
OEbar <= '0';

wait for 4 ns;

clk <= '0';

wait for 4 ns;

wait for 1 ns;

--------------------------

clk <= '1'; --	 Cycle No: 521

wait for 1 ns;

I <= "0111";
CCEN_BAR <= '0';
CC_BAR <= '1';
RLD_BAR <= '1';
CI <= '0';
OEbar <= '0';

wait for 4 ns;

clk <= '0';

wait for 4 ns;

assert (Y = "111111111101")
report
"Assert 370 : < Y /= 111111111101 >" -- 	Vector No: 224
severity warning;

wait for 1 ns;

-- **************************************************
--------------------------

clk <= '1'; --	 Cycle No: 522

wait for 1 ns;

I <= "1110";
CCEN_BAR <= '1';
CC_BAR <= '1';
RLD_BAR <= '0';
CI <= '0';
OEbar <= '0';
D <= "111111111100";

wait for 4 ns;

clk <= '0';

wait for 4 ns;

wait for 1 ns;

--------------------------

clk <= '1'; --	 Cycle No: 523

wait for 1 ns;

I <= "1000";
CCEN_BAR <= '0';
CC_BAR <= '1';
RLD_BAR <= '1';
CI <= '0';
OEbar <= '0';

wait for 4 ns;

clk <= '0';

wait for 4 ns;

wait for 1 ns;

--------------------------

clk <= '1'; --	 Cycle No: 524

wait for 1 ns;

I <= "0111";
CCEN_BAR <= '0';
CC_BAR <= '1';
RLD_BAR <= '1';
CI <= '0';
OEbar <= '0';

wait for 4 ns;

clk <= '0';

wait for 4 ns;

assert (Y = "111111111011")
report
"Assert 371 : < Y /= 111111111011 >" -- 	Vector No: 225
severity warning;

wait for 1 ns;

-- **************************************************
--------------------------

clk <= '1'; --	 Cycle No: 525

wait for 1 ns;

I <= "1110";
CCEN_BAR <= '1';
CC_BAR <= '1';
RLD_BAR <= '0';
CI <= '0';
OEbar <= '0';
D <= "111111111000";

wait for 4 ns;

clk <= '0';

wait for 4 ns;

wait for 1 ns;

--------------------------

clk <= '1'; --	 Cycle No: 526

wait for 1 ns;

I <= "1000";
CCEN_BAR <= '0';
CC_BAR <= '1';
RLD_BAR <= '1';
CI <= '0';
OEbar <= '0';

wait for 4 ns;

clk <= '0';

wait for 4 ns;

wait for 1 ns;

--------------------------

clk <= '1'; --	 Cycle No: 527

wait for 1 ns;

I <= "0111";
CCEN_BAR <= '0';
CC_BAR <= '1';
RLD_BAR <= '1';
CI <= '0';
OEbar <= '0';

wait for 4 ns;

clk <= '0';

wait for 4 ns;

assert (Y = "111111110111")
report
"Assert 372 : < Y /= 111111110111 >" -- 	Vector No: 226
severity warning;

wait for 1 ns;

-- **************************************************
--------------------------

clk <= '1'; --	 Cycle No: 528

wait for 1 ns;

I <= "1110";
CCEN_BAR <= '1';
CC_BAR <= '1';
RLD_BAR <= '0';
CI <= '0';
OEbar <= '0';
D <= "111111110000";

wait for 4 ns;

clk <= '0';

wait for 4 ns;

wait for 1 ns;

--------------------------

clk <= '1'; --	 Cycle No: 529

wait for 1 ns;

I <= "1000";
CCEN_BAR <= '0';
CC_BAR <= '1';
RLD_BAR <= '1';
CI <= '0';
OEbar <= '0';

wait for 4 ns;

clk <= '0';

wait for 4 ns;

wait for 1 ns;

--------------------------

clk <= '1'; --	 Cycle No: 530

wait for 1 ns;

I <= "0111";
CCEN_BAR <= '0';
CC_BAR <= '1';
RLD_BAR <= '1';
CI <= '0';
OEbar <= '0';

wait for 4 ns;

clk <= '0';

wait for 4 ns;

assert (Y = "111111101111")
report
"Assert 373 : < Y /= 111111101111 >" -- 	Vector No: 227
severity warning;

wait for 1 ns;

-- **************************************************
--------------------------

clk <= '1'; --	 Cycle No: 531

wait for 1 ns;

I <= "1110";
CCEN_BAR <= '1';
CC_BAR <= '1';
RLD_BAR <= '0';
CI <= '0';
OEbar <= '0';
D <= "111111100000";

wait for 4 ns;

clk <= '0';

wait for 4 ns;

wait for 1 ns;

--------------------------

clk <= '1'; --	 Cycle No: 532

wait for 1 ns;

I <= "1000";
CCEN_BAR <= '0';
CC_BAR <= '1';
RLD_BAR <= '1';
CI <= '0';
OEbar <= '0';

wait for 4 ns;

clk <= '0';

wait for 4 ns;

wait for 1 ns;

--------------------------

clk <= '1'; --	 Cycle No: 533

wait for 1 ns;

I <= "0111";
CCEN_BAR <= '0';
CC_BAR <= '1';
RLD_BAR <= '1';
CI <= '0';
OEbar <= '0';

wait for 4 ns;

clk <= '0';

wait for 4 ns;

assert (Y = "111111011111")
report
"Assert 374 : < Y /= 111111011111 >" -- 	Vector No: 228
severity warning;

wait for 1 ns;

-- **************************************************
--------------------------

clk <= '1'; --	 Cycle No: 534

wait for 1 ns;

I <= "1110";
CCEN_BAR <= '1';
CC_BAR <= '1';
RLD_BAR <= '0';
CI <= '0';
OEbar <= '0';
D <= "111111000000";

wait for 4 ns;

clk <= '0';

wait for 4 ns;

wait for 1 ns;

--------------------------

clk <= '1'; --	 Cycle No: 535

wait for 1 ns;

I <= "1000";
CCEN_BAR <= '0';
CC_BAR <= '1';
RLD_BAR <= '1';
CI <= '0';
OEbar <= '0';

wait for 4 ns;

clk <= '0';

wait for 4 ns;

wait for 1 ns;

--------------------------

clk <= '1'; --	 Cycle No: 536

wait for 1 ns;

I <= "0111";
CCEN_BAR <= '0';
CC_BAR <= '1';
RLD_BAR <= '1';
CI <= '0';
OEbar <= '0';

wait for 4 ns;

clk <= '0';

wait for 4 ns;

assert (Y = "111110111111")
report
"Assert 375 : < Y /= 111110111111 >" -- 	Vector No: 229
severity warning;

wait for 1 ns;

-- **************************************************
--------------------------

clk <= '1'; --	 Cycle No: 537

wait for 1 ns;

I <= "1110";
CCEN_BAR <= '1';
CC_BAR <= '1';
RLD_BAR <= '0';
CI <= '0';
OEbar <= '0';
D <= "111110000000";

wait for 4 ns;

clk <= '0';

wait for 4 ns;

wait for 1 ns;

--------------------------

clk <= '1'; --	 Cycle No: 538

wait for 1 ns;

I <= "1000";
CCEN_BAR <= '0';
CC_BAR <= '1';
RLD_BAR <= '1';
CI <= '0';
OEbar <= '0';

wait for 4 ns;

clk <= '0';

wait for 4 ns;

wait for 1 ns;

--------------------------

clk <= '1'; --	 Cycle No: 539

wait for 1 ns;

I <= "0111";
CCEN_BAR <= '0';
CC_BAR <= '1';
RLD_BAR <= '1';
CI <= '0';
OEbar <= '0';

wait for 4 ns;

clk <= '0';

wait for 4 ns;

assert (Y = "111101111111")
report
"Assert 376 : < Y /= 111101111111 >" -- 	Vector No: 230
severity warning;

wait for 1 ns;

-- **************************************************
--------------------------

clk <= '1'; --	 Cycle No: 540

wait for 1 ns;

I <= "1110";
CCEN_BAR <= '1';
CC_BAR <= '1';
RLD_BAR <= '0';
CI <= '0';
OEbar <= '0';
D <= "111100000000";

wait for 4 ns;

clk <= '0';

wait for 4 ns;

wait for 1 ns;

--------------------------

clk <= '1'; --	 Cycle No: 541

wait for 1 ns;

I <= "1000";
CCEN_BAR <= '0';
CC_BAR <= '1';
RLD_BAR <= '1';
CI <= '0';
OEbar <= '0';

wait for 4 ns;

clk <= '0';

wait for 4 ns;

wait for 1 ns;

--------------------------

clk <= '1'; --	 Cycle No: 542

wait for 1 ns;

I <= "0111";
CCEN_BAR <= '0';
CC_BAR <= '1';
RLD_BAR <= '1';
CI <= '0';
OEbar <= '0';

wait for 4 ns;

clk <= '0';

wait for 4 ns;

assert (Y = "111011111111")
report
"Assert 377 : < Y /= 111011111111 >" -- 	Vector No: 231
severity warning;

wait for 1 ns;

-- **************************************************
--------------------------

clk <= '1'; --	 Cycle No: 543

wait for 1 ns;

I <= "1110";
CCEN_BAR <= '1';
CC_BAR <= '1';
RLD_BAR <= '0';
CI <= '0';
OEbar <= '0';
D <= "111000000000";

wait for 4 ns;

clk <= '0';

wait for 4 ns;

wait for 1 ns;

--------------------------

clk <= '1'; --	 Cycle No: 544

wait for 1 ns;

I <= "1000";
CCEN_BAR <= '0';
CC_BAR <= '1';
RLD_BAR <= '1';
CI <= '0';
OEbar <= '0';

wait for 4 ns;

clk <= '0';

wait for 4 ns;

wait for 1 ns;

--------------------------

clk <= '1'; --	 Cycle No: 545

wait for 1 ns;

I <= "0111";
CCEN_BAR <= '0';
CC_BAR <= '1';
RLD_BAR <= '1';
CI <= '0';
OEbar <= '0';

wait for 4 ns;

clk <= '0';

wait for 4 ns;

assert (Y = "110111111111")
report
"Assert 378 : < Y /= 110111111111 >" -- 	Vector No: 232
severity warning;

wait for 1 ns;

-- **************************************************
--------------------------

clk <= '1'; --	 Cycle No: 546

wait for 1 ns;

I <= "1110";
CCEN_BAR <= '1';
CC_BAR <= '1';
RLD_BAR <= '0';
CI <= '0';
OEbar <= '0';
D <= "110000000000";

wait for 4 ns;

clk <= '0';

wait for 4 ns;

wait for 1 ns;

--------------------------

clk <= '1'; --	 Cycle No: 547

wait for 1 ns;

I <= "1000";
CCEN_BAR <= '0';
CC_BAR <= '1';
RLD_BAR <= '1';
CI <= '0';
OEbar <= '0';

wait for 4 ns;

clk <= '0';

wait for 4 ns;

wait for 1 ns;

--------------------------

clk <= '1'; --	 Cycle No: 548

wait for 1 ns;

I <= "0111";
CCEN_BAR <= '0';
CC_BAR <= '1';
RLD_BAR <= '1';
CI <= '0';
OEbar <= '0';

wait for 4 ns;

clk <= '0';

wait for 4 ns;

assert (Y = "101111111111")
report
"Assert 379 : < Y /= 101111111111 >" -- 	Vector No: 233
severity warning;

wait for 1 ns;

-- **************************************************
--------------------------

clk <= '1'; --	 Cycle No: 549

wait for 1 ns;

I <= "1110";
CCEN_BAR <= '1';
CC_BAR <= '1';
RLD_BAR <= '0';
CI <= '0';
OEbar <= '0';
D <= "100000000000";

wait for 4 ns;

clk <= '0';

wait for 4 ns;

wait for 1 ns;

--------------------------

clk <= '1'; --	 Cycle No: 550

wait for 1 ns;

I <= "1000";
CCEN_BAR <= '0';
CC_BAR <= '1';
RLD_BAR <= '1';
CI <= '0';
OEbar <= '0';

wait for 4 ns;

clk <= '0';

wait for 4 ns;

wait for 1 ns;

--------------------------

clk <= '1'; --	 Cycle No: 551

wait for 1 ns;

I <= "0111";
CCEN_BAR <= '0';
CC_BAR <= '1';
RLD_BAR <= '1';
CI <= '0';
OEbar <= '0';

wait for 4 ns;

clk <= '0';

wait for 4 ns;

assert (Y = "011111111111")
report
"Assert 380 : < Y /= 011111111111 >" -- 	Vector No: 234
severity warning;

wait for 1 ns;

-- **************************************************
-- *****************************************
-- *	                                *
-- * TEST VECTORS FOR MICROPROGRAM COUNTER *
-- *       	         	        *
-- *****************************************
-- ******* TEST CLEARING OPERATION ********
--------------------------

clk <= '1'; --	 Cycle No: 552

wait for 1 ns;

I <= "0000";
CCEN_BAR <= '0';
CC_BAR <= '0';
RLD_BAR <= '1';
CI <= '0';
OEbar <= '0';
D <= "111111111111";

wait for 4 ns;

clk <= '0';

wait for 4 ns;

wait for 1 ns;

--------------------------

clk <= '1'; --	 Cycle No: 553

wait for 1 ns;

I <= "0001";
CCEN_BAR <= '0';
CC_BAR <= '1';
RLD_BAR <= '1';
CI <= '0';
OEbar <= '0';

wait for 4 ns;

clk <= '0';

wait for 4 ns;

assert (Y = "000000000000")
report
"Assert 381 : < Y /= 000000000000 >" -- 	Vector No: 235
severity warning;

wait for 1 ns;

-- **************************************************
--------------------------

clk <= '1'; --	 Cycle No: 554

wait for 1 ns;

I <= "0000";
CCEN_BAR <= '0';
CC_BAR <= '0';
RLD_BAR <= '1';
CI <= '0';
OEbar <= '0';
D <= "000000000000";

wait for 4 ns;

clk <= '0';

wait for 4 ns;

wait for 1 ns;

--------------------------

clk <= '1'; --	 Cycle No: 555

wait for 1 ns;

I <= "0001";
CCEN_BAR <= '0';
CC_BAR <= '1';
RLD_BAR <= '1';
CI <= '0';
OEbar <= '0';

wait for 4 ns;

clk <= '0';

wait for 4 ns;

assert (Y = "000000000000")
report
"Assert 382 : < Y /= 000000000000 >" -- 	Vector No: 236
severity warning;

wait for 1 ns;

-- **************************************************
-- ******* TEST LOADING OPERATION ********
--------------------------

clk <= '1'; --	 Cycle No: 556

wait for 1 ns;

I <= "0010";
CCEN_BAR <= '0';
CC_BAR <= '0';
RLD_BAR <= '1';
CI <= '0';
OEbar <= '0';
D <= "111111111111";

wait for 4 ns;

clk <= '0';

wait for 4 ns;

wait for 1 ns;

--------------------------

clk <= '1'; --	 Cycle No: 557

wait for 1 ns;

I <= "0001";
CCEN_BAR <= '0';
CC_BAR <= '1';
RLD_BAR <= '1';
CI <= '0';
OEbar <= '0';

wait for 4 ns;

clk <= '0';

wait for 4 ns;

assert (Y = "111111111111")
report
"Assert 383 : < Y /= 111111111111 >" -- 	Vector No: 237
severity warning;

wait for 1 ns;

-- **************************************************
--------------------------

clk <= '1'; --	 Cycle No: 558

wait for 1 ns;

I <= "0010";
CCEN_BAR <= '0';
CC_BAR <= '0';
RLD_BAR <= '1';
CI <= '0';
OEbar <= '0';
D <= "000000000000";

wait for 4 ns;

clk <= '0';

wait for 4 ns;

wait for 1 ns;

--------------------------

clk <= '1'; --	 Cycle No: 559

wait for 1 ns;

I <= "0001";
CCEN_BAR <= '0';
CC_BAR <= '1';
RLD_BAR <= '1';
CI <= '0';
OEbar <= '0';

wait for 4 ns;

clk <= '0';

wait for 4 ns;

assert (Y = "000000000000")
report
"Assert 384 : < Y /= 000000000000 >" -- 	Vector No: 238
severity warning;

wait for 1 ns;

-- **************************************************
-- ******* TEST INCREMENT CARRY RIPPLE ********
--------------------------

clk <= '1'; --	 Cycle No: 560

wait for 1 ns;

I <= "0010";
CCEN_BAR <= '0';
CC_BAR <= '0';
RLD_BAR <= '1';
CI <= '1';
OEbar <= '0';
D <= "000000000000";

wait for 4 ns;

clk <= '0';

wait for 4 ns;

wait for 1 ns;

--------------------------

clk <= '1'; --	 Cycle No: 561

wait for 1 ns;

I <= "0001";
CCEN_BAR <= '0';
CC_BAR <= '1';
RLD_BAR <= '1';
CI <= '0';
OEbar <= '0';

wait for 4 ns;

clk <= '0';

wait for 4 ns;

assert (Y = "000000000001")
report
"Assert 385 : < Y /= 000000000001 >" -- 	Vector No: 239
severity warning;

wait for 1 ns;

-- **************************************************
--------------------------

clk <= '1'; --	 Cycle No: 562

wait for 1 ns;

I <= "0010";
CCEN_BAR <= '0';
CC_BAR <= '0';
RLD_BAR <= '1';
CI <= '1';
OEbar <= '0';
D <= "000000000001";

wait for 4 ns;

clk <= '0';

wait for 4 ns;

wait for 1 ns;

--------------------------

clk <= '1'; --	 Cycle No: 563

wait for 1 ns;

I <= "0001";
CCEN_BAR <= '0';
CC_BAR <= '1';
RLD_BAR <= '1';
CI <= '0';
OEbar <= '0';

wait for 4 ns;

clk <= '0';

wait for 4 ns;

assert (Y = "000000000010")
report
"Assert 386 : < Y /= 000000000010 >" -- 	Vector No: 240
severity warning;

wait for 1 ns;

-- **************************************************
--------------------------

clk <= '1'; --	 Cycle No: 564

wait for 1 ns;

I <= "0010";
CCEN_BAR <= '0';
CC_BAR <= '0';
RLD_BAR <= '1';
CI <= '1';
OEbar <= '0';
D <= "000000000011";

wait for 4 ns;

clk <= '0';

wait for 4 ns;

wait for 1 ns;

--------------------------

clk <= '1'; --	 Cycle No: 565

wait for 1 ns;

I <= "0001";
CCEN_BAR <= '0';
CC_BAR <= '1';
RLD_BAR <= '1';
CI <= '0';
OEbar <= '0';

wait for 4 ns;

clk <= '0';

wait for 4 ns;

assert (Y = "000000000100")
report
"Assert 387 : < Y /= 000000000100 >" -- 	Vector No: 241
severity warning;

wait for 1 ns;

-- **************************************************
--------------------------

clk <= '1'; --	 Cycle No: 566

wait for 1 ns;

I <= "0010";
CCEN_BAR <= '0';
CC_BAR <= '0';
RLD_BAR <= '1';
CI <= '1';
OEbar <= '0';
D <= "000000000111";

wait for 4 ns;

clk <= '0';

wait for 4 ns;

wait for 1 ns;

--------------------------

clk <= '1'; --	 Cycle No: 567

wait for 1 ns;

I <= "0001";
CCEN_BAR <= '0';
CC_BAR <= '1';
RLD_BAR <= '1';
CI <= '0';
OEbar <= '0';

wait for 4 ns;

clk <= '0';

wait for 4 ns;

assert (Y = "000000001000")
report
"Assert 388 : < Y /= 000000001000 >" -- 	Vector No: 242
severity warning;

wait for 1 ns;

-- **************************************************
--------------------------

clk <= '1'; --	 Cycle No: 568

wait for 1 ns;

I <= "0010";
CCEN_BAR <= '0';
CC_BAR <= '0';
RLD_BAR <= '1';
CI <= '1';
OEbar <= '0';
D <= "000000001111";

wait for 4 ns;

clk <= '0';

wait for 4 ns;

wait for 1 ns;

--------------------------

clk <= '1'; --	 Cycle No: 569

wait for 1 ns;

I <= "0001";
CCEN_BAR <= '0';
CC_BAR <= '1';
RLD_BAR <= '1';
CI <= '0';
OEbar <= '0';

wait for 4 ns;

clk <= '0';

wait for 4 ns;

assert (Y = "000000010000")
report
"Assert 389 : < Y /= 000000010000 >" -- 	Vector No: 243
severity warning;

wait for 1 ns;

-- **************************************************
--------------------------

clk <= '1'; --	 Cycle No: 570

wait for 1 ns;

I <= "0010";
CCEN_BAR <= '0';
CC_BAR <= '0';
RLD_BAR <= '1';
CI <= '1';
OEbar <= '0';
D <= "000000011111";

wait for 4 ns;

clk <= '0';

wait for 4 ns;

wait for 1 ns;

--------------------------

clk <= '1'; --	 Cycle No: 571

wait for 1 ns;

I <= "0001";
CCEN_BAR <= '0';
CC_BAR <= '1';
RLD_BAR <= '1';
CI <= '0';
OEbar <= '0';

wait for 4 ns;

clk <= '0';

wait for 4 ns;

assert (Y = "000000100000")
report
"Assert 390 : < Y /= 000000100000 >" -- 	Vector No: 244
severity warning;

wait for 1 ns;

-- **************************************************
--------------------------

clk <= '1'; --	 Cycle No: 572

wait for 1 ns;

I <= "0010";
CCEN_BAR <= '0';
CC_BAR <= '0';
RLD_BAR <= '1';
CI <= '1';
OEbar <= '0';
D <= "000000111111";

wait for 4 ns;

clk <= '0';

wait for 4 ns;

wait for 1 ns;

--------------------------

clk <= '1'; --	 Cycle No: 573

wait for 1 ns;

I <= "0001";
CCEN_BAR <= '0';
CC_BAR <= '1';
RLD_BAR <= '1';
CI <= '0';
OEbar <= '0';

wait for 4 ns;

clk <= '0';

wait for 4 ns;

assert (Y = "000001000000")
report
"Assert 391 : < Y /= 000001000000 >" -- 	Vector No: 245
severity warning;

wait for 1 ns;

-- **************************************************
--------------------------

clk <= '1'; --	 Cycle No: 574

wait for 1 ns;

I <= "0010";
CCEN_BAR <= '0';
CC_BAR <= '0';
RLD_BAR <= '1';
CI <= '1';
OEbar <= '0';
D <= "000001111111";

wait for 4 ns;

clk <= '0';

wait for 4 ns;

wait for 1 ns;

--------------------------

clk <= '1'; --	 Cycle No: 575

wait for 1 ns;

I <= "0001";
CCEN_BAR <= '0';
CC_BAR <= '1';
RLD_BAR <= '1';
CI <= '0';
OEbar <= '0';

wait for 4 ns;

clk <= '0';

wait for 4 ns;

assert (Y = "000010000000")
report
"Assert 392 : < Y /= 000010000000 >" -- 	Vector No: 246
severity warning;

wait for 1 ns;

-- **************************************************
--------------------------

clk <= '1'; --	 Cycle No: 576

wait for 1 ns;

I <= "0010";
CCEN_BAR <= '0';
CC_BAR <= '0';
RLD_BAR <= '1';
CI <= '1';
OEbar <= '0';
D <= "000011111111";

wait for 4 ns;

clk <= '0';

wait for 4 ns;

wait for 1 ns;

--------------------------

clk <= '1'; --	 Cycle No: 577

wait for 1 ns;

I <= "0001";
CCEN_BAR <= '0';
CC_BAR <= '1';
RLD_BAR <= '1';
CI <= '0';
OEbar <= '0';

wait for 4 ns;

clk <= '0';

wait for 4 ns;

assert (Y = "000100000000")
report
"Assert 393 : < Y /= 000100000000 >" -- 	Vector No: 247
severity warning;

wait for 1 ns;

-- **************************************************
--------------------------

clk <= '1'; --	 Cycle No: 578

wait for 1 ns;

I <= "0010";
CCEN_BAR <= '0';
CC_BAR <= '0';
RLD_BAR <= '1';
CI <= '1';
OEbar <= '0';
D <= "000111111111";

wait for 4 ns;

clk <= '0';

wait for 4 ns;

wait for 1 ns;

--------------------------

clk <= '1'; --	 Cycle No: 579

wait for 1 ns;

I <= "0001";
CCEN_BAR <= '0';
CC_BAR <= '1';
RLD_BAR <= '1';
CI <= '0';
OEbar <= '0';

wait for 4 ns;

clk <= '0';

wait for 4 ns;

assert (Y = "001000000000")
report
"Assert 394 : < Y /= 001000000000 >" -- 	Vector No: 248
severity warning;

wait for 1 ns;

-- **************************************************
--------------------------

clk <= '1'; --	 Cycle No: 580

wait for 1 ns;

I <= "0010";
CCEN_BAR <= '0';
CC_BAR <= '0';
RLD_BAR <= '1';
CI <= '1';
OEbar <= '0';
D <= "001111111111";

wait for 4 ns;

clk <= '0';

wait for 4 ns;

wait for 1 ns;

--------------------------

clk <= '1'; --	 Cycle No: 581

wait for 1 ns;

I <= "0001";
CCEN_BAR <= '0';
CC_BAR <= '1';
RLD_BAR <= '1';
CI <= '0';
OEbar <= '0';

wait for 4 ns;

clk <= '0';

wait for 4 ns;

assert (Y = "010000000000")
report
"Assert 395 : < Y /= 010000000000 >" -- 	Vector No: 249
severity warning;

wait for 1 ns;

-- **************************************************
--------------------------

clk <= '1'; --	 Cycle No: 582

wait for 1 ns;

I <= "0010";
CCEN_BAR <= '0';
CC_BAR <= '0';
RLD_BAR <= '1';
CI <= '1';
OEbar <= '0';
D <= "011111111111";

wait for 4 ns;

clk <= '0';

wait for 4 ns;

wait for 1 ns;

--------------------------

clk <= '1'; --	 Cycle No: 583

wait for 1 ns;

I <= "0001";
CCEN_BAR <= '0';
CC_BAR <= '1';
RLD_BAR <= '1';
CI <= '0';
OEbar <= '0';

wait for 4 ns;

clk <= '0';

wait for 4 ns;

assert (Y = "100000000000")
report
"Assert 396 : < Y /= 100000000000 >" -- 	Vector No: 250
severity warning;

wait for 1 ns;

-- **************************************************
--------------------------

clk <= '1'; --	 Cycle No: 584

wait for 1 ns;

I <= "0010";
CCEN_BAR <= '0';
CC_BAR <= '0';
RLD_BAR <= '1';
CI <= '1';
OEbar <= '0';
D <= "111111111111";

wait for 4 ns;

clk <= '0';

wait for 4 ns;

wait for 1 ns;

--------------------------

clk <= '1'; --	 Cycle No: 585

wait for 1 ns;

I <= "0001";
CCEN_BAR <= '0';
CC_BAR <= '1';
RLD_BAR <= '1';
CI <= '0';
OEbar <= '0';

wait for 4 ns;

clk <= '0';

wait for 4 ns;

assert (Y = "000000000000")
report
"Assert 397 : < Y /= 000000000000 >" -- 	Vector No: 251
severity warning;

wait for 1 ns;

-- **************************************************
-- ********************************
-- *	                       *
-- * TEST VECTORS FOR MULTIPLEXER *
-- *              	               *
-- ********************************
-- *************************
-- ******* SELECT 0 ********
-- *************************
-- ** I = 0 **
--------------------------

clk <= '1'; --	 Cycle No: 586

wait for 1 ns;

I <= "0000";
CCEN_BAR <= '0';
CC_BAR <= '0';
RLD_BAR <= '0';
CI <= '0';
OEbar <= '0';
D <= "000000000000";

wait for 4 ns;

clk <= '0';

wait for 4 ns;

wait for 1 ns;

--------------------------

clk <= '1'; --	 Cycle No: 587

wait for 1 ns;

I <= "0000";
CCEN_BAR <= '0';
CC_BAR <= '0';
RLD_BAR <= '1';
CI <= '0';
OEbar <= '0';
D <= "000000000000";

wait for 4 ns;

clk <= '0';

wait for 4 ns;

assert (Y = "000000000000")
report
"Assert 398 : < Y /= 000000000000 >" -- 	Vector No: 252
severity warning;

wait for 1 ns;

-- **************************************************
--------------------------

clk <= '1'; --	 Cycle No: 588

wait for 1 ns;

I <= "0000";
CCEN_BAR <= '0';
CC_BAR <= '0';
RLD_BAR <= '0';
CI <= '0';
OEbar <= '0';
D <= "000000000000";

wait for 4 ns;

clk <= '0';

wait for 4 ns;

wait for 1 ns;

--------------------------

clk <= '1'; --	 Cycle No: 589

wait for 1 ns;

I <= "0000";
CCEN_BAR <= '0';
CC_BAR <= '0';
RLD_BAR <= '1';
CI <= '0';
OEbar <= '0';
D <= "111111111111";

wait for 4 ns;

clk <= '0';

wait for 4 ns;

assert (Y = "000000000000")
report
"Assert 399 : < Y /= 000000000000 >" -- 	Vector No: 253
severity warning;

wait for 1 ns;

-- **************************************************
--------------------------

clk <= '1'; --	 Cycle No: 590

wait for 1 ns;

I <= "0000";
CCEN_BAR <= '0';
CC_BAR <= '0';
RLD_BAR <= '0';
CI <= '0';
OEbar <= '0';
D <= "111111111111";

wait for 4 ns;

clk <= '0';

wait for 4 ns;

wait for 1 ns;

--------------------------

clk <= '1'; --	 Cycle No: 591

wait for 1 ns;

I <= "0000";
CCEN_BAR <= '0';
CC_BAR <= '0';
RLD_BAR <= '1';
CI <= '0';
OEbar <= '0';
D <= "000000000000";

wait for 4 ns;

clk <= '0';

wait for 4 ns;

assert (Y = "000000000000")
report
"Assert 400 : < Y /= 000000000000 >" -- 	Vector No: 254
severity warning;

wait for 1 ns;

-- **************************************************
--------------------------

clk <= '1'; --	 Cycle No: 592

wait for 1 ns;

I <= "0000";
CCEN_BAR <= '0';
CC_BAR <= '0';
RLD_BAR <= '0';
CI <= '0';
OEbar <= '0';
D <= "111111111111";

wait for 4 ns;

clk <= '0';

wait for 4 ns;

wait for 1 ns;

--------------------------

clk <= '1'; --	 Cycle No: 593

wait for 1 ns;

I <= "0000";
CCEN_BAR <= '0';
CC_BAR <= '0';
RLD_BAR <= '1';
CI <= '0';
OEbar <= '0';
D <= "111111111111";

wait for 4 ns;

clk <= '0';

wait for 4 ns;

assert (Y = "000000000000")
report
"Assert 401 : < Y /= 000000000000 >" -- 	Vector No: 255
severity warning;

wait for 1 ns;

-- **************************************************
-- **************************************************
--------------------------

clk <= '1'; --	 Cycle No: 594

wait for 1 ns;

I <= "0000";
CCEN_BAR <= '0';
CC_BAR <= '0';
RLD_BAR <= '0';
CI <= '0';
OEbar <= '0';
D <= "000000000000";

wait for 4 ns;

clk <= '0';

wait for 4 ns;

wait for 1 ns;

--------------------------

clk <= '1'; --	 Cycle No: 595

wait for 1 ns;

I <= "0000";
CCEN_BAR <= '0';
CC_BAR <= '1';
RLD_BAR <= '1';
CI <= '0';
OEbar <= '0';
D <= "000000000000";

wait for 4 ns;

clk <= '0';

wait for 4 ns;

assert (Y = "000000000000")
report
"Assert 402 : < Y /= 000000000000 >" -- 	Vector No: 256
severity warning;

wait for 1 ns;

-- **************************************************
--------------------------

clk <= '1'; --	 Cycle No: 596

wait for 1 ns;

I <= "0000";
CCEN_BAR <= '0';
CC_BAR <= '0';
RLD_BAR <= '0';
CI <= '0';
OEbar <= '0';
D <= "000000000000";

wait for 4 ns;

clk <= '0';

wait for 4 ns;

wait for 1 ns;

--------------------------

clk <= '1'; --	 Cycle No: 597

wait for 1 ns;

I <= "0000";
CCEN_BAR <= '0';
CC_BAR <= '1';
RLD_BAR <= '1';
CI <= '0';
OEbar <= '0';
D <= "111111111111";

wait for 4 ns;

clk <= '0';

wait for 4 ns;

assert (Y = "000000000000")
report
"Assert 403 : < Y /= 000000000000 >" -- 	Vector No: 257
severity warning;

wait for 1 ns;

-- **************************************************
--------------------------

clk <= '1'; --	 Cycle No: 598

wait for 1 ns;

I <= "0000";
CCEN_BAR <= '0';
CC_BAR <= '0';
RLD_BAR <= '0';
CI <= '0';
OEbar <= '0';
D <= "111111111111";

wait for 4 ns;

clk <= '0';

wait for 4 ns;

wait for 1 ns;

--------------------------

clk <= '1'; --	 Cycle No: 599

wait for 1 ns;

I <= "0000";
CCEN_BAR <= '0';
CC_BAR <= '1';
RLD_BAR <= '1';
CI <= '0';
OEbar <= '0';
D <= "000000000000";

wait for 4 ns;

clk <= '0';

wait for 4 ns;

assert (Y = "000000000000")
report
"Assert 404 : < Y /= 000000000000 >" -- 	Vector No: 258
severity warning;

wait for 1 ns;

-- **************************************************
--------------------------

clk <= '1'; --	 Cycle No: 600

wait for 1 ns;

I <= "0000";
CCEN_BAR <= '0';
CC_BAR <= '0';
RLD_BAR <= '0';
CI <= '0';
OEbar <= '0';
D <= "111111111111";

wait for 4 ns;

clk <= '0';

wait for 4 ns;

wait for 1 ns;

--------------------------

clk <= '1'; --	 Cycle No: 601

wait for 1 ns;

I <= "0000";
CCEN_BAR <= '0';
CC_BAR <= '1';
RLD_BAR <= '1';
CI <= '0';
OEbar <= '0';
D <= "111111111111";

wait for 4 ns;

clk <= '0';

wait for 4 ns;

assert (Y = "000000000000")
report
"Assert 405 : < Y /= 000000000000 >" -- 	Vector No: 259
severity warning;

wait for 1 ns;

-- **************************************************
-- **************************************************
--------------------------

clk <= '1'; --	 Cycle No: 602

wait for 1 ns;

I <= "0000";
CCEN_BAR <= '0';
CC_BAR <= '0';
RLD_BAR <= '0';
CI <= '0';
OEbar <= '0';
D <= "000000000000";

wait for 4 ns;

clk <= '0';

wait for 4 ns;

wait for 1 ns;

--------------------------

clk <= '1'; --	 Cycle No: 603

wait for 1 ns;

I <= "0000";
CCEN_BAR <= '1';
CC_BAR <= '1';
RLD_BAR <= '1';
CI <= '0';
OEbar <= '0';
D <= "000000000000";

wait for 4 ns;

clk <= '0';

wait for 4 ns;

assert (Y = "000000000000")
report
"Assert 406 : < Y /= 000000000000 >" -- 	Vector No: 260
severity warning;

wait for 1 ns;

-- **************************************************
--------------------------

clk <= '1'; --	 Cycle No: 604

wait for 1 ns;

I <= "0000";
CCEN_BAR <= '0';
CC_BAR <= '0';
RLD_BAR <= '0';
CI <= '0';
OEbar <= '0';
D <= "000000000000";

wait for 4 ns;

clk <= '0';

wait for 4 ns;

wait for 1 ns;

--------------------------

clk <= '1'; --	 Cycle No: 605

wait for 1 ns;

I <= "0000";
CCEN_BAR <= '1';
CC_BAR <= '1';
RLD_BAR <= '1';
CI <= '0';
OEbar <= '0';
D <= "111111111111";

wait for 4 ns;

clk <= '0';

wait for 4 ns;

assert (Y = "000000000000")
report
"Assert 407 : < Y /= 000000000000 >" -- 	Vector No: 261
severity warning;

wait for 1 ns;

-- **************************************************
--------------------------

clk <= '1'; --	 Cycle No: 606

wait for 1 ns;

I <= "0000";
CCEN_BAR <= '0';
CC_BAR <= '0';
RLD_BAR <= '0';
CI <= '0';
OEbar <= '0';
D <= "111111111111";

wait for 4 ns;

clk <= '0';

wait for 4 ns;

wait for 1 ns;

--------------------------

clk <= '1'; --	 Cycle No: 607

wait for 1 ns;

I <= "0000";
CCEN_BAR <= '1';
CC_BAR <= '1';
RLD_BAR <= '1';
CI <= '0';
OEbar <= '0';
D <= "000000000000";

wait for 4 ns;

clk <= '0';

wait for 4 ns;

assert (Y = "000000000000")
report
"Assert 408 : < Y /= 000000000000 >" -- 	Vector No: 262
severity warning;

wait for 1 ns;

-- **************************************************
--------------------------

clk <= '1'; --	 Cycle No: 608

wait for 1 ns;

I <= "0000";
CCEN_BAR <= '0';
CC_BAR <= '0';
RLD_BAR <= '0';
CI <= '0';
OEbar <= '0';
D <= "111111111111";

wait for 4 ns;

clk <= '0';

wait for 4 ns;

wait for 1 ns;

--------------------------

clk <= '1'; --	 Cycle No: 609

wait for 1 ns;

I <= "0000";
CCEN_BAR <= '1';
CC_BAR <= '1';
RLD_BAR <= '1';
CI <= '0';
OEbar <= '0';
D <= "111111111111";

wait for 4 ns;

clk <= '0';

wait for 4 ns;

assert (Y = "000000000000")
report
"Assert 409 : < Y /= 000000000000 >" -- 	Vector No: 263
severity warning;

wait for 1 ns;

-- **************************************************
-- **************************************************
--------------------------

clk <= '1'; --	 Cycle No: 610

wait for 1 ns;

I <= "0000";
CCEN_BAR <= '0';
CC_BAR <= '0';
RLD_BAR <= '0';
CI <= '0';
OEbar <= '0';
D <= "000000000000";

wait for 4 ns;

clk <= '0';

wait for 4 ns;

wait for 1 ns;

--------------------------

clk <= '1'; --	 Cycle No: 611

wait for 1 ns;

I <= "0000";
CCEN_BAR <= '1';
CC_BAR <= '0';
RLD_BAR <= '1';
CI <= '0';
OEbar <= '0';
D <= "000000000000";

wait for 4 ns;

clk <= '0';

wait for 4 ns;

assert (Y = "000000000000")
report
"Assert 410 : < Y /= 000000000000 >" -- 	Vector No: 264
severity warning;

wait for 1 ns;

-- **************************************************
--------------------------

clk <= '1'; --	 Cycle No: 612

wait for 1 ns;

I <= "0000";
CCEN_BAR <= '0';
CC_BAR <= '0';
RLD_BAR <= '0';
CI <= '0';
OEbar <= '0';
D <= "000000000000";

wait for 4 ns;

clk <= '0';

wait for 4 ns;

wait for 1 ns;

--------------------------

clk <= '1'; --	 Cycle No: 613

wait for 1 ns;

I <= "0000";
CCEN_BAR <= '1';
CC_BAR <= '0';
RLD_BAR <= '1';
CI <= '0';
OEbar <= '0';
D <= "111111111111";

wait for 4 ns;

clk <= '0';

wait for 4 ns;

assert (Y = "000000000000")
report
"Assert 411 : < Y /= 000000000000 >" -- 	Vector No: 265
severity warning;

wait for 1 ns;

-- **************************************************
--------------------------

clk <= '1'; --	 Cycle No: 614

wait for 1 ns;

I <= "0000";
CCEN_BAR <= '0';
CC_BAR <= '0';
RLD_BAR <= '0';
CI <= '0';
OEbar <= '0';
D <= "111111111111";

wait for 4 ns;

clk <= '0';

wait for 4 ns;

wait for 1 ns;

--------------------------

clk <= '1'; --	 Cycle No: 615

wait for 1 ns;

I <= "0000";
CCEN_BAR <= '1';
CC_BAR <= '0';
RLD_BAR <= '1';
CI <= '0';
OEbar <= '0';
D <= "000000000000";

wait for 4 ns;

clk <= '0';

wait for 4 ns;

assert (Y = "000000000000")
report
"Assert 412 : < Y /= 000000000000 >" -- 	Vector No: 266
severity warning;

wait for 1 ns;

-- **************************************************
--------------------------

clk <= '1'; --	 Cycle No: 616

wait for 1 ns;

I <= "0000";
CCEN_BAR <= '0';
CC_BAR <= '0';
RLD_BAR <= '0';
CI <= '0';
OEbar <= '0';
D <= "111111111111";

wait for 4 ns;

clk <= '0';

wait for 4 ns;

wait for 1 ns;

--------------------------

clk <= '1'; --	 Cycle No: 617

wait for 1 ns;

I <= "0000";
CCEN_BAR <= '1';
CC_BAR <= '0';
RLD_BAR <= '1';
CI <= '0';
OEbar <= '0';
D <= "111111111111";

wait for 4 ns;

clk <= '0';

wait for 4 ns;

assert (Y = "000000000000")
report
"Assert 413 : < Y /= 000000000000 >" -- 	Vector No: 267
severity warning;

wait for 1 ns;

-- **************************************************
-- **************************************************
-- *************************
-- ******* SELECT D ********
-- *************************
-- ** I = 1 **
--------------------------

clk <= '1'; --	 Cycle No: 618

wait for 1 ns;

I <= "0000";
CCEN_BAR <= '0';
CC_BAR <= '0';
RLD_BAR <= '0';
CI <= '0';
OEbar <= '0';
D <= "000000000000";

wait for 4 ns;

clk <= '0';

wait for 4 ns;

wait for 1 ns;

--------------------------

clk <= '1'; --	 Cycle No: 619

wait for 1 ns;

I <= "0001";
CCEN_BAR <= '0';
CC_BAR <= '0';
RLD_BAR <= '1';
CI <= '0';
OEbar <= '0';
D <= "000000000000";

wait for 4 ns;

clk <= '0';

wait for 4 ns;

assert (Y = "000000000000")
report
"Assert 414 : < Y /= 000000000000 >" -- 	Vector No: 268
severity warning;

wait for 1 ns;

-- **************************************************
--------------------------

clk <= '1'; --	 Cycle No: 620

wait for 1 ns;

I <= "0000";
CCEN_BAR <= '0';
CC_BAR <= '0';
RLD_BAR <= '0';
CI <= '0';
OEbar <= '0';
D <= "000000000000";

wait for 4 ns;

clk <= '0';

wait for 4 ns;

wait for 1 ns;

--------------------------

clk <= '1'; --	 Cycle No: 621

wait for 1 ns;

I <= "0001";
CCEN_BAR <= '0';
CC_BAR <= '0';
RLD_BAR <= '1';
CI <= '0';
OEbar <= '0';
D <= "111111111111";

wait for 4 ns;

clk <= '0';

wait for 4 ns;

assert (Y = "111111111111")
report
"Assert 415 : < Y /= 111111111111 >" -- 	Vector No: 269
severity warning;

wait for 1 ns;

-- **************************************************
--------------------------

clk <= '1'; --	 Cycle No: 622

wait for 1 ns;

I <= "0000";
CCEN_BAR <= '0';
CC_BAR <= '0';
RLD_BAR <= '0';
CI <= '0';
OEbar <= '0';
D <= "111111111111";

wait for 4 ns;

clk <= '0';

wait for 4 ns;

wait for 1 ns;

--------------------------

clk <= '1'; --	 Cycle No: 623

wait for 1 ns;

I <= "0001";
CCEN_BAR <= '0';
CC_BAR <= '0';
RLD_BAR <= '1';
CI <= '0';
OEbar <= '0';
D <= "000000000000";

wait for 4 ns;

clk <= '0';

wait for 4 ns;

assert (Y = "000000000000")
report
"Assert 416 : < Y /= 000000000000 >" -- 	Vector No: 270
severity warning;

wait for 1 ns;

-- **************************************************
--------------------------

clk <= '1'; --	 Cycle No: 624

wait for 1 ns;

I <= "0000";
CCEN_BAR <= '0';
CC_BAR <= '0';
RLD_BAR <= '0';
CI <= '0';
OEbar <= '0';
D <= "111111111111";

wait for 4 ns;

clk <= '0';

wait for 4 ns;

wait for 1 ns;

--------------------------

clk <= '1'; --	 Cycle No: 625

wait for 1 ns;

I <= "0001";
CCEN_BAR <= '0';
CC_BAR <= '0';
RLD_BAR <= '1';
CI <= '0';
OEbar <= '0';
D <= "111111111111";

wait for 4 ns;

clk <= '0';

wait for 4 ns;

assert (Y = "111111111111")
report
"Assert 417 : < Y /= 111111111111 >" -- 	Vector No: 271
severity warning;

wait for 1 ns;

-- **************************************************
-- **************************************************
--------------------------

clk <= '1'; --	 Cycle No: 626

wait for 1 ns;

I <= "0000";
CCEN_BAR <= '0';
CC_BAR <= '0';
RLD_BAR <= '0';
CI <= '0';
OEbar <= '0';
D <= "000000000000";

wait for 4 ns;

clk <= '0';

wait for 4 ns;

wait for 1 ns;

--------------------------

clk <= '1'; --	 Cycle No: 627

wait for 1 ns;

I <= "0001";
CCEN_BAR <= '1';
CC_BAR <= '1';
RLD_BAR <= '1';
CI <= '0';
OEbar <= '0';
D <= "000000000000";

wait for 4 ns;

clk <= '0';

wait for 4 ns;

assert (Y = "000000000000")
report
"Assert 418 : < Y /= 000000000000 >" -- 	Vector No: 272
severity warning;

wait for 1 ns;

-- **************************************************
--------------------------

clk <= '1'; --	 Cycle No: 628

wait for 1 ns;

I <= "0000";
CCEN_BAR <= '0';
CC_BAR <= '0';
RLD_BAR <= '0';
CI <= '0';
OEbar <= '0';
D <= "000000000000";

wait for 4 ns;

clk <= '0';

wait for 4 ns;

wait for 1 ns;

--------------------------

clk <= '1'; --	 Cycle No: 629

wait for 1 ns;

I <= "0001";
CCEN_BAR <= '1';
CC_BAR <= '1';
RLD_BAR <= '1';
CI <= '0';
OEbar <= '0';
D <= "111111111111";

wait for 4 ns;

clk <= '0';

wait for 4 ns;

assert (Y = "111111111111")
report
"Assert 419 : < Y /= 111111111111 >" -- 	Vector No: 273
severity warning;

wait for 1 ns;

-- **************************************************
--------------------------

clk <= '1'; --	 Cycle No: 630

wait for 1 ns;

I <= "0000";
CCEN_BAR <= '0';
CC_BAR <= '0';
RLD_BAR <= '0';
CI <= '0';
OEbar <= '0';
D <= "111111111111";

wait for 4 ns;

clk <= '0';

wait for 4 ns;

wait for 1 ns;

--------------------------

clk <= '1'; --	 Cycle No: 631

wait for 1 ns;

I <= "0001";
CCEN_BAR <= '1';
CC_BAR <= '1';
RLD_BAR <= '1';
CI <= '0';
OEbar <= '0';
D <= "000000000000";

wait for 4 ns;

clk <= '0';

wait for 4 ns;

assert (Y = "000000000000")
report
"Assert 420 : < Y /= 000000000000 >" -- 	Vector No: 274
severity warning;

wait for 1 ns;

-- **************************************************
--------------------------

clk <= '1'; --	 Cycle No: 632

wait for 1 ns;

I <= "0000";
CCEN_BAR <= '0';
CC_BAR <= '0';
RLD_BAR <= '0';
CI <= '0';
OEbar <= '0';
D <= "111111111111";

wait for 4 ns;

clk <= '0';

wait for 4 ns;

wait for 1 ns;

--------------------------

clk <= '1'; --	 Cycle No: 633

wait for 1 ns;

I <= "0001";
CCEN_BAR <= '1';
CC_BAR <= '1';
RLD_BAR <= '1';
CI <= '0';
OEbar <= '0';
D <= "111111111111";

wait for 4 ns;

clk <= '0';

wait for 4 ns;

assert (Y = "111111111111")
report
"Assert 421 : < Y /= 111111111111 >" -- 	Vector No: 275
severity warning;

wait for 1 ns;

-- **************************************************
-- **************************************************
--------------------------

clk <= '1'; --	 Cycle No: 634

wait for 1 ns;

I <= "0000";
CCEN_BAR <= '0';
CC_BAR <= '0';
RLD_BAR <= '0';
CI <= '0';
OEbar <= '0';
D <= "000000000000";

wait for 4 ns;

clk <= '0';

wait for 4 ns;

wait for 1 ns;

--------------------------

clk <= '1'; --	 Cycle No: 635

wait for 1 ns;

I <= "0001";
CCEN_BAR <= '1';
CC_BAR <= '0';
RLD_BAR <= '1';
CI <= '0';
OEbar <= '0';
D <= "000000000000";

wait for 4 ns;

clk <= '0';

wait for 4 ns;

assert (Y = "000000000000")
report
"Assert 422 : < Y /= 000000000000 >" -- 	Vector No: 276
severity warning;

wait for 1 ns;

-- **************************************************
--------------------------

clk <= '1'; --	 Cycle No: 636

wait for 1 ns;

I <= "0000";
CCEN_BAR <= '0';
CC_BAR <= '0';
RLD_BAR <= '0';
CI <= '0';
OEbar <= '0';
D <= "000000000000";

wait for 4 ns;

clk <= '0';

wait for 4 ns;

wait for 1 ns;

--------------------------

clk <= '1'; --	 Cycle No: 637

wait for 1 ns;

I <= "0001";
CCEN_BAR <= '1';
CC_BAR <= '0';
RLD_BAR <= '1';
CI <= '0';
OEbar <= '0';
D <= "111111111111";

wait for 4 ns;

clk <= '0';

wait for 4 ns;

assert (Y = "111111111111")
report
"Assert 423 : < Y /= 111111111111 >" -- 	Vector No: 277
severity warning;

wait for 1 ns;

-- **************************************************
--------------------------

clk <= '1'; --	 Cycle No: 638

wait for 1 ns;

I <= "0000";
CCEN_BAR <= '0';
CC_BAR <= '0';
RLD_BAR <= '0';
CI <= '0';
OEbar <= '0';
D <= "111111111111";

wait for 4 ns;

clk <= '0';

wait for 4 ns;

wait for 1 ns;

--------------------------

clk <= '1'; --	 Cycle No: 639

wait for 1 ns;

I <= "0001";
CCEN_BAR <= '1';
CC_BAR <= '0';
RLD_BAR <= '1';
CI <= '0';
OEbar <= '0';
D <= "000000000000";

wait for 4 ns;

clk <= '0';

wait for 4 ns;

assert (Y = "000000000000")
report
"Assert 424 : < Y /= 000000000000 >" -- 	Vector No: 278
severity warning;

wait for 1 ns;

-- **************************************************
--------------------------

clk <= '1'; --	 Cycle No: 640

wait for 1 ns;

I <= "0000";
CCEN_BAR <= '0';
CC_BAR <= '0';
RLD_BAR <= '0';
CI <= '0';
OEbar <= '0';
D <= "111111111111";

wait for 4 ns;

clk <= '0';

wait for 4 ns;

wait for 1 ns;

--------------------------

clk <= '1'; --	 Cycle No: 641

wait for 1 ns;

I <= "0001";
CCEN_BAR <= '1';
CC_BAR <= '0';
RLD_BAR <= '1';
CI <= '0';
OEbar <= '0';
D <= "111111111111";

wait for 4 ns;

clk <= '0';

wait for 4 ns;

assert (Y = "111111111111")
report
"Assert 425 : < Y /= 111111111111 >" -- 	Vector No: 279
severity warning;

wait for 1 ns;

-- **************************************************
-- **************************************************
-- ** I = 2 **
--------------------------

clk <= '1'; --	 Cycle No: 642

wait for 1 ns;

I <= "0000";
CCEN_BAR <= '0';
CC_BAR <= '0';
RLD_BAR <= '0';
CI <= '0';
OEbar <= '0';
D <= "000000000000";

wait for 4 ns;

clk <= '0';

wait for 4 ns;

wait for 1 ns;

--------------------------

clk <= '1'; --	 Cycle No: 643

wait for 1 ns;

I <= "0010";
CCEN_BAR <= '0';
CC_BAR <= '0';
RLD_BAR <= '1';
CI <= '0';
OEbar <= '0';
D <= "000000000000";

wait for 4 ns;

clk <= '0';

wait for 4 ns;

assert (Y = "000000000000")
report
"Assert 426 : < Y /= 000000000000 >" -- 	Vector No: 280
severity warning;

wait for 1 ns;

-- **************************************************
--------------------------

clk <= '1'; --	 Cycle No: 644

wait for 1 ns;

I <= "0000";
CCEN_BAR <= '0';
CC_BAR <= '0';
RLD_BAR <= '0';
CI <= '0';
OEbar <= '0';
D <= "000000000000";

wait for 4 ns;

clk <= '0';

wait for 4 ns;

wait for 1 ns;

--------------------------

clk <= '1'; --	 Cycle No: 645

wait for 1 ns;

I <= "0010";
CCEN_BAR <= '0';
CC_BAR <= '0';
RLD_BAR <= '1';
CI <= '0';
OEbar <= '0';
D <= "111111111111";

wait for 4 ns;

clk <= '0';

wait for 4 ns;

assert (Y = "111111111111")
report
"Assert 427 : < Y /= 111111111111 >" -- 	Vector No: 281
severity warning;

wait for 1 ns;

-- **************************************************
--------------------------

clk <= '1'; --	 Cycle No: 646

wait for 1 ns;

I <= "0000";
CCEN_BAR <= '0';
CC_BAR <= '0';
RLD_BAR <= '0';
CI <= '0';
OEbar <= '0';
D <= "111111111111";

wait for 4 ns;

clk <= '0';

wait for 4 ns;

wait for 1 ns;

--------------------------

clk <= '1'; --	 Cycle No: 647

wait for 1 ns;

I <= "0010";
CCEN_BAR <= '0';
CC_BAR <= '0';
RLD_BAR <= '1';
CI <= '0';
OEbar <= '0';
D <= "000000000000";

wait for 4 ns;

clk <= '0';

wait for 4 ns;

assert (Y = "000000000000")
report
"Assert 428 : < Y /= 000000000000 >" -- 	Vector No: 282
severity warning;

wait for 1 ns;

-- **************************************************
--------------------------

clk <= '1'; --	 Cycle No: 648

wait for 1 ns;

I <= "0000";
CCEN_BAR <= '0';
CC_BAR <= '0';
RLD_BAR <= '0';
CI <= '0';
OEbar <= '0';
D <= "111111111111";

wait for 4 ns;

clk <= '0';

wait for 4 ns;

wait for 1 ns;

--------------------------

clk <= '1'; --	 Cycle No: 649

wait for 1 ns;

I <= "0010";
CCEN_BAR <= '0';
CC_BAR <= '0';
RLD_BAR <= '1';
CI <= '0';
OEbar <= '0';
D <= "111111111111";

wait for 4 ns;

clk <= '0';

wait for 4 ns;

assert (Y = "111111111111")
report
"Assert 429 : < Y /= 111111111111 >" -- 	Vector No: 283
severity warning;

wait for 1 ns;

-- **************************************************
-- **************************************************
--------------------------

clk <= '1'; --	 Cycle No: 650

wait for 1 ns;

I <= "0000";
CCEN_BAR <= '0';
CC_BAR <= '0';
RLD_BAR <= '0';
CI <= '0';
OEbar <= '0';
D <= "000000000000";

wait for 4 ns;

clk <= '0';

wait for 4 ns;

wait for 1 ns;

--------------------------

clk <= '1'; --	 Cycle No: 651

wait for 1 ns;

I <= "0010";
CCEN_BAR <= '0';
CC_BAR <= '1';
RLD_BAR <= '1';
CI <= '0';
OEbar <= '0';
D <= "000000000000";

wait for 4 ns;

clk <= '0';

wait for 4 ns;

assert (Y = "000000000000")
report
"Assert 430 : < Y /= 000000000000 >" -- 	Vector No: 284
severity warning;

wait for 1 ns;

-- **************************************************
--------------------------

clk <= '1'; --	 Cycle No: 652

wait for 1 ns;

I <= "0000";
CCEN_BAR <= '0';
CC_BAR <= '0';
RLD_BAR <= '0';
CI <= '0';
OEbar <= '0';
D <= "000000000000";

wait for 4 ns;

clk <= '0';

wait for 4 ns;

wait for 1 ns;

--------------------------

clk <= '1'; --	 Cycle No: 653

wait for 1 ns;

I <= "0010";
CCEN_BAR <= '0';
CC_BAR <= '1';
RLD_BAR <= '1';
CI <= '0';
OEbar <= '0';
D <= "111111111111";

wait for 4 ns;

clk <= '0';

wait for 4 ns;

assert (Y = "111111111111")
report
"Assert 431 : < Y /= 111111111111 >" -- 	Vector No: 285
severity warning;

wait for 1 ns;

-- **************************************************
--------------------------

clk <= '1'; --	 Cycle No: 654

wait for 1 ns;

I <= "0000";
CCEN_BAR <= '0';
CC_BAR <= '0';
RLD_BAR <= '0';
CI <= '0';
OEbar <= '0';
D <= "111111111111";

wait for 4 ns;

clk <= '0';

wait for 4 ns;

wait for 1 ns;

--------------------------

clk <= '1'; --	 Cycle No: 655

wait for 1 ns;

I <= "0010";
CCEN_BAR <= '0';
CC_BAR <= '1';
RLD_BAR <= '1';
CI <= '0';
OEbar <= '0';
D <= "000000000000";

wait for 4 ns;

clk <= '0';

wait for 4 ns;

assert (Y = "000000000000")
report
"Assert 432 : < Y /= 000000000000 >" -- 	Vector No: 286
severity warning;

wait for 1 ns;

-- **************************************************
--------------------------

clk <= '1'; --	 Cycle No: 656

wait for 1 ns;

I <= "0000";
CCEN_BAR <= '0';
CC_BAR <= '0';
RLD_BAR <= '0';
CI <= '0';
OEbar <= '0';
D <= "111111111111";

wait for 4 ns;

clk <= '0';

wait for 4 ns;

wait for 1 ns;

--------------------------

clk <= '1'; --	 Cycle No: 657

wait for 1 ns;

I <= "0010";
CCEN_BAR <= '0';
CC_BAR <= '1';
RLD_BAR <= '1';
CI <= '0';
OEbar <= '0';
D <= "111111111111";

wait for 4 ns;

clk <= '0';

wait for 4 ns;

assert (Y = "111111111111")
report
"Assert 433 : < Y /= 111111111111 >" -- 	Vector No: 287
severity warning;

wait for 1 ns;

-- **************************************************
-- **************************************************
--------------------------

clk <= '1'; --	 Cycle No: 658

wait for 1 ns;

I <= "0000";
CCEN_BAR <= '0';
CC_BAR <= '0';
RLD_BAR <= '0';
CI <= '0';
OEbar <= '0';
D <= "000000000000";

wait for 4 ns;

clk <= '0';

wait for 4 ns;

wait for 1 ns;

--------------------------

clk <= '1'; --	 Cycle No: 659

wait for 1 ns;

I <= "0010";
CCEN_BAR <= '1';
CC_BAR <= '1';
RLD_BAR <= '1';
CI <= '0';
OEbar <= '0';
D <= "000000000000";

wait for 4 ns;

clk <= '0';

wait for 4 ns;

assert (Y = "000000000000")
report
"Assert 434 : < Y /= 000000000000 >" -- 	Vector No: 288
severity warning;

wait for 1 ns;

-- **************************************************
--------------------------

clk <= '1'; --	 Cycle No: 660

wait for 1 ns;

I <= "0000";
CCEN_BAR <= '0';
CC_BAR <= '0';
RLD_BAR <= '0';
CI <= '0';
OEbar <= '0';
D <= "000000000000";

wait for 4 ns;

clk <= '0';

wait for 4 ns;

wait for 1 ns;

--------------------------

clk <= '1'; --	 Cycle No: 661

wait for 1 ns;

I <= "0010";
CCEN_BAR <= '1';
CC_BAR <= '1';
RLD_BAR <= '1';
CI <= '0';
OEbar <= '0';
D <= "111111111111";

wait for 4 ns;

clk <= '0';

wait for 4 ns;

assert (Y = "111111111111")
report
"Assert 435 : < Y /= 111111111111 >" -- 	Vector No: 289
severity warning;

wait for 1 ns;

-- **************************************************
--------------------------

clk <= '1'; --	 Cycle No: 662

wait for 1 ns;

I <= "0000";
CCEN_BAR <= '0';
CC_BAR <= '0';
RLD_BAR <= '0';
CI <= '0';
OEbar <= '0';
D <= "111111111111";

wait for 4 ns;

clk <= '0';

wait for 4 ns;

wait for 1 ns;

--------------------------

clk <= '1'; --	 Cycle No: 663

wait for 1 ns;

I <= "0010";
CCEN_BAR <= '1';
CC_BAR <= '1';
RLD_BAR <= '1';
CI <= '0';
OEbar <= '0';
D <= "000000000000";

wait for 4 ns;

clk <= '0';

wait for 4 ns;

assert (Y = "000000000000")
report
"Assert 436 : < Y /= 000000000000 >" -- 	Vector No: 290
severity warning;

wait for 1 ns;

-- **************************************************
--------------------------

clk <= '1'; --	 Cycle No: 664

wait for 1 ns;

I <= "0000";
CCEN_BAR <= '0';
CC_BAR <= '0';
RLD_BAR <= '0';
CI <= '0';
OEbar <= '0';
D <= "111111111111";

wait for 4 ns;

clk <= '0';

wait for 4 ns;

wait for 1 ns;

--------------------------

clk <= '1'; --	 Cycle No: 665

wait for 1 ns;

I <= "0010";
CCEN_BAR <= '1';
CC_BAR <= '1';
RLD_BAR <= '1';
CI <= '0';
OEbar <= '0';
D <= "111111111111";

wait for 4 ns;

clk <= '0';

wait for 4 ns;

assert (Y = "111111111111")
report
"Assert 437 : < Y /= 111111111111 >" -- 	Vector No: 291
severity warning;

wait for 1 ns;

-- **************************************************
-- **************************************************
--------------------------

clk <= '1'; --	 Cycle No: 666

wait for 1 ns;

I <= "0000";
CCEN_BAR <= '0';
CC_BAR <= '0';
RLD_BAR <= '0';
CI <= '0';
OEbar <= '0';
D <= "000000000000";

wait for 4 ns;

clk <= '0';

wait for 4 ns;

wait for 1 ns;

--------------------------

clk <= '1'; --	 Cycle No: 667

wait for 1 ns;

I <= "0010";
CCEN_BAR <= '1';
CC_BAR <= '0';
RLD_BAR <= '1';
CI <= '0';
OEbar <= '0';
D <= "000000000000";

wait for 4 ns;

clk <= '0';

wait for 4 ns;

assert (Y = "000000000000")
report
"Assert 438 : < Y /= 000000000000 >" -- 	Vector No: 292
severity warning;

wait for 1 ns;

-- **************************************************
--------------------------

clk <= '1'; --	 Cycle No: 668

wait for 1 ns;

I <= "0000";
CCEN_BAR <= '0';
CC_BAR <= '0';
RLD_BAR <= '0';
CI <= '0';
OEbar <= '0';
D <= "000000000000";

wait for 4 ns;

clk <= '0';

wait for 4 ns;

wait for 1 ns;

--------------------------

clk <= '1'; --	 Cycle No: 669

wait for 1 ns;

I <= "0010";
CCEN_BAR <= '1';
CC_BAR <= '0';
RLD_BAR <= '1';
CI <= '0';
OEbar <= '0';
D <= "111111111111";

wait for 4 ns;

clk <= '0';

wait for 4 ns;

assert (Y = "111111111111")
report
"Assert 439 : < Y /= 111111111111 >" -- 	Vector No: 293
severity warning;

wait for 1 ns;

-- **************************************************
--------------------------

clk <= '1'; --	 Cycle No: 670

wait for 1 ns;

I <= "0000";
CCEN_BAR <= '0';
CC_BAR <= '0';
RLD_BAR <= '0';
CI <= '0';
OEbar <= '0';
D <= "111111111111";

wait for 4 ns;

clk <= '0';

wait for 4 ns;

wait for 1 ns;

--------------------------

clk <= '1'; --	 Cycle No: 671

wait for 1 ns;

I <= "0010";
CCEN_BAR <= '1';
CC_BAR <= '0';
RLD_BAR <= '1';
CI <= '0';
OEbar <= '0';
D <= "000000000000";

wait for 4 ns;

clk <= '0';

wait for 4 ns;

assert (Y = "000000000000")
report
"Assert 440 : < Y /= 000000000000 >" -- 	Vector No: 294
severity warning;

wait for 1 ns;

-- **************************************************
--------------------------

clk <= '1'; --	 Cycle No: 672

wait for 1 ns;

I <= "0000";
CCEN_BAR <= '0';
CC_BAR <= '0';
RLD_BAR <= '0';
CI <= '0';
OEbar <= '0';
D <= "111111111111";

wait for 4 ns;

clk <= '0';

wait for 4 ns;

wait for 1 ns;

--------------------------

clk <= '1'; --	 Cycle No: 673

wait for 1 ns;

I <= "0010";
CCEN_BAR <= '1';
CC_BAR <= '0';
RLD_BAR <= '1';
CI <= '0';
OEbar <= '0';
D <= "111111111111";

wait for 4 ns;

clk <= '0';

wait for 4 ns;

assert (Y = "111111111111")
report
"Assert 441 : < Y /= 111111111111 >" -- 	Vector No: 295
severity warning;

wait for 1 ns;

-- **************************************************
-- **************************************************
-- ** I = 3 **
--------------------------

clk <= '1'; --	 Cycle No: 674

wait for 1 ns;

I <= "0000";
CCEN_BAR <= '0';
CC_BAR <= '0';
RLD_BAR <= '0';
CI <= '0';
OEbar <= '0';
D <= "000000000000";

wait for 4 ns;

clk <= '0';

wait for 4 ns;

wait for 1 ns;

--------------------------

clk <= '1'; --	 Cycle No: 675

wait for 1 ns;

I <= "0011";
CCEN_BAR <= '0';
CC_BAR <= '0';
RLD_BAR <= '1';
CI <= '0';
OEbar <= '0';
D <= "000000000000";

wait for 4 ns;

clk <= '0';

wait for 4 ns;

assert (Y = "000000000000")
report
"Assert 442 : < Y /= 000000000000 >" -- 	Vector No: 296
severity warning;

wait for 1 ns;

-- **************************************************
--------------------------

clk <= '1'; --	 Cycle No: 676

wait for 1 ns;

I <= "0000";
CCEN_BAR <= '0';
CC_BAR <= '0';
RLD_BAR <= '0';
CI <= '0';
OEbar <= '0';
D <= "000000000000";

wait for 4 ns;

clk <= '0';

wait for 4 ns;

wait for 1 ns;

--------------------------

clk <= '1'; --	 Cycle No: 677

wait for 1 ns;

I <= "0011";
CCEN_BAR <= '0';
CC_BAR <= '0';
RLD_BAR <= '1';
CI <= '0';
OEbar <= '0';
D <= "111111111111";

wait for 4 ns;

clk <= '0';

wait for 4 ns;

assert (Y = "111111111111")
report
"Assert 443 : < Y /= 111111111111 >" -- 	Vector No: 297
severity warning;

wait for 1 ns;

-- **************************************************
--------------------------

clk <= '1'; --	 Cycle No: 678

wait for 1 ns;

I <= "0000";
CCEN_BAR <= '0';
CC_BAR <= '0';
RLD_BAR <= '0';
CI <= '0';
OEbar <= '0';
D <= "111111111111";

wait for 4 ns;

clk <= '0';

wait for 4 ns;

wait for 1 ns;

--------------------------

clk <= '1'; --	 Cycle No: 679

wait for 1 ns;

I <= "0011";
CCEN_BAR <= '0';
CC_BAR <= '0';
RLD_BAR <= '1';
CI <= '0';
OEbar <= '0';
D <= "000000000000";

wait for 4 ns;

clk <= '0';

wait for 4 ns;

assert (Y = "000000000000")
report
"Assert 444 : < Y /= 000000000000 >" -- 	Vector No: 298
severity warning;

wait for 1 ns;

-- **************************************************
--------------------------

clk <= '1'; --	 Cycle No: 680

wait for 1 ns;

I <= "0000";
CCEN_BAR <= '0';
CC_BAR <= '0';
RLD_BAR <= '0';
CI <= '0';
OEbar <= '0';
D <= "111111111111";

wait for 4 ns;

clk <= '0';

wait for 4 ns;

wait for 1 ns;

--------------------------

clk <= '1'; --	 Cycle No: 681

wait for 1 ns;

I <= "0011";
CCEN_BAR <= '0';
CC_BAR <= '0';
RLD_BAR <= '1';
CI <= '0';
OEbar <= '0';
D <= "111111111111";

wait for 4 ns;

clk <= '0';

wait for 4 ns;

assert (Y = "111111111111")
report
"Assert 445 : < Y /= 111111111111 >" -- 	Vector No: 299
severity warning;

wait for 1 ns;

-- **************************************************
-- **************************************************
--------------------------

clk <= '1'; --	 Cycle No: 682

wait for 1 ns;

I <= "0000";
CCEN_BAR <= '0';
CC_BAR <= '0';
RLD_BAR <= '0';
CI <= '0';
OEbar <= '0';
D <= "000000000000";

wait for 4 ns;

clk <= '0';

wait for 4 ns;

wait for 1 ns;

--------------------------

clk <= '1'; --	 Cycle No: 683

wait for 1 ns;

I <= "0011";
CCEN_BAR <= '1';
CC_BAR <= '1';
RLD_BAR <= '1';
CI <= '0';
OEbar <= '0';
D <= "000000000000";

wait for 4 ns;

clk <= '0';

wait for 4 ns;

assert (Y = "000000000000")
report
"Assert 446 : < Y /= 000000000000 >" -- 	Vector No: 300
severity warning;

wait for 1 ns;

-- **************************************************
--------------------------

clk <= '1'; --	 Cycle No: 684

wait for 1 ns;

I <= "0000";
CCEN_BAR <= '0';
CC_BAR <= '0';
RLD_BAR <= '0';
CI <= '0';
OEbar <= '0';
D <= "000000000000";

wait for 4 ns;

clk <= '0';

wait for 4 ns;

wait for 1 ns;

--------------------------

clk <= '1'; --	 Cycle No: 685

wait for 1 ns;

I <= "0011";
CCEN_BAR <= '1';
CC_BAR <= '1';
RLD_BAR <= '1';
CI <= '0';
OEbar <= '0';
D <= "111111111111";

wait for 4 ns;

clk <= '0';

wait for 4 ns;

assert (Y = "111111111111")
report
"Assert 447 : < Y /= 111111111111 >" -- 	Vector No: 301
severity warning;

wait for 1 ns;

-- **************************************************
--------------------------

clk <= '1'; --	 Cycle No: 686

wait for 1 ns;

I <= "0000";
CCEN_BAR <= '0';
CC_BAR <= '0';
RLD_BAR <= '0';
CI <= '0';
OEbar <= '0';
D <= "111111111111";

wait for 4 ns;

clk <= '0';

wait for 4 ns;

wait for 1 ns;

--------------------------

clk <= '1'; --	 Cycle No: 687

wait for 1 ns;

I <= "0011";
CCEN_BAR <= '1';
CC_BAR <= '1';
RLD_BAR <= '1';
CI <= '0';
OEbar <= '0';
D <= "000000000000";

wait for 4 ns;

clk <= '0';

wait for 4 ns;

assert (Y = "000000000000")
report
"Assert 448 : < Y /= 000000000000 >" -- 	Vector No: 302
severity warning;

wait for 1 ns;

-- **************************************************
--------------------------

clk <= '1'; --	 Cycle No: 688

wait for 1 ns;

I <= "0000";
CCEN_BAR <= '0';
CC_BAR <= '0';
RLD_BAR <= '0';
CI <= '0';
OEbar <= '0';
D <= "111111111111";

wait for 4 ns;

clk <= '0';

wait for 4 ns;

wait for 1 ns;

--------------------------

clk <= '1'; --	 Cycle No: 689

wait for 1 ns;

I <= "0011";
CCEN_BAR <= '1';
CC_BAR <= '1';
RLD_BAR <= '1';
CI <= '0';
OEbar <= '0';
D <= "111111111111";

wait for 4 ns;

clk <= '0';

wait for 4 ns;

assert (Y = "111111111111")
report
"Assert 449 : < Y /= 111111111111 >" -- 	Vector No: 303
severity warning;

wait for 1 ns;

-- **************************************************
-- **************************************************
--------------------------

clk <= '1'; --	 Cycle No: 690

wait for 1 ns;

I <= "0000";
CCEN_BAR <= '0';
CC_BAR <= '0';
RLD_BAR <= '0';
CI <= '0';
OEbar <= '0';
D <= "000000000000";

wait for 4 ns;

clk <= '0';

wait for 4 ns;

wait for 1 ns;

--------------------------

clk <= '1'; --	 Cycle No: 691

wait for 1 ns;

I <= "0011";
CCEN_BAR <= '1';
CC_BAR <= '0';
RLD_BAR <= '1';
CI <= '0';
OEbar <= '0';
D <= "000000000000";

wait for 4 ns;

clk <= '0';

wait for 4 ns;

assert (Y = "000000000000")
report
"Assert 450 : < Y /= 000000000000 >" -- 	Vector No: 304
severity warning;

wait for 1 ns;

-- **************************************************
--------------------------

clk <= '1'; --	 Cycle No: 692

wait for 1 ns;

I <= "0000";
CCEN_BAR <= '0';
CC_BAR <= '0';
RLD_BAR <= '0';
CI <= '0';
OEbar <= '0';
D <= "000000000000";

wait for 4 ns;

clk <= '0';

wait for 4 ns;

wait for 1 ns;

--------------------------

clk <= '1'; --	 Cycle No: 693

wait for 1 ns;

I <= "0011";
CCEN_BAR <= '1';
CC_BAR <= '0';
RLD_BAR <= '1';
CI <= '0';
OEbar <= '0';
D <= "111111111111";

wait for 4 ns;

clk <= '0';

wait for 4 ns;

assert (Y = "111111111111")
report
"Assert 451 : < Y /= 111111111111 >" -- 	Vector No: 305
severity warning;

wait for 1 ns;

-- **************************************************
--------------------------

clk <= '1'; --	 Cycle No: 694

wait for 1 ns;

I <= "0000";
CCEN_BAR <= '0';
CC_BAR <= '0';
RLD_BAR <= '0';
CI <= '0';
OEbar <= '0';
D <= "111111111111";

wait for 4 ns;

clk <= '0';

wait for 4 ns;

wait for 1 ns;

--------------------------

clk <= '1'; --	 Cycle No: 695

wait for 1 ns;

I <= "0011";
CCEN_BAR <= '1';
CC_BAR <= '0';
RLD_BAR <= '1';
CI <= '0';
OEbar <= '0';
D <= "000000000000";

wait for 4 ns;

clk <= '0';

wait for 4 ns;

assert (Y = "000000000000")
report
"Assert 452 : < Y /= 000000000000 >" -- 	Vector No: 306
severity warning;

wait for 1 ns;

-- **************************************************
--------------------------

clk <= '1'; --	 Cycle No: 696

wait for 1 ns;

I <= "0000";
CCEN_BAR <= '0';
CC_BAR <= '0';
RLD_BAR <= '0';
CI <= '0';
OEbar <= '0';
D <= "111111111111";

wait for 4 ns;

clk <= '0';

wait for 4 ns;

wait for 1 ns;

--------------------------

clk <= '1'; --	 Cycle No: 697

wait for 1 ns;

I <= "0011";
CCEN_BAR <= '1';
CC_BAR <= '0';
RLD_BAR <= '1';
CI <= '0';
OEbar <= '0';
D <= "111111111111";

wait for 4 ns;

clk <= '0';

wait for 4 ns;

assert (Y = "111111111111")
report
"Assert 453 : < Y /= 111111111111 >" -- 	Vector No: 307
severity warning;

wait for 1 ns;

-- **************************************************
-- **************************************************
-- ** I = 5 **
--------------------------

clk <= '1'; --	 Cycle No: 698

wait for 1 ns;

I <= "0000";
CCEN_BAR <= '0';
CC_BAR <= '0';
RLD_BAR <= '0';
CI <= '0';
OEbar <= '0';
D <= "000000000000";

wait for 4 ns;

clk <= '0';

wait for 4 ns;

wait for 1 ns;

--------------------------

clk <= '1'; --	 Cycle No: 699

wait for 1 ns;

I <= "0101";
CCEN_BAR <= '0';
CC_BAR <= '0';
RLD_BAR <= '1';
CI <= '0';
OEbar <= '0';
D <= "000000000000";

wait for 4 ns;

clk <= '0';

wait for 4 ns;

assert (Y = "000000000000")
report
"Assert 454 : < Y /= 000000000000 >" -- 	Vector No: 308
severity warning;

wait for 1 ns;

-- **************************************************
--------------------------

clk <= '1'; --	 Cycle No: 700

wait for 1 ns;

I <= "0000";
CCEN_BAR <= '0';
CC_BAR <= '0';
RLD_BAR <= '0';
CI <= '0';
OEbar <= '0';
D <= "000000000000";

wait for 4 ns;

clk <= '0';

wait for 4 ns;

wait for 1 ns;

--------------------------

clk <= '1'; --	 Cycle No: 701

wait for 1 ns;

I <= "0101";
CCEN_BAR <= '0';
CC_BAR <= '0';
RLD_BAR <= '1';
CI <= '0';
OEbar <= '0';
D <= "111111111111";

wait for 4 ns;

clk <= '0';

wait for 4 ns;

assert (Y = "111111111111")
report
"Assert 455 : < Y /= 111111111111 >" -- 	Vector No: 309
severity warning;

wait for 1 ns;

-- **************************************************
--------------------------

clk <= '1'; --	 Cycle No: 702

wait for 1 ns;

I <= "0000";
CCEN_BAR <= '0';
CC_BAR <= '0';
RLD_BAR <= '0';
CI <= '0';
OEbar <= '0';
D <= "111111111111";

wait for 4 ns;

clk <= '0';

wait for 4 ns;

wait for 1 ns;

--------------------------

clk <= '1'; --	 Cycle No: 703

wait for 1 ns;

I <= "0101";
CCEN_BAR <= '0';
CC_BAR <= '0';
RLD_BAR <= '1';
CI <= '0';
OEbar <= '0';
D <= "000000000000";

wait for 4 ns;

clk <= '0';

wait for 4 ns;

assert (Y = "000000000000")
report
"Assert 456 : < Y /= 000000000000 >" -- 	Vector No: 310
severity warning;

wait for 1 ns;

-- **************************************************
--------------------------

clk <= '1'; --	 Cycle No: 704

wait for 1 ns;

I <= "0000";
CCEN_BAR <= '0';
CC_BAR <= '0';
RLD_BAR <= '0';
CI <= '0';
OEbar <= '0';
D <= "111111111111";

wait for 4 ns;

clk <= '0';

wait for 4 ns;

wait for 1 ns;

--------------------------

clk <= '1'; --	 Cycle No: 705

wait for 1 ns;

I <= "0101";
CCEN_BAR <= '0';
CC_BAR <= '0';
RLD_BAR <= '1';
CI <= '0';
OEbar <= '0';
D <= "111111111111";

wait for 4 ns;

clk <= '0';

wait for 4 ns;

assert (Y = "111111111111")
report
"Assert 457 : < Y /= 111111111111 >" -- 	Vector No: 311
severity warning;

wait for 1 ns;

-- **************************************************
-- **************************************************
--------------------------

clk <= '1'; --	 Cycle No: 706

wait for 1 ns;

I <= "0000";
CCEN_BAR <= '0';
CC_BAR <= '0';
RLD_BAR <= '0';
CI <= '0';
OEbar <= '0';
D <= "000000000000";

wait for 4 ns;

clk <= '0';

wait for 4 ns;

wait for 1 ns;

--------------------------

clk <= '1'; --	 Cycle No: 707

wait for 1 ns;

I <= "0101";
CCEN_BAR <= '1';
CC_BAR <= '1';
RLD_BAR <= '1';
CI <= '0';
OEbar <= '0';
D <= "000000000000";

wait for 4 ns;

clk <= '0';

wait for 4 ns;

assert (Y = "000000000000")
report
"Assert 458 : < Y /= 000000000000 >" -- 	Vector No: 312
severity warning;

wait for 1 ns;

-- **************************************************
--------------------------

clk <= '1'; --	 Cycle No: 708

wait for 1 ns;

I <= "0000";
CCEN_BAR <= '0';
CC_BAR <= '0';
RLD_BAR <= '0';
CI <= '0';
OEbar <= '0';
D <= "000000000000";

wait for 4 ns;

clk <= '0';

wait for 4 ns;

wait for 1 ns;

--------------------------

clk <= '1'; --	 Cycle No: 709

wait for 1 ns;

I <= "0101";
CCEN_BAR <= '1';
CC_BAR <= '1';
RLD_BAR <= '1';
CI <= '0';
OEbar <= '0';
D <= "111111111111";

wait for 4 ns;

clk <= '0';

wait for 4 ns;

assert (Y = "111111111111")
report
"Assert 459 : < Y /= 111111111111 >" -- 	Vector No: 313
severity warning;

wait for 1 ns;

-- **************************************************
--------------------------

clk <= '1'; --	 Cycle No: 710

wait for 1 ns;

I <= "0000";
CCEN_BAR <= '0';
CC_BAR <= '0';
RLD_BAR <= '0';
CI <= '0';
OEbar <= '0';
D <= "111111111111";

wait for 4 ns;

clk <= '0';

wait for 4 ns;

wait for 1 ns;

--------------------------

clk <= '1'; --	 Cycle No: 711

wait for 1 ns;

I <= "0101";
CCEN_BAR <= '1';
CC_BAR <= '1';
RLD_BAR <= '1';
CI <= '0';
OEbar <= '0';
D <= "000000000000";

wait for 4 ns;

clk <= '0';

wait for 4 ns;

assert (Y = "000000000000")
report
"Assert 460 : < Y /= 000000000000 >" -- 	Vector No: 314
severity warning;

wait for 1 ns;

-- **************************************************
--------------------------

clk <= '1'; --	 Cycle No: 712

wait for 1 ns;

I <= "0000";
CCEN_BAR <= '0';
CC_BAR <= '0';
RLD_BAR <= '0';
CI <= '0';
OEbar <= '0';
D <= "111111111111";

wait for 4 ns;

clk <= '0';

wait for 4 ns;

wait for 1 ns;

--------------------------

clk <= '1'; --	 Cycle No: 713

wait for 1 ns;

I <= "0101";
CCEN_BAR <= '1';
CC_BAR <= '1';
RLD_BAR <= '1';
CI <= '0';
OEbar <= '0';
D <= "111111111111";

wait for 4 ns;

clk <= '0';

wait for 4 ns;

assert (Y = "111111111111")
report
"Assert 461 : < Y /= 111111111111 >" -- 	Vector No: 315
severity warning;

wait for 1 ns;

-- **************************************************
-- **************************************************
--------------------------

clk <= '1'; --	 Cycle No: 714

wait for 1 ns;

I <= "0000";
CCEN_BAR <= '0';
CC_BAR <= '0';
RLD_BAR <= '0';
CI <= '0';
OEbar <= '0';
D <= "000000000000";

wait for 4 ns;

clk <= '0';

wait for 4 ns;

wait for 1 ns;

--------------------------

clk <= '1'; --	 Cycle No: 715

wait for 1 ns;

I <= "0101";
CCEN_BAR <= '1';
CC_BAR <= '0';
RLD_BAR <= '1';
CI <= '0';
OEbar <= '0';
D <= "000000000000";

wait for 4 ns;

clk <= '0';

wait for 4 ns;

assert (Y = "000000000000")
report
"Assert 462 : < Y /= 000000000000 >" -- 	Vector No: 316
severity warning;

wait for 1 ns;

-- **************************************************
--------------------------

clk <= '1'; --	 Cycle No: 716

wait for 1 ns;

I <= "0000";
CCEN_BAR <= '0';
CC_BAR <= '0';
RLD_BAR <= '0';
CI <= '0';
OEbar <= '0';
D <= "000000000000";

wait for 4 ns;

clk <= '0';

wait for 4 ns;

wait for 1 ns;

--------------------------

clk <= '1'; --	 Cycle No: 717

wait for 1 ns;

I <= "0101";
CCEN_BAR <= '1';
CC_BAR <= '0';
RLD_BAR <= '1';
CI <= '0';
OEbar <= '0';
D <= "111111111111";

wait for 4 ns;

clk <= '0';

wait for 4 ns;

assert (Y = "111111111111")
report
"Assert 463 : < Y /= 111111111111 >" -- 	Vector No: 317
severity warning;

wait for 1 ns;

-- **************************************************
--------------------------

clk <= '1'; --	 Cycle No: 718

wait for 1 ns;

I <= "0000";
CCEN_BAR <= '0';
CC_BAR <= '0';
RLD_BAR <= '0';
CI <= '0';
OEbar <= '0';
D <= "111111111111";

wait for 4 ns;

clk <= '0';

wait for 4 ns;

wait for 1 ns;

--------------------------

clk <= '1'; --	 Cycle No: 719

wait for 1 ns;

I <= "0101";
CCEN_BAR <= '1';
CC_BAR <= '0';
RLD_BAR <= '1';
CI <= '0';
OEbar <= '0';
D <= "000000000000";

wait for 4 ns;

clk <= '0';

wait for 4 ns;

assert (Y = "000000000000")
report
"Assert 464 : < Y /= 000000000000 >" -- 	Vector No: 318
severity warning;

wait for 1 ns;

-- **************************************************
--------------------------

clk <= '1'; --	 Cycle No: 720

wait for 1 ns;

I <= "0000";
CCEN_BAR <= '0';
CC_BAR <= '0';
RLD_BAR <= '0';
CI <= '0';
OEbar <= '0';
D <= "111111111111";

wait for 4 ns;

clk <= '0';

wait for 4 ns;

wait for 1 ns;

--------------------------

clk <= '1'; --	 Cycle No: 721

wait for 1 ns;

I <= "0101";
CCEN_BAR <= '1';
CC_BAR <= '0';
RLD_BAR <= '1';
CI <= '0';
OEbar <= '0';
D <= "111111111111";

wait for 4 ns;

clk <= '0';

wait for 4 ns;

assert (Y = "111111111111")
report
"Assert 465 : < Y /= 111111111111 >" -- 	Vector No: 319
severity warning;

wait for 1 ns;

-- **************************************************
-- **************************************************
-- ** I = 6 **
--------------------------

clk <= '1'; --	 Cycle No: 722

wait for 1 ns;

I <= "0000";
CCEN_BAR <= '0';
CC_BAR <= '0';
RLD_BAR <= '0';
CI <= '0';
OEbar <= '0';
D <= "000000000000";

wait for 4 ns;

clk <= '0';

wait for 4 ns;

wait for 1 ns;

--------------------------

clk <= '1'; --	 Cycle No: 723

wait for 1 ns;

I <= "0110";
CCEN_BAR <= '0';
CC_BAR <= '0';
RLD_BAR <= '1';
CI <= '0';
OEbar <= '0';
D <= "000000000000";

wait for 4 ns;

clk <= '0';

wait for 4 ns;

assert (Y = "000000000000")
report
"Assert 466 : < Y /= 000000000000 >" -- 	Vector No: 320
severity warning;

wait for 1 ns;

-- **************************************************
--------------------------

clk <= '1'; --	 Cycle No: 724

wait for 1 ns;

I <= "0000";
CCEN_BAR <= '0';
CC_BAR <= '0';
RLD_BAR <= '0';
CI <= '0';
OEbar <= '0';
D <= "000000000000";

wait for 4 ns;

clk <= '0';

wait for 4 ns;

wait for 1 ns;

--------------------------

clk <= '1'; --	 Cycle No: 725

wait for 1 ns;

I <= "0110";
CCEN_BAR <= '0';
CC_BAR <= '0';
RLD_BAR <= '1';
CI <= '0';
OEbar <= '0';
D <= "111111111111";

wait for 4 ns;

clk <= '0';

wait for 4 ns;

assert (Y = "111111111111")
report
"Assert 467 : < Y /= 111111111111 >" -- 	Vector No: 321
severity warning;

wait for 1 ns;

-- **************************************************
--------------------------

clk <= '1'; --	 Cycle No: 726

wait for 1 ns;

I <= "0000";
CCEN_BAR <= '0';
CC_BAR <= '0';
RLD_BAR <= '0';
CI <= '0';
OEbar <= '0';
D <= "111111111111";

wait for 4 ns;

clk <= '0';

wait for 4 ns;

wait for 1 ns;

--------------------------

clk <= '1'; --	 Cycle No: 727

wait for 1 ns;

I <= "0110";
CCEN_BAR <= '0';
CC_BAR <= '0';
RLD_BAR <= '1';
CI <= '0';
OEbar <= '0';
D <= "000000000000";

wait for 4 ns;

clk <= '0';

wait for 4 ns;

assert (Y = "000000000000")
report
"Assert 468 : < Y /= 000000000000 >" -- 	Vector No: 322
severity warning;

wait for 1 ns;

-- **************************************************
--------------------------

clk <= '1'; --	 Cycle No: 728

wait for 1 ns;

I <= "0000";
CCEN_BAR <= '0';
CC_BAR <= '0';
RLD_BAR <= '0';
CI <= '0';
OEbar <= '0';
D <= "111111111111";

wait for 4 ns;

clk <= '0';

wait for 4 ns;

wait for 1 ns;

--------------------------

clk <= '1'; --	 Cycle No: 729

wait for 1 ns;

I <= "0110";
CCEN_BAR <= '0';
CC_BAR <= '0';
RLD_BAR <= '1';
CI <= '0';
OEbar <= '0';
D <= "111111111111";

wait for 4 ns;

clk <= '0';

wait for 4 ns;

assert (Y = "111111111111")
report
"Assert 469 : < Y /= 111111111111 >" -- 	Vector No: 323
severity warning;

wait for 1 ns;

-- **************************************************
-- **************************************************
--------------------------

clk <= '1'; --	 Cycle No: 730

wait for 1 ns;

I <= "0000";
CCEN_BAR <= '0';
CC_BAR <= '0';
RLD_BAR <= '0';
CI <= '0';
OEbar <= '0';
D <= "000000000000";

wait for 4 ns;

clk <= '0';

wait for 4 ns;

wait for 1 ns;

--------------------------

clk <= '1'; --	 Cycle No: 731

wait for 1 ns;

I <= "0110";
CCEN_BAR <= '1';
CC_BAR <= '1';
RLD_BAR <= '1';
CI <= '0';
OEbar <= '0';
D <= "000000000000";

wait for 4 ns;

clk <= '0';

wait for 4 ns;

assert (Y = "000000000000")
report
"Assert 470 : < Y /= 000000000000 >" -- 	Vector No: 324
severity warning;

wait for 1 ns;

-- **************************************************
--------------------------

clk <= '1'; --	 Cycle No: 732

wait for 1 ns;

I <= "0000";
CCEN_BAR <= '0';
CC_BAR <= '0';
RLD_BAR <= '0';
CI <= '0';
OEbar <= '0';
D <= "000000000000";

wait for 4 ns;

clk <= '0';

wait for 4 ns;

wait for 1 ns;

--------------------------

clk <= '1'; --	 Cycle No: 733

wait for 1 ns;

I <= "0110";
CCEN_BAR <= '1';
CC_BAR <= '1';
RLD_BAR <= '1';
CI <= '0';
OEbar <= '0';
D <= "111111111111";

wait for 4 ns;

clk <= '0';

wait for 4 ns;

assert (Y = "111111111111")
report
"Assert 471 : < Y /= 111111111111 >" -- 	Vector No: 325
severity warning;

wait for 1 ns;

-- **************************************************
--------------------------

clk <= '1'; --	 Cycle No: 734

wait for 1 ns;

I <= "0000";
CCEN_BAR <= '0';
CC_BAR <= '0';
RLD_BAR <= '0';
CI <= '0';
OEbar <= '0';
D <= "111111111111";

wait for 4 ns;

clk <= '0';

wait for 4 ns;

wait for 1 ns;

--------------------------

clk <= '1'; --	 Cycle No: 735

wait for 1 ns;

I <= "0110";
CCEN_BAR <= '1';
CC_BAR <= '1';
RLD_BAR <= '1';
CI <= '0';
OEbar <= '0';
D <= "000000000000";

wait for 4 ns;

clk <= '0';

wait for 4 ns;

assert (Y = "000000000000")
report
"Assert 472 : < Y /= 000000000000 >" -- 	Vector No: 326
severity warning;

wait for 1 ns;

-- **************************************************
--------------------------

clk <= '1'; --	 Cycle No: 736

wait for 1 ns;

I <= "0000";
CCEN_BAR <= '0';
CC_BAR <= '0';
RLD_BAR <= '0';
CI <= '0';
OEbar <= '0';
D <= "111111111111";

wait for 4 ns;

clk <= '0';

wait for 4 ns;

wait for 1 ns;

--------------------------

clk <= '1'; --	 Cycle No: 737

wait for 1 ns;

I <= "0110";
CCEN_BAR <= '1';
CC_BAR <= '1';
RLD_BAR <= '1';
CI <= '0';
OEbar <= '0';
D <= "111111111111";

wait for 4 ns;

clk <= '0';

wait for 4 ns;

assert (Y = "111111111111")
report
"Assert 473 : < Y /= 111111111111 >" -- 	Vector No: 327
severity warning;

wait for 1 ns;

-- **************************************************
-- **************************************************
--------------------------

clk <= '1'; --	 Cycle No: 738

wait for 1 ns;

I <= "0000";
CCEN_BAR <= '0';
CC_BAR <= '0';
RLD_BAR <= '0';
CI <= '0';
OEbar <= '0';
D <= "000000000000";

wait for 4 ns;

clk <= '0';

wait for 4 ns;

wait for 1 ns;

--------------------------

clk <= '1'; --	 Cycle No: 739

wait for 1 ns;

I <= "0110";
CCEN_BAR <= '1';
CC_BAR <= '0';
RLD_BAR <= '1';
CI <= '0';
OEbar <= '0';
D <= "000000000000";

wait for 4 ns;

clk <= '0';

wait for 4 ns;

assert (Y = "000000000000")
report
"Assert 474 : < Y /= 000000000000 >" -- 	Vector No: 328
severity warning;

wait for 1 ns;

-- **************************************************
--------------------------

clk <= '1'; --	 Cycle No: 740

wait for 1 ns;

I <= "0000";
CCEN_BAR <= '0';
CC_BAR <= '0';
RLD_BAR <= '0';
CI <= '0';
OEbar <= '0';
D <= "000000000000";

wait for 4 ns;

clk <= '0';

wait for 4 ns;

wait for 1 ns;

--------------------------

clk <= '1'; --	 Cycle No: 741

wait for 1 ns;

I <= "0110";
CCEN_BAR <= '1';
CC_BAR <= '0';
RLD_BAR <= '1';
CI <= '0';
OEbar <= '0';
D <= "111111111111";

wait for 4 ns;

clk <= '0';

wait for 4 ns;

assert (Y = "111111111111")
report
"Assert 475 : < Y /= 111111111111 >" -- 	Vector No: 329
severity warning;

wait for 1 ns;

-- **************************************************
--------------------------

clk <= '1'; --	 Cycle No: 742

wait for 1 ns;

I <= "0000";
CCEN_BAR <= '0';
CC_BAR <= '0';
RLD_BAR <= '0';
CI <= '0';
OEbar <= '0';
D <= "111111111111";

wait for 4 ns;

clk <= '0';

wait for 4 ns;

wait for 1 ns;

--------------------------

clk <= '1'; --	 Cycle No: 743

wait for 1 ns;

I <= "0110";
CCEN_BAR <= '1';
CC_BAR <= '0';
RLD_BAR <= '1';
CI <= '0';
OEbar <= '0';
D <= "000000000000";

wait for 4 ns;

clk <= '0';

wait for 4 ns;

assert (Y = "000000000000")
report
"Assert 476 : < Y /= 000000000000 >" -- 	Vector No: 330
severity warning;

wait for 1 ns;

-- **************************************************
--------------------------

clk <= '1'; --	 Cycle No: 744

wait for 1 ns;

I <= "0000";
CCEN_BAR <= '0';
CC_BAR <= '0';
RLD_BAR <= '0';
CI <= '0';
OEbar <= '0';
D <= "111111111111";

wait for 4 ns;

clk <= '0';

wait for 4 ns;

wait for 1 ns;

--------------------------

clk <= '1'; --	 Cycle No: 745

wait for 1 ns;

I <= "0110";
CCEN_BAR <= '1';
CC_BAR <= '0';
RLD_BAR <= '1';
CI <= '0';
OEbar <= '0';
D <= "111111111111";

wait for 4 ns;

clk <= '0';

wait for 4 ns;

assert (Y = "111111111111")
report
"Assert 477 : < Y /= 111111111111 >" -- 	Vector No: 331
severity warning;

wait for 1 ns;

-- **************************************************
-- **************************************************
-- ** I = 7 **
--------------------------

clk <= '1'; --	 Cycle No: 746

wait for 1 ns;

I <= "0000";
CCEN_BAR <= '0';
CC_BAR <= '0';
RLD_BAR <= '0';
CI <= '0';
OEbar <= '0';
D <= "000000000000";

wait for 4 ns;

clk <= '0';

wait for 4 ns;

wait for 1 ns;

--------------------------

clk <= '1'; --	 Cycle No: 747

wait for 1 ns;

I <= "0111";
CCEN_BAR <= '0';
CC_BAR <= '0';
RLD_BAR <= '1';
CI <= '0';
OEbar <= '0';
D <= "000000000000";

wait for 4 ns;

clk <= '0';

wait for 4 ns;

assert (Y = "000000000000")
report
"Assert 478 : < Y /= 000000000000 >" -- 	Vector No: 332
severity warning;

wait for 1 ns;

-- **************************************************
--------------------------

clk <= '1'; --	 Cycle No: 748

wait for 1 ns;

I <= "0000";
CCEN_BAR <= '0';
CC_BAR <= '0';
RLD_BAR <= '0';
CI <= '0';
OEbar <= '0';
D <= "000000000000";

wait for 4 ns;

clk <= '0';

wait for 4 ns;

wait for 1 ns;

--------------------------

clk <= '1'; --	 Cycle No: 749

wait for 1 ns;

I <= "0111";
CCEN_BAR <= '0';
CC_BAR <= '0';
RLD_BAR <= '1';
CI <= '0';
OEbar <= '0';
D <= "111111111111";

wait for 4 ns;

clk <= '0';

wait for 4 ns;

assert (Y = "111111111111")
report
"Assert 479 : < Y /= 111111111111 >" -- 	Vector No: 333
severity warning;

wait for 1 ns;

-- **************************************************
--------------------------

clk <= '1'; --	 Cycle No: 750

wait for 1 ns;

I <= "0000";
CCEN_BAR <= '0';
CC_BAR <= '0';
RLD_BAR <= '0';
CI <= '0';
OEbar <= '0';
D <= "111111111111";

wait for 4 ns;

clk <= '0';

wait for 4 ns;

wait for 1 ns;

--------------------------

clk <= '1'; --	 Cycle No: 751

wait for 1 ns;

I <= "0111";
CCEN_BAR <= '0';
CC_BAR <= '0';
RLD_BAR <= '1';
CI <= '0';
OEbar <= '0';
D <= "000000000000";

wait for 4 ns;

clk <= '0';

wait for 4 ns;

assert (Y = "000000000000")
report
"Assert 480 : < Y /= 000000000000 >" -- 	Vector No: 334
severity warning;

wait for 1 ns;

-- **************************************************
--------------------------

clk <= '1'; --	 Cycle No: 752

wait for 1 ns;

I <= "0000";
CCEN_BAR <= '0';
CC_BAR <= '0';
RLD_BAR <= '0';
CI <= '0';
OEbar <= '0';
D <= "111111111111";

wait for 4 ns;

clk <= '0';

wait for 4 ns;

wait for 1 ns;

--------------------------

clk <= '1'; --	 Cycle No: 753

wait for 1 ns;

I <= "0111";
CCEN_BAR <= '0';
CC_BAR <= '0';
RLD_BAR <= '1';
CI <= '0';
OEbar <= '0';
D <= "111111111111";

wait for 4 ns;

clk <= '0';

wait for 4 ns;

assert (Y = "111111111111")
report
"Assert 481 : < Y /= 111111111111 >" -- 	Vector No: 335
severity warning;

wait for 1 ns;

-- **************************************************
-- **************************************************
--------------------------

clk <= '1'; --	 Cycle No: 754

wait for 1 ns;

I <= "0000";
CCEN_BAR <= '0';
CC_BAR <= '0';
RLD_BAR <= '0';
CI <= '0';
OEbar <= '0';
D <= "000000000000";

wait for 4 ns;

clk <= '0';

wait for 4 ns;

wait for 1 ns;

--------------------------

clk <= '1'; --	 Cycle No: 755

wait for 1 ns;

I <= "0111";
CCEN_BAR <= '1';
CC_BAR <= '1';
RLD_BAR <= '1';
CI <= '0';
OEbar <= '0';
D <= "000000000000";

wait for 4 ns;

clk <= '0';

wait for 4 ns;

assert (Y = "000000000000")
report
"Assert 482 : < Y /= 000000000000 >" -- 	Vector No: 336
severity warning;

wait for 1 ns;

-- **************************************************
--------------------------

clk <= '1'; --	 Cycle No: 756

wait for 1 ns;

I <= "0000";
CCEN_BAR <= '0';
CC_BAR <= '0';
RLD_BAR <= '0';
CI <= '0';
OEbar <= '0';
D <= "000000000000";

wait for 4 ns;

clk <= '0';

wait for 4 ns;

wait for 1 ns;

--------------------------

clk <= '1'; --	 Cycle No: 757

wait for 1 ns;

I <= "0111";
CCEN_BAR <= '1';
CC_BAR <= '1';
RLD_BAR <= '1';
CI <= '0';
OEbar <= '0';
D <= "111111111111";

wait for 4 ns;

clk <= '0';

wait for 4 ns;

assert (Y = "111111111111")
report
"Assert 483 : < Y /= 111111111111 >" -- 	Vector No: 337
severity warning;

wait for 1 ns;

-- **************************************************
--------------------------

clk <= '1'; --	 Cycle No: 758

wait for 1 ns;

I <= "0000";
CCEN_BAR <= '0';
CC_BAR <= '0';
RLD_BAR <= '0';
CI <= '0';
OEbar <= '0';
D <= "111111111111";

wait for 4 ns;

clk <= '0';

wait for 4 ns;

wait for 1 ns;

--------------------------

clk <= '1'; --	 Cycle No: 759

wait for 1 ns;

I <= "0111";
CCEN_BAR <= '1';
CC_BAR <= '1';
RLD_BAR <= '1';
CI <= '0';
OEbar <= '0';
D <= "000000000000";

wait for 4 ns;

clk <= '0';

wait for 4 ns;

assert (Y = "000000000000")
report
"Assert 484 : < Y /= 000000000000 >" -- 	Vector No: 338
severity warning;

wait for 1 ns;

-- **************************************************
--------------------------

clk <= '1'; --	 Cycle No: 760

wait for 1 ns;

I <= "0000";
CCEN_BAR <= '0';
CC_BAR <= '0';
RLD_BAR <= '0';
CI <= '0';
OEbar <= '0';
D <= "111111111111";

wait for 4 ns;

clk <= '0';

wait for 4 ns;

wait for 1 ns;

--------------------------

clk <= '1'; --	 Cycle No: 761

wait for 1 ns;

I <= "0111";
CCEN_BAR <= '1';
CC_BAR <= '1';
RLD_BAR <= '1';
CI <= '0';
OEbar <= '0';
D <= "111111111111";

wait for 4 ns;

clk <= '0';

wait for 4 ns;

assert (Y = "111111111111")
report
"Assert 485 : < Y /= 111111111111 >" -- 	Vector No: 339
severity warning;

wait for 1 ns;

-- **************************************************
-- **************************************************
--------------------------

clk <= '1'; --	 Cycle No: 762

wait for 1 ns;

I <= "0000";
CCEN_BAR <= '0';
CC_BAR <= '0';
RLD_BAR <= '0';
CI <= '0';
OEbar <= '0';
D <= "000000000000";

wait for 4 ns;

clk <= '0';

wait for 4 ns;

wait for 1 ns;

--------------------------

clk <= '1'; --	 Cycle No: 763

wait for 1 ns;

I <= "0111";
CCEN_BAR <= '1';
CC_BAR <= '0';
RLD_BAR <= '1';
CI <= '0';
OEbar <= '0';
D <= "000000000000";

wait for 4 ns;

clk <= '0';

wait for 4 ns;

assert (Y = "000000000000")
report
"Assert 486 : < Y /= 000000000000 >" -- 	Vector No: 340
severity warning;

wait for 1 ns;

-- **************************************************
--------------------------

clk <= '1'; --	 Cycle No: 764

wait for 1 ns;

I <= "0000";
CCEN_BAR <= '0';
CC_BAR <= '0';
RLD_BAR <= '0';
CI <= '0';
OEbar <= '0';
D <= "000000000000";

wait for 4 ns;

clk <= '0';

wait for 4 ns;

wait for 1 ns;

--------------------------

clk <= '1'; --	 Cycle No: 765

wait for 1 ns;

I <= "0111";
CCEN_BAR <= '1';
CC_BAR <= '0';
RLD_BAR <= '1';
CI <= '0';
OEbar <= '0';
D <= "111111111111";

wait for 4 ns;

clk <= '0';

wait for 4 ns;

assert (Y = "111111111111")
report
"Assert 487 : < Y /= 111111111111 >" -- 	Vector No: 341
severity warning;

wait for 1 ns;

-- **************************************************
--------------------------

clk <= '1'; --	 Cycle No: 766

wait for 1 ns;

I <= "0000";
CCEN_BAR <= '0';
CC_BAR <= '0';
RLD_BAR <= '0';
CI <= '0';
OEbar <= '0';
D <= "111111111111";

wait for 4 ns;

clk <= '0';

wait for 4 ns;

wait for 1 ns;

--------------------------

clk <= '1'; --	 Cycle No: 767

wait for 1 ns;

I <= "0111";
CCEN_BAR <= '1';
CC_BAR <= '0';
RLD_BAR <= '1';
CI <= '0';
OEbar <= '0';
D <= "000000000000";

wait for 4 ns;

clk <= '0';

wait for 4 ns;

assert (Y = "000000000000")
report
"Assert 488 : < Y /= 000000000000 >" -- 	Vector No: 342
severity warning;

wait for 1 ns;

-- **************************************************
--------------------------

clk <= '1'; --	 Cycle No: 768

wait for 1 ns;

I <= "0000";
CCEN_BAR <= '0';
CC_BAR <= '0';
RLD_BAR <= '0';
CI <= '0';
OEbar <= '0';
D <= "111111111111";

wait for 4 ns;

clk <= '0';

wait for 4 ns;

wait for 1 ns;

--------------------------

clk <= '1'; --	 Cycle No: 769

wait for 1 ns;

I <= "0111";
CCEN_BAR <= '1';
CC_BAR <= '0';
RLD_BAR <= '1';
CI <= '0';
OEbar <= '0';
D <= "111111111111";

wait for 4 ns;

clk <= '0';

wait for 4 ns;

assert (Y = "111111111111")
report
"Assert 489 : < Y /= 111111111111 >" -- 	Vector No: 343
severity warning;

wait for 1 ns;

-- **************************************************
-- **************************************************
-- ** I = 9 **
--------------------------

clk <= '1'; --	 Cycle No: 770

wait for 1 ns;

I <= "0000";
CCEN_BAR <= '0';
CC_BAR <= '0';
RLD_BAR <= '0';
CI <= '0';
OEbar <= '0';
D <= "111111111111";

wait for 4 ns;

clk <= '0';

wait for 4 ns;

wait for 1 ns;

--------------------------

clk <= '1'; --	 Cycle No: 771

wait for 1 ns;

I <= "1001";
CCEN_BAR <= '0';
CC_BAR <= '0';
RLD_BAR <= '1';
CI <= '0';
OEbar <= '0';
D <= "000000000000";

wait for 4 ns;

clk <= '0';

wait for 4 ns;

assert (Y = "000000000000")
report
"Assert 490 : < Y /= 000000000000 >" -- 	Vector No: 344
severity warning;

wait for 1 ns;

-- **************************************************
--------------------------

clk <= '1'; --	 Cycle No: 772

wait for 1 ns;

I <= "0000";
CCEN_BAR <= '0';
CC_BAR <= '0';
RLD_BAR <= '0';
CI <= '0';
OEbar <= '0';
D <= "111111111111";

wait for 4 ns;

clk <= '0';

wait for 4 ns;

wait for 1 ns;

--------------------------

clk <= '1'; --	 Cycle No: 773

wait for 1 ns;

I <= "1001";
CCEN_BAR <= '0';
CC_BAR <= '0';
RLD_BAR <= '1';
CI <= '0';
OEbar <= '0';
D <= "111111111111";

wait for 4 ns;

clk <= '0';

wait for 4 ns;

assert (Y = "111111111111")
report
"Assert 491 : < Y /= 111111111111 >" -- 	Vector No: 345
severity warning;

wait for 1 ns;

-- **************************************************
-- **************************************************
--------------------------

clk <= '1'; --	 Cycle No: 774

wait for 1 ns;

I <= "0000";
CCEN_BAR <= '0';
CC_BAR <= '0';
RLD_BAR <= '0';
CI <= '0';
OEbar <= '0';
D <= "111111111111";

wait for 4 ns;

clk <= '0';

wait for 4 ns;

wait for 1 ns;

--------------------------

clk <= '1'; --	 Cycle No: 775

wait for 1 ns;

I <= "1001";
CCEN_BAR <= '0';
CC_BAR <= '1';
RLD_BAR <= '1';
CI <= '0';
OEbar <= '0';
D <= "000000000000";

wait for 4 ns;

clk <= '0';

wait for 4 ns;

assert (Y = "000000000000")
report
"Assert 492 : < Y /= 000000000000 >" -- 	Vector No: 346
severity warning;

wait for 1 ns;

-- **************************************************
--------------------------

clk <= '1'; --	 Cycle No: 776

wait for 1 ns;

I <= "0000";
CCEN_BAR <= '0';
CC_BAR <= '0';
RLD_BAR <= '0';
CI <= '0';
OEbar <= '0';
D <= "111111111111";

wait for 4 ns;

clk <= '0';

wait for 4 ns;

wait for 1 ns;

--------------------------

clk <= '1'; --	 Cycle No: 777

wait for 1 ns;

I <= "1001";
CCEN_BAR <= '0';
CC_BAR <= '1';
RLD_BAR <= '1';
CI <= '0';
OEbar <= '0';
D <= "111111111111";

wait for 4 ns;

clk <= '0';

wait for 4 ns;

assert (Y = "111111111111")
report
"Assert 493 : < Y /= 111111111111 >" -- 	Vector No: 347
severity warning;

wait for 1 ns;

-- **************************************************
-- **************************************************
--------------------------

clk <= '1'; --	 Cycle No: 778

wait for 1 ns;

I <= "0000";
CCEN_BAR <= '0';
CC_BAR <= '0';
RLD_BAR <= '0';
CI <= '0';
OEbar <= '0';
D <= "111111111111";

wait for 4 ns;

clk <= '0';

wait for 4 ns;

wait for 1 ns;

--------------------------

clk <= '1'; --	 Cycle No: 779

wait for 1 ns;

I <= "1001";
CCEN_BAR <= '1';
CC_BAR <= '1';
RLD_BAR <= '1';
CI <= '0';
OEbar <= '0';
D <= "000000000000";

wait for 4 ns;

clk <= '0';

wait for 4 ns;

assert (Y = "000000000000")
report
"Assert 494 : < Y /= 000000000000 >" -- 	Vector No: 348
severity warning;

wait for 1 ns;

-- **************************************************
--------------------------

clk <= '1'; --	 Cycle No: 780

wait for 1 ns;

I <= "0000";
CCEN_BAR <= '0';
CC_BAR <= '0';
RLD_BAR <= '0';
CI <= '0';
OEbar <= '0';
D <= "111111111111";

wait for 4 ns;

clk <= '0';

wait for 4 ns;

wait for 1 ns;

--------------------------

clk <= '1'; --	 Cycle No: 781

wait for 1 ns;

I <= "1001";
CCEN_BAR <= '1';
CC_BAR <= '1';
RLD_BAR <= '1';
CI <= '0';
OEbar <= '0';
D <= "111111111111";

wait for 4 ns;

clk <= '0';

wait for 4 ns;

assert (Y = "111111111111")
report
"Assert 495 : < Y /= 111111111111 >" -- 	Vector No: 349
severity warning;

wait for 1 ns;

-- **************************************************
-- **************************************************
--------------------------

clk <= '1'; --	 Cycle No: 782

wait for 1 ns;

I <= "0000";
CCEN_BAR <= '0';
CC_BAR <= '0';
RLD_BAR <= '0';
CI <= '0';
OEbar <= '0';
D <= "111111111111";

wait for 4 ns;

clk <= '0';

wait for 4 ns;

wait for 1 ns;

--------------------------

clk <= '1'; --	 Cycle No: 783

wait for 1 ns;

I <= "1001";
CCEN_BAR <= '1';
CC_BAR <= '0';
RLD_BAR <= '1';
CI <= '0';
OEbar <= '0';
D <= "000000000000";

wait for 4 ns;

clk <= '0';

wait for 4 ns;

assert (Y = "000000000000")
report
"Assert 496 : < Y /= 000000000000 >" -- 	Vector No: 350
severity warning;

wait for 1 ns;

-- **************************************************
--------------------------

clk <= '1'; --	 Cycle No: 784

wait for 1 ns;

I <= "0000";
CCEN_BAR <= '0';
CC_BAR <= '0';
RLD_BAR <= '0';
CI <= '0';
OEbar <= '0';
D <= "111111111111";

wait for 4 ns;

clk <= '0';

wait for 4 ns;

wait for 1 ns;

--------------------------

clk <= '1'; --	 Cycle No: 785

wait for 1 ns;

I <= "1001";
CCEN_BAR <= '1';
CC_BAR <= '0';
RLD_BAR <= '1';
CI <= '0';
OEbar <= '0';
D <= "111111111111";

wait for 4 ns;

clk <= '0';

wait for 4 ns;

assert (Y = "111111111111")
report
"Assert 497 : < Y /= 111111111111 >" -- 	Vector No: 351
severity warning;

wait for 1 ns;

-- **************************************************
-- **************************************************
-- ** I = 11 **
--------------------------

clk <= '1'; --	 Cycle No: 786

wait for 1 ns;

I <= "0000";
CCEN_BAR <= '0';
CC_BAR <= '0';
RLD_BAR <= '0';
CI <= '0';
OEbar <= '0';
D <= "000000000000";

wait for 4 ns;

clk <= '0';

wait for 4 ns;

wait for 1 ns;

--------------------------

clk <= '1'; --	 Cycle No: 787

wait for 1 ns;

I <= "1011";
CCEN_BAR <= '0';
CC_BAR <= '0';
RLD_BAR <= '1';
CI <= '0';
OEbar <= '0';
D <= "000000000000";

wait for 4 ns;

clk <= '0';

wait for 4 ns;

assert (Y = "000000000000")
report
"Assert 498 : < Y /= 000000000000 >" -- 	Vector No: 352
severity warning;

wait for 1 ns;

-- **************************************************
--------------------------

clk <= '1'; --	 Cycle No: 788

wait for 1 ns;

I <= "0000";
CCEN_BAR <= '0';
CC_BAR <= '0';
RLD_BAR <= '0';
CI <= '0';
OEbar <= '0';
D <= "000000000000";

wait for 4 ns;

clk <= '0';

wait for 4 ns;

wait for 1 ns;

--------------------------

clk <= '1'; --	 Cycle No: 789

wait for 1 ns;

I <= "1011";
CCEN_BAR <= '0';
CC_BAR <= '0';
RLD_BAR <= '1';
CI <= '0';
OEbar <= '0';
D <= "111111111111";

wait for 4 ns;

clk <= '0';

wait for 4 ns;

assert (Y = "111111111111")
report
"Assert 499 : < Y /= 111111111111 >" -- 	Vector No: 353
severity warning;

wait for 1 ns;

-- **************************************************
--------------------------

clk <= '1'; --	 Cycle No: 790

wait for 1 ns;

I <= "0000";
CCEN_BAR <= '0';
CC_BAR <= '0';
RLD_BAR <= '0';
CI <= '0';
OEbar <= '0';
D <= "111111111111";

wait for 4 ns;

clk <= '0';

wait for 4 ns;

wait for 1 ns;

--------------------------

clk <= '1'; --	 Cycle No: 791

wait for 1 ns;

I <= "1011";
CCEN_BAR <= '0';
CC_BAR <= '0';
RLD_BAR <= '1';
CI <= '0';
OEbar <= '0';
D <= "000000000000";

wait for 4 ns;

clk <= '0';

wait for 4 ns;

assert (Y = "000000000000")
report
"Assert 500 : < Y /= 000000000000 >" -- 	Vector No: 354
severity warning;

wait for 1 ns;

-- **************************************************
--------------------------

clk <= '1'; --	 Cycle No: 792

wait for 1 ns;

I <= "0000";
CCEN_BAR <= '0';
CC_BAR <= '0';
RLD_BAR <= '0';
CI <= '0';
OEbar <= '0';
D <= "111111111111";

wait for 4 ns;

clk <= '0';

wait for 4 ns;

wait for 1 ns;

--------------------------

clk <= '1'; --	 Cycle No: 793

wait for 1 ns;

I <= "1011";
CCEN_BAR <= '0';
CC_BAR <= '0';
RLD_BAR <= '1';
CI <= '0';
OEbar <= '0';
D <= "111111111111";

wait for 4 ns;

clk <= '0';

wait for 4 ns;

assert (Y = "111111111111")
report
"Assert 501 : < Y /= 111111111111 >" -- 	Vector No: 355
severity warning;

wait for 1 ns;

-- **************************************************
-- **************************************************
--------------------------

clk <= '1'; --	 Cycle No: 794

wait for 1 ns;

I <= "0000";
CCEN_BAR <= '0';
CC_BAR <= '0';
RLD_BAR <= '0';
CI <= '0';
OEbar <= '0';
D <= "000000000000";

wait for 4 ns;

clk <= '0';

wait for 4 ns;

wait for 1 ns;

--------------------------

clk <= '1'; --	 Cycle No: 795

wait for 1 ns;

I <= "1011";
CCEN_BAR <= '1';
CC_BAR <= '1';
RLD_BAR <= '1';
CI <= '0';
OEbar <= '0';
D <= "000000000000";

wait for 4 ns;

clk <= '0';

wait for 4 ns;

assert (Y = "000000000000")
report
"Assert 502 : < Y /= 000000000000 >" -- 	Vector No: 356
severity warning;

wait for 1 ns;

-- **************************************************
--------------------------

clk <= '1'; --	 Cycle No: 796

wait for 1 ns;

I <= "0000";
CCEN_BAR <= '0';
CC_BAR <= '0';
RLD_BAR <= '0';
CI <= '0';
OEbar <= '0';
D <= "000000000000";

wait for 4 ns;

clk <= '0';

wait for 4 ns;

wait for 1 ns;

--------------------------

clk <= '1'; --	 Cycle No: 797

wait for 1 ns;

I <= "1011";
CCEN_BAR <= '1';
CC_BAR <= '1';
RLD_BAR <= '1';
CI <= '0';
OEbar <= '0';
D <= "111111111111";

wait for 4 ns;

clk <= '0';

wait for 4 ns;

assert (Y = "111111111111")
report
"Assert 503 : < Y /= 111111111111 >" -- 	Vector No: 357
severity warning;

wait for 1 ns;

-- **************************************************
--------------------------

clk <= '1'; --	 Cycle No: 798

wait for 1 ns;

I <= "0000";
CCEN_BAR <= '0';
CC_BAR <= '0';
RLD_BAR <= '0';
CI <= '0';
OEbar <= '0';
D <= "111111111111";

wait for 4 ns;

clk <= '0';

wait for 4 ns;

wait for 1 ns;

--------------------------

clk <= '1'; --	 Cycle No: 799

wait for 1 ns;

I <= "1011";
CCEN_BAR <= '1';
CC_BAR <= '1';
RLD_BAR <= '1';
CI <= '0';
OEbar <= '0';
D <= "000000000000";

wait for 4 ns;

clk <= '0';

wait for 4 ns;

assert (Y = "000000000000")
report
"Assert 504 : < Y /= 000000000000 >" -- 	Vector No: 358
severity warning;

wait for 1 ns;

-- **************************************************
--------------------------

clk <= '1'; --	 Cycle No: 800

wait for 1 ns;

I <= "0000";
CCEN_BAR <= '0';
CC_BAR <= '0';
RLD_BAR <= '0';
CI <= '0';
OEbar <= '0';
D <= "111111111111";

wait for 4 ns;

clk <= '0';

wait for 4 ns;

wait for 1 ns;

--------------------------

clk <= '1'; --	 Cycle No: 801

wait for 1 ns;

I <= "1011";
CCEN_BAR <= '1';
CC_BAR <= '1';
RLD_BAR <= '1';
CI <= '0';
OEbar <= '0';
D <= "111111111111";

wait for 4 ns;

clk <= '0';

wait for 4 ns;

assert (Y = "111111111111")
report
"Assert 505 : < Y /= 111111111111 >" -- 	Vector No: 359
severity warning;

wait for 1 ns;

-- **************************************************
-- **************************************************
--------------------------

clk <= '1'; --	 Cycle No: 802

wait for 1 ns;

I <= "0000";
CCEN_BAR <= '0';
CC_BAR <= '0';
RLD_BAR <= '0';
CI <= '0';
OEbar <= '0';
D <= "000000000000";

wait for 4 ns;

clk <= '0';

wait for 4 ns;

wait for 1 ns;

--------------------------

clk <= '1'; --	 Cycle No: 803

wait for 1 ns;

I <= "1011";
CCEN_BAR <= '1';
CC_BAR <= '0';
RLD_BAR <= '1';
CI <= '0';
OEbar <= '0';
D <= "000000000000";

wait for 4 ns;

clk <= '0';

wait for 4 ns;

assert (Y = "000000000000")
report
"Assert 506 : < Y /= 000000000000 >" -- 	Vector No: 360
severity warning;

wait for 1 ns;

-- **************************************************
--------------------------

clk <= '1'; --	 Cycle No: 804

wait for 1 ns;

I <= "0000";
CCEN_BAR <= '0';
CC_BAR <= '0';
RLD_BAR <= '0';
CI <= '0';
OEbar <= '0';
D <= "000000000000";

wait for 4 ns;

clk <= '0';

wait for 4 ns;

wait for 1 ns;

--------------------------

clk <= '1'; --	 Cycle No: 805

wait for 1 ns;

I <= "1011";
CCEN_BAR <= '1';
CC_BAR <= '0';
RLD_BAR <= '1';
CI <= '0';
OEbar <= '0';
D <= "111111111111";

wait for 4 ns;

clk <= '0';

wait for 4 ns;

assert (Y = "111111111111")
report
"Assert 507 : < Y /= 111111111111 >" -- 	Vector No: 361
severity warning;

wait for 1 ns;

-- **************************************************
--------------------------

clk <= '1'; --	 Cycle No: 806

wait for 1 ns;

I <= "0000";
CCEN_BAR <= '0';
CC_BAR <= '0';
RLD_BAR <= '0';
CI <= '0';
OEbar <= '0';
D <= "111111111111";

wait for 4 ns;

clk <= '0';

wait for 4 ns;

wait for 1 ns;

--------------------------

clk <= '1'; --	 Cycle No: 807

wait for 1 ns;

I <= "1011";
CCEN_BAR <= '1';
CC_BAR <= '0';
RLD_BAR <= '1';
CI <= '0';
OEbar <= '0';
D <= "000000000000";

wait for 4 ns;

clk <= '0';

wait for 4 ns;

assert (Y = "000000000000")
report
"Assert 508 : < Y /= 000000000000 >" -- 	Vector No: 362
severity warning;

wait for 1 ns;

-- **************************************************
--------------------------

clk <= '1'; --	 Cycle No: 808

wait for 1 ns;

I <= "0000";
CCEN_BAR <= '0';
CC_BAR <= '0';
RLD_BAR <= '0';
CI <= '0';
OEbar <= '0';
D <= "111111111111";

wait for 4 ns;

clk <= '0';

wait for 4 ns;

wait for 1 ns;

--------------------------

clk <= '1'; --	 Cycle No: 809

wait for 1 ns;

I <= "1011";
CCEN_BAR <= '1';
CC_BAR <= '0';
RLD_BAR <= '1';
CI <= '0';
OEbar <= '0';
D <= "111111111111";

wait for 4 ns;

clk <= '0';

wait for 4 ns;

assert (Y = "111111111111")
report
"Assert 509 : < Y /= 111111111111 >" -- 	Vector No: 363
severity warning;

wait for 1 ns;

-- **************************************************
-- **************************************************
-- ** I = 15 **
--------------------------

clk <= '1'; --	 Cycle No: 810

wait for 1 ns;

I <= "0000";
CCEN_BAR <= '0';
CC_BAR <= '0';
RLD_BAR <= '0';
CI <= '0';
OEbar <= '0';
D <= "000000000000";

wait for 4 ns;

clk <= '0';

wait for 4 ns;

wait for 1 ns;

--------------------------

clk <= '1'; --	 Cycle No: 811

wait for 1 ns;

I <= "1111";
CCEN_BAR <= '0';
CC_BAR <= '1';
RLD_BAR <= '1';
CI <= '0';
OEbar <= '0';
D <= "000000000000";

wait for 4 ns;

clk <= '0';

wait for 4 ns;

assert (Y = "000000000000")
report
"Assert 510 : < Y /= 000000000000 >" -- 	Vector No: 364
severity warning;

wait for 1 ns;

-- **************************************************
--------------------------

clk <= '1'; --	 Cycle No: 812

wait for 1 ns;

I <= "0000";
CCEN_BAR <= '0';
CC_BAR <= '0';
RLD_BAR <= '0';
CI <= '0';
OEbar <= '0';
D <= "000000000000";

wait for 4 ns;

clk <= '0';

wait for 4 ns;

wait for 1 ns;

--------------------------

clk <= '1'; --	 Cycle No: 813

wait for 1 ns;

I <= "1111";
CCEN_BAR <= '0';
CC_BAR <= '1';
RLD_BAR <= '1';
CI <= '0';
OEbar <= '0';
D <= "111111111111";

wait for 4 ns;

clk <= '0';

wait for 4 ns;

assert (Y = "111111111111")
report
"Assert 511 : < Y /= 111111111111 >" -- 	Vector No: 365
severity warning;

wait for 1 ns;

-- **************************************************
-- **************************************************
-- ***************************
-- ******* SELECT REG ********
-- ***************************
-- ** I = 5 **
--------------------------

clk <= '1'; --	 Cycle No: 814

wait for 1 ns;

I <= "0000";
CCEN_BAR <= '0';
CC_BAR <= '0';
RLD_BAR <= '0';
CI <= '0';
OEbar <= '0';
D <= "000000000000";

wait for 4 ns;

clk <= '0';

wait for 4 ns;

wait for 1 ns;

--------------------------

clk <= '1'; --	 Cycle No: 815

wait for 1 ns;

I <= "0101";
CCEN_BAR <= '0';
CC_BAR <= '1';
RLD_BAR <= '1';
CI <= '0';
OEbar <= '0';

wait for 4 ns;

clk <= '0';

wait for 4 ns;

assert (Y = "000000000000")
report
"Assert 512 : < Y /= 000000000000 >" -- 	Vector No: 366
severity warning;

wait for 1 ns;

-- **************************************************
--------------------------

clk <= '1'; --	 Cycle No: 816

wait for 1 ns;

I <= "0000";
CCEN_BAR <= '0';
CC_BAR <= '0';
RLD_BAR <= '0';
CI <= '0';
OEbar <= '0';
D <= "111111111111";

wait for 4 ns;

clk <= '0';

wait for 4 ns;

wait for 1 ns;

--------------------------

clk <= '1'; --	 Cycle No: 817

wait for 1 ns;

I <= "0101";
CCEN_BAR <= '0';
CC_BAR <= '1';
RLD_BAR <= '1';
CI <= '0';
OEbar <= '0';

wait for 4 ns;

clk <= '0';

wait for 4 ns;

assert (Y = "111111111111")
report
"Assert 513 : < Y /= 111111111111 >" -- 	Vector No: 367
severity warning;

wait for 1 ns;

-- **************************************************
-- ** I = 7 **
--------------------------

clk <= '1'; --	 Cycle No: 818

wait for 1 ns;

I <= "0000";
CCEN_BAR <= '0';
CC_BAR <= '0';
RLD_BAR <= '0';
CI <= '0';
OEbar <= '0';
D <= "000000000000";

wait for 4 ns;

clk <= '0';

wait for 4 ns;

wait for 1 ns;

--------------------------

clk <= '1'; --	 Cycle No: 819

wait for 1 ns;

I <= "0111";
CCEN_BAR <= '0';
CC_BAR <= '1';
RLD_BAR <= '1';
CI <= '0';
OEbar <= '0';

wait for 4 ns;

clk <= '0';

wait for 4 ns;

assert (Y = "000000000000")
report
"Assert 514 : < Y /= 000000000000 >" -- 	Vector No: 368
severity warning;

wait for 1 ns;

-- **************************************************
--------------------------

clk <= '1'; --	 Cycle No: 820

wait for 1 ns;

I <= "0000";
CCEN_BAR <= '0';
CC_BAR <= '0';
RLD_BAR <= '0';
CI <= '0';
OEbar <= '0';
D <= "111111111111";

wait for 4 ns;

clk <= '0';

wait for 4 ns;

wait for 1 ns;

--------------------------

clk <= '1'; --	 Cycle No: 821

wait for 1 ns;

I <= "0111";
CCEN_BAR <= '0';
CC_BAR <= '1';
RLD_BAR <= '1';
CI <= '0';
OEbar <= '0';

wait for 4 ns;

clk <= '0';

wait for 4 ns;

assert (Y = "111111111111")
report
"Assert 515 : < Y /= 111111111111 >" -- 	Vector No: 369
severity warning;

wait for 1 ns;

-- **************************************************
-- ******* SELECT STACK ********
-- *****************************
-- ** I = 8 **
--------------------------

clk <= '1'; --	 Cycle No: 822

wait for 1 ns;

I <= "0000";
CCEN_BAR <= '0';
CC_BAR <= '0';
RLD_BAR <= '0';
CI <= '0';
OEbar <= '0';
D <= "111111111111";

wait for 4 ns;

clk <= '0';

wait for 4 ns;

wait for 1 ns;

--------------------------

clk <= '1'; --	 Cycle No: 823

wait for 1 ns;

I <= "0010";
CCEN_BAR <= '0';
CC_BAR <= '0';
RLD_BAR <= '1';
CI <= '0';
OEbar <= '0';
D <= "000000000000";

wait for 4 ns;

clk <= '0';

wait for 4 ns;

wait for 1 ns;

--------------------------

clk <= '1'; --	 Cycle No: 824

wait for 1 ns;

I <= "0001";
CCEN_BAR <= '1';
CC_BAR <= '1';
RLD_BAR <= '1';
CI <= '0';
OEbar <= '0';

wait for 4 ns;

clk <= '0';

wait for 4 ns;

wait for 1 ns;

--------------------------

clk <= '1'; --	 Cycle No: 825

wait for 1 ns;

I <= "1010";
CCEN_BAR <= '0';
CC_BAR <= '0';
RLD_BAR <= '1';
CI <= '0';
OEbar <= '0';

wait for 4 ns;

clk <= '0';

wait for 4 ns;

assert (Y = "000000000000")
report
"Assert 516 : < Y /= 000000000000 >" -- 	Vector No: 370
severity warning;

wait for 1 ns;

-- **************************************************
--------------------------

clk <= '1'; --	 Cycle No: 826

wait for 1 ns;

I <= "0000";
CCEN_BAR <= '0';
CC_BAR <= '0';
RLD_BAR <= '0';
CI <= '0';
OEbar <= '0';
D <= "111111111111";

wait for 4 ns;

clk <= '0';

wait for 4 ns;

wait for 1 ns;

--------------------------

clk <= '1'; --	 Cycle No: 827

wait for 1 ns;

I <= "0010";
CCEN_BAR <= '0';
CC_BAR <= '0';
RLD_BAR <= '1';
CI <= '0';
OEbar <= '0';
D <= "111111111111";

wait for 4 ns;

clk <= '0';

wait for 4 ns;

wait for 1 ns;

--------------------------

clk <= '1'; --	 Cycle No: 828

wait for 1 ns;

I <= "0001";
CCEN_BAR <= '1';
CC_BAR <= '1';
RLD_BAR <= '1';
CI <= '0';
OEbar <= '0';

wait for 4 ns;

clk <= '0';

wait for 4 ns;

wait for 1 ns;

--------------------------

clk <= '1'; --	 Cycle No: 829

wait for 1 ns;

I <= "1010";
CCEN_BAR <= '0';
CC_BAR <= '0';
RLD_BAR <= '1';
CI <= '0';
OEbar <= '0';

wait for 4 ns;

clk <= '0';

wait for 4 ns;

assert (Y = "111111111111")
report
"Assert 517 : < Y /= 111111111111 >" -- 	Vector No: 371
severity warning;

wait for 1 ns;

-- **************************************************
-- **************************************************
--------------------------

clk <= '1'; --	 Cycle No: 830

wait for 1 ns;

I <= "0000";
CCEN_BAR <= '0';
CC_BAR <= '0';
RLD_BAR <= '0';
CI <= '0';
OEbar <= '0';
D <= "111111111111";

wait for 4 ns;

clk <= '0';

wait for 4 ns;

wait for 1 ns;

--------------------------

clk <= '1'; --	 Cycle No: 831

wait for 1 ns;

I <= "0010";
CCEN_BAR <= '0';
CC_BAR <= '0';
RLD_BAR <= '1';
CI <= '0';
OEbar <= '0';
D <= "000000000000";

wait for 4 ns;

clk <= '0';

wait for 4 ns;

wait for 1 ns;

--------------------------

clk <= '1'; --	 Cycle No: 832

wait for 1 ns;

I <= "0001";
CCEN_BAR <= '1';
CC_BAR <= '1';
RLD_BAR <= '1';
CI <= '0';
OEbar <= '0';

wait for 4 ns;

clk <= '0';

wait for 4 ns;

wait for 1 ns;

--------------------------

clk <= '1'; --	 Cycle No: 833

wait for 1 ns;

I <= "1010";
CCEN_BAR <= '1';
CC_BAR <= '0';
RLD_BAR <= '1';
CI <= '0';
OEbar <= '0';

wait for 4 ns;

clk <= '0';

wait for 4 ns;

assert (Y = "000000000000")
report
"Assert 518 : < Y /= 000000000000 >" -- 	Vector No: 372
severity warning;

wait for 1 ns;

-- **************************************************
--------------------------

clk <= '1'; --	 Cycle No: 834

wait for 1 ns;

I <= "0000";
CCEN_BAR <= '0';
CC_BAR <= '0';
RLD_BAR <= '0';
CI <= '0';
OEbar <= '0';
D <= "111111111111";

wait for 4 ns;

clk <= '0';

wait for 4 ns;

wait for 1 ns;

--------------------------

clk <= '1'; --	 Cycle No: 835

wait for 1 ns;

I <= "0010";
CCEN_BAR <= '0';
CC_BAR <= '0';
RLD_BAR <= '1';
CI <= '0';
OEbar <= '0';
D <= "111111111111";

wait for 4 ns;

clk <= '0';

wait for 4 ns;

wait for 1 ns;

--------------------------

clk <= '1'; --	 Cycle No: 836

wait for 1 ns;

I <= "0001";
CCEN_BAR <= '1';
CC_BAR <= '1';
RLD_BAR <= '1';
CI <= '0';
OEbar <= '0';

wait for 4 ns;

clk <= '0';

wait for 4 ns;

wait for 1 ns;

--------------------------

clk <= '1'; --	 Cycle No: 837

wait for 1 ns;

I <= "1010";
CCEN_BAR <= '1';
CC_BAR <= '0';
RLD_BAR <= '1';
CI <= '0';
OEbar <= '0';

wait for 4 ns;

clk <= '0';

wait for 4 ns;

assert (Y = "111111111111")
report
"Assert 519 : < Y /= 111111111111 >" -- 	Vector No: 373
severity warning;

wait for 1 ns;

-- **************************************************
-- **************************************************
--------------------------

clk <= '1'; --	 Cycle No: 838

wait for 1 ns;

I <= "0000";
CCEN_BAR <= '0';
CC_BAR <= '0';
RLD_BAR <= '0';
CI <= '0';
OEbar <= '0';
D <= "111111111111";

wait for 4 ns;

clk <= '0';

wait for 4 ns;

wait for 1 ns;

--------------------------

clk <= '1'; --	 Cycle No: 839

wait for 1 ns;

I <= "0010";
CCEN_BAR <= '0';
CC_BAR <= '0';
RLD_BAR <= '1';
CI <= '0';
OEbar <= '0';
D <= "000000000000";

wait for 4 ns;

clk <= '0';

wait for 4 ns;

wait for 1 ns;

--------------------------

clk <= '1'; --	 Cycle No: 840

wait for 1 ns;

I <= "0001";
CCEN_BAR <= '1';
CC_BAR <= '1';
RLD_BAR <= '1';
CI <= '0';
OEbar <= '0';

wait for 4 ns;

clk <= '0';

wait for 4 ns;

wait for 1 ns;

--------------------------

clk <= '1'; --	 Cycle No: 841

wait for 1 ns;

I <= "1010";
CCEN_BAR <= '1';
CC_BAR <= '0';
RLD_BAR <= '1';
CI <= '0';
OEbar <= '0';

wait for 4 ns;

clk <= '0';

wait for 4 ns;

assert (Y = "000000000000")
report
"Assert 520 : < Y /= 000000000000 >" -- 	Vector No: 374
severity warning;

wait for 1 ns;

-- **************************************************
--------------------------

clk <= '1'; --	 Cycle No: 842

wait for 1 ns;

I <= "0000";
CCEN_BAR <= '0';
CC_BAR <= '0';
RLD_BAR <= '0';
CI <= '0';
OEbar <= '0';
D <= "111111111111";

wait for 4 ns;

clk <= '0';

wait for 4 ns;

wait for 1 ns;

--------------------------

clk <= '1'; --	 Cycle No: 843

wait for 1 ns;

I <= "0010";
CCEN_BAR <= '0';
CC_BAR <= '0';
RLD_BAR <= '1';
CI <= '0';
OEbar <= '0';
D <= "111111111111";

wait for 4 ns;

clk <= '0';

wait for 4 ns;

wait for 1 ns;

--------------------------

clk <= '1'; --	 Cycle No: 844

wait for 1 ns;

I <= "0001";
CCEN_BAR <= '1';
CC_BAR <= '1';
RLD_BAR <= '1';
CI <= '0';
OEbar <= '0';

wait for 4 ns;

clk <= '0';

wait for 4 ns;

wait for 1 ns;

--------------------------

clk <= '1'; --	 Cycle No: 845

wait for 1 ns;

I <= "1010";
CCEN_BAR <= '1';
CC_BAR <= '0';
RLD_BAR <= '1';
CI <= '0';
OEbar <= '0';

wait for 4 ns;

clk <= '0';

wait for 4 ns;

assert (Y = "111111111111")
report
"Assert 521 : < Y /= 111111111111 >" -- 	Vector No: 375
severity warning;

wait for 1 ns;

-- **************************************************
-- **************************************************
--------------------------

clk <= '1'; --	 Cycle No: 846

wait for 1 ns;

I <= "0000";
CCEN_BAR <= '0';
CC_BAR <= '0';
RLD_BAR <= '0';
CI <= '0';
OEbar <= '0';
D <= "111111111111";

wait for 4 ns;

clk <= '0';

wait for 4 ns;

wait for 1 ns;

--------------------------

clk <= '1'; --	 Cycle No: 847

wait for 1 ns;

I <= "0010";
CCEN_BAR <= '0';
CC_BAR <= '0';
RLD_BAR <= '1';
CI <= '0';
OEbar <= '0';
D <= "000000000000";

wait for 4 ns;

clk <= '0';

wait for 4 ns;

wait for 1 ns;

--------------------------

clk <= '1'; --	 Cycle No: 848

wait for 1 ns;

I <= "0001";
CCEN_BAR <= '1';
CC_BAR <= '1';
RLD_BAR <= '1';
CI <= '0';
OEbar <= '0';

wait for 4 ns;

clk <= '0';

wait for 4 ns;

wait for 1 ns;

--------------------------

clk <= '1'; --	 Cycle No: 849

wait for 1 ns;

I <= "1010";
CCEN_BAR <= '1';
CC_BAR <= '0';
RLD_BAR <= '1';
CI <= '0';
OEbar <= '0';

wait for 4 ns;

clk <= '0';

wait for 4 ns;

assert (Y = "000000000000")
report
"Assert 522 : < Y /= 000000000000 >" -- 	Vector No: 376
severity warning;

wait for 1 ns;

-- **************************************************
--------------------------

clk <= '1'; --	 Cycle No: 850

wait for 1 ns;

I <= "0000";
CCEN_BAR <= '0';
CC_BAR <= '0';
RLD_BAR <= '0';
CI <= '0';
OEbar <= '0';
D <= "111111111111";

wait for 4 ns;

clk <= '0';

wait for 4 ns;

wait for 1 ns;

--------------------------

clk <= '1'; --	 Cycle No: 851

wait for 1 ns;

I <= "0010";
CCEN_BAR <= '0';
CC_BAR <= '0';
RLD_BAR <= '1';
CI <= '0';
OEbar <= '0';
D <= "111111111111";

wait for 4 ns;

clk <= '0';

wait for 4 ns;

wait for 1 ns;

--------------------------

clk <= '1'; --	 Cycle No: 852

wait for 1 ns;

I <= "0001";
CCEN_BAR <= '1';
CC_BAR <= '1';
RLD_BAR <= '1';
CI <= '0';
OEbar <= '0';

wait for 4 ns;

clk <= '0';

wait for 4 ns;

wait for 1 ns;

--------------------------

clk <= '1'; --	 Cycle No: 853

wait for 1 ns;

I <= "1010";
CCEN_BAR <= '1';
CC_BAR <= '0';
RLD_BAR <= '1';
CI <= '0';
OEbar <= '0';

wait for 4 ns;

clk <= '0';

wait for 4 ns;

assert (Y = "111111111111")
report
"Assert 523 : < Y /= 111111111111 >" -- 	Vector No: 377
severity warning;

wait for 1 ns;

-- **************************************************
-- **************************************************
-- ** I = 10 **
--------------------------

clk <= '1'; --	 Cycle No: 854

wait for 1 ns;

I <= "0000";
CCEN_BAR <= '0';
CC_BAR <= '0';
RLD_BAR <= '0';
CI <= '0';
OEbar <= '0';
D <= "000000000000";

wait for 4 ns;

clk <= '0';

wait for 4 ns;

wait for 1 ns;

--------------------------

clk <= '1'; --	 Cycle No: 855

wait for 1 ns;

I <= "0010";
CCEN_BAR <= '0';
CC_BAR <= '0';
RLD_BAR <= '1';
CI <= '0';
OEbar <= '0';
D <= "000000000000";

wait for 4 ns;

clk <= '0';

wait for 4 ns;

wait for 1 ns;

--------------------------

clk <= '1'; --	 Cycle No: 856

wait for 1 ns;

I <= "0001";
CCEN_BAR <= '1';
CC_BAR <= '1';
RLD_BAR <= '1';
CI <= '0';
OEbar <= '0';

wait for 4 ns;

clk <= '0';

wait for 4 ns;

wait for 1 ns;

--------------------------

clk <= '1'; --	 Cycle No: 857

wait for 1 ns;

I <= "1010";
CCEN_BAR <= '0';
CC_BAR <= '0';
RLD_BAR <= '1';
CI <= '0';
OEbar <= '0';

wait for 4 ns;

clk <= '0';

wait for 4 ns;

assert (Y = "000000000000")
report
"Assert 524 : < Y /= 000000000000 >" -- 	Vector No: 378
severity warning;

wait for 1 ns;

-- **************************************************
--------------------------

clk <= '1'; --	 Cycle No: 858

wait for 1 ns;

I <= "0000";
CCEN_BAR <= '0';
CC_BAR <= '0';
RLD_BAR <= '0';
CI <= '0';
OEbar <= '0';
D <= "000000000000";

wait for 4 ns;

clk <= '0';

wait for 4 ns;

wait for 1 ns;

--------------------------

clk <= '1'; --	 Cycle No: 859

wait for 1 ns;

I <= "0010";
CCEN_BAR <= '0';
CC_BAR <= '0';
RLD_BAR <= '1';
CI <= '0';
OEbar <= '0';
D <= "111111111111";

wait for 4 ns;

clk <= '0';

wait for 4 ns;

wait for 1 ns;

--------------------------

clk <= '1'; --	 Cycle No: 860

wait for 1 ns;

I <= "0001";
CCEN_BAR <= '1';
CC_BAR <= '1';
RLD_BAR <= '1';
CI <= '0';
OEbar <= '0';

wait for 4 ns;

clk <= '0';

wait for 4 ns;

wait for 1 ns;

--------------------------

clk <= '1'; --	 Cycle No: 861

wait for 1 ns;

I <= "1010";
CCEN_BAR <= '0';
CC_BAR <= '0';
RLD_BAR <= '1';
CI <= '0';
OEbar <= '0';

wait for 4 ns;

clk <= '0';

wait for 4 ns;

assert (Y = "111111111111")
report
"Assert 525 : < Y /= 111111111111 >" -- 	Vector No: 379
severity warning;

wait for 1 ns;

-- **************************************************
--------------------------

clk <= '1'; --	 Cycle No: 862

wait for 1 ns;

I <= "0000";
CCEN_BAR <= '0';
CC_BAR <= '0';
RLD_BAR <= '0';
CI <= '0';
OEbar <= '0';
D <= "111111111111";

wait for 4 ns;

clk <= '0';

wait for 4 ns;

wait for 1 ns;

--------------------------

clk <= '1'; --	 Cycle No: 863

wait for 1 ns;

I <= "0010";
CCEN_BAR <= '0';
CC_BAR <= '0';
RLD_BAR <= '1';
CI <= '0';
OEbar <= '0';
D <= "000000000000";

wait for 4 ns;

clk <= '0';

wait for 4 ns;

wait for 1 ns;

--------------------------

clk <= '1'; --	 Cycle No: 864

wait for 1 ns;

I <= "0001";
CCEN_BAR <= '1';
CC_BAR <= '1';
RLD_BAR <= '1';
CI <= '0';
OEbar <= '0';

wait for 4 ns;

clk <= '0';

wait for 4 ns;

wait for 1 ns;

--------------------------

clk <= '1'; --	 Cycle No: 865

wait for 1 ns;

I <= "1010";
CCEN_BAR <= '0';
CC_BAR <= '0';
RLD_BAR <= '1';
CI <= '0';
OEbar <= '0';

wait for 4 ns;

clk <= '0';

wait for 4 ns;

assert (Y = "000000000000")
report
"Assert 526 : < Y /= 000000000000 >" -- 	Vector No: 380
severity warning;

wait for 1 ns;

-- **************************************************
--------------------------

clk <= '1'; --	 Cycle No: 866

wait for 1 ns;

I <= "0000";
CCEN_BAR <= '0';
CC_BAR <= '0';
RLD_BAR <= '0';
CI <= '0';
OEbar <= '0';
D <= "111111111111";

wait for 4 ns;

clk <= '0';

wait for 4 ns;

wait for 1 ns;

--------------------------

clk <= '1'; --	 Cycle No: 867

wait for 1 ns;

I <= "0010";
CCEN_BAR <= '0';
CC_BAR <= '0';
RLD_BAR <= '1';
CI <= '0';
OEbar <= '0';
D <= "111111111111";

wait for 4 ns;

clk <= '0';

wait for 4 ns;

wait for 1 ns;

--------------------------

clk <= '1'; --	 Cycle No: 868

wait for 1 ns;

I <= "0001";
CCEN_BAR <= '1';
CC_BAR <= '1';
RLD_BAR <= '1';
CI <= '0';
OEbar <= '0';

wait for 4 ns;

clk <= '0';

wait for 4 ns;

wait for 1 ns;

--------------------------

clk <= '1'; --	 Cycle No: 869

wait for 1 ns;

I <= "1010";
CCEN_BAR <= '0';
CC_BAR <= '0';
RLD_BAR <= '1';
CI <= '0';
OEbar <= '0';

wait for 4 ns;

clk <= '0';

wait for 4 ns;

assert (Y = "111111111111")
report
"Assert 527 : < Y /= 111111111111 >" -- 	Vector No: 381
severity warning;

wait for 1 ns;

-- **************************************************
-- **************************************************
--------------------------

clk <= '1'; --	 Cycle No: 870

wait for 1 ns;

I <= "0000";
CCEN_BAR <= '0';
CC_BAR <= '0';
RLD_BAR <= '0';
CI <= '0';
OEbar <= '0';
D <= "000000000000";

wait for 4 ns;

clk <= '0';

wait for 4 ns;

wait for 1 ns;

--------------------------

clk <= '1'; --	 Cycle No: 871

wait for 1 ns;

I <= "0010";
CCEN_BAR <= '0';
CC_BAR <= '0';
RLD_BAR <= '1';
CI <= '0';
OEbar <= '0';
D <= "000000000000";

wait for 4 ns;

clk <= '0';

wait for 4 ns;

wait for 1 ns;

--------------------------

clk <= '1'; --	 Cycle No: 872

wait for 1 ns;

I <= "0001";
CCEN_BAR <= '1';
CC_BAR <= '1';
RLD_BAR <= '1';
CI <= '0';
OEbar <= '0';

wait for 4 ns;

clk <= '0';

wait for 4 ns;

wait for 1 ns;

--------------------------

clk <= '1'; --	 Cycle No: 873

wait for 1 ns;

I <= "1010";
CCEN_BAR <= '1';
CC_BAR <= '1';
RLD_BAR <= '1';
CI <= '0';
OEbar <= '0';

wait for 4 ns;

clk <= '0';

wait for 4 ns;

assert (Y = "000000000000")
report
"Assert 528 : < Y /= 000000000000 >" -- 	Vector No: 382
severity warning;

wait for 1 ns;

-- **************************************************
--------------------------

clk <= '1'; --	 Cycle No: 874

wait for 1 ns;

I <= "0000";
CCEN_BAR <= '0';
CC_BAR <= '0';
RLD_BAR <= '0';
CI <= '0';
OEbar <= '0';
D <= "000000000000";

wait for 4 ns;

clk <= '0';

wait for 4 ns;

wait for 1 ns;

--------------------------

clk <= '1'; --	 Cycle No: 875

wait for 1 ns;

I <= "0010";
CCEN_BAR <= '0';
CC_BAR <= '0';
RLD_BAR <= '1';
CI <= '0';
OEbar <= '0';
D <= "111111111111";

wait for 4 ns;

clk <= '0';

wait for 4 ns;

wait for 1 ns;

--------------------------

clk <= '1'; --	 Cycle No: 876

wait for 1 ns;

I <= "0001";
CCEN_BAR <= '1';
CC_BAR <= '1';
RLD_BAR <= '1';
CI <= '0';
OEbar <= '0';

wait for 4 ns;

clk <= '0';

wait for 4 ns;

wait for 1 ns;

--------------------------

clk <= '1'; --	 Cycle No: 877

wait for 1 ns;

I <= "1010";
CCEN_BAR <= '1';
CC_BAR <= '1';
RLD_BAR <= '1';
CI <= '0';
OEbar <= '0';

wait for 4 ns;

clk <= '0';

wait for 4 ns;

assert (Y = "111111111111")
report
"Assert 529 : < Y /= 111111111111 >" -- 	Vector No: 383
severity warning;

wait for 1 ns;

-- **************************************************
--------------------------

clk <= '1'; --	 Cycle No: 878

wait for 1 ns;

I <= "0000";
CCEN_BAR <= '0';
CC_BAR <= '0';
RLD_BAR <= '0';
CI <= '0';
OEbar <= '0';
D <= "111111111111";

wait for 4 ns;

clk <= '0';

wait for 4 ns;

wait for 1 ns;

--------------------------

clk <= '1'; --	 Cycle No: 879

wait for 1 ns;

I <= "0010";
CCEN_BAR <= '0';
CC_BAR <= '0';
RLD_BAR <= '1';
CI <= '0';
OEbar <= '0';
D <= "000000000000";

wait for 4 ns;

clk <= '0';

wait for 4 ns;

wait for 1 ns;

--------------------------

clk <= '1'; --	 Cycle No: 880

wait for 1 ns;

I <= "0001";
CCEN_BAR <= '1';
CC_BAR <= '1';
RLD_BAR <= '1';
CI <= '0';
OEbar <= '0';

wait for 4 ns;

clk <= '0';

wait for 4 ns;

wait for 1 ns;

--------------------------

clk <= '1'; --	 Cycle No: 881

wait for 1 ns;

I <= "1010";
CCEN_BAR <= '1';
CC_BAR <= '1';
RLD_BAR <= '1';
CI <= '0';
OEbar <= '0';

wait for 4 ns;

clk <= '0';

wait for 4 ns;

assert (Y = "000000000000")
report
"Assert 530 : < Y /= 000000000000 >" -- 	Vector No: 384
severity warning;

wait for 1 ns;

-- **************************************************
--------------------------

clk <= '1'; --	 Cycle No: 882

wait for 1 ns;

I <= "0000";
CCEN_BAR <= '0';
CC_BAR <= '0';
RLD_BAR <= '0';
CI <= '0';
OEbar <= '0';
D <= "111111111111";

wait for 4 ns;

clk <= '0';

wait for 4 ns;

wait for 1 ns;

--------------------------

clk <= '1'; --	 Cycle No: 883

wait for 1 ns;

I <= "0010";
CCEN_BAR <= '0';
CC_BAR <= '0';
RLD_BAR <= '1';
CI <= '0';
OEbar <= '0';
D <= "111111111111";

wait for 4 ns;

clk <= '0';

wait for 4 ns;

wait for 1 ns;

--------------------------

clk <= '1'; --	 Cycle No: 884

wait for 1 ns;

I <= "0001";
CCEN_BAR <= '1';
CC_BAR <= '1';
RLD_BAR <= '1';
CI <= '0';
OEbar <= '0';

wait for 4 ns;

clk <= '0';

wait for 4 ns;

wait for 1 ns;

--------------------------

clk <= '1'; --	 Cycle No: 885

wait for 1 ns;

I <= "1010";
CCEN_BAR <= '1';
CC_BAR <= '1';
RLD_BAR <= '1';
CI <= '0';
OEbar <= '0';

wait for 4 ns;

clk <= '0';

wait for 4 ns;

assert (Y = "111111111111")
report
"Assert 531 : < Y /= 111111111111 >" -- 	Vector No: 385
severity warning;

wait for 1 ns;

-- **************************************************
-- **************************************************
--------------------------

clk <= '1'; --	 Cycle No: 886

wait for 1 ns;

I <= "0000";
CCEN_BAR <= '0';
CC_BAR <= '0';
RLD_BAR <= '0';
CI <= '0';
OEbar <= '0';
D <= "000000000000";

wait for 4 ns;

clk <= '0';

wait for 4 ns;

wait for 1 ns;

--------------------------

clk <= '1'; --	 Cycle No: 887

wait for 1 ns;

I <= "0010";
CCEN_BAR <= '0';
CC_BAR <= '0';
RLD_BAR <= '1';
CI <= '0';
OEbar <= '0';
D <= "000000000000";

wait for 4 ns;

clk <= '0';

wait for 4 ns;

wait for 1 ns;

--------------------------

clk <= '1'; --	 Cycle No: 888

wait for 1 ns;

I <= "0001";
CCEN_BAR <= '1';
CC_BAR <= '1';
RLD_BAR <= '1';
CI <= '0';
OEbar <= '0';

wait for 4 ns;

clk <= '0';

wait for 4 ns;

wait for 1 ns;

--------------------------

clk <= '1'; --	 Cycle No: 889

wait for 1 ns;

I <= "1010";
CCEN_BAR <= '1';
CC_BAR <= '0';
RLD_BAR <= '1';
CI <= '0';
OEbar <= '0';

wait for 4 ns;

clk <= '0';

wait for 4 ns;

assert (Y = "000000000000")
report
"Assert 532 : < Y /= 000000000000 >" -- 	Vector No: 386
severity warning;

wait for 1 ns;

-- **************************************************
--------------------------

clk <= '1'; --	 Cycle No: 890

wait for 1 ns;

I <= "0000";
CCEN_BAR <= '0';
CC_BAR <= '0';
RLD_BAR <= '0';
CI <= '0';
OEbar <= '0';
D <= "000000000000";

wait for 4 ns;

clk <= '0';

wait for 4 ns;

wait for 1 ns;

--------------------------

clk <= '1'; --	 Cycle No: 891

wait for 1 ns;

I <= "0010";
CCEN_BAR <= '0';
CC_BAR <= '0';
RLD_BAR <= '1';
CI <= '0';
OEbar <= '0';
D <= "111111111111";

wait for 4 ns;

clk <= '0';

wait for 4 ns;

wait for 1 ns;

--------------------------

clk <= '1'; --	 Cycle No: 892

wait for 1 ns;

I <= "0001";
CCEN_BAR <= '1';
CC_BAR <= '1';
RLD_BAR <= '1';
CI <= '0';
OEbar <= '0';

wait for 4 ns;

clk <= '0';

wait for 4 ns;

wait for 1 ns;

--------------------------

clk <= '1'; --	 Cycle No: 893

wait for 1 ns;

I <= "1010";
CCEN_BAR <= '1';
CC_BAR <= '0';
RLD_BAR <= '1';
CI <= '0';
OEbar <= '0';

wait for 4 ns;

clk <= '0';

wait for 4 ns;

assert (Y = "111111111111")
report
"Assert 533 : < Y /= 111111111111 >" -- 	Vector No: 387
severity warning;

wait for 1 ns;

-- **************************************************
--------------------------

clk <= '1'; --	 Cycle No: 894

wait for 1 ns;

I <= "0000";
CCEN_BAR <= '0';
CC_BAR <= '0';
RLD_BAR <= '0';
CI <= '0';
OEbar <= '0';
D <= "111111111111";

wait for 4 ns;

clk <= '0';

wait for 4 ns;

wait for 1 ns;

--------------------------

clk <= '1'; --	 Cycle No: 895

wait for 1 ns;

I <= "0010";
CCEN_BAR <= '0';
CC_BAR <= '0';
RLD_BAR <= '1';
CI <= '0';
OEbar <= '0';
D <= "000000000000";

wait for 4 ns;

clk <= '0';

wait for 4 ns;

wait for 1 ns;

--------------------------

clk <= '1'; --	 Cycle No: 896

wait for 1 ns;

I <= "0001";
CCEN_BAR <= '1';
CC_BAR <= '1';
RLD_BAR <= '1';
CI <= '0';
OEbar <= '0';

wait for 4 ns;

clk <= '0';

wait for 4 ns;

wait for 1 ns;

--------------------------

clk <= '1'; --	 Cycle No: 897

wait for 1 ns;

I <= "1010";
CCEN_BAR <= '1';
CC_BAR <= '0';
RLD_BAR <= '1';
CI <= '0';
OEbar <= '0';

wait for 4 ns;

clk <= '0';

wait for 4 ns;

assert (Y = "000000000000")
report
"Assert 534 : < Y /= 000000000000 >" -- 	Vector No: 388
severity warning;

wait for 1 ns;

-- **************************************************
--------------------------

clk <= '1'; --	 Cycle No: 898

wait for 1 ns;

I <= "0000";
CCEN_BAR <= '0';
CC_BAR <= '0';
RLD_BAR <= '0';
CI <= '0';
OEbar <= '0';
D <= "111111111111";

wait for 4 ns;

clk <= '0';

wait for 4 ns;

wait for 1 ns;

--------------------------

clk <= '1'; --	 Cycle No: 899

wait for 1 ns;

I <= "0010";
CCEN_BAR <= '0';
CC_BAR <= '0';
RLD_BAR <= '1';
CI <= '0';
OEbar <= '0';
D <= "111111111111";

wait for 4 ns;

clk <= '0';

wait for 4 ns;

wait for 1 ns;

--------------------------

clk <= '1'; --	 Cycle No: 900

wait for 1 ns;

I <= "0001";
CCEN_BAR <= '1';
CC_BAR <= '1';
RLD_BAR <= '1';
CI <= '0';
OEbar <= '0';

wait for 4 ns;

clk <= '0';

wait for 4 ns;

wait for 1 ns;

--------------------------

clk <= '1'; --	 Cycle No: 901

wait for 1 ns;

I <= "1010";
CCEN_BAR <= '1';
CC_BAR <= '0';
RLD_BAR <= '1';
CI <= '0';
OEbar <= '0';

wait for 4 ns;

clk <= '0';

wait for 4 ns;

assert (Y = "111111111111")
report
"Assert 535 : < Y /= 111111111111 >" -- 	Vector No: 389
severity warning;

wait for 1 ns;

-- **************************************************
-- **************************************************
-- ** I = 13 **
--------------------------

clk <= '1'; --	 Cycle No: 902

wait for 1 ns;

I <= "0000";
CCEN_BAR <= '0';
CC_BAR <= '0';
RLD_BAR <= '0';
CI <= '0';
OEbar <= '0';
D <= "000000000000";

wait for 4 ns;

clk <= '0';

wait for 4 ns;

wait for 1 ns;

--------------------------

clk <= '1'; --	 Cycle No: 903

wait for 1 ns;

I <= "0010";
CCEN_BAR <= '0';
CC_BAR <= '0';
RLD_BAR <= '1';
CI <= '0';
OEbar <= '0';
D <= "000000000000";

wait for 4 ns;

clk <= '0';

wait for 4 ns;

wait for 1 ns;

--------------------------

clk <= '1'; --	 Cycle No: 904

wait for 1 ns;

I <= "0001";
CCEN_BAR <= '1';
CC_BAR <= '1';
RLD_BAR <= '1';
CI <= '0';
OEbar <= '0';

wait for 4 ns;

clk <= '0';

wait for 4 ns;

wait for 1 ns;

--------------------------

clk <= '1'; --	 Cycle No: 905

wait for 1 ns;

I <= "1101";
CCEN_BAR <= '0';
CC_BAR <= '1';
RLD_BAR <= '1';
CI <= '0';
OEbar <= '0';

wait for 4 ns;

clk <= '0';

wait for 4 ns;

assert (Y = "000000000000")
report
"Assert 536 : < Y /= 000000000000 >" -- 	Vector No: 390
severity warning;

wait for 1 ns;

-- **************************************************
--------------------------

clk <= '1'; --	 Cycle No: 906

wait for 1 ns;

I <= "0000";
CCEN_BAR <= '0';
CC_BAR <= '0';
RLD_BAR <= '0';
CI <= '0';
OEbar <= '0';
D <= "000000000000";

wait for 4 ns;

clk <= '0';

wait for 4 ns;

wait for 1 ns;

--------------------------

clk <= '1'; --	 Cycle No: 907

wait for 1 ns;

I <= "0010";
CCEN_BAR <= '0';
CC_BAR <= '0';
RLD_BAR <= '1';
CI <= '0';
OEbar <= '0';
D <= "111111111111";

wait for 4 ns;

clk <= '0';

wait for 4 ns;

wait for 1 ns;

--------------------------

clk <= '1'; --	 Cycle No: 908

wait for 1 ns;

I <= "0001";
CCEN_BAR <= '1';
CC_BAR <= '1';
RLD_BAR <= '1';
CI <= '0';
OEbar <= '0';

wait for 4 ns;

clk <= '0';

wait for 4 ns;

wait for 1 ns;

--------------------------

clk <= '1'; --	 Cycle No: 909

wait for 1 ns;

I <= "1101";
CCEN_BAR <= '0';
CC_BAR <= '1';
RLD_BAR <= '1';
CI <= '0';
OEbar <= '0';

wait for 4 ns;

clk <= '0';

wait for 4 ns;

assert (Y = "111111111111")
report
"Assert 537 : < Y /= 111111111111 >" -- 	Vector No: 391
severity warning;

wait for 1 ns;

-- **************************************************
--------------------------

clk <= '1'; --	 Cycle No: 910

wait for 1 ns;

I <= "0000";
CCEN_BAR <= '0';
CC_BAR <= '0';
RLD_BAR <= '0';
CI <= '0';
OEbar <= '0';
D <= "111111111111";

wait for 4 ns;

clk <= '0';

wait for 4 ns;

wait for 1 ns;

--------------------------

clk <= '1'; --	 Cycle No: 911

wait for 1 ns;

I <= "0010";
CCEN_BAR <= '0';
CC_BAR <= '0';
RLD_BAR <= '1';
CI <= '0';
OEbar <= '0';
D <= "000000000000";

wait for 4 ns;

clk <= '0';

wait for 4 ns;

wait for 1 ns;

--------------------------

clk <= '1'; --	 Cycle No: 912

wait for 1 ns;

I <= "0001";
CCEN_BAR <= '1';
CC_BAR <= '1';
RLD_BAR <= '1';
CI <= '0';
OEbar <= '0';

wait for 4 ns;

clk <= '0';

wait for 4 ns;

wait for 1 ns;

--------------------------

clk <= '1'; --	 Cycle No: 913

wait for 1 ns;

I <= "1101";
CCEN_BAR <= '0';
CC_BAR <= '1';
RLD_BAR <= '1';
CI <= '0';
OEbar <= '0';

wait for 4 ns;

clk <= '0';

wait for 4 ns;

assert (Y = "000000000000")
report
"Assert 538 : < Y /= 000000000000 >" -- 	Vector No: 392
severity warning;

wait for 1 ns;

-- **************************************************
--------------------------

clk <= '1'; --	 Cycle No: 914

wait for 1 ns;

I <= "0000";
CCEN_BAR <= '0';
CC_BAR <= '0';
RLD_BAR <= '0';
CI <= '0';
OEbar <= '0';
D <= "111111111111";

wait for 4 ns;

clk <= '0';

wait for 4 ns;

wait for 1 ns;

--------------------------

clk <= '1'; --	 Cycle No: 915

wait for 1 ns;

I <= "0010";
CCEN_BAR <= '0';
CC_BAR <= '0';
RLD_BAR <= '1';
CI <= '0';
OEbar <= '0';
D <= "111111111111";

wait for 4 ns;

clk <= '0';

wait for 4 ns;

wait for 1 ns;

--------------------------

clk <= '1'; --	 Cycle No: 916

wait for 1 ns;

I <= "0001";
CCEN_BAR <= '1';
CC_BAR <= '1';
RLD_BAR <= '1';
CI <= '0';
OEbar <= '0';

wait for 4 ns;

clk <= '0';

wait for 4 ns;

wait for 1 ns;

--------------------------

clk <= '1'; --	 Cycle No: 917

wait for 1 ns;

I <= "1101";
CCEN_BAR <= '0';
CC_BAR <= '1';
RLD_BAR <= '1';
CI <= '0';
OEbar <= '0';

wait for 4 ns;

clk <= '0';

wait for 4 ns;

assert (Y = "111111111111")
report
"Assert 539 : < Y /= 111111111111 >" -- 	Vector No: 393
severity warning;

wait for 1 ns;

-- **************************************************
-- ** I = 15 **
--------------------------

clk <= '1'; --	 Cycle No: 918

wait for 1 ns;

I <= "0000";
CCEN_BAR <= '0';
CC_BAR <= '0';
RLD_BAR <= '0';
CI <= '0';
OEbar <= '0';
D <= "111111111111";

wait for 4 ns;

clk <= '0';

wait for 4 ns;

wait for 1 ns;

--------------------------

clk <= '1'; --	 Cycle No: 919

wait for 1 ns;

I <= "0010";
CCEN_BAR <= '0';
CC_BAR <= '0';
RLD_BAR <= '1';
CI <= '0';
OEbar <= '0';
D <= "000000000000";

wait for 4 ns;

clk <= '0';

wait for 4 ns;

wait for 1 ns;

--------------------------

clk <= '1'; --	 Cycle No: 920

wait for 1 ns;

I <= "0001";
CCEN_BAR <= '1';
CC_BAR <= '1';
RLD_BAR <= '1';
CI <= '0';
OEbar <= '0';

wait for 4 ns;

clk <= '0';

wait for 4 ns;

wait for 1 ns;

--------------------------

clk <= '1'; --	 Cycle No: 921

wait for 1 ns;

I <= "1101";
CCEN_BAR <= '0';
CC_BAR <= '1';
RLD_BAR <= '1';
CI <= '0';
OEbar <= '0';

wait for 4 ns;

clk <= '0';

wait for 4 ns;

assert (Y = "000000000000")
report
"Assert 540 : < Y /= 000000000000 >" -- 	Vector No: 394
severity warning;

wait for 1 ns;

-- **************************************************
--------------------------

clk <= '1'; --	 Cycle No: 922

wait for 1 ns;

I <= "0000";
CCEN_BAR <= '0';
CC_BAR <= '0';
RLD_BAR <= '0';
CI <= '0';
OEbar <= '0';
D <= "111111111111";

wait for 4 ns;

clk <= '0';

wait for 4 ns;

wait for 1 ns;

--------------------------

clk <= '1'; --	 Cycle No: 923

wait for 1 ns;

I <= "0010";
CCEN_BAR <= '0';
CC_BAR <= '0';
RLD_BAR <= '1';
CI <= '0';
OEbar <= '0';
D <= "111111111111";

wait for 4 ns;

clk <= '0';

wait for 4 ns;

wait for 1 ns;

--------------------------

clk <= '1'; --	 Cycle No: 924

wait for 1 ns;

I <= "0001";
CCEN_BAR <= '1';
CC_BAR <= '1';
RLD_BAR <= '1';
CI <= '0';
OEbar <= '0';

wait for 4 ns;

clk <= '0';

wait for 4 ns;

wait for 1 ns;

--------------------------

clk <= '1'; --	 Cycle No: 925

wait for 1 ns;

I <= "1101";
CCEN_BAR <= '0';
CC_BAR <= '1';
RLD_BAR <= '1';
CI <= '0';
OEbar <= '0';

wait for 4 ns;

clk <= '0';

wait for 4 ns;

assert (Y = "111111111111")
report
"Assert 541 : < Y /= 111111111111 >" -- 	Vector No: 395
severity warning;

wait for 1 ns;

-- **************************************************
-- ***************************
-- ******* SELECT uPC ********
-- ***************************
-- ** I = 1 **
--------------------------

clk <= '1'; --	 Cycle No: 926

wait for 1 ns;

I <= "0000";
CCEN_BAR <= '0';
CC_BAR <= '0';
RLD_BAR <= '0';
CI <= '0';
OEbar <= '0';
D <= "000000000000";

wait for 4 ns;

clk <= '0';

wait for 4 ns;

wait for 1 ns;

--------------------------

clk <= '1'; --	 Cycle No: 927

wait for 1 ns;

I <= "0010";
CCEN_BAR <= '0';
CC_BAR <= '0';
RLD_BAR <= '1';
CI <= '0';
OEbar <= '0';
D <= "000000000000";

wait for 4 ns;

clk <= '0';

wait for 4 ns;

wait for 1 ns;

--------------------------

clk <= '1'; --	 Cycle No: 928

wait for 1 ns;

I <= "0001";
CCEN_BAR <= '0';
CC_BAR <= '1';
RLD_BAR <= '1';
CI <= '0';
OEbar <= '0';

wait for 4 ns;

clk <= '0';

wait for 4 ns;

assert (Y = "000000000000")
report
"Assert 542 : < Y /= 000000000000 >" -- 	Vector No: 396
severity warning;

wait for 1 ns;

-- **************************************************
--------------------------

clk <= '1'; --	 Cycle No: 929

wait for 1 ns;

I <= "0000";
CCEN_BAR <= '0';
CC_BAR <= '0';
RLD_BAR <= '0';
CI <= '0';
OEbar <= '0';
D <= "000000000000";

wait for 4 ns;

clk <= '0';

wait for 4 ns;

wait for 1 ns;

--------------------------

clk <= '1'; --	 Cycle No: 930

wait for 1 ns;

I <= "0010";
CCEN_BAR <= '0';
CC_BAR <= '0';
RLD_BAR <= '1';
CI <= '0';
OEbar <= '0';
D <= "111111111111";

wait for 4 ns;

clk <= '0';

wait for 4 ns;

wait for 1 ns;

--------------------------

clk <= '1'; --	 Cycle No: 931

wait for 1 ns;

I <= "0001";
CCEN_BAR <= '0';
CC_BAR <= '1';
RLD_BAR <= '1';
CI <= '0';
OEbar <= '0';

wait for 4 ns;

clk <= '0';

wait for 4 ns;

assert (Y = "111111111111")
report
"Assert 543 : < Y /= 111111111111 >" -- 	Vector No: 397
severity warning;

wait for 1 ns;

-- **************************************************
--------------------------

clk <= '1'; --	 Cycle No: 932

wait for 1 ns;

I <= "0000";
CCEN_BAR <= '0';
CC_BAR <= '0';
RLD_BAR <= '0';
CI <= '0';
OEbar <= '0';
D <= "111111111111";

wait for 4 ns;

clk <= '0';

wait for 4 ns;

wait for 1 ns;

--------------------------

clk <= '1'; --	 Cycle No: 933

wait for 1 ns;

I <= "0010";
CCEN_BAR <= '0';
CC_BAR <= '0';
RLD_BAR <= '1';
CI <= '0';
OEbar <= '0';
D <= "000000000000";

wait for 4 ns;

clk <= '0';

wait for 4 ns;

wait for 1 ns;

--------------------------

clk <= '1'; --	 Cycle No: 934

wait for 1 ns;

I <= "0001";
CCEN_BAR <= '0';
CC_BAR <= '1';
RLD_BAR <= '1';
CI <= '0';
OEbar <= '0';

wait for 4 ns;

clk <= '0';

wait for 4 ns;

assert (Y = "000000000000")
report
"Assert 544 : < Y /= 000000000000 >" -- 	Vector No: 398
severity warning;

wait for 1 ns;

-- **************************************************
--------------------------

clk <= '1'; --	 Cycle No: 935

wait for 1 ns;

I <= "0000";
CCEN_BAR <= '0';
CC_BAR <= '0';
RLD_BAR <= '0';
CI <= '0';
OEbar <= '0';
D <= "111111111111";

wait for 4 ns;

clk <= '0';

wait for 4 ns;

wait for 1 ns;

--------------------------

clk <= '1'; --	 Cycle No: 936

wait for 1 ns;

I <= "0010";
CCEN_BAR <= '0';
CC_BAR <= '0';
RLD_BAR <= '1';
CI <= '0';
OEbar <= '0';
D <= "111111111111";

wait for 4 ns;

clk <= '0';

wait for 4 ns;

wait for 1 ns;

--------------------------

clk <= '1'; --	 Cycle No: 937

wait for 1 ns;

I <= "0001";
CCEN_BAR <= '0';
CC_BAR <= '1';
RLD_BAR <= '1';
CI <= '0';
OEbar <= '0';

wait for 4 ns;

clk <= '0';

wait for 4 ns;

assert (Y = "111111111111")
report
"Assert 545 : < Y /= 111111111111 >" -- 	Vector No: 399
severity warning;

wait for 1 ns;

-- **************************************************
-- **************************************************
-- ** I = 3 **
--------------------------

clk <= '1'; --	 Cycle No: 938

wait for 1 ns;

I <= "0000";
CCEN_BAR <= '0';
CC_BAR <= '0';
RLD_BAR <= '0';
CI <= '0';
OEbar <= '0';
D <= "000000000000";

wait for 4 ns;

clk <= '0';

wait for 4 ns;

wait for 1 ns;

--------------------------

clk <= '1'; --	 Cycle No: 939

wait for 1 ns;

I <= "0010";
CCEN_BAR <= '0';
CC_BAR <= '0';
RLD_BAR <= '1';
CI <= '0';
OEbar <= '0';
D <= "000000000000";

wait for 4 ns;

clk <= '0';

wait for 4 ns;

wait for 1 ns;

--------------------------

clk <= '1'; --	 Cycle No: 940

wait for 1 ns;

I <= "0011";
CCEN_BAR <= '0';
CC_BAR <= '1';
RLD_BAR <= '1';
CI <= '0';
OEbar <= '0';

wait for 4 ns;

clk <= '0';

wait for 4 ns;

assert (Y = "000000000000")
report
"Assert 546 : < Y /= 000000000000 >" -- 	Vector No: 400
severity warning;

wait for 1 ns;

-- **************************************************
--------------------------

clk <= '1'; --	 Cycle No: 941

wait for 1 ns;

I <= "0000";
CCEN_BAR <= '0';
CC_BAR <= '0';
RLD_BAR <= '0';
CI <= '0';
OEbar <= '0';
D <= "000000000000";

wait for 4 ns;

clk <= '0';

wait for 4 ns;

wait for 1 ns;

--------------------------

clk <= '1'; --	 Cycle No: 942

wait for 1 ns;

I <= "0010";
CCEN_BAR <= '0';
CC_BAR <= '0';
RLD_BAR <= '1';
CI <= '0';
OEbar <= '0';
D <= "111111111111";

wait for 4 ns;

clk <= '0';

wait for 4 ns;

wait for 1 ns;

--------------------------

clk <= '1'; --	 Cycle No: 943

wait for 1 ns;

I <= "0011";
CCEN_BAR <= '0';
CC_BAR <= '1';
RLD_BAR <= '1';
CI <= '0';
OEbar <= '0';

wait for 4 ns;

clk <= '0';

wait for 4 ns;

assert (Y = "111111111111")
report
"Assert 547 : < Y /= 111111111111 >" -- 	Vector No: 401
severity warning;

wait for 1 ns;

-- **************************************************
--------------------------

clk <= '1'; --	 Cycle No: 944

wait for 1 ns;

I <= "0000";
CCEN_BAR <= '0';
CC_BAR <= '0';
RLD_BAR <= '0';
CI <= '0';
OEbar <= '0';
D <= "111111111111";

wait for 4 ns;

clk <= '0';

wait for 4 ns;

wait for 1 ns;

--------------------------

clk <= '1'; --	 Cycle No: 945

wait for 1 ns;

I <= "0010";
CCEN_BAR <= '0';
CC_BAR <= '0';
RLD_BAR <= '1';
CI <= '0';
OEbar <= '0';
D <= "000000000000";

wait for 4 ns;

clk <= '0';

wait for 4 ns;

wait for 1 ns;

--------------------------

clk <= '1'; --	 Cycle No: 946

wait for 1 ns;

I <= "0011";
CCEN_BAR <= '0';
CC_BAR <= '1';
RLD_BAR <= '1';
CI <= '0';
OEbar <= '0';

wait for 4 ns;

clk <= '0';

wait for 4 ns;

assert (Y = "000000000000")
report
"Assert 548 : < Y /= 000000000000 >" -- 	Vector No: 402
severity warning;

wait for 1 ns;

-- **************************************************
--------------------------

clk <= '1'; --	 Cycle No: 947

wait for 1 ns;

I <= "0000";
CCEN_BAR <= '0';
CC_BAR <= '0';
RLD_BAR <= '0';
CI <= '0';
OEbar <= '0';
D <= "111111111111";

wait for 4 ns;

clk <= '0';

wait for 4 ns;

wait for 1 ns;

--------------------------

clk <= '1'; --	 Cycle No: 948

wait for 1 ns;

I <= "0010";
CCEN_BAR <= '0';
CC_BAR <= '0';
RLD_BAR <= '1';
CI <= '0';
OEbar <= '0';
D <= "111111111111";

wait for 4 ns;

clk <= '0';

wait for 4 ns;

wait for 1 ns;

--------------------------

clk <= '1'; --	 Cycle No: 949

wait for 1 ns;

I <= "0011";
CCEN_BAR <= '0';
CC_BAR <= '1';
RLD_BAR <= '1';
CI <= '0';
OEbar <= '0';

wait for 4 ns;

clk <= '0';

wait for 4 ns;

assert (Y = "111111111111")
report
"Assert 549 : < Y /= 111111111111 >" -- 	Vector No: 403
severity warning;

wait for 1 ns;

-- **************************************************
-- **************************************************
-- ** I = 4 **
--------------------------

clk <= '1'; --	 Cycle No: 950

wait for 1 ns;

I <= "0000";
CCEN_BAR <= '0';
CC_BAR <= '0';
RLD_BAR <= '0';
CI <= '0';
OEbar <= '0';
D <= "000000000000";

wait for 4 ns;

clk <= '0';

wait for 4 ns;

wait for 1 ns;

--------------------------

clk <= '1'; --	 Cycle No: 951

wait for 1 ns;

I <= "0010";
CCEN_BAR <= '0';
CC_BAR <= '0';
RLD_BAR <= '1';
CI <= '0';
OEbar <= '0';
D <= "000000000000";

wait for 4 ns;

clk <= '0';

wait for 4 ns;

wait for 1 ns;

--------------------------

clk <= '1'; --	 Cycle No: 952

wait for 1 ns;

I <= "0100";
CCEN_BAR <= '0';
CC_BAR <= '0';
RLD_BAR <= '1';
CI <= '0';
OEbar <= '0';

wait for 4 ns;

clk <= '0';

wait for 4 ns;

assert (Y = "000000000000")
report
"Assert 550 : < Y /= 000000000000 >" -- 	Vector No: 404
severity warning;

wait for 1 ns;

-- **************************************************
--------------------------

clk <= '1'; --	 Cycle No: 953

wait for 1 ns;

I <= "0000";
CCEN_BAR <= '0';
CC_BAR <= '0';
RLD_BAR <= '0';
CI <= '0';
OEbar <= '0';
D <= "000000000000";

wait for 4 ns;

clk <= '0';

wait for 4 ns;

wait for 1 ns;

--------------------------

clk <= '1'; --	 Cycle No: 954

wait for 1 ns;

I <= "0010";
CCEN_BAR <= '0';
CC_BAR <= '0';
RLD_BAR <= '1';
CI <= '0';
OEbar <= '0';
D <= "111111111111";

wait for 4 ns;

clk <= '0';

wait for 4 ns;

wait for 1 ns;

--------------------------

clk <= '1'; --	 Cycle No: 955

wait for 1 ns;

I <= "0100";
CCEN_BAR <= '0';
CC_BAR <= '0';
RLD_BAR <= '1';
CI <= '0';
OEbar <= '0';

wait for 4 ns;

clk <= '0';

wait for 4 ns;

assert (Y = "111111111111")
report
"Assert 551 : < Y /= 111111111111 >" -- 	Vector No: 405
severity warning;

wait for 1 ns;

-- **************************************************
--------------------------

clk <= '1'; --	 Cycle No: 956

wait for 1 ns;

I <= "0000";
CCEN_BAR <= '0';
CC_BAR <= '0';
RLD_BAR <= '0';
CI <= '0';
OEbar <= '0';
D <= "111111111111";

wait for 4 ns;

clk <= '0';

wait for 4 ns;

wait for 1 ns;

--------------------------

clk <= '1'; --	 Cycle No: 957

wait for 1 ns;

I <= "0010";
CCEN_BAR <= '0';
CC_BAR <= '0';
RLD_BAR <= '1';
CI <= '0';
OEbar <= '0';
D <= "000000000000";

wait for 4 ns;

clk <= '0';

wait for 4 ns;

wait for 1 ns;

--------------------------

clk <= '1'; --	 Cycle No: 958

wait for 1 ns;

I <= "0100";
CCEN_BAR <= '0';
CC_BAR <= '0';
RLD_BAR <= '1';
CI <= '0';
OEbar <= '0';

wait for 4 ns;

clk <= '0';

wait for 4 ns;

assert (Y = "000000000000")
report
"Assert 552 : < Y /= 000000000000 >" -- 	Vector No: 406
severity warning;

wait for 1 ns;

-- **************************************************
--------------------------

clk <= '1'; --	 Cycle No: 959

wait for 1 ns;

I <= "0000";
CCEN_BAR <= '0';
CC_BAR <= '0';
RLD_BAR <= '0';
CI <= '0';
OEbar <= '0';
D <= "111111111111";

wait for 4 ns;

clk <= '0';

wait for 4 ns;

wait for 1 ns;

--------------------------

clk <= '1'; --	 Cycle No: 960

wait for 1 ns;

I <= "0010";
CCEN_BAR <= '0';
CC_BAR <= '0';
RLD_BAR <= '1';
CI <= '0';
OEbar <= '0';
D <= "111111111111";

wait for 4 ns;

clk <= '0';

wait for 4 ns;

wait for 1 ns;

--------------------------

clk <= '1'; --	 Cycle No: 961

wait for 1 ns;

I <= "0100";
CCEN_BAR <= '0';
CC_BAR <= '0';
RLD_BAR <= '1';
CI <= '0';
OEbar <= '0';

wait for 4 ns;

clk <= '0';

wait for 4 ns;

assert (Y = "111111111111")
report
"Assert 553 : < Y /= 111111111111 >" -- 	Vector No: 407
severity warning;

wait for 1 ns;

-- **************************************************
-- **************************************************
--------------------------

clk <= '1'; --	 Cycle No: 962

wait for 1 ns;

I <= "0000";
CCEN_BAR <= '0';
CC_BAR <= '0';
RLD_BAR <= '0';
CI <= '0';
OEbar <= '0';
D <= "000000000000";

wait for 4 ns;

clk <= '0';

wait for 4 ns;

wait for 1 ns;

--------------------------

clk <= '1'; --	 Cycle No: 963

wait for 1 ns;

I <= "0010";
CCEN_BAR <= '0';
CC_BAR <= '0';
RLD_BAR <= '1';
CI <= '0';
OEbar <= '0';
D <= "000000000000";

wait for 4 ns;

clk <= '0';

wait for 4 ns;

wait for 1 ns;

--------------------------

clk <= '1'; --	 Cycle No: 964

wait for 1 ns;

I <= "0100";
CCEN_BAR <= '0';
CC_BAR <= '1';
RLD_BAR <= '1';
CI <= '0';
OEbar <= '0';

wait for 4 ns;

clk <= '0';

wait for 4 ns;

assert (Y = "000000000000")
report
"Assert 554 : < Y /= 000000000000 >" -- 	Vector No: 408
severity warning;

wait for 1 ns;

-- **************************************************
--------------------------

clk <= '1'; --	 Cycle No: 965

wait for 1 ns;

I <= "0000";
CCEN_BAR <= '0';
CC_BAR <= '0';
RLD_BAR <= '0';
CI <= '0';
OEbar <= '0';
D <= "000000000000";

wait for 4 ns;

clk <= '0';

wait for 4 ns;

wait for 1 ns;

--------------------------

clk <= '1'; --	 Cycle No: 966

wait for 1 ns;

I <= "0010";
CCEN_BAR <= '0';
CC_BAR <= '0';
RLD_BAR <= '1';
CI <= '0';
OEbar <= '0';
D <= "111111111111";

wait for 4 ns;

clk <= '0';

wait for 4 ns;

wait for 1 ns;

--------------------------

clk <= '1'; --	 Cycle No: 967

wait for 1 ns;

I <= "0100";
CCEN_BAR <= '0';
CC_BAR <= '1';
RLD_BAR <= '1';
CI <= '0';
OEbar <= '0';

wait for 4 ns;

clk <= '0';

wait for 4 ns;

assert (Y = "111111111111")
report
"Assert 555 : < Y /= 111111111111 >" -- 	Vector No: 409
severity warning;

wait for 1 ns;

-- **************************************************
--------------------------

clk <= '1'; --	 Cycle No: 968

wait for 1 ns;

I <= "0000";
CCEN_BAR <= '0';
CC_BAR <= '0';
RLD_BAR <= '0';
CI <= '0';
OEbar <= '0';
D <= "111111111111";

wait for 4 ns;

clk <= '0';

wait for 4 ns;

wait for 1 ns;

--------------------------

clk <= '1'; --	 Cycle No: 969

wait for 1 ns;

I <= "0010";
CCEN_BAR <= '0';
CC_BAR <= '0';
RLD_BAR <= '1';
CI <= '0';
OEbar <= '0';
D <= "000000000000";

wait for 4 ns;

clk <= '0';

wait for 4 ns;

wait for 1 ns;

--------------------------

clk <= '1'; --	 Cycle No: 970

wait for 1 ns;

I <= "0100";
CCEN_BAR <= '0';
CC_BAR <= '1';
RLD_BAR <= '1';
CI <= '0';
OEbar <= '0';

wait for 4 ns;

clk <= '0';

wait for 4 ns;

assert (Y = "000000000000")
report
"Assert 556 : < Y /= 000000000000 >" -- 	Vector No: 410
severity warning;

wait for 1 ns;

-- **************************************************
--------------------------

clk <= '1'; --	 Cycle No: 971

wait for 1 ns;

I <= "0000";
CCEN_BAR <= '0';
CC_BAR <= '0';
RLD_BAR <= '0';
CI <= '0';
OEbar <= '0';
D <= "111111111111";

wait for 4 ns;

clk <= '0';

wait for 4 ns;

wait for 1 ns;

--------------------------

clk <= '1'; --	 Cycle No: 972

wait for 1 ns;

I <= "0010";
CCEN_BAR <= '0';
CC_BAR <= '0';
RLD_BAR <= '1';
CI <= '0';
OEbar <= '0';
D <= "111111111111";

wait for 4 ns;

clk <= '0';

wait for 4 ns;

wait for 1 ns;

--------------------------

clk <= '1'; --	 Cycle No: 973

wait for 1 ns;

I <= "0100";
CCEN_BAR <= '0';
CC_BAR <= '1';
RLD_BAR <= '1';
CI <= '0';
OEbar <= '0';

wait for 4 ns;

clk <= '0';

wait for 4 ns;

assert (Y = "111111111111")
report
"Assert 557 : < Y /= 111111111111 >" -- 	Vector No: 411
severity warning;

wait for 1 ns;

-- **************************************************
-- **************************************************
--------------------------

clk <= '1'; --	 Cycle No: 974

wait for 1 ns;

I <= "0000";
CCEN_BAR <= '0';
CC_BAR <= '0';
RLD_BAR <= '0';
CI <= '0';
OEbar <= '0';
D <= "000000000000";

wait for 4 ns;

clk <= '0';

wait for 4 ns;

wait for 1 ns;

--------------------------

clk <= '1'; --	 Cycle No: 975

wait for 1 ns;

I <= "0010";
CCEN_BAR <= '0';
CC_BAR <= '0';
RLD_BAR <= '1';
CI <= '0';
OEbar <= '0';
D <= "000000000000";

wait for 4 ns;

clk <= '0';

wait for 4 ns;

wait for 1 ns;

--------------------------

clk <= '1'; --	 Cycle No: 976

wait for 1 ns;

I <= "0100";
CCEN_BAR <= '1';
CC_BAR <= '1';
RLD_BAR <= '1';
CI <= '0';
OEbar <= '0';

wait for 4 ns;

clk <= '0';

wait for 4 ns;

assert (Y = "000000000000")
report
"Assert 558 : < Y /= 000000000000 >" -- 	Vector No: 412
severity warning;

wait for 1 ns;

-- **************************************************
--------------------------

clk <= '1'; --	 Cycle No: 977

wait for 1 ns;

I <= "0000";
CCEN_BAR <= '0';
CC_BAR <= '0';
RLD_BAR <= '0';
CI <= '0';
OEbar <= '0';
D <= "000000000000";

wait for 4 ns;

clk <= '0';

wait for 4 ns;

wait for 1 ns;

--------------------------

clk <= '1'; --	 Cycle No: 978

wait for 1 ns;

I <= "0010";
CCEN_BAR <= '0';
CC_BAR <= '0';
RLD_BAR <= '1';
CI <= '0';
OEbar <= '0';
D <= "111111111111";

wait for 4 ns;

clk <= '0';

wait for 4 ns;

wait for 1 ns;

--------------------------

clk <= '1'; --	 Cycle No: 979

wait for 1 ns;

I <= "0100";
CCEN_BAR <= '1';
CC_BAR <= '1';
RLD_BAR <= '1';
CI <= '0';
OEbar <= '0';

wait for 4 ns;

clk <= '0';

wait for 4 ns;

assert (Y = "111111111111")
report
"Assert 559 : < Y /= 111111111111 >" -- 	Vector No: 413
severity warning;

wait for 1 ns;

-- **************************************************
--------------------------

clk <= '1'; --	 Cycle No: 980

wait for 1 ns;

I <= "0000";
CCEN_BAR <= '0';
CC_BAR <= '0';
RLD_BAR <= '0';
CI <= '0';
OEbar <= '0';
D <= "111111111111";

wait for 4 ns;

clk <= '0';

wait for 4 ns;

wait for 1 ns;

--------------------------

clk <= '1'; --	 Cycle No: 981

wait for 1 ns;

I <= "0010";
CCEN_BAR <= '0';
CC_BAR <= '0';
RLD_BAR <= '1';
CI <= '0';
OEbar <= '0';
D <= "000000000000";

wait for 4 ns;

clk <= '0';

wait for 4 ns;

wait for 1 ns;

--------------------------

clk <= '1'; --	 Cycle No: 982

wait for 1 ns;

I <= "0100";
CCEN_BAR <= '1';
CC_BAR <= '1';
RLD_BAR <= '1';
CI <= '0';
OEbar <= '0';

wait for 4 ns;

clk <= '0';

wait for 4 ns;

assert (Y = "000000000000")
report
"Assert 560 : < Y /= 000000000000 >" -- 	Vector No: 414
severity warning;

wait for 1 ns;

-- **************************************************
--------------------------

clk <= '1'; --	 Cycle No: 983

wait for 1 ns;

I <= "0000";
CCEN_BAR <= '0';
CC_BAR <= '0';
RLD_BAR <= '0';
CI <= '0';
OEbar <= '0';
D <= "111111111111";

wait for 4 ns;

clk <= '0';

wait for 4 ns;

wait for 1 ns;

--------------------------

clk <= '1'; --	 Cycle No: 984

wait for 1 ns;

I <= "0010";
CCEN_BAR <= '0';
CC_BAR <= '0';
RLD_BAR <= '1';
CI <= '0';
OEbar <= '0';
D <= "111111111111";

wait for 4 ns;

clk <= '0';

wait for 4 ns;

wait for 1 ns;

--------------------------

clk <= '1'; --	 Cycle No: 985

wait for 1 ns;

I <= "0100";
CCEN_BAR <= '1';
CC_BAR <= '1';
RLD_BAR <= '1';
CI <= '0';
OEbar <= '0';

wait for 4 ns;

clk <= '0';

wait for 4 ns;

assert (Y = "111111111111")
report
"Assert 561 : < Y /= 111111111111 >" -- 	Vector No: 415
severity warning;

wait for 1 ns;

-- **************************************************
-- **************************************************
--------------------------

clk <= '1'; --	 Cycle No: 986

wait for 1 ns;

I <= "0000";
CCEN_BAR <= '0';
CC_BAR <= '0';
RLD_BAR <= '0';
CI <= '0';
OEbar <= '0';
D <= "000000000000";

wait for 4 ns;

clk <= '0';

wait for 4 ns;

wait for 1 ns;

--------------------------

clk <= '1'; --	 Cycle No: 987

wait for 1 ns;

I <= "0010";
CCEN_BAR <= '0';
CC_BAR <= '0';
RLD_BAR <= '1';
CI <= '0';
OEbar <= '0';
D <= "000000000000";

wait for 4 ns;

clk <= '0';

wait for 4 ns;

wait for 1 ns;

--------------------------

clk <= '1'; --	 Cycle No: 988

wait for 1 ns;

I <= "0100";
CCEN_BAR <= '1';
CC_BAR <= '0';
RLD_BAR <= '1';
CI <= '0';
OEbar <= '0';

wait for 4 ns;

clk <= '0';

wait for 4 ns;

assert (Y = "000000000000")
report
"Assert 562 : < Y /= 000000000000 >" -- 	Vector No: 416
severity warning;

wait for 1 ns;

-- **************************************************
--------------------------

clk <= '1'; --	 Cycle No: 989

wait for 1 ns;

I <= "0000";
CCEN_BAR <= '0';
CC_BAR <= '0';
RLD_BAR <= '0';
CI <= '0';
OEbar <= '0';
D <= "000000000000";

wait for 4 ns;

clk <= '0';

wait for 4 ns;

wait for 1 ns;

--------------------------

clk <= '1'; --	 Cycle No: 990

wait for 1 ns;

I <= "0010";
CCEN_BAR <= '0';
CC_BAR <= '0';
RLD_BAR <= '1';
CI <= '0';
OEbar <= '0';
D <= "111111111111";

wait for 4 ns;

clk <= '0';

wait for 4 ns;

wait for 1 ns;

--------------------------

clk <= '1'; --	 Cycle No: 991

wait for 1 ns;

I <= "0100";
CCEN_BAR <= '1';
CC_BAR <= '0';
RLD_BAR <= '1';
CI <= '0';
OEbar <= '0';

wait for 4 ns;

clk <= '0';

wait for 4 ns;

assert (Y = "111111111111")
report
"Assert 563 : < Y /= 111111111111 >" -- 	Vector No: 417
severity warning;

wait for 1 ns;

-- **************************************************
--------------------------

clk <= '1'; --	 Cycle No: 992

wait for 1 ns;

I <= "0000";
CCEN_BAR <= '0';
CC_BAR <= '0';
RLD_BAR <= '0';
CI <= '0';
OEbar <= '0';
D <= "111111111111";

wait for 4 ns;

clk <= '0';

wait for 4 ns;

wait for 1 ns;

--------------------------

clk <= '1'; --	 Cycle No: 993

wait for 1 ns;

I <= "0010";
CCEN_BAR <= '0';
CC_BAR <= '0';
RLD_BAR <= '1';
CI <= '0';
OEbar <= '0';
D <= "000000000000";

wait for 4 ns;

clk <= '0';

wait for 4 ns;

wait for 1 ns;

--------------------------

clk <= '1'; --	 Cycle No: 994

wait for 1 ns;

I <= "0100";
CCEN_BAR <= '1';
CC_BAR <= '0';
RLD_BAR <= '1';
CI <= '0';
OEbar <= '0';

wait for 4 ns;

clk <= '0';

wait for 4 ns;

assert (Y = "000000000000")
report
"Assert 564 : < Y /= 000000000000 >" -- 	Vector No: 418
severity warning;

wait for 1 ns;

-- **************************************************
--------------------------

clk <= '1'; --	 Cycle No: 995

wait for 1 ns;

I <= "0000";
CCEN_BAR <= '0';
CC_BAR <= '0';
RLD_BAR <= '0';
CI <= '0';
OEbar <= '0';
D <= "111111111111";

wait for 4 ns;

clk <= '0';

wait for 4 ns;

wait for 1 ns;

--------------------------

clk <= '1'; --	 Cycle No: 996

wait for 1 ns;

I <= "0010";
CCEN_BAR <= '0';
CC_BAR <= '0';
RLD_BAR <= '1';
CI <= '0';
OEbar <= '0';
D <= "111111111111";

wait for 4 ns;

clk <= '0';

wait for 4 ns;

wait for 1 ns;

--------------------------

clk <= '1'; --	 Cycle No: 997

wait for 1 ns;

I <= "0100";
CCEN_BAR <= '1';
CC_BAR <= '0';
RLD_BAR <= '1';
CI <= '0';
OEbar <= '0';

wait for 4 ns;

clk <= '0';

wait for 4 ns;

assert (Y = "111111111111")
report
"Assert 565 : < Y /= 111111111111 >" -- 	Vector No: 419
severity warning;

wait for 1 ns;

-- **************************************************
-- **************************************************
-- ** I = 6 **
--------------------------

clk <= '1'; --	 Cycle No: 998

wait for 1 ns;

I <= "0000";
CCEN_BAR <= '0';
CC_BAR <= '0';
RLD_BAR <= '0';
CI <= '0';
OEbar <= '0';
D <= "000000000000";

wait for 4 ns;

clk <= '0';

wait for 4 ns;

wait for 1 ns;

--------------------------

clk <= '1'; --	 Cycle No: 999

wait for 1 ns;

I <= "0010";
CCEN_BAR <= '0';
CC_BAR <= '0';
RLD_BAR <= '1';
CI <= '0';
OEbar <= '0';
D <= "000000000000";

wait for 4 ns;

clk <= '0';

wait for 4 ns;

wait for 1 ns;

--------------------------

clk <= '1'; --	 Cycle No: 1000

wait for 1 ns;

I <= "0110";
CCEN_BAR <= '0';
CC_BAR <= '1';
RLD_BAR <= '1';
CI <= '0';
OEbar <= '0';

wait for 4 ns;

clk <= '0';

wait for 4 ns;

assert (Y = "000000000000")
report
"Assert 566 : < Y /= 000000000000 >" -- 	Vector No: 420
severity warning;

wait for 1 ns;

-- **************************************************
--------------------------

clk <= '1'; --	 Cycle No: 1001

wait for 1 ns;

I <= "0000";
CCEN_BAR <= '0';
CC_BAR <= '0';
RLD_BAR <= '0';
CI <= '0';
OEbar <= '0';
D <= "000000000000";

wait for 4 ns;

clk <= '0';

wait for 4 ns;

wait for 1 ns;

--------------------------

clk <= '1'; --	 Cycle No: 1002

wait for 1 ns;

I <= "0010";
CCEN_BAR <= '0';
CC_BAR <= '0';
RLD_BAR <= '1';
CI <= '0';
OEbar <= '0';
D <= "111111111111";

wait for 4 ns;

clk <= '0';

wait for 4 ns;

wait for 1 ns;

--------------------------

clk <= '1'; --	 Cycle No: 1003

wait for 1 ns;

I <= "0110";
CCEN_BAR <= '0';
CC_BAR <= '1';
RLD_BAR <= '1';
CI <= '0';
OEbar <= '0';

wait for 4 ns;

clk <= '0';

wait for 4 ns;

assert (Y = "111111111111")
report
"Assert 567 : < Y /= 111111111111 >" -- 	Vector No: 421
severity warning;

wait for 1 ns;

-- **************************************************
--------------------------

clk <= '1'; --	 Cycle No: 1004

wait for 1 ns;

I <= "0000";
CCEN_BAR <= '0';
CC_BAR <= '0';
RLD_BAR <= '0';
CI <= '0';
OEbar <= '0';
D <= "111111111111";

wait for 4 ns;

clk <= '0';

wait for 4 ns;

wait for 1 ns;

--------------------------

clk <= '1'; --	 Cycle No: 1005

wait for 1 ns;

I <= "0010";
CCEN_BAR <= '0';
CC_BAR <= '0';
RLD_BAR <= '1';
CI <= '0';
OEbar <= '0';
D <= "000000000000";

wait for 4 ns;

clk <= '0';

wait for 4 ns;

wait for 1 ns;

--------------------------

clk <= '1'; --	 Cycle No: 1006

wait for 1 ns;

I <= "0110";
CCEN_BAR <= '0';
CC_BAR <= '1';
RLD_BAR <= '1';
CI <= '0';
OEbar <= '0';

wait for 4 ns;

clk <= '0';

wait for 4 ns;

assert (Y = "000000000000")
report
"Assert 568 : < Y /= 000000000000 >" -- 	Vector No: 422
severity warning;

wait for 1 ns;

-- **************************************************
--------------------------

clk <= '1'; --	 Cycle No: 1007

wait for 1 ns;

I <= "0000";
CCEN_BAR <= '0';
CC_BAR <= '0';
RLD_BAR <= '0';
CI <= '0';
OEbar <= '0';
D <= "111111111111";

wait for 4 ns;

clk <= '0';

wait for 4 ns;

wait for 1 ns;

--------------------------

clk <= '1'; --	 Cycle No: 1008

wait for 1 ns;

I <= "0010";
CCEN_BAR <= '0';
CC_BAR <= '0';
RLD_BAR <= '1';
CI <= '0';
OEbar <= '0';
D <= "111111111111";

wait for 4 ns;

clk <= '0';

wait for 4 ns;

wait for 1 ns;

--------------------------

clk <= '1'; --	 Cycle No: 1009

wait for 1 ns;

I <= "0110";
CCEN_BAR <= '0';
CC_BAR <= '1';
RLD_BAR <= '1';
CI <= '0';
OEbar <= '0';

wait for 4 ns;

clk <= '0';

wait for 4 ns;

assert (Y = "111111111111")
report
"Assert 569 : < Y /= 111111111111 >" -- 	Vector No: 423
severity warning;

wait for 1 ns;

-- **************************************************
-- **************************************************
-- ** I = 8 **
--------------------------

clk <= '1'; --	 Cycle No: 1010

wait for 1 ns;

I <= "0000";
CCEN_BAR <= '0';
CC_BAR <= '0';
RLD_BAR <= '0';
CI <= '0';
OEbar <= '0';
D <= "000000000000";

wait for 4 ns;

clk <= '0';

wait for 4 ns;

wait for 1 ns;

--------------------------

clk <= '1'; --	 Cycle No: 1011

wait for 1 ns;

I <= "0010";
CCEN_BAR <= '0';
CC_BAR <= '0';
RLD_BAR <= '1';
CI <= '0';
OEbar <= '0';
D <= "000000000000";

wait for 4 ns;

clk <= '0';

wait for 4 ns;

wait for 1 ns;

--------------------------

clk <= '1'; --	 Cycle No: 1012

wait for 1 ns;

I <= "1000";
CCEN_BAR <= '0';
CC_BAR <= '0';
RLD_BAR <= '1';
CI <= '0';
OEbar <= '0';

wait for 4 ns;

clk <= '0';

wait for 4 ns;

assert (Y = "000000000000")
report
"Assert 570 : < Y /= 000000000000 >" -- 	Vector No: 424
severity warning;

wait for 1 ns;

-- **************************************************
--------------------------

clk <= '1'; --	 Cycle No: 1013

wait for 1 ns;

I <= "0000";
CCEN_BAR <= '0';
CC_BAR <= '0';
RLD_BAR <= '0';
CI <= '0';
OEbar <= '0';
D <= "000000000000";

wait for 4 ns;

clk <= '0';

wait for 4 ns;

wait for 1 ns;

--------------------------

clk <= '1'; --	 Cycle No: 1014

wait for 1 ns;

I <= "0010";
CCEN_BAR <= '0';
CC_BAR <= '0';
RLD_BAR <= '1';
CI <= '0';
OEbar <= '0';
D <= "111111111111";

wait for 4 ns;

clk <= '0';

wait for 4 ns;

wait for 1 ns;

--------------------------

clk <= '1'; --	 Cycle No: 1015

wait for 1 ns;

I <= "1000";
CCEN_BAR <= '0';
CC_BAR <= '0';
RLD_BAR <= '1';
CI <= '0';
OEbar <= '0';

wait for 4 ns;

clk <= '0';

wait for 4 ns;

assert (Y = "111111111111")
report
"Assert 571 : < Y /= 111111111111 >" -- 	Vector No: 425
severity warning;

wait for 1 ns;

-- **************************************************
-- **************************************************
--------------------------

clk <= '1'; --	 Cycle No: 1016

wait for 1 ns;

I <= "0000";
CCEN_BAR <= '0';
CC_BAR <= '0';
RLD_BAR <= '0';
CI <= '0';
OEbar <= '0';
D <= "000000000000";

wait for 4 ns;

clk <= '0';

wait for 4 ns;

wait for 1 ns;

--------------------------

clk <= '1'; --	 Cycle No: 1017

wait for 1 ns;

I <= "0010";
CCEN_BAR <= '0';
CC_BAR <= '0';
RLD_BAR <= '1';
CI <= '0';
OEbar <= '0';
D <= "000000000000";

wait for 4 ns;

clk <= '0';

wait for 4 ns;

wait for 1 ns;

--------------------------

clk <= '1'; --	 Cycle No: 1018

wait for 1 ns;

I <= "1000";
CCEN_BAR <= '0';
CC_BAR <= '1';
RLD_BAR <= '1';
CI <= '0';
OEbar <= '0';

wait for 4 ns;

clk <= '0';

wait for 4 ns;

assert (Y = "000000000000")
report
"Assert 572 : < Y /= 000000000000 >" -- 	Vector No: 426
severity warning;

wait for 1 ns;

-- **************************************************
--------------------------

clk <= '1'; --	 Cycle No: 1019

wait for 1 ns;

I <= "0000";
CCEN_BAR <= '0';
CC_BAR <= '0';
RLD_BAR <= '0';
CI <= '0';
OEbar <= '0';
D <= "000000000000";

wait for 4 ns;

clk <= '0';

wait for 4 ns;

wait for 1 ns;

--------------------------

clk <= '1'; --	 Cycle No: 1020

wait for 1 ns;

I <= "0010";
CCEN_BAR <= '0';
CC_BAR <= '0';
RLD_BAR <= '1';
CI <= '0';
OEbar <= '0';
D <= "111111111111";

wait for 4 ns;

clk <= '0';

wait for 4 ns;

wait for 1 ns;

--------------------------

clk <= '1'; --	 Cycle No: 1021

wait for 1 ns;

I <= "1000";
CCEN_BAR <= '0';
CC_BAR <= '1';
RLD_BAR <= '1';
CI <= '0';
OEbar <= '0';

wait for 4 ns;

clk <= '0';

wait for 4 ns;

assert (Y = "111111111111")
report
"Assert 573 : < Y /= 111111111111 >" -- 	Vector No: 427
severity warning;

wait for 1 ns;

-- **************************************************
-- **************************************************
--------------------------

clk <= '1'; --	 Cycle No: 1022

wait for 1 ns;

I <= "0000";
CCEN_BAR <= '0';
CC_BAR <= '0';
RLD_BAR <= '0';
CI <= '0';
OEbar <= '0';
D <= "000000000000";

wait for 4 ns;

clk <= '0';

wait for 4 ns;

wait for 1 ns;

--------------------------

clk <= '1'; --	 Cycle No: 1023

wait for 1 ns;

I <= "0010";
CCEN_BAR <= '0';
CC_BAR <= '0';
RLD_BAR <= '1';
CI <= '0';
OEbar <= '0';
D <= "000000000000";

wait for 4 ns;

clk <= '0';

wait for 4 ns;

wait for 1 ns;

--------------------------

clk <= '1'; --	 Cycle No: 1024

wait for 1 ns;

I <= "1000";
CCEN_BAR <= '1';
CC_BAR <= '1';
RLD_BAR <= '1';
CI <= '0';
OEbar <= '0';

wait for 4 ns;

clk <= '0';

wait for 4 ns;

assert (Y = "000000000000")
report
"Assert 574 : < Y /= 000000000000 >" -- 	Vector No: 428
severity warning;

wait for 1 ns;

-- **************************************************
--------------------------

clk <= '1'; --	 Cycle No: 1025

wait for 1 ns;

I <= "0000";
CCEN_BAR <= '0';
CC_BAR <= '0';
RLD_BAR <= '0';
CI <= '0';
OEbar <= '0';
D <= "000000000000";

wait for 4 ns;

clk <= '0';

wait for 4 ns;

wait for 1 ns;

--------------------------

clk <= '1'; --	 Cycle No: 1026

wait for 1 ns;

I <= "0010";
CCEN_BAR <= '0';
CC_BAR <= '0';
RLD_BAR <= '1';
CI <= '0';
OEbar <= '0';
D <= "111111111111";

wait for 4 ns;

clk <= '0';

wait for 4 ns;

wait for 1 ns;

--------------------------

clk <= '1'; --	 Cycle No: 1027

wait for 1 ns;

I <= "1000";
CCEN_BAR <= '1';
CC_BAR <= '1';
RLD_BAR <= '1';
CI <= '0';
OEbar <= '0';

wait for 4 ns;

clk <= '0';

wait for 4 ns;

assert (Y = "111111111111")
report
"Assert 575 : < Y /= 111111111111 >" -- 	Vector No: 429
severity warning;

wait for 1 ns;

-- **************************************************
-- **************************************************
--------------------------

clk <= '1'; --	 Cycle No: 1028

wait for 1 ns;

I <= "0000";
CCEN_BAR <= '0';
CC_BAR <= '0';
RLD_BAR <= '0';
CI <= '0';
OEbar <= '0';
D <= "000000000000";

wait for 4 ns;

clk <= '0';

wait for 4 ns;

wait for 1 ns;

--------------------------

clk <= '1'; --	 Cycle No: 1029

wait for 1 ns;

I <= "0010";
CCEN_BAR <= '0';
CC_BAR <= '0';
RLD_BAR <= '1';
CI <= '0';
OEbar <= '0';
D <= "000000000000";

wait for 4 ns;

clk <= '0';

wait for 4 ns;

wait for 1 ns;

--------------------------

clk <= '1'; --	 Cycle No: 1030

wait for 1 ns;

I <= "1000";
CCEN_BAR <= '1';
CC_BAR <= '0';
RLD_BAR <= '1';
CI <= '0';
OEbar <= '0';

wait for 4 ns;

clk <= '0';

wait for 4 ns;

assert (Y = "000000000000")
report
"Assert 576 : < Y /= 000000000000 >" -- 	Vector No: 430
severity warning;

wait for 1 ns;

-- **************************************************
--------------------------

clk <= '1'; --	 Cycle No: 1031

wait for 1 ns;

I <= "0000";
CCEN_BAR <= '0';
CC_BAR <= '0';
RLD_BAR <= '0';
CI <= '0';
OEbar <= '0';
D <= "000000000000";

wait for 4 ns;

clk <= '0';

wait for 4 ns;

wait for 1 ns;

--------------------------

clk <= '1'; --	 Cycle No: 1032

wait for 1 ns;

I <= "0010";
CCEN_BAR <= '0';
CC_BAR <= '0';
RLD_BAR <= '1';
CI <= '0';
OEbar <= '0';
D <= "111111111111";

wait for 4 ns;

clk <= '0';

wait for 4 ns;

wait for 1 ns;

--------------------------

clk <= '1'; --	 Cycle No: 1033

wait for 1 ns;

I <= "1000";
CCEN_BAR <= '1';
CC_BAR <= '0';
RLD_BAR <= '1';
CI <= '0';
OEbar <= '0';

wait for 4 ns;

clk <= '0';

wait for 4 ns;

assert (Y = "111111111111")
report
"Assert 577 : < Y /= 111111111111 >" -- 	Vector No: 431
severity warning;

wait for 1 ns;

-- **************************************************
-- **************************************************
-- ** I = 9 **
--------------------------

clk <= '1'; --	 Cycle No: 1034

wait for 1 ns;

I <= "0000";
CCEN_BAR <= '0';
CC_BAR <= '0';
RLD_BAR <= '0';
CI <= '0';
OEbar <= '0';
D <= "000000000000";

wait for 4 ns;

clk <= '0';

wait for 4 ns;

wait for 1 ns;

--------------------------

clk <= '1'; --	 Cycle No: 1035

wait for 1 ns;

I <= "0010";
CCEN_BAR <= '0';
CC_BAR <= '0';
RLD_BAR <= '1';
CI <= '0';
OEbar <= '0';
D <= "000000000000";

wait for 4 ns;

clk <= '0';

wait for 4 ns;

wait for 1 ns;

--------------------------

clk <= '1'; --	 Cycle No: 1036

wait for 1 ns;

I <= "1001";
CCEN_BAR <= '0';
CC_BAR <= '0';
RLD_BAR <= '1';
CI <= '0';
OEbar <= '0';

wait for 4 ns;

clk <= '0';

wait for 4 ns;

assert (Y = "000000000000")
report
"Assert 578 : < Y /= 000000000000 >" -- 	Vector No: 432
severity warning;

wait for 1 ns;

-- **************************************************
--------------------------

clk <= '1'; --	 Cycle No: 1037

wait for 1 ns;

I <= "0000";
CCEN_BAR <= '0';
CC_BAR <= '0';
RLD_BAR <= '0';
CI <= '0';
OEbar <= '0';
D <= "000000000000";

wait for 4 ns;

clk <= '0';

wait for 4 ns;

wait for 1 ns;

--------------------------

clk <= '1'; --	 Cycle No: 1038

wait for 1 ns;

I <= "0010";
CCEN_BAR <= '0';
CC_BAR <= '0';
RLD_BAR <= '1';
CI <= '0';
OEbar <= '0';
D <= "111111111111";

wait for 4 ns;

clk <= '0';

wait for 4 ns;

wait for 1 ns;

--------------------------

clk <= '1'; --	 Cycle No: 1039

wait for 1 ns;

I <= "1001";
CCEN_BAR <= '0';
CC_BAR <= '0';
RLD_BAR <= '1';
CI <= '0';
OEbar <= '0';

wait for 4 ns;

clk <= '0';

wait for 4 ns;

assert (Y = "111111111111")
report
"Assert 579 : < Y /= 111111111111 >" -- 	Vector No: 433
severity warning;

wait for 1 ns;

-- **************************************************
-- **************************************************
--------------------------

clk <= '1'; --	 Cycle No: 1040

wait for 1 ns;

I <= "0000";
CCEN_BAR <= '0';
CC_BAR <= '0';
RLD_BAR <= '0';
CI <= '0';
OEbar <= '0';
D <= "000000000000";

wait for 4 ns;

clk <= '0';

wait for 4 ns;

wait for 1 ns;

--------------------------

clk <= '1'; --	 Cycle No: 1041

wait for 1 ns;

I <= "0010";
CCEN_BAR <= '0';
CC_BAR <= '0';
RLD_BAR <= '1';
CI <= '0';
OEbar <= '0';
D <= "000000000000";

wait for 4 ns;

clk <= '0';

wait for 4 ns;

wait for 1 ns;

--------------------------

clk <= '1'; --	 Cycle No: 1042

wait for 1 ns;

I <= "1001";
CCEN_BAR <= '0';
CC_BAR <= '1';
RLD_BAR <= '1';
CI <= '0';
OEbar <= '0';

wait for 4 ns;

clk <= '0';

wait for 4 ns;

assert (Y = "000000000000")
report
"Assert 580 : < Y /= 000000000000 >" -- 	Vector No: 434
severity warning;

wait for 1 ns;

-- **************************************************
--------------------------

clk <= '1'; --	 Cycle No: 1043

wait for 1 ns;

I <= "0000";
CCEN_BAR <= '0';
CC_BAR <= '0';
RLD_BAR <= '0';
CI <= '0';
OEbar <= '0';
D <= "000000000000";

wait for 4 ns;

clk <= '0';

wait for 4 ns;

wait for 1 ns;

--------------------------

clk <= '1'; --	 Cycle No: 1044

wait for 1 ns;

I <= "0010";
CCEN_BAR <= '0';
CC_BAR <= '0';
RLD_BAR <= '1';
CI <= '0';
OEbar <= '0';
D <= "111111111111";

wait for 4 ns;

clk <= '0';

wait for 4 ns;

wait for 1 ns;

--------------------------

clk <= '1'; --	 Cycle No: 1045

wait for 1 ns;

I <= "1001";
CCEN_BAR <= '0';
CC_BAR <= '1';
RLD_BAR <= '1';
CI <= '0';
OEbar <= '0';

wait for 4 ns;

clk <= '0';

wait for 4 ns;

assert (Y = "111111111111")
report
"Assert 581 : < Y /= 111111111111 >" -- 	Vector No: 435
severity warning;

wait for 1 ns;

-- **************************************************
-- **************************************************
--------------------------

clk <= '1'; --	 Cycle No: 1046

wait for 1 ns;

I <= "0000";
CCEN_BAR <= '0';
CC_BAR <= '0';
RLD_BAR <= '0';
CI <= '0';
OEbar <= '0';
D <= "000000000000";

wait for 4 ns;

clk <= '0';

wait for 4 ns;

wait for 1 ns;

--------------------------

clk <= '1'; --	 Cycle No: 1047

wait for 1 ns;

I <= "0010";
CCEN_BAR <= '0';
CC_BAR <= '0';
RLD_BAR <= '1';
CI <= '0';
OEbar <= '0';
D <= "000000000000";

wait for 4 ns;

clk <= '0';

wait for 4 ns;

wait for 1 ns;

--------------------------

clk <= '1'; --	 Cycle No: 1048

wait for 1 ns;

I <= "1001";
CCEN_BAR <= '1';
CC_BAR <= '1';
RLD_BAR <= '1';
CI <= '0';
OEbar <= '0';

wait for 4 ns;

clk <= '0';

wait for 4 ns;

assert (Y = "000000000000")
report
"Assert 582 : < Y /= 000000000000 >" -- 	Vector No: 436
severity warning;

wait for 1 ns;

-- **************************************************
--------------------------

clk <= '1'; --	 Cycle No: 1049

wait for 1 ns;

I <= "0000";
CCEN_BAR <= '0';
CC_BAR <= '0';
RLD_BAR <= '0';
CI <= '0';
OEbar <= '0';
D <= "000000000000";

wait for 4 ns;

clk <= '0';

wait for 4 ns;

wait for 1 ns;

--------------------------

clk <= '1'; --	 Cycle No: 1050

wait for 1 ns;

I <= "0010";
CCEN_BAR <= '0';
CC_BAR <= '0';
RLD_BAR <= '1';
CI <= '0';
OEbar <= '0';
D <= "111111111111";

wait for 4 ns;

clk <= '0';

wait for 4 ns;

wait for 1 ns;

--------------------------

clk <= '1'; --	 Cycle No: 1051

wait for 1 ns;

I <= "1001";
CCEN_BAR <= '1';
CC_BAR <= '1';
RLD_BAR <= '1';
CI <= '0';
OEbar <= '0';

wait for 4 ns;

clk <= '0';

wait for 4 ns;

assert (Y = "111111111111")
report
"Assert 583 : < Y /= 111111111111 >" -- 	Vector No: 437
severity warning;

wait for 1 ns;

-- **************************************************
-- **************************************************
--------------------------

clk <= '1'; --	 Cycle No: 1052

wait for 1 ns;

I <= "0000";
CCEN_BAR <= '0';
CC_BAR <= '0';
RLD_BAR <= '0';
CI <= '0';
OEbar <= '0';
D <= "000000000000";

wait for 4 ns;

clk <= '0';

wait for 4 ns;

wait for 1 ns;

--------------------------

clk <= '1'; --	 Cycle No: 1053

wait for 1 ns;

I <= "0010";
CCEN_BAR <= '0';
CC_BAR <= '0';
RLD_BAR <= '1';
CI <= '0';
OEbar <= '0';
D <= "000000000000";

wait for 4 ns;

clk <= '0';

wait for 4 ns;

wait for 1 ns;

--------------------------

clk <= '1'; --	 Cycle No: 1054

wait for 1 ns;

I <= "1001";
CCEN_BAR <= '1';
CC_BAR <= '0';
RLD_BAR <= '1';
CI <= '0';
OEbar <= '0';

wait for 4 ns;

clk <= '0';

wait for 4 ns;

assert (Y = "000000000000")
report
"Assert 584 : < Y /= 000000000000 >" -- 	Vector No: 438
severity warning;

wait for 1 ns;

-- **************************************************
--------------------------

clk <= '1'; --	 Cycle No: 1055

wait for 1 ns;

I <= "0000";
CCEN_BAR <= '0';
CC_BAR <= '0';
RLD_BAR <= '0';
CI <= '0';
OEbar <= '0';
D <= "000000000000";

wait for 4 ns;

clk <= '0';

wait for 4 ns;

wait for 1 ns;

--------------------------

clk <= '1'; --	 Cycle No: 1056

wait for 1 ns;

I <= "0010";
CCEN_BAR <= '0';
CC_BAR <= '0';
RLD_BAR <= '1';
CI <= '0';
OEbar <= '0';
D <= "111111111111";

wait for 4 ns;

clk <= '0';

wait for 4 ns;

wait for 1 ns;

--------------------------

clk <= '1'; --	 Cycle No: 1057

wait for 1 ns;

I <= "1001";
CCEN_BAR <= '1';
CC_BAR <= '0';
RLD_BAR <= '1';
CI <= '0';
OEbar <= '0';

wait for 4 ns;

clk <= '0';

wait for 4 ns;

assert (Y = "111111111111")
report
"Assert 585 : < Y /= 111111111111 >" -- 	Vector No: 439
severity warning;

wait for 1 ns;

-- **************************************************
-- **************************************************
-- ** I = 10 **
--------------------------

clk <= '1'; --	 Cycle No: 1058

wait for 1 ns;

I <= "0000";
CCEN_BAR <= '0';
CC_BAR <= '0';
RLD_BAR <= '0';
CI <= '0';
OEbar <= '0';
D <= "000000000000";

wait for 4 ns;

clk <= '0';

wait for 4 ns;

wait for 1 ns;

--------------------------

clk <= '1'; --	 Cycle No: 1059

wait for 1 ns;

I <= "0010";
CCEN_BAR <= '0';
CC_BAR <= '0';
RLD_BAR <= '1';
CI <= '0';
OEbar <= '0';
D <= "000000000000";

wait for 4 ns;

clk <= '0';

wait for 4 ns;

wait for 1 ns;

--------------------------

clk <= '1'; --	 Cycle No: 1060

wait for 1 ns;

I <= "1010";
CCEN_BAR <= '0';
CC_BAR <= '1';
RLD_BAR <= '1';
CI <= '0';
OEbar <= '0';

wait for 4 ns;

clk <= '0';

wait for 4 ns;

assert (Y = "000000000000")
report
"Assert 586 : < Y /= 000000000000 >" -- 	Vector No: 440
severity warning;

wait for 1 ns;

-- **************************************************
--------------------------

clk <= '1'; --	 Cycle No: 1061

wait for 1 ns;

I <= "0000";
CCEN_BAR <= '0';
CC_BAR <= '0';
RLD_BAR <= '0';
CI <= '0';
OEbar <= '0';
D <= "000000000000";

wait for 4 ns;

clk <= '0';

wait for 4 ns;

wait for 1 ns;

--------------------------

clk <= '1'; --	 Cycle No: 1062

wait for 1 ns;

I <= "0010";
CCEN_BAR <= '0';
CC_BAR <= '0';
RLD_BAR <= '1';
CI <= '0';
OEbar <= '0';
D <= "111111111111";

wait for 4 ns;

clk <= '0';

wait for 4 ns;

wait for 1 ns;

--------------------------

clk <= '1'; --	 Cycle No: 1063

wait for 1 ns;

I <= "1010";
CCEN_BAR <= '0';
CC_BAR <= '1';
RLD_BAR <= '1';
CI <= '0';
OEbar <= '0';

wait for 4 ns;

clk <= '0';

wait for 4 ns;

assert (Y = "111111111111")
report
"Assert 587 : < Y /= 111111111111 >" -- 	Vector No: 441
severity warning;

wait for 1 ns;

-- **************************************************
--------------------------

clk <= '1'; --	 Cycle No: 1064

wait for 1 ns;

I <= "0000";
CCEN_BAR <= '0';
CC_BAR <= '0';
RLD_BAR <= '0';
CI <= '0';
OEbar <= '0';
D <= "111111111111";

wait for 4 ns;

clk <= '0';

wait for 4 ns;

wait for 1 ns;

--------------------------

clk <= '1'; --	 Cycle No: 1065

wait for 1 ns;

I <= "0010";
CCEN_BAR <= '0';
CC_BAR <= '0';
RLD_BAR <= '1';
CI <= '0';
OEbar <= '0';
D <= "000000000000";

wait for 4 ns;

clk <= '0';

wait for 4 ns;

wait for 1 ns;

--------------------------

clk <= '1'; --	 Cycle No: 1066

wait for 1 ns;

I <= "1010";
CCEN_BAR <= '0';
CC_BAR <= '1';
RLD_BAR <= '1';
CI <= '0';
OEbar <= '0';

wait for 4 ns;

clk <= '0';

wait for 4 ns;

assert (Y = "000000000000")
report
"Assert 588 : < Y /= 000000000000 >" -- 	Vector No: 442
severity warning;

wait for 1 ns;

-- **************************************************
--------------------------

clk <= '1'; --	 Cycle No: 1067

wait for 1 ns;

I <= "0000";
CCEN_BAR <= '0';
CC_BAR <= '0';
RLD_BAR <= '0';
CI <= '0';
OEbar <= '0';
D <= "111111111111";

wait for 4 ns;

clk <= '0';

wait for 4 ns;

wait for 1 ns;

--------------------------

clk <= '1'; --	 Cycle No: 1068

wait for 1 ns;

I <= "0010";
CCEN_BAR <= '0';
CC_BAR <= '0';
RLD_BAR <= '1';
CI <= '0';
OEbar <= '0';
D <= "111111111111";

wait for 4 ns;

clk <= '0';

wait for 4 ns;

wait for 1 ns;

--------------------------

clk <= '1'; --	 Cycle No: 1069

wait for 1 ns;

I <= "1010";
CCEN_BAR <= '0';
CC_BAR <= '1';
RLD_BAR <= '1';
CI <= '0';
OEbar <= '0';

wait for 4 ns;

clk <= '0';

wait for 4 ns;

assert (Y = "111111111111")
report
"Assert 589 : < Y /= 111111111111 >" -- 	Vector No: 443
severity warning;

wait for 1 ns;

-- **************************************************
-- **************************************************
-- ** I = 11 **
--------------------------

clk <= '1'; --	 Cycle No: 1070

wait for 1 ns;

I <= "0000";
CCEN_BAR <= '0';
CC_BAR <= '0';
RLD_BAR <= '0';
CI <= '0';
OEbar <= '0';
D <= "000000000000";

wait for 4 ns;

clk <= '0';

wait for 4 ns;

wait for 1 ns;

--------------------------

clk <= '1'; --	 Cycle No: 1071

wait for 1 ns;

I <= "0010";
CCEN_BAR <= '0';
CC_BAR <= '0';
RLD_BAR <= '1';
CI <= '0';
OEbar <= '0';
D <= "000000000000";

wait for 4 ns;

clk <= '0';

wait for 4 ns;

wait for 1 ns;

--------------------------

clk <= '1'; --	 Cycle No: 1072

wait for 1 ns;

I <= "1011";
CCEN_BAR <= '0';
CC_BAR <= '1';
RLD_BAR <= '1';
CI <= '0';
OEbar <= '0';

wait for 4 ns;

clk <= '0';

wait for 4 ns;

assert (Y = "000000000000")
report
"Assert 590 : < Y /= 000000000000 >" -- 	Vector No: 444
severity warning;

wait for 1 ns;

-- **************************************************
--------------------------

clk <= '1'; --	 Cycle No: 1073

wait for 1 ns;

I <= "0000";
CCEN_BAR <= '0';
CC_BAR <= '0';
RLD_BAR <= '0';
CI <= '0';
OEbar <= '0';
D <= "000000000000";

wait for 4 ns;

clk <= '0';

wait for 4 ns;

wait for 1 ns;

--------------------------

clk <= '1'; --	 Cycle No: 1074

wait for 1 ns;

I <= "0010";
CCEN_BAR <= '0';
CC_BAR <= '0';
RLD_BAR <= '1';
CI <= '0';
OEbar <= '0';
D <= "111111111111";

wait for 4 ns;

clk <= '0';

wait for 4 ns;

wait for 1 ns;

--------------------------

clk <= '1'; --	 Cycle No: 1075

wait for 1 ns;

I <= "1011";
CCEN_BAR <= '0';
CC_BAR <= '1';
RLD_BAR <= '1';
CI <= '0';
OEbar <= '0';

wait for 4 ns;

clk <= '0';

wait for 4 ns;

assert (Y = "111111111111")
report
"Assert 591 : < Y /= 111111111111 >" -- 	Vector No: 445
severity warning;

wait for 1 ns;

-- **************************************************
--------------------------

clk <= '1'; --	 Cycle No: 1076

wait for 1 ns;

I <= "0000";
CCEN_BAR <= '0';
CC_BAR <= '0';
RLD_BAR <= '0';
CI <= '0';
OEbar <= '0';
D <= "111111111111";

wait for 4 ns;

clk <= '0';

wait for 4 ns;

wait for 1 ns;

--------------------------

clk <= '1'; --	 Cycle No: 1077

wait for 1 ns;

I <= "0010";
CCEN_BAR <= '0';
CC_BAR <= '0';
RLD_BAR <= '1';
CI <= '0';
OEbar <= '0';
D <= "000000000000";

wait for 4 ns;

clk <= '0';

wait for 4 ns;

wait for 1 ns;

--------------------------

clk <= '1'; --	 Cycle No: 1078

wait for 1 ns;

I <= "1011";
CCEN_BAR <= '0';
CC_BAR <= '1';
RLD_BAR <= '1';
CI <= '0';
OEbar <= '0';

wait for 4 ns;

clk <= '0';

wait for 4 ns;

assert (Y = "000000000000")
report
"Assert 592 : < Y /= 000000000000 >" -- 	Vector No: 446
severity warning;

wait for 1 ns;

-- **************************************************
--------------------------

clk <= '1'; --	 Cycle No: 1079

wait for 1 ns;

I <= "0000";
CCEN_BAR <= '0';
CC_BAR <= '0';
RLD_BAR <= '0';
CI <= '0';
OEbar <= '0';
D <= "111111111111";

wait for 4 ns;

clk <= '0';

wait for 4 ns;

wait for 1 ns;

--------------------------

clk <= '1'; --	 Cycle No: 1080

wait for 1 ns;

I <= "0010";
CCEN_BAR <= '0';
CC_BAR <= '0';
RLD_BAR <= '1';
CI <= '0';
OEbar <= '0';
D <= "111111111111";

wait for 4 ns;

clk <= '0';

wait for 4 ns;

wait for 1 ns;

--------------------------

clk <= '1'; --	 Cycle No: 1081

wait for 1 ns;

I <= "1011";
CCEN_BAR <= '0';
CC_BAR <= '1';
RLD_BAR <= '1';
CI <= '0';
OEbar <= '0';

wait for 4 ns;

clk <= '0';

wait for 4 ns;

assert (Y = "111111111111")
report
"Assert 593 : < Y /= 111111111111 >" -- 	Vector No: 447
severity warning;

wait for 1 ns;

-- **************************************************
-- **************************************************
-- ** I = 12 **
--------------------------

clk <= '1'; --	 Cycle No: 1082

wait for 1 ns;

I <= "0000";
CCEN_BAR <= '0';
CC_BAR <= '0';
RLD_BAR <= '0';
CI <= '0';
OEbar <= '0';
D <= "000000000000";

wait for 4 ns;

clk <= '0';

wait for 4 ns;

wait for 1 ns;

--------------------------

clk <= '1'; --	 Cycle No: 1083

wait for 1 ns;

I <= "0010";
CCEN_BAR <= '0';
CC_BAR <= '0';
RLD_BAR <= '1';
CI <= '0';
OEbar <= '0';
D <= "000000000000";

wait for 4 ns;

clk <= '0';

wait for 4 ns;

wait for 1 ns;

--------------------------

clk <= '1'; --	 Cycle No: 1084

wait for 1 ns;

I <= "1100";
CCEN_BAR <= '0';
CC_BAR <= '0';
RLD_BAR <= '1';
CI <= '0';
OEbar <= '0';

wait for 4 ns;

clk <= '0';

wait for 4 ns;

assert (Y = "000000000000")
report
"Assert 594 : < Y /= 000000000000 >" -- 	Vector No: 448
severity warning;

wait for 1 ns;

-- **************************************************
--------------------------

clk <= '1'; --	 Cycle No: 1085

wait for 1 ns;

I <= "0000";
CCEN_BAR <= '0';
CC_BAR <= '0';
RLD_BAR <= '0';
CI <= '0';
OEbar <= '0';
D <= "000000000000";

wait for 4 ns;

clk <= '0';

wait for 4 ns;

wait for 1 ns;

--------------------------

clk <= '1'; --	 Cycle No: 1086

wait for 1 ns;

I <= "0010";
CCEN_BAR <= '0';
CC_BAR <= '0';
RLD_BAR <= '1';
CI <= '0';
OEbar <= '0';
D <= "111111111111";

wait for 4 ns;

clk <= '0';

wait for 4 ns;

wait for 1 ns;

--------------------------

clk <= '1'; --	 Cycle No: 1087

wait for 1 ns;

I <= "1100";
CCEN_BAR <= '0';
CC_BAR <= '0';
RLD_BAR <= '1';
CI <= '0';
OEbar <= '0';

wait for 4 ns;

clk <= '0';

wait for 4 ns;

assert (Y = "111111111111")
report
"Assert 595 : < Y /= 111111111111 >" -- 	Vector No: 449
severity warning;

wait for 1 ns;

-- **************************************************
--------------------------

clk <= '1'; --	 Cycle No: 1088

wait for 1 ns;

I <= "0000";
CCEN_BAR <= '0';
CC_BAR <= '0';
RLD_BAR <= '0';
CI <= '0';
OEbar <= '0';
D <= "111111111111";

wait for 4 ns;

clk <= '0';

wait for 4 ns;

wait for 1 ns;

--------------------------

clk <= '1'; --	 Cycle No: 1089

wait for 1 ns;

I <= "0010";
CCEN_BAR <= '0';
CC_BAR <= '0';
RLD_BAR <= '1';
CI <= '0';
OEbar <= '0';
D <= "000000000000";

wait for 4 ns;

clk <= '0';

wait for 4 ns;

wait for 1 ns;

--------------------------

clk <= '1'; --	 Cycle No: 1090

wait for 1 ns;

I <= "1100";
CCEN_BAR <= '0';
CC_BAR <= '0';
RLD_BAR <= '1';
CI <= '0';
OEbar <= '0';

wait for 4 ns;

clk <= '0';

wait for 4 ns;

assert (Y = "000000000000")
report
"Assert 596 : < Y /= 000000000000 >" -- 	Vector No: 450
severity warning;

wait for 1 ns;

-- **************************************************
--------------------------

clk <= '1'; --	 Cycle No: 1091

wait for 1 ns;

I <= "0000";
CCEN_BAR <= '0';
CC_BAR <= '0';
RLD_BAR <= '0';
CI <= '0';
OEbar <= '0';
D <= "111111111111";

wait for 4 ns;

clk <= '0';

wait for 4 ns;

wait for 1 ns;

--------------------------

clk <= '1'; --	 Cycle No: 1092

wait for 1 ns;

I <= "0010";
CCEN_BAR <= '0';
CC_BAR <= '0';
RLD_BAR <= '1';
CI <= '0';
OEbar <= '0';
D <= "111111111111";

wait for 4 ns;

clk <= '0';

wait for 4 ns;

wait for 1 ns;

--------------------------

clk <= '1'; --	 Cycle No: 1093

wait for 1 ns;

I <= "1100";
CCEN_BAR <= '0';
CC_BAR <= '0';
RLD_BAR <= '1';
CI <= '0';
OEbar <= '0';

wait for 4 ns;

clk <= '0';

wait for 4 ns;

assert (Y = "111111111111")
report
"Assert 597 : < Y /= 111111111111 >" -- 	Vector No: 451
severity warning;

wait for 1 ns;

-- **************************************************
-- **************************************************
--------------------------

clk <= '1'; --	 Cycle No: 1094

wait for 1 ns;

I <= "0000";
CCEN_BAR <= '0';
CC_BAR <= '0';
RLD_BAR <= '0';
CI <= '0';
OEbar <= '0';
D <= "000000000000";

wait for 4 ns;

clk <= '0';

wait for 4 ns;

wait for 1 ns;

--------------------------

clk <= '1'; --	 Cycle No: 1095

wait for 1 ns;

I <= "0010";
CCEN_BAR <= '0';
CC_BAR <= '0';
RLD_BAR <= '1';
CI <= '0';
OEbar <= '0';
D <= "000000000000";

wait for 4 ns;

clk <= '0';

wait for 4 ns;

wait for 1 ns;

--------------------------

clk <= '1'; --	 Cycle No: 1096

wait for 1 ns;

I <= "1100";
CCEN_BAR <= '0';
CC_BAR <= '1';
RLD_BAR <= '1';
CI <= '0';
OEbar <= '0';

wait for 4 ns;

clk <= '0';

wait for 4 ns;

assert (Y = "000000000000")
report
"Assert 598 : < Y /= 000000000000 >" -- 	Vector No: 452
severity warning;

wait for 1 ns;

-- **************************************************
--------------------------

clk <= '1'; --	 Cycle No: 1097

wait for 1 ns;

I <= "0000";
CCEN_BAR <= '0';
CC_BAR <= '0';
RLD_BAR <= '0';
CI <= '0';
OEbar <= '0';
D <= "000000000000";

wait for 4 ns;

clk <= '0';

wait for 4 ns;

wait for 1 ns;

--------------------------

clk <= '1'; --	 Cycle No: 1098

wait for 1 ns;

I <= "0010";
CCEN_BAR <= '0';
CC_BAR <= '0';
RLD_BAR <= '1';
CI <= '0';
OEbar <= '0';
D <= "111111111111";

wait for 4 ns;

clk <= '0';

wait for 4 ns;

wait for 1 ns;

--------------------------

clk <= '1'; --	 Cycle No: 1099

wait for 1 ns;

I <= "1100";
CCEN_BAR <= '0';
CC_BAR <= '1';
RLD_BAR <= '1';
CI <= '0';
OEbar <= '0';

wait for 4 ns;

clk <= '0';

wait for 4 ns;

assert (Y = "111111111111")
report
"Assert 599 : < Y /= 111111111111 >" -- 	Vector No: 453
severity warning;

wait for 1 ns;

-- **************************************************
--------------------------

clk <= '1'; --	 Cycle No: 1100

wait for 1 ns;

I <= "0000";
CCEN_BAR <= '0';
CC_BAR <= '0';
RLD_BAR <= '0';
CI <= '0';
OEbar <= '0';
D <= "111111111111";

wait for 4 ns;

clk <= '0';

wait for 4 ns;

wait for 1 ns;

--------------------------

clk <= '1'; --	 Cycle No: 1101

wait for 1 ns;

I <= "0010";
CCEN_BAR <= '0';
CC_BAR <= '0';
RLD_BAR <= '1';
CI <= '0';
OEbar <= '0';
D <= "000000000000";

wait for 4 ns;

clk <= '0';

wait for 4 ns;

wait for 1 ns;

--------------------------

clk <= '1'; --	 Cycle No: 1102

wait for 1 ns;

I <= "1100";
CCEN_BAR <= '0';
CC_BAR <= '1';
RLD_BAR <= '1';
CI <= '0';
OEbar <= '0';

wait for 4 ns;

clk <= '0';

wait for 4 ns;

assert (Y = "000000000000")
report
"Assert 600 : < Y /= 000000000000 >" -- 	Vector No: 454
severity warning;

wait for 1 ns;

-- **************************************************
--------------------------

clk <= '1'; --	 Cycle No: 1103

wait for 1 ns;

I <= "0000";
CCEN_BAR <= '0';
CC_BAR <= '0';
RLD_BAR <= '0';
CI <= '0';
OEbar <= '0';
D <= "111111111111";

wait for 4 ns;

clk <= '0';

wait for 4 ns;

wait for 1 ns;

--------------------------

clk <= '1'; --	 Cycle No: 1104

wait for 1 ns;

I <= "0010";
CCEN_BAR <= '0';
CC_BAR <= '0';
RLD_BAR <= '1';
CI <= '0';
OEbar <= '0';
D <= "111111111111";

wait for 4 ns;

clk <= '0';

wait for 4 ns;

wait for 1 ns;

--------------------------

clk <= '1'; --	 Cycle No: 1105

wait for 1 ns;

I <= "1100";
CCEN_BAR <= '0';
CC_BAR <= '1';
RLD_BAR <= '1';
CI <= '0';
OEbar <= '0';

wait for 4 ns;

clk <= '0';

wait for 4 ns;

assert (Y = "111111111111")
report
"Assert 601 : < Y /= 111111111111 >" -- 	Vector No: 455
severity warning;

wait for 1 ns;

-- **************************************************
-- **************************************************
--------------------------

clk <= '1'; --	 Cycle No: 1106

wait for 1 ns;

I <= "0000";
CCEN_BAR <= '0';
CC_BAR <= '0';
RLD_BAR <= '0';
CI <= '0';
OEbar <= '0';
D <= "000000000000";

wait for 4 ns;

clk <= '0';

wait for 4 ns;

wait for 1 ns;

--------------------------

clk <= '1'; --	 Cycle No: 1107

wait for 1 ns;

I <= "0010";
CCEN_BAR <= '0';
CC_BAR <= '0';
RLD_BAR <= '1';
CI <= '0';
OEbar <= '0';
D <= "000000000000";

wait for 4 ns;

clk <= '0';

wait for 4 ns;

wait for 1 ns;

--------------------------

clk <= '1'; --	 Cycle No: 1108

wait for 1 ns;

I <= "1100";
CCEN_BAR <= '1';
CC_BAR <= '1';
RLD_BAR <= '1';
CI <= '0';
OEbar <= '0';

wait for 4 ns;

clk <= '0';

wait for 4 ns;

assert (Y = "000000000000")
report
"Assert 602 : < Y /= 000000000000 >" -- 	Vector No: 456
severity warning;

wait for 1 ns;

-- **************************************************
--------------------------

clk <= '1'; --	 Cycle No: 1109

wait for 1 ns;

I <= "0000";
CCEN_BAR <= '0';
CC_BAR <= '0';
RLD_BAR <= '0';
CI <= '0';
OEbar <= '0';
D <= "000000000000";

wait for 4 ns;

clk <= '0';

wait for 4 ns;

wait for 1 ns;

--------------------------

clk <= '1'; --	 Cycle No: 1110

wait for 1 ns;

I <= "0010";
CCEN_BAR <= '0';
CC_BAR <= '0';
RLD_BAR <= '1';
CI <= '0';
OEbar <= '0';
D <= "111111111111";

wait for 4 ns;

clk <= '0';

wait for 4 ns;

wait for 1 ns;

--------------------------

clk <= '1'; --	 Cycle No: 1111

wait for 1 ns;

I <= "1100";
CCEN_BAR <= '1';
CC_BAR <= '1';
RLD_BAR <= '1';
CI <= '0';
OEbar <= '0';

wait for 4 ns;

clk <= '0';

wait for 4 ns;

assert (Y = "111111111111")
report
"Assert 603 : < Y /= 111111111111 >" -- 	Vector No: 457
severity warning;

wait for 1 ns;

-- **************************************************
--------------------------

clk <= '1'; --	 Cycle No: 1112

wait for 1 ns;

I <= "0000";
CCEN_BAR <= '0';
CC_BAR <= '0';
RLD_BAR <= '0';
CI <= '0';
OEbar <= '0';
D <= "111111111111";

wait for 4 ns;

clk <= '0';

wait for 4 ns;

wait for 1 ns;

--------------------------

clk <= '1'; --	 Cycle No: 1113

wait for 1 ns;

I <= "0010";
CCEN_BAR <= '0';
CC_BAR <= '0';
RLD_BAR <= '1';
CI <= '0';
OEbar <= '0';
D <= "000000000000";

wait for 4 ns;

clk <= '0';

wait for 4 ns;

wait for 1 ns;

--------------------------

clk <= '1'; --	 Cycle No: 1114

wait for 1 ns;

I <= "1100";
CCEN_BAR <= '1';
CC_BAR <= '1';
RLD_BAR <= '1';
CI <= '0';
OEbar <= '0';

wait for 4 ns;

clk <= '0';

wait for 4 ns;

assert (Y = "000000000000")
report
"Assert 604 : < Y /= 000000000000 >" -- 	Vector No: 458
severity warning;

wait for 1 ns;

-- **************************************************
--------------------------

clk <= '1'; --	 Cycle No: 1115

wait for 1 ns;

I <= "0000";
CCEN_BAR <= '0';
CC_BAR <= '0';
RLD_BAR <= '0';
CI <= '0';
OEbar <= '0';
D <= "111111111111";

wait for 4 ns;

clk <= '0';

wait for 4 ns;

wait for 1 ns;

--------------------------

clk <= '1'; --	 Cycle No: 1116

wait for 1 ns;

I <= "0010";
CCEN_BAR <= '0';
CC_BAR <= '0';
RLD_BAR <= '1';
CI <= '0';
OEbar <= '0';
D <= "111111111111";

wait for 4 ns;

clk <= '0';

wait for 4 ns;

wait for 1 ns;

--------------------------

clk <= '1'; --	 Cycle No: 1117

wait for 1 ns;

I <= "1100";
CCEN_BAR <= '1';
CC_BAR <= '1';
RLD_BAR <= '1';
CI <= '0';
OEbar <= '0';

wait for 4 ns;

clk <= '0';

wait for 4 ns;

assert (Y = "111111111111")
report
"Assert 605 : < Y /= 111111111111 >" -- 	Vector No: 459
severity warning;

wait for 1 ns;

-- **************************************************
-- **************************************************
--------------------------

clk <= '1'; --	 Cycle No: 1118

wait for 1 ns;

I <= "0000";
CCEN_BAR <= '0';
CC_BAR <= '0';
RLD_BAR <= '0';
CI <= '0';
OEbar <= '0';
D <= "000000000000";

wait for 4 ns;

clk <= '0';

wait for 4 ns;

wait for 1 ns;

--------------------------

clk <= '1'; --	 Cycle No: 1119

wait for 1 ns;

I <= "0010";
CCEN_BAR <= '0';
CC_BAR <= '0';
RLD_BAR <= '1';
CI <= '0';
OEbar <= '0';
D <= "000000000000";

wait for 4 ns;

clk <= '0';

wait for 4 ns;

wait for 1 ns;

--------------------------

clk <= '1'; --	 Cycle No: 1120

wait for 1 ns;

I <= "1100";
CCEN_BAR <= '1';
CC_BAR <= '0';
RLD_BAR <= '1';
CI <= '0';
OEbar <= '0';

wait for 4 ns;

clk <= '0';

wait for 4 ns;

assert (Y = "000000000000")
report
"Assert 606 : < Y /= 000000000000 >" -- 	Vector No: 460
severity warning;

wait for 1 ns;

-- **************************************************
--------------------------

clk <= '1'; --	 Cycle No: 1121

wait for 1 ns;

I <= "0000";
CCEN_BAR <= '0';
CC_BAR <= '0';
RLD_BAR <= '0';
CI <= '0';
OEbar <= '0';
D <= "000000000000";

wait for 4 ns;

clk <= '0';

wait for 4 ns;

wait for 1 ns;

--------------------------

clk <= '1'; --	 Cycle No: 1122

wait for 1 ns;

I <= "0010";
CCEN_BAR <= '0';
CC_BAR <= '0';
RLD_BAR <= '1';
CI <= '0';
OEbar <= '0';
D <= "111111111111";

wait for 4 ns;

clk <= '0';

wait for 4 ns;

wait for 1 ns;

--------------------------

clk <= '1'; --	 Cycle No: 1123

wait for 1 ns;

I <= "1100";
CCEN_BAR <= '1';
CC_BAR <= '0';
RLD_BAR <= '1';
CI <= '0';
OEbar <= '0';

wait for 4 ns;

clk <= '0';

wait for 4 ns;

assert (Y = "111111111111")
report
"Assert 607 : < Y /= 111111111111 >" -- 	Vector No: 461
severity warning;

wait for 1 ns;

-- **************************************************
--------------------------

clk <= '1'; --	 Cycle No: 1124

wait for 1 ns;

I <= "0000";
CCEN_BAR <= '0';
CC_BAR <= '0';
RLD_BAR <= '0';
CI <= '0';
OEbar <= '0';
D <= "111111111111";

wait for 4 ns;

clk <= '0';

wait for 4 ns;

wait for 1 ns;

--------------------------

clk <= '1'; --	 Cycle No: 1125

wait for 1 ns;

I <= "0010";
CCEN_BAR <= '0';
CC_BAR <= '0';
RLD_BAR <= '1';
CI <= '0';
OEbar <= '0';
D <= "000000000000";

wait for 4 ns;

clk <= '0';

wait for 4 ns;

wait for 1 ns;

--------------------------

clk <= '1'; --	 Cycle No: 1126

wait for 1 ns;

I <= "1100";
CCEN_BAR <= '1';
CC_BAR <= '0';
RLD_BAR <= '1';
CI <= '0';
OEbar <= '0';

wait for 4 ns;

clk <= '0';

wait for 4 ns;

assert (Y = "000000000000")
report
"Assert 608 : < Y /= 000000000000 >" -- 	Vector No: 462
severity warning;

wait for 1 ns;

-- **************************************************
--------------------------

clk <= '1'; --	 Cycle No: 1127

wait for 1 ns;

I <= "0000";
CCEN_BAR <= '0';
CC_BAR <= '0';
RLD_BAR <= '0';
CI <= '0';
OEbar <= '0';
D <= "111111111111";

wait for 4 ns;

clk <= '0';

wait for 4 ns;

wait for 1 ns;

--------------------------

clk <= '1'; --	 Cycle No: 1128

wait for 1 ns;

I <= "0010";
CCEN_BAR <= '0';
CC_BAR <= '0';
RLD_BAR <= '1';
CI <= '0';
OEbar <= '0';
D <= "111111111111";

wait for 4 ns;

clk <= '0';

wait for 4 ns;

wait for 1 ns;

--------------------------

clk <= '1'; --	 Cycle No: 1129

wait for 1 ns;

I <= "1100";
CCEN_BAR <= '1';
CC_BAR <= '0';
RLD_BAR <= '1';
CI <= '0';
OEbar <= '0';

wait for 4 ns;

clk <= '0';

wait for 4 ns;

assert (Y = "111111111111")
report
"Assert 609 : < Y /= 111111111111 >" -- 	Vector No: 463
severity warning;

wait for 1 ns;

-- **************************************************
-- **************************************************
-- ** I = 13 **
--------------------------

clk <= '1'; --	 Cycle No: 1130

wait for 1 ns;

I <= "0000";
CCEN_BAR <= '0';
CC_BAR <= '0';
RLD_BAR <= '0';
CI <= '0';
OEbar <= '0';
D <= "000000000000";

wait for 4 ns;

clk <= '0';

wait for 4 ns;

wait for 1 ns;

--------------------------

clk <= '1'; --	 Cycle No: 1131

wait for 1 ns;

I <= "0010";
CCEN_BAR <= '0';
CC_BAR <= '0';
RLD_BAR <= '1';
CI <= '0';
OEbar <= '0';
D <= "000000000000";

wait for 4 ns;

clk <= '0';

wait for 4 ns;

wait for 1 ns;

--------------------------

clk <= '1'; --	 Cycle No: 1132

wait for 1 ns;

I <= "1101";
CCEN_BAR <= '0';
CC_BAR <= '0';
RLD_BAR <= '1';
CI <= '0';
OEbar <= '0';

wait for 4 ns;

clk <= '0';

wait for 4 ns;

assert (Y = "000000000000")
report
"Assert 610 : < Y /= 000000000000 >" -- 	Vector No: 464
severity warning;

wait for 1 ns;

-- **************************************************
--------------------------

clk <= '1'; --	 Cycle No: 1133

wait for 1 ns;

I <= "0000";
CCEN_BAR <= '0';
CC_BAR <= '0';
RLD_BAR <= '0';
CI <= '0';
OEbar <= '0';
D <= "000000000000";

wait for 4 ns;

clk <= '0';

wait for 4 ns;

wait for 1 ns;

--------------------------

clk <= '1'; --	 Cycle No: 1134

wait for 1 ns;

I <= "0010";
CCEN_BAR <= '0';
CC_BAR <= '0';
RLD_BAR <= '1';
CI <= '0';
OEbar <= '0';
D <= "111111111111";

wait for 4 ns;

clk <= '0';

wait for 4 ns;

wait for 1 ns;

--------------------------

clk <= '1'; --	 Cycle No: 1135

wait for 1 ns;

I <= "1101";
CCEN_BAR <= '0';
CC_BAR <= '0';
RLD_BAR <= '1';
CI <= '0';
OEbar <= '0';

wait for 4 ns;

clk <= '0';

wait for 4 ns;

assert (Y = "111111111111")
report
"Assert 611 : < Y /= 111111111111 >" -- 	Vector No: 465
severity warning;

wait for 1 ns;

-- **************************************************
--------------------------

clk <= '1'; --	 Cycle No: 1136

wait for 1 ns;

I <= "0000";
CCEN_BAR <= '0';
CC_BAR <= '0';
RLD_BAR <= '0';
CI <= '0';
OEbar <= '0';
D <= "111111111111";

wait for 4 ns;

clk <= '0';

wait for 4 ns;

wait for 1 ns;

--------------------------

clk <= '1'; --	 Cycle No: 1137

wait for 1 ns;

I <= "0010";
CCEN_BAR <= '0';
CC_BAR <= '0';
RLD_BAR <= '1';
CI <= '0';
OEbar <= '0';
D <= "000000000000";

wait for 4 ns;

clk <= '0';

wait for 4 ns;

wait for 1 ns;

--------------------------

clk <= '1'; --	 Cycle No: 1138

wait for 1 ns;

I <= "1101";
CCEN_BAR <= '0';
CC_BAR <= '0';
RLD_BAR <= '1';
CI <= '0';
OEbar <= '0';

wait for 4 ns;

clk <= '0';

wait for 4 ns;

assert (Y = "000000000000")
report
"Assert 612 : < Y /= 000000000000 >" -- 	Vector No: 466
severity warning;

wait for 1 ns;

-- **************************************************
--------------------------

clk <= '1'; --	 Cycle No: 1139

wait for 1 ns;

I <= "0000";
CCEN_BAR <= '0';
CC_BAR <= '0';
RLD_BAR <= '0';
CI <= '0';
OEbar <= '0';
D <= "111111111111";

wait for 4 ns;

clk <= '0';

wait for 4 ns;

wait for 1 ns;

--------------------------

clk <= '1'; --	 Cycle No: 1140

wait for 1 ns;

I <= "0010";
CCEN_BAR <= '0';
CC_BAR <= '0';
RLD_BAR <= '1';
CI <= '0';
OEbar <= '0';
D <= "111111111111";

wait for 4 ns;

clk <= '0';

wait for 4 ns;

wait for 1 ns;

--------------------------

clk <= '1'; --	 Cycle No: 1141

wait for 1 ns;

I <= "1101";
CCEN_BAR <= '0';
CC_BAR <= '0';
RLD_BAR <= '1';
CI <= '0';
OEbar <= '0';

wait for 4 ns;

clk <= '0';

wait for 4 ns;

assert (Y = "111111111111")
report
"Assert 613 : < Y /= 111111111111 >" -- 	Vector No: 467
severity warning;

wait for 1 ns;

-- **************************************************
-- **************************************************
--------------------------

clk <= '1'; --	 Cycle No: 1142

wait for 1 ns;

I <= "0000";
CCEN_BAR <= '0';
CC_BAR <= '0';
RLD_BAR <= '0';
CI <= '0';
OEbar <= '0';
D <= "000000000000";

wait for 4 ns;

clk <= '0';

wait for 4 ns;

wait for 1 ns;

--------------------------

clk <= '1'; --	 Cycle No: 1143

wait for 1 ns;

I <= "0010";
CCEN_BAR <= '0';
CC_BAR <= '0';
RLD_BAR <= '1';
CI <= '0';
OEbar <= '0';
D <= "000000000000";

wait for 4 ns;

clk <= '0';

wait for 4 ns;

wait for 1 ns;

--------------------------

clk <= '1'; --	 Cycle No: 1144

wait for 1 ns;

I <= "1101";
CCEN_BAR <= '1';
CC_BAR <= '1';
RLD_BAR <= '1';
CI <= '0';
OEbar <= '0';

wait for 4 ns;

clk <= '0';

wait for 4 ns;

assert (Y = "000000000000")
report
"Assert 614 : < Y /= 000000000000 >" -- 	Vector No: 468
severity warning;

wait for 1 ns;

-- **************************************************
--------------------------

clk <= '1'; --	 Cycle No: 1145

wait for 1 ns;

I <= "0000";
CCEN_BAR <= '0';
CC_BAR <= '0';
RLD_BAR <= '0';
CI <= '0';
OEbar <= '0';
D <= "000000000000";

wait for 4 ns;

clk <= '0';

wait for 4 ns;

wait for 1 ns;

--------------------------

clk <= '1'; --	 Cycle No: 1146

wait for 1 ns;

I <= "0010";
CCEN_BAR <= '0';
CC_BAR <= '0';
RLD_BAR <= '1';
CI <= '0';
OEbar <= '0';
D <= "111111111111";

wait for 4 ns;

clk <= '0';

wait for 4 ns;

wait for 1 ns;

--------------------------

clk <= '1'; --	 Cycle No: 1147

wait for 1 ns;

I <= "1101";
CCEN_BAR <= '1';
CC_BAR <= '1';
RLD_BAR <= '1';
CI <= '0';
OEbar <= '0';

wait for 4 ns;

clk <= '0';

wait for 4 ns;

assert (Y = "111111111111")
report
"Assert 615 : < Y /= 111111111111 >" -- 	Vector No: 469
severity warning;

wait for 1 ns;

-- **************************************************
--------------------------

clk <= '1'; --	 Cycle No: 1148

wait for 1 ns;

I <= "0000";
CCEN_BAR <= '0';
CC_BAR <= '0';
RLD_BAR <= '0';
CI <= '0';
OEbar <= '0';
D <= "111111111111";

wait for 4 ns;

clk <= '0';

wait for 4 ns;

wait for 1 ns;

--------------------------

clk <= '1'; --	 Cycle No: 1149

wait for 1 ns;

I <= "0010";
CCEN_BAR <= '0';
CC_BAR <= '0';
RLD_BAR <= '1';
CI <= '0';
OEbar <= '0';
D <= "000000000000";

wait for 4 ns;

clk <= '0';

wait for 4 ns;

wait for 1 ns;

--------------------------

clk <= '1'; --	 Cycle No: 1150

wait for 1 ns;

I <= "1101";
CCEN_BAR <= '1';
CC_BAR <= '1';
RLD_BAR <= '1';
CI <= '0';
OEbar <= '0';

wait for 4 ns;

clk <= '0';

wait for 4 ns;

assert (Y = "000000000000")
report
"Assert 616 : < Y /= 000000000000 >" -- 	Vector No: 470
severity warning;

wait for 1 ns;

-- **************************************************
--------------------------

clk <= '1'; --	 Cycle No: 1151

wait for 1 ns;

I <= "0000";
CCEN_BAR <= '0';
CC_BAR <= '0';
RLD_BAR <= '0';
CI <= '0';
OEbar <= '0';
D <= "111111111111";

wait for 4 ns;

clk <= '0';

wait for 4 ns;

wait for 1 ns;

--------------------------

clk <= '1'; --	 Cycle No: 1152

wait for 1 ns;

I <= "0010";
CCEN_BAR <= '0';
CC_BAR <= '0';
RLD_BAR <= '1';
CI <= '0';
OEbar <= '0';
D <= "111111111111";

wait for 4 ns;

clk <= '0';

wait for 4 ns;

wait for 1 ns;

--------------------------

clk <= '1'; --	 Cycle No: 1153

wait for 1 ns;

I <= "1101";
CCEN_BAR <= '1';
CC_BAR <= '1';
RLD_BAR <= '1';
CI <= '0';
OEbar <= '0';

wait for 4 ns;

clk <= '0';

wait for 4 ns;

assert (Y = "111111111111")
report
"Assert 617 : < Y /= 111111111111 >" -- 	Vector No: 471
severity warning;

wait for 1 ns;

-- **************************************************
-- **************************************************
--------------------------

clk <= '1'; --	 Cycle No: 1154

wait for 1 ns;

I <= "0000";
CCEN_BAR <= '0';
CC_BAR <= '0';
RLD_BAR <= '0';
CI <= '0';
OEbar <= '0';
D <= "000000000000";

wait for 4 ns;

clk <= '0';

wait for 4 ns;

wait for 1 ns;

--------------------------

clk <= '1'; --	 Cycle No: 1155

wait for 1 ns;

I <= "0010";
CCEN_BAR <= '0';
CC_BAR <= '0';
RLD_BAR <= '1';
CI <= '0';
OEbar <= '0';
D <= "000000000000";

wait for 4 ns;

clk <= '0';

wait for 4 ns;

wait for 1 ns;

--------------------------

clk <= '1'; --	 Cycle No: 1156

wait for 1 ns;

I <= "1101";
CCEN_BAR <= '1';
CC_BAR <= '0';
RLD_BAR <= '1';
CI <= '0';
OEbar <= '0';

wait for 4 ns;

clk <= '0';

wait for 4 ns;

assert (Y = "000000000000")
report
"Assert 618 : < Y /= 000000000000 >" -- 	Vector No: 472
severity warning;

wait for 1 ns;

-- **************************************************
--------------------------

clk <= '1'; --	 Cycle No: 1157

wait for 1 ns;

I <= "0000";
CCEN_BAR <= '0';
CC_BAR <= '0';
RLD_BAR <= '0';
CI <= '0';
OEbar <= '0';
D <= "000000000000";

wait for 4 ns;

clk <= '0';

wait for 4 ns;

wait for 1 ns;

--------------------------

clk <= '1'; --	 Cycle No: 1158

wait for 1 ns;

I <= "0010";
CCEN_BAR <= '0';
CC_BAR <= '0';
RLD_BAR <= '1';
CI <= '0';
OEbar <= '0';
D <= "111111111111";

wait for 4 ns;

clk <= '0';

wait for 4 ns;

wait for 1 ns;

--------------------------

clk <= '1'; --	 Cycle No: 1159

wait for 1 ns;

I <= "1101";
CCEN_BAR <= '1';
CC_BAR <= '0';
RLD_BAR <= '1';
CI <= '0';
OEbar <= '0';

wait for 4 ns;

clk <= '0';

wait for 4 ns;

assert (Y = "111111111111")
report
"Assert 619 : < Y /= 111111111111 >" -- 	Vector No: 473
severity warning;

wait for 1 ns;

-- **************************************************
--------------------------

clk <= '1'; --	 Cycle No: 1160

wait for 1 ns;

I <= "0000";
CCEN_BAR <= '0';
CC_BAR <= '0';
RLD_BAR <= '0';
CI <= '0';
OEbar <= '0';
D <= "111111111111";

wait for 4 ns;

clk <= '0';

wait for 4 ns;

wait for 1 ns;

--------------------------

clk <= '1'; --	 Cycle No: 1161

wait for 1 ns;

I <= "0010";
CCEN_BAR <= '0';
CC_BAR <= '0';
RLD_BAR <= '1';
CI <= '0';
OEbar <= '0';
D <= "000000000000";

wait for 4 ns;

clk <= '0';

wait for 4 ns;

wait for 1 ns;

--------------------------

clk <= '1'; --	 Cycle No: 1162

wait for 1 ns;

I <= "1101";
CCEN_BAR <= '1';
CC_BAR <= '0';
RLD_BAR <= '1';
CI <= '0';
OEbar <= '0';

wait for 4 ns;

clk <= '0';

wait for 4 ns;

assert (Y = "000000000000")
report
"Assert 620 : < Y /= 000000000000 >" -- 	Vector No: 474
severity warning;

wait for 1 ns;

-- **************************************************
--------------------------

clk <= '1'; --	 Cycle No: 1163

wait for 1 ns;

I <= "0000";
CCEN_BAR <= '0';
CC_BAR <= '0';
RLD_BAR <= '0';
CI <= '0';
OEbar <= '0';
D <= "111111111111";

wait for 4 ns;

clk <= '0';

wait for 4 ns;

wait for 1 ns;

--------------------------

clk <= '1'; --	 Cycle No: 1164

wait for 1 ns;

I <= "0010";
CCEN_BAR <= '0';
CC_BAR <= '0';
RLD_BAR <= '1';
CI <= '0';
OEbar <= '0';
D <= "111111111111";

wait for 4 ns;

clk <= '0';

wait for 4 ns;

wait for 1 ns;

--------------------------

clk <= '1'; --	 Cycle No: 1165

wait for 1 ns;

I <= "1101";
CCEN_BAR <= '1';
CC_BAR <= '0';
RLD_BAR <= '1';
CI <= '0';
OEbar <= '0';

wait for 4 ns;

clk <= '0';

wait for 4 ns;

assert (Y = "111111111111")
report
"Assert 621 : < Y /= 111111111111 >" -- 	Vector No: 475
severity warning;

wait for 1 ns;

-- **************************************************
-- **************************************************
-- ** I = 14 **
--------------------------

clk <= '1'; --	 Cycle No: 1166

wait for 1 ns;

I <= "0000";
CCEN_BAR <= '0';
CC_BAR <= '0';
RLD_BAR <= '0';
CI <= '0';
OEbar <= '0';
D <= "000000000000";

wait for 4 ns;

clk <= '0';

wait for 4 ns;

wait for 1 ns;

--------------------------

clk <= '1'; --	 Cycle No: 1167

wait for 1 ns;

I <= "0010";
CCEN_BAR <= '0';
CC_BAR <= '0';
RLD_BAR <= '1';
CI <= '0';
OEbar <= '0';
D <= "000000000000";

wait for 4 ns;

clk <= '0';

wait for 4 ns;

wait for 1 ns;

--------------------------

clk <= '1'; --	 Cycle No: 1168

wait for 1 ns;

I <= "1110";
CCEN_BAR <= '0';
CC_BAR <= '0';
RLD_BAR <= '1';
CI <= '0';
OEbar <= '0';

wait for 4 ns;

clk <= '0';

wait for 4 ns;

assert (Y = "000000000000")
report
"Assert 622 : < Y /= 000000000000 >" -- 	Vector No: 476
severity warning;

wait for 1 ns;

-- **************************************************
--------------------------

clk <= '1'; --	 Cycle No: 1169

wait for 1 ns;

I <= "0000";
CCEN_BAR <= '0';
CC_BAR <= '0';
RLD_BAR <= '0';
CI <= '0';
OEbar <= '0';
D <= "000000000000";

wait for 4 ns;

clk <= '0';

wait for 4 ns;

wait for 1 ns;

--------------------------

clk <= '1'; --	 Cycle No: 1170

wait for 1 ns;

I <= "0010";
CCEN_BAR <= '0';
CC_BAR <= '0';
RLD_BAR <= '1';
CI <= '0';
OEbar <= '0';
D <= "111111111111";

wait for 4 ns;

clk <= '0';

wait for 4 ns;

wait for 1 ns;

--------------------------

clk <= '1'; --	 Cycle No: 1171

wait for 1 ns;

I <= "1110";
CCEN_BAR <= '0';
CC_BAR <= '0';
RLD_BAR <= '1';
CI <= '0';
OEbar <= '0';

wait for 4 ns;

clk <= '0';

wait for 4 ns;

assert (Y = "111111111111")
report
"Assert 623 : < Y /= 111111111111 >" -- 	Vector No: 477
severity warning;

wait for 1 ns;

-- **************************************************
--------------------------

clk <= '1'; --	 Cycle No: 1172

wait for 1 ns;

I <= "0000";
CCEN_BAR <= '0';
CC_BAR <= '0';
RLD_BAR <= '0';
CI <= '0';
OEbar <= '0';
D <= "111111111111";

wait for 4 ns;

clk <= '0';

wait for 4 ns;

wait for 1 ns;

--------------------------

clk <= '1'; --	 Cycle No: 1173

wait for 1 ns;

I <= "0010";
CCEN_BAR <= '0';
CC_BAR <= '0';
RLD_BAR <= '1';
CI <= '0';
OEbar <= '0';
D <= "000000000000";

wait for 4 ns;

clk <= '0';

wait for 4 ns;

wait for 1 ns;

--------------------------

clk <= '1'; --	 Cycle No: 1174

wait for 1 ns;

I <= "1110";
CCEN_BAR <= '0';
CC_BAR <= '0';
RLD_BAR <= '1';
CI <= '0';
OEbar <= '0';

wait for 4 ns;

clk <= '0';

wait for 4 ns;

assert (Y = "000000000000")
report
"Assert 624 : < Y /= 000000000000 >" -- 	Vector No: 478
severity warning;

wait for 1 ns;

-- **************************************************
--------------------------

clk <= '1'; --	 Cycle No: 1175

wait for 1 ns;

I <= "0000";
CCEN_BAR <= '0';
CC_BAR <= '0';
RLD_BAR <= '0';
CI <= '0';
OEbar <= '0';
D <= "111111111111";

wait for 4 ns;

clk <= '0';

wait for 4 ns;

wait for 1 ns;

--------------------------

clk <= '1'; --	 Cycle No: 1176

wait for 1 ns;

I <= "0010";
CCEN_BAR <= '0';
CC_BAR <= '0';
RLD_BAR <= '1';
CI <= '0';
OEbar <= '0';
D <= "111111111111";

wait for 4 ns;

clk <= '0';

wait for 4 ns;

wait for 1 ns;

--------------------------

clk <= '1'; --	 Cycle No: 1177

wait for 1 ns;

I <= "1110";
CCEN_BAR <= '0';
CC_BAR <= '0';
RLD_BAR <= '1';
CI <= '0';
OEbar <= '0';

wait for 4 ns;

clk <= '0';

wait for 4 ns;

assert (Y = "111111111111")
report
"Assert 625 : < Y /= 111111111111 >" -- 	Vector No: 479
severity warning;

wait for 1 ns;

-- **************************************************
-- **************************************************
--------------------------

clk <= '1'; --	 Cycle No: 1178

wait for 1 ns;

I <= "0000";
CCEN_BAR <= '0';
CC_BAR <= '0';
RLD_BAR <= '0';
CI <= '0';
OEbar <= '0';
D <= "000000000000";

wait for 4 ns;

clk <= '0';

wait for 4 ns;

wait for 1 ns;

--------------------------

clk <= '1'; --	 Cycle No: 1179

wait for 1 ns;

I <= "0010";
CCEN_BAR <= '0';
CC_BAR <= '0';
RLD_BAR <= '1';
CI <= '0';
OEbar <= '0';
D <= "000000000000";

wait for 4 ns;

clk <= '0';

wait for 4 ns;

wait for 1 ns;

--------------------------

clk <= '1'; --	 Cycle No: 1180

wait for 1 ns;

I <= "1110";
CCEN_BAR <= '0';
CC_BAR <= '1';
RLD_BAR <= '1';
CI <= '0';
OEbar <= '0';

wait for 4 ns;

clk <= '0';

wait for 4 ns;

assert (Y = "000000000000")
report
"Assert 626 : < Y /= 000000000000 >" -- 	Vector No: 480
severity warning;

wait for 1 ns;

-- **************************************************
--------------------------

clk <= '1'; --	 Cycle No: 1181

wait for 1 ns;

I <= "0000";
CCEN_BAR <= '0';
CC_BAR <= '0';
RLD_BAR <= '0';
CI <= '0';
OEbar <= '0';
D <= "000000000000";

wait for 4 ns;

clk <= '0';

wait for 4 ns;

wait for 1 ns;

--------------------------

clk <= '1'; --	 Cycle No: 1182

wait for 1 ns;

I <= "0010";
CCEN_BAR <= '0';
CC_BAR <= '0';
RLD_BAR <= '1';
CI <= '0';
OEbar <= '0';
D <= "111111111111";

wait for 4 ns;

clk <= '0';

wait for 4 ns;

wait for 1 ns;

--------------------------

clk <= '1'; --	 Cycle No: 1183

wait for 1 ns;

I <= "1110";
CCEN_BAR <= '0';
CC_BAR <= '1';
RLD_BAR <= '1';
CI <= '0';
OEbar <= '0';

wait for 4 ns;

clk <= '0';

wait for 4 ns;

assert (Y = "111111111111")
report
"Assert 627 : < Y /= 111111111111 >" -- 	Vector No: 481
severity warning;

wait for 1 ns;

-- **************************************************
--------------------------

clk <= '1'; --	 Cycle No: 1184

wait for 1 ns;

I <= "0000";
CCEN_BAR <= '0';
CC_BAR <= '0';
RLD_BAR <= '0';
CI <= '0';
OEbar <= '0';
D <= "111111111111";

wait for 4 ns;

clk <= '0';

wait for 4 ns;

wait for 1 ns;

--------------------------

clk <= '1'; --	 Cycle No: 1185

wait for 1 ns;

I <= "0010";
CCEN_BAR <= '0';
CC_BAR <= '0';
RLD_BAR <= '1';
CI <= '0';
OEbar <= '0';
D <= "000000000000";

wait for 4 ns;

clk <= '0';

wait for 4 ns;

wait for 1 ns;

--------------------------

clk <= '1'; --	 Cycle No: 1186

wait for 1 ns;

I <= "1110";
CCEN_BAR <= '0';
CC_BAR <= '1';
RLD_BAR <= '1';
CI <= '0';
OEbar <= '0';

wait for 4 ns;

clk <= '0';

wait for 4 ns;

assert (Y = "000000000000")
report
"Assert 628 : < Y /= 000000000000 >" -- 	Vector No: 482
severity warning;

wait for 1 ns;

-- **************************************************
--------------------------

clk <= '1'; --	 Cycle No: 1187

wait for 1 ns;

I <= "0000";
CCEN_BAR <= '0';
CC_BAR <= '0';
RLD_BAR <= '0';
CI <= '0';
OEbar <= '0';
D <= "111111111111";

wait for 4 ns;

clk <= '0';

wait for 4 ns;

wait for 1 ns;

--------------------------

clk <= '1'; --	 Cycle No: 1188

wait for 1 ns;

I <= "0010";
CCEN_BAR <= '0';
CC_BAR <= '0';
RLD_BAR <= '1';
CI <= '0';
OEbar <= '0';
D <= "111111111111";

wait for 4 ns;

clk <= '0';

wait for 4 ns;

wait for 1 ns;

--------------------------

clk <= '1'; --	 Cycle No: 1189

wait for 1 ns;

I <= "1110";
CCEN_BAR <= '0';
CC_BAR <= '1';
RLD_BAR <= '1';
CI <= '0';
OEbar <= '0';

wait for 4 ns;

clk <= '0';

wait for 4 ns;

assert (Y = "111111111111")
report
"Assert 629 : < Y /= 111111111111 >" -- 	Vector No: 483
severity warning;

wait for 1 ns;

-- **************************************************
-- **************************************************
--------------------------

clk <= '1'; --	 Cycle No: 1190

wait for 1 ns;

I <= "0000";
CCEN_BAR <= '0';
CC_BAR <= '0';
RLD_BAR <= '0';
CI <= '0';
OEbar <= '0';
D <= "000000000000";

wait for 4 ns;

clk <= '0';

wait for 4 ns;

wait for 1 ns;

--------------------------

clk <= '1'; --	 Cycle No: 1191

wait for 1 ns;

I <= "0010";
CCEN_BAR <= '0';
CC_BAR <= '0';
RLD_BAR <= '1';
CI <= '0';
OEbar <= '0';
D <= "000000000000";

wait for 4 ns;

clk <= '0';

wait for 4 ns;

wait for 1 ns;

--------------------------

clk <= '1'; --	 Cycle No: 1192

wait for 1 ns;

I <= "1110";
CCEN_BAR <= '1';
CC_BAR <= '1';
RLD_BAR <= '1';
CI <= '0';
OEbar <= '0';

wait for 4 ns;

clk <= '0';

wait for 4 ns;

assert (Y = "000000000000")
report
"Assert 630 : < Y /= 000000000000 >" -- 	Vector No: 484
severity warning;

wait for 1 ns;

-- **************************************************
--------------------------

clk <= '1'; --	 Cycle No: 1193

wait for 1 ns;

I <= "0000";
CCEN_BAR <= '0';
CC_BAR <= '0';
RLD_BAR <= '0';
CI <= '0';
OEbar <= '0';
D <= "000000000000";

wait for 4 ns;

clk <= '0';

wait for 4 ns;

wait for 1 ns;

--------------------------

clk <= '1'; --	 Cycle No: 1194

wait for 1 ns;

I <= "0010";
CCEN_BAR <= '0';
CC_BAR <= '0';
RLD_BAR <= '1';
CI <= '0';
OEbar <= '0';
D <= "111111111111";

wait for 4 ns;

clk <= '0';

wait for 4 ns;

wait for 1 ns;

--------------------------

clk <= '1'; --	 Cycle No: 1195

wait for 1 ns;

I <= "1110";
CCEN_BAR <= '1';
CC_BAR <= '1';
RLD_BAR <= '1';
CI <= '0';
OEbar <= '0';

wait for 4 ns;

clk <= '0';

wait for 4 ns;

assert (Y = "111111111111")
report
"Assert 631 : < Y /= 111111111111 >" -- 	Vector No: 485
severity warning;

wait for 1 ns;

-- **************************************************
--------------------------

clk <= '1'; --	 Cycle No: 1196

wait for 1 ns;

I <= "0000";
CCEN_BAR <= '0';
CC_BAR <= '0';
RLD_BAR <= '0';
CI <= '0';
OEbar <= '0';
D <= "111111111111";

wait for 4 ns;

clk <= '0';

wait for 4 ns;

wait for 1 ns;

--------------------------

clk <= '1'; --	 Cycle No: 1197

wait for 1 ns;

I <= "0010";
CCEN_BAR <= '0';
CC_BAR <= '0';
RLD_BAR <= '1';
CI <= '0';
OEbar <= '0';
D <= "000000000000";

wait for 4 ns;

clk <= '0';

wait for 4 ns;

wait for 1 ns;

--------------------------

clk <= '1'; --	 Cycle No: 1198

wait for 1 ns;

I <= "1110";
CCEN_BAR <= '1';
CC_BAR <= '1';
RLD_BAR <= '1';
CI <= '0';
OEbar <= '0';

wait for 4 ns;

clk <= '0';

wait for 4 ns;

assert (Y = "000000000000")
report
"Assert 632 : < Y /= 000000000000 >" -- 	Vector No: 486
severity warning;

wait for 1 ns;

-- **************************************************
--------------------------

clk <= '1'; --	 Cycle No: 1199

wait for 1 ns;

I <= "0000";
CCEN_BAR <= '0';
CC_BAR <= '0';
RLD_BAR <= '0';
CI <= '0';
OEbar <= '0';
D <= "111111111111";

wait for 4 ns;

clk <= '0';

wait for 4 ns;

wait for 1 ns;

--------------------------

clk <= '1'; --	 Cycle No: 1200

wait for 1 ns;

I <= "0010";
CCEN_BAR <= '0';
CC_BAR <= '0';
RLD_BAR <= '1';
CI <= '0';
OEbar <= '0';
D <= "111111111111";

wait for 4 ns;

clk <= '0';

wait for 4 ns;

wait for 1 ns;

--------------------------

clk <= '1'; --	 Cycle No: 1201

wait for 1 ns;

I <= "1110";
CCEN_BAR <= '1';
CC_BAR <= '1';
RLD_BAR <= '1';
CI <= '0';
OEbar <= '0';

wait for 4 ns;

clk <= '0';

wait for 4 ns;

assert (Y = "111111111111")
report
"Assert 633 : < Y /= 111111111111 >" -- 	Vector No: 487
severity warning;

wait for 1 ns;

-- **************************************************
-- **************************************************
--------------------------

clk <= '1'; --	 Cycle No: 1202

wait for 1 ns;

I <= "0000";
CCEN_BAR <= '0';
CC_BAR <= '0';
RLD_BAR <= '0';
CI <= '0';
OEbar <= '0';
D <= "000000000000";

wait for 4 ns;

clk <= '0';

wait for 4 ns;

wait for 1 ns;

--------------------------

clk <= '1'; --	 Cycle No: 1203

wait for 1 ns;

I <= "0010";
CCEN_BAR <= '0';
CC_BAR <= '0';
RLD_BAR <= '1';
CI <= '0';
OEbar <= '0';
D <= "000000000000";

wait for 4 ns;

clk <= '0';

wait for 4 ns;

wait for 1 ns;

--------------------------

clk <= '1'; --	 Cycle No: 1204

wait for 1 ns;

I <= "1110";
CCEN_BAR <= '1';
CC_BAR <= '0';
RLD_BAR <= '1';
CI <= '0';
OEbar <= '0';

wait for 4 ns;

clk <= '0';

wait for 4 ns;

assert (Y = "000000000000")
report
"Assert 634 : < Y /= 000000000000 >" -- 	Vector No: 488
severity warning;

wait for 1 ns;

-- **************************************************
--------------------------

clk <= '1'; --	 Cycle No: 1205

wait for 1 ns;

I <= "0000";
CCEN_BAR <= '0';
CC_BAR <= '0';
RLD_BAR <= '0';
CI <= '0';
OEbar <= '0';
D <= "000000000000";

wait for 4 ns;

clk <= '0';

wait for 4 ns;

wait for 1 ns;

--------------------------

clk <= '1'; --	 Cycle No: 1206

wait for 1 ns;

I <= "0010";
CCEN_BAR <= '0';
CC_BAR <= '0';
RLD_BAR <= '1';
CI <= '0';
OEbar <= '0';
D <= "111111111111";

wait for 4 ns;

clk <= '0';

wait for 4 ns;

wait for 1 ns;

--------------------------

clk <= '1'; --	 Cycle No: 1207

wait for 1 ns;

I <= "1110";
CCEN_BAR <= '1';
CC_BAR <= '0';
RLD_BAR <= '1';
CI <= '0';
OEbar <= '0';

wait for 4 ns;

clk <= '0';

wait for 4 ns;

assert (Y = "111111111111")
report
"Assert 635 : < Y /= 111111111111 >" -- 	Vector No: 489
severity warning;

wait for 1 ns;

-- **************************************************
--------------------------

clk <= '1'; --	 Cycle No: 1208

wait for 1 ns;

I <= "0000";
CCEN_BAR <= '0';
CC_BAR <= '0';
RLD_BAR <= '0';
CI <= '0';
OEbar <= '0';
D <= "111111111111";

wait for 4 ns;

clk <= '0';

wait for 4 ns;

wait for 1 ns;

--------------------------

clk <= '1'; --	 Cycle No: 1209

wait for 1 ns;

I <= "0010";
CCEN_BAR <= '0';
CC_BAR <= '0';
RLD_BAR <= '1';
CI <= '0';
OEbar <= '0';
D <= "000000000000";

wait for 4 ns;

clk <= '0';

wait for 4 ns;

wait for 1 ns;

--------------------------

clk <= '1'; --	 Cycle No: 1210

wait for 1 ns;

I <= "1110";
CCEN_BAR <= '1';
CC_BAR <= '0';
RLD_BAR <= '1';
CI <= '0';
OEbar <= '0';

wait for 4 ns;

clk <= '0';

wait for 4 ns;

assert (Y = "000000000000")
report
"Assert 636 : < Y /= 000000000000 >" -- 	Vector No: 490
severity warning;

wait for 1 ns;

-- **************************************************
--------------------------

clk <= '1'; --	 Cycle No: 1211

wait for 1 ns;

I <= "0000";
CCEN_BAR <= '0';
CC_BAR <= '0';
RLD_BAR <= '0';
CI <= '0';
OEbar <= '0';
D <= "111111111111";

wait for 4 ns;

clk <= '0';

wait for 4 ns;

wait for 1 ns;

--------------------------

clk <= '1'; --	 Cycle No: 1212

wait for 1 ns;

I <= "0010";
CCEN_BAR <= '0';
CC_BAR <= '0';
RLD_BAR <= '1';
CI <= '0';
OEbar <= '0';
D <= "111111111111";

wait for 4 ns;

clk <= '0';

wait for 4 ns;

wait for 1 ns;

--------------------------

clk <= '1'; --	 Cycle No: 1213

wait for 1 ns;

I <= "1110";
CCEN_BAR <= '1';
CC_BAR <= '0';
RLD_BAR <= '1';
CI <= '0';
OEbar <= '0';

wait for 4 ns;

clk <= '0';

wait for 4 ns;

assert (Y = "111111111111")
report
"Assert 637 : < Y /= 111111111111 >" -- 	Vector No: 491
severity warning;

wait for 1 ns;

-- **************************************************
-- **************************************************
-- ** I = 15 **
--------------------------

clk <= '1'; --	 Cycle No: 1214

wait for 1 ns;

I <= "0000";
CCEN_BAR <= '0';
CC_BAR <= '0';
RLD_BAR <= '0';
CI <= '0';
OEbar <= '0';
D <= "000000000000";

wait for 4 ns;

clk <= '0';

wait for 4 ns;

wait for 1 ns;

--------------------------

clk <= '1'; --	 Cycle No: 1215

wait for 1 ns;

I <= "0010";
CCEN_BAR <= '0';
CC_BAR <= '0';
RLD_BAR <= '1';
CI <= '0';
OEbar <= '0';
D <= "000000000000";

wait for 4 ns;

clk <= '0';

wait for 4 ns;

wait for 1 ns;

--------------------------

clk <= '1'; --	 Cycle No: 1216

wait for 1 ns;

I <= "1111";
CCEN_BAR <= '0';
CC_BAR <= '0';
RLD_BAR <= '1';
CI <= '0';
OEbar <= '0';

wait for 4 ns;

clk <= '0';

wait for 4 ns;

assert (Y = "000000000000")
report
"Assert 638 : < Y /= 000000000000 >" -- 	Vector No: 492
severity warning;

wait for 1 ns;

-- **************************************************
--------------------------

clk <= '1'; --	 Cycle No: 1217

wait for 1 ns;

I <= "0000";
CCEN_BAR <= '0';
CC_BAR <= '0';
RLD_BAR <= '0';
CI <= '0';
OEbar <= '0';
D <= "000000000000";

wait for 4 ns;

clk <= '0';

wait for 4 ns;

wait for 1 ns;

--------------------------

clk <= '1'; --	 Cycle No: 1218

wait for 1 ns;

I <= "0010";
CCEN_BAR <= '0';
CC_BAR <= '0';
RLD_BAR <= '1';
CI <= '0';
OEbar <= '0';
D <= "111111111111";

wait for 4 ns;

clk <= '0';

wait for 4 ns;

wait for 1 ns;

--------------------------

clk <= '1'; --	 Cycle No: 1219

wait for 1 ns;

I <= "1111";
CCEN_BAR <= '0';
CC_BAR <= '0';
RLD_BAR <= '1';
CI <= '0';
OEbar <= '0';

wait for 4 ns;

clk <= '0';

wait for 4 ns;

assert (Y = "111111111111")
report
"Assert 639 : < Y /= 111111111111 >" -- 	Vector No: 493
severity warning;

wait for 1 ns;

-- **************************************************
--------------------------

clk <= '1'; --	 Cycle No: 1220

wait for 1 ns;

I <= "0000";
CCEN_BAR <= '0';
CC_BAR <= '0';
RLD_BAR <= '0';
CI <= '0';
OEbar <= '0';
D <= "111111111111";

wait for 4 ns;

clk <= '0';

wait for 4 ns;

wait for 1 ns;

--------------------------

clk <= '1'; --	 Cycle No: 1221

wait for 1 ns;

I <= "0010";
CCEN_BAR <= '0';
CC_BAR <= '0';
RLD_BAR <= '1';
CI <= '0';
OEbar <= '0';
D <= "000000000000";

wait for 4 ns;

clk <= '0';

wait for 4 ns;

wait for 1 ns;

--------------------------

clk <= '1'; --	 Cycle No: 1222

wait for 1 ns;

I <= "1111";
CCEN_BAR <= '0';
CC_BAR <= '0';
RLD_BAR <= '1';
CI <= '0';
OEbar <= '0';

wait for 4 ns;

clk <= '0';

wait for 4 ns;

assert (Y = "000000000000")
report
"Assert 640 : < Y /= 000000000000 >" -- 	Vector No: 494
severity warning;

wait for 1 ns;

-- **************************************************
--------------------------

clk <= '1'; --	 Cycle No: 1223

wait for 1 ns;

I <= "0000";
CCEN_BAR <= '0';
CC_BAR <= '0';
RLD_BAR <= '0';
CI <= '0';
OEbar <= '0';
D <= "111111111111";

wait for 4 ns;

clk <= '0';

wait for 4 ns;

wait for 1 ns;

--------------------------

clk <= '1'; --	 Cycle No: 1224

wait for 1 ns;

I <= "0010";
CCEN_BAR <= '0';
CC_BAR <= '0';
RLD_BAR <= '1';
CI <= '0';
OEbar <= '0';
D <= "111111111111";

wait for 4 ns;

clk <= '0';

wait for 4 ns;

wait for 1 ns;

--------------------------

clk <= '1'; --	 Cycle No: 1225

wait for 1 ns;

I <= "1111";
CCEN_BAR <= '0';
CC_BAR <= '0';
RLD_BAR <= '1';
CI <= '0';
OEbar <= '0';

wait for 4 ns;

clk <= '0';

wait for 4 ns;

assert (Y = "111111111111")
report
"Assert 641 : < Y /= 111111111111 >" -- 	Vector No: 495
severity warning;

wait for 1 ns;

-- **************************************************
-- **************************************************
--------------------------

clk <= '1'; --	 Cycle No: 1226

wait for 1 ns;

I <= "0000";
CCEN_BAR <= '0';
CC_BAR <= '0';
RLD_BAR <= '0';
CI <= '0';
OEbar <= '0';
D <= "000000000000";

wait for 4 ns;

clk <= '0';

wait for 4 ns;

wait for 1 ns;

--------------------------

clk <= '1'; --	 Cycle No: 1227

wait for 1 ns;

I <= "0010";
CCEN_BAR <= '0';
CC_BAR <= '0';
RLD_BAR <= '1';
CI <= '0';
OEbar <= '0';
D <= "000000000000";

wait for 4 ns;

clk <= '0';

wait for 4 ns;

wait for 1 ns;

--------------------------

clk <= '1'; --	 Cycle No: 1228

wait for 1 ns;

I <= "1111";
CCEN_BAR <= '1';
CC_BAR <= '1';
RLD_BAR <= '1';
CI <= '0';
OEbar <= '0';

wait for 4 ns;

clk <= '0';

wait for 4 ns;

assert (Y = "000000000000")
report
"Assert 642 : < Y /= 000000000000 >" -- 	Vector No: 496
severity warning;

wait for 1 ns;

-- **************************************************
--------------------------

clk <= '1'; --	 Cycle No: 1229

wait for 1 ns;

I <= "0000";
CCEN_BAR <= '0';
CC_BAR <= '0';
RLD_BAR <= '0';
CI <= '0';
OEbar <= '0';
D <= "000000000000";

wait for 4 ns;

clk <= '0';

wait for 4 ns;

wait for 1 ns;

--------------------------

clk <= '1'; --	 Cycle No: 1230

wait for 1 ns;

I <= "0010";
CCEN_BAR <= '0';
CC_BAR <= '0';
RLD_BAR <= '1';
CI <= '0';
OEbar <= '0';
D <= "111111111111";

wait for 4 ns;

clk <= '0';

wait for 4 ns;

wait for 1 ns;

--------------------------

clk <= '1'; --	 Cycle No: 1231

wait for 1 ns;

I <= "1111";
CCEN_BAR <= '1';
CC_BAR <= '1';
RLD_BAR <= '1';
CI <= '0';
OEbar <= '0';

wait for 4 ns;

clk <= '0';

wait for 4 ns;

assert (Y = "111111111111")
report
"Assert 643 : < Y /= 111111111111 >" -- 	Vector No: 497
severity warning;

wait for 1 ns;

-- **************************************************
--------------------------

clk <= '1'; --	 Cycle No: 1232

wait for 1 ns;

I <= "0000";
CCEN_BAR <= '0';
CC_BAR <= '0';
RLD_BAR <= '0';
CI <= '0';
OEbar <= '0';
D <= "111111111111";

wait for 4 ns;

clk <= '0';

wait for 4 ns;

wait for 1 ns;

--------------------------

clk <= '1'; --	 Cycle No: 1233

wait for 1 ns;

I <= "0010";
CCEN_BAR <= '0';
CC_BAR <= '0';
RLD_BAR <= '1';
CI <= '0';
OEbar <= '0';
D <= "000000000000";

wait for 4 ns;

clk <= '0';

wait for 4 ns;

wait for 1 ns;

--------------------------

clk <= '1'; --	 Cycle No: 1234

wait for 1 ns;

I <= "1111";
CCEN_BAR <= '1';
CC_BAR <= '1';
RLD_BAR <= '1';
CI <= '0';
OEbar <= '0';

wait for 4 ns;

clk <= '0';

wait for 4 ns;

assert (Y = "000000000000")
report
"Assert 644 : < Y /= 000000000000 >" -- 	Vector No: 498
severity warning;

wait for 1 ns;

-- **************************************************
--------------------------

clk <= '1'; --	 Cycle No: 1235

wait for 1 ns;

I <= "0000";
CCEN_BAR <= '0';
CC_BAR <= '0';
RLD_BAR <= '0';
CI <= '0';
OEbar <= '0';
D <= "111111111111";

wait for 4 ns;

clk <= '0';

wait for 4 ns;

wait for 1 ns;

--------------------------

clk <= '1'; --	 Cycle No: 1236

wait for 1 ns;

I <= "0010";
CCEN_BAR <= '0';
CC_BAR <= '0';
RLD_BAR <= '1';
CI <= '0';
OEbar <= '0';
D <= "111111111111";

wait for 4 ns;

clk <= '0';

wait for 4 ns;

wait for 1 ns;

--------------------------

clk <= '1'; --	 Cycle No: 1237

wait for 1 ns;

I <= "1111";
CCEN_BAR <= '1';
CC_BAR <= '1';
RLD_BAR <= '1';
CI <= '0';
OEbar <= '0';

wait for 4 ns;

clk <= '0';

wait for 4 ns;

assert (Y = "111111111111")
report
"Assert 645 : < Y /= 111111111111 >" -- 	Vector No: 499
severity warning;

wait for 1 ns;

-- **************************************************
-- **************************************************
--------------------------

clk <= '1'; --	 Cycle No: 1238

wait for 1 ns;

I <= "0000";
CCEN_BAR <= '0';
CC_BAR <= '0';
RLD_BAR <= '0';
CI <= '0';
OEbar <= '0';
D <= "000000000000";

wait for 4 ns;

clk <= '0';

wait for 4 ns;

wait for 1 ns;

--------------------------

clk <= '1'; --	 Cycle No: 1239

wait for 1 ns;

I <= "0010";
CCEN_BAR <= '0';
CC_BAR <= '0';
RLD_BAR <= '1';
CI <= '0';
OEbar <= '0';
D <= "000000000000";

wait for 4 ns;

clk <= '0';

wait for 4 ns;

wait for 1 ns;

--------------------------

clk <= '1'; --	 Cycle No: 1240

wait for 1 ns;

I <= "1111";
CCEN_BAR <= '1';
CC_BAR <= '0';
RLD_BAR <= '1';
CI <= '0';
OEbar <= '0';

wait for 4 ns;

clk <= '0';

wait for 4 ns;

assert (Y = "000000000000")
report
"Assert 646 : < Y /= 000000000000 >" -- 	Vector No: 500
severity warning;

wait for 1 ns;

-- **************************************************
--------------------------

clk <= '1'; --	 Cycle No: 1241

wait for 1 ns;

I <= "0000";
CCEN_BAR <= '0';
CC_BAR <= '0';
RLD_BAR <= '0';
CI <= '0';
OEbar <= '0';
D <= "000000000000";

wait for 4 ns;

clk <= '0';

wait for 4 ns;

wait for 1 ns;

--------------------------

clk <= '1'; --	 Cycle No: 1242

wait for 1 ns;

I <= "0010";
CCEN_BAR <= '0';
CC_BAR <= '0';
RLD_BAR <= '1';
CI <= '0';
OEbar <= '0';
D <= "111111111111";

wait for 4 ns;

clk <= '0';

wait for 4 ns;

wait for 1 ns;

--------------------------

clk <= '1'; --	 Cycle No: 1243

wait for 1 ns;

I <= "1111";
CCEN_BAR <= '1';
CC_BAR <= '0';
RLD_BAR <= '1';
CI <= '0';
OEbar <= '0';

wait for 4 ns;

clk <= '0';

wait for 4 ns;

assert (Y = "111111111111")
report
"Assert 647 : < Y /= 111111111111 >" -- 	Vector No: 501
severity warning;

wait for 1 ns;

-- **************************************************
--------------------------

clk <= '1'; --	 Cycle No: 1244

wait for 1 ns;

I <= "0000";
CCEN_BAR <= '0';
CC_BAR <= '0';
RLD_BAR <= '0';
CI <= '0';
OEbar <= '0';
D <= "111111111111";

wait for 4 ns;

clk <= '0';

wait for 4 ns;

wait for 1 ns;

--------------------------

clk <= '1'; --	 Cycle No: 1245

wait for 1 ns;

I <= "0010";
CCEN_BAR <= '0';
CC_BAR <= '0';
RLD_BAR <= '1';
CI <= '0';
OEbar <= '0';
D <= "000000000000";

wait for 4 ns;

clk <= '0';

wait for 4 ns;

wait for 1 ns;

--------------------------

clk <= '1'; --	 Cycle No: 1246

wait for 1 ns;

I <= "1111";
CCEN_BAR <= '1';
CC_BAR <= '0';
RLD_BAR <= '1';
CI <= '0';
OEbar <= '0';

wait for 4 ns;

clk <= '0';

wait for 4 ns;

assert (Y = "000000000000")
report
"Assert 648 : < Y /= 000000000000 >" -- 	Vector No: 502
severity warning;

wait for 1 ns;

-- **************************************************
--------------------------

clk <= '1'; --	 Cycle No: 1247

wait for 1 ns;

I <= "0000";
CCEN_BAR <= '0';
CC_BAR <= '0';
RLD_BAR <= '0';
CI <= '0';
OEbar <= '0';
D <= "111111111111";

wait for 4 ns;

clk <= '0';

wait for 4 ns;

wait for 1 ns;

--------------------------

clk <= '1'; --	 Cycle No: 1248

wait for 1 ns;

I <= "0010";
CCEN_BAR <= '0';
CC_BAR <= '0';
RLD_BAR <= '1';
CI <= '0';
OEbar <= '0';
D <= "111111111111";

wait for 4 ns;

clk <= '0';

wait for 4 ns;

wait for 1 ns;

--------------------------

clk <= '1'; --	 Cycle No: 1249

wait for 1 ns;

I <= "1111";
CCEN_BAR <= '1';
CC_BAR <= '0';
RLD_BAR <= '1';
CI <= '0';
OEbar <= '0';

wait for 4 ns;

clk <= '0';

wait for 4 ns;

assert (Y = "111111111111")
report
"Assert 649 : < Y /= 111111111111 >" -- 	Vector No: 503
severity warning;

wait for 1 ns;

-- **************************************************
-- **************************************************
-- *************************************
-- *	                            *
-- * TEST VECTORS FOR TRI-STATE-DRIVER *
-- *              	                    *
-- *************************************
--------------------------

clk <= '1'; --	 Cycle No: 1250

wait for 1 ns;

I <= "0001";
CCEN_BAR <= '0';
CC_BAR <= '0';
RLD_BAR <= '1';
CI <= '0';
OEbar <= '0';
D <= "000000000000";

wait for 4 ns;

clk <= '0';

wait for 4 ns;

assert (Y = "000000000000")
report
"Assert 650 : < Y /= 000000000000 >" -- 	Vector No: 504
severity warning;

wait for 1 ns;

-- **************************************************
--------------------------

clk <= '1'; --	 Cycle No: 1251

wait for 1 ns;

I <= "0001";
CCEN_BAR <= '0';
CC_BAR <= '0';
RLD_BAR <= '1';
CI <= '0';
OEbar <= '0';
D <= "111111111111";

wait for 4 ns;

clk <= '0';

wait for 4 ns;

assert (Y = "111111111111")
report
"Assert 651 : < Y /= 111111111111 >" -- 	Vector No: 505
severity warning;

wait for 1 ns;

-- **************************************************
--------------------------

clk <= '1'; --	 Cycle No: 1252

wait for 1 ns;

I <= "0001";
CCEN_BAR <= '0';
CC_BAR <= '0';
RLD_BAR <= '1';
CI <= '0';
OEbar <= '1';
D <= "000000000000";

wait for 4 ns;

clk <= '0';

wait for 4 ns;

assert (Y = "ZZZZZZZZZZZZ")
report
"Assert 652 : < Y /= ZZZZZZZZZZZZ >" -- 	Vector No: 506
severity warning;

wait for 1 ns;

-- **************************************************
--------------------------

clk <= '1'; --	 Cycle No: 1253

wait for 1 ns;

I <= "0001";
CCEN_BAR <= '0';
CC_BAR <= '0';
RLD_BAR <= '1';
CI <= '0';
OEbar <= '1';
D <= "111111111111";

wait for 4 ns;

clk <= '0';

wait for 4 ns;

assert (Y = "ZZZZZZZZZZZZ")
report
"Assert 653 : < Y /= ZZZZZZZZZZZZ >" -- 	Vector No: 507
severity warning;

wait for 1 ns;

-- **************************************************
-- *************************************
-- *	                            *
-- * TEST VECTORS FOR ENABLE SIGNALS   *
-- *              	                    *
-- *************************************
-- ****** I = 0 *******
-- *** R = 0 ***
--------------------------

clk <= '1'; --	 Cycle No: 1254

wait for 1 ns;

I <= "0000";
CCEN_BAR <= '0';
CC_BAR <= '0';
RLD_BAR <= '0';
CI <= '0';
OEbar <= '0';
D <= "000000000000";

wait for 4 ns;

clk <= '0';

wait for 4 ns;

wait for 1 ns;

--------------------------

clk <= '1'; --	 Cycle No: 1255

wait for 1 ns;

I <= "0000";
CCEN_BAR <= '0';
CC_BAR <= '0';
RLD_BAR <= '1';
CI <= '0';
OEbar <= '0';

wait for 4 ns;

clk <= '0';

wait for 4 ns;

assert (PL_BAR = '0')
report
"Assert 654 : < PL_BAR /= 0 >" -- 	Vector No: 508
severity warning;

assert (VECT_BAR = '1')
report
"Assert 655 : < VECT_BAR /= 1 >"
severity warning;

assert (MAP_BAR = '1')
report
"Assert 656 : < MAP_BAR /= 1 >"
severity warning;

wait for 1 ns;

-- **************************************************
--------------------------

clk <= '1'; --	 Cycle No: 1256

wait for 1 ns;

I <= "0000";
CCEN_BAR <= '0';
CC_BAR <= '0';
RLD_BAR <= '0';
CI <= '0';
OEbar <= '0';
D <= "000000000000";

wait for 4 ns;

clk <= '0';

wait for 4 ns;

wait for 1 ns;

--------------------------

clk <= '1'; --	 Cycle No: 1257

wait for 1 ns;

I <= "0000";
CCEN_BAR <= '0';
CC_BAR <= '1';
RLD_BAR <= '1';
CI <= '0';
OEbar <= '0';

wait for 4 ns;

clk <= '0';

wait for 4 ns;

assert (PL_BAR = '0')
report
"Assert 657 : < PL_BAR /= 0 >" -- 	Vector No: 509
severity warning;

assert (VECT_BAR = '1')
report
"Assert 658 : < VECT_BAR /= 1 >"
severity warning;

assert (MAP_BAR = '1')
report
"Assert 659 : < MAP_BAR /= 1 >"
severity warning;

wait for 1 ns;

-- **************************************************
--------------------------

clk <= '1'; --	 Cycle No: 1258

wait for 1 ns;

I <= "0000";
CCEN_BAR <= '0';
CC_BAR <= '0';
RLD_BAR <= '0';
CI <= '0';
OEbar <= '0';
D <= "000000000000";

wait for 4 ns;

clk <= '0';

wait for 4 ns;

wait for 1 ns;

--------------------------

clk <= '1'; --	 Cycle No: 1259

wait for 1 ns;

I <= "0000";
CCEN_BAR <= '1';
CC_BAR <= '1';
RLD_BAR <= '1';
CI <= '0';
OEbar <= '0';

wait for 4 ns;

clk <= '0';

wait for 4 ns;

assert (PL_BAR = '0')
report
"Assert 660 : < PL_BAR /= 0 >" -- 	Vector No: 510
severity warning;

assert (VECT_BAR = '1')
report
"Assert 661 : < VECT_BAR /= 1 >"
severity warning;

assert (MAP_BAR = '1')
report
"Assert 662 : < MAP_BAR /= 1 >"
severity warning;

wait for 1 ns;

-- **************************************************
--------------------------

clk <= '1'; --	 Cycle No: 1260

wait for 1 ns;

I <= "0000";
CCEN_BAR <= '0';
CC_BAR <= '0';
RLD_BAR <= '0';
CI <= '0';
OEbar <= '0';
D <= "000000000000";

wait for 4 ns;

clk <= '0';

wait for 4 ns;

wait for 1 ns;

--------------------------

clk <= '1'; --	 Cycle No: 1261

wait for 1 ns;

I <= "0000";
CCEN_BAR <= '1';
CC_BAR <= '0';
RLD_BAR <= '1';
CI <= '0';
OEbar <= '0';

wait for 4 ns;

clk <= '0';

wait for 4 ns;

assert (PL_BAR = '0')
report
"Assert 663 : < PL_BAR /= 0 >" -- 	Vector No: 511
severity warning;

assert (VECT_BAR = '1')
report
"Assert 664 : < VECT_BAR /= 1 >"
severity warning;

assert (MAP_BAR = '1')
report
"Assert 665 : < MAP_BAR /= 1 >"
severity warning;

wait for 1 ns;

-- **************************************************
-- *** R = 1 ***
--------------------------

clk <= '1'; --	 Cycle No: 1262

wait for 1 ns;

I <= "0000";
CCEN_BAR <= '0';
CC_BAR <= '0';
RLD_BAR <= '0';
CI <= '0';
OEbar <= '0';
D <= "111111111111";

wait for 4 ns;

clk <= '0';

wait for 4 ns;

wait for 1 ns;

--------------------------

clk <= '1'; --	 Cycle No: 1263

wait for 1 ns;

I <= "0000";
CCEN_BAR <= '0';
CC_BAR <= '0';
RLD_BAR <= '1';
CI <= '0';
OEbar <= '0';

wait for 4 ns;

clk <= '0';

wait for 4 ns;

assert (PL_BAR = '0')
report
"Assert 666 : < PL_BAR /= 0 >" -- 	Vector No: 512
severity warning;

assert (VECT_BAR = '1')
report
"Assert 667 : < VECT_BAR /= 1 >"
severity warning;

assert (MAP_BAR = '1')
report
"Assert 668 : < MAP_BAR /= 1 >"
severity warning;

wait for 1 ns;

-- **************************************************
--------------------------

clk <= '1'; --	 Cycle No: 1264

wait for 1 ns;

I <= "0000";
CCEN_BAR <= '0';
CC_BAR <= '0';
RLD_BAR <= '0';
CI <= '0';
OEbar <= '0';
D <= "111111111111";

wait for 4 ns;

clk <= '0';

wait for 4 ns;

wait for 1 ns;

--------------------------

clk <= '1'; --	 Cycle No: 1265

wait for 1 ns;

I <= "0000";
CCEN_BAR <= '0';
CC_BAR <= '1';
RLD_BAR <= '1';
CI <= '0';
OEbar <= '0';

wait for 4 ns;

clk <= '0';

wait for 4 ns;

assert (PL_BAR = '0')
report
"Assert 669 : < PL_BAR /= 0 >" -- 	Vector No: 513
severity warning;

assert (VECT_BAR = '1')
report
"Assert 670 : < VECT_BAR /= 1 >"
severity warning;

assert (MAP_BAR = '1')
report
"Assert 671 : < MAP_BAR /= 1 >"
severity warning;

wait for 1 ns;

-- **************************************************
--------------------------

clk <= '1'; --	 Cycle No: 1266

wait for 1 ns;

I <= "0000";
CCEN_BAR <= '0';
CC_BAR <= '0';
RLD_BAR <= '0';
CI <= '0';
OEbar <= '0';
D <= "111111111111";

wait for 4 ns;

clk <= '0';

wait for 4 ns;

wait for 1 ns;

--------------------------

clk <= '1'; --	 Cycle No: 1267

wait for 1 ns;

I <= "0000";
CCEN_BAR <= '1';
CC_BAR <= '1';
RLD_BAR <= '1';
CI <= '0';
OEbar <= '0';

wait for 4 ns;

clk <= '0';

wait for 4 ns;

assert (PL_BAR = '0')
report
"Assert 672 : < PL_BAR /= 0 >" -- 	Vector No: 514
severity warning;

assert (VECT_BAR = '1')
report
"Assert 673 : < VECT_BAR /= 1 >"
severity warning;

assert (MAP_BAR = '1')
report
"Assert 674 : < MAP_BAR /= 1 >"
severity warning;

wait for 1 ns;

-- **************************************************
--------------------------

clk <= '1'; --	 Cycle No: 1268

wait for 1 ns;

I <= "0000";
CCEN_BAR <= '0';
CC_BAR <= '0';
RLD_BAR <= '0';
CI <= '0';
OEbar <= '0';
D <= "111111111111";

wait for 4 ns;

clk <= '0';

wait for 4 ns;

wait for 1 ns;

--------------------------

clk <= '1'; --	 Cycle No: 1269

wait for 1 ns;

I <= "0000";
CCEN_BAR <= '1';
CC_BAR <= '0';
RLD_BAR <= '1';
CI <= '0';
OEbar <= '0';

wait for 4 ns;

clk <= '0';

wait for 4 ns;

assert (PL_BAR = '0')
report
"Assert 675 : < PL_BAR /= 0 >" -- 	Vector No: 515
severity warning;

assert (VECT_BAR = '1')
report
"Assert 676 : < VECT_BAR /= 1 >"
severity warning;

assert (MAP_BAR = '1')
report
"Assert 677 : < MAP_BAR /= 1 >"
severity warning;

wait for 1 ns;

-- **************************************************
-- ****** I = 1 *******
-- *** R = 0 ***
--------------------------

clk <= '1'; --	 Cycle No: 1270

wait for 1 ns;

I <= "0000";
CCEN_BAR <= '0';
CC_BAR <= '0';
RLD_BAR <= '0';
CI <= '0';
OEbar <= '0';
D <= "000000000000";

wait for 4 ns;

clk <= '0';

wait for 4 ns;

wait for 1 ns;

--------------------------

clk <= '1'; --	 Cycle No: 1271

wait for 1 ns;

I <= "0001";
CCEN_BAR <= '0';
CC_BAR <= '0';
RLD_BAR <= '1';
CI <= '0';
OEbar <= '0';

wait for 4 ns;

clk <= '0';

wait for 4 ns;

assert (PL_BAR = '0')
report
"Assert 678 : < PL_BAR /= 0 >" -- 	Vector No: 516
severity warning;

assert (VECT_BAR = '1')
report
"Assert 679 : < VECT_BAR /= 1 >"
severity warning;

assert (MAP_BAR = '1')
report
"Assert 680 : < MAP_BAR /= 1 >"
severity warning;

wait for 1 ns;

-- **************************************************
--------------------------

clk <= '1'; --	 Cycle No: 1272

wait for 1 ns;

I <= "0000";
CCEN_BAR <= '0';
CC_BAR <= '0';
RLD_BAR <= '0';
CI <= '0';
OEbar <= '0';
D <= "000000000000";

wait for 4 ns;

clk <= '0';

wait for 4 ns;

wait for 1 ns;

--------------------------

clk <= '1'; --	 Cycle No: 1273

wait for 1 ns;

I <= "0001";
CCEN_BAR <= '0';
CC_BAR <= '1';
RLD_BAR <= '1';
CI <= '0';
OEbar <= '0';

wait for 4 ns;

clk <= '0';

wait for 4 ns;

assert (PL_BAR = '0')
report
"Assert 681 : < PL_BAR /= 0 >" -- 	Vector No: 517
severity warning;

assert (VECT_BAR = '1')
report
"Assert 682 : < VECT_BAR /= 1 >"
severity warning;

assert (MAP_BAR = '1')
report
"Assert 683 : < MAP_BAR /= 1 >"
severity warning;

wait for 1 ns;

-- **************************************************
--------------------------

clk <= '1'; --	 Cycle No: 1274

wait for 1 ns;

I <= "0000";
CCEN_BAR <= '0';
CC_BAR <= '0';
RLD_BAR <= '0';
CI <= '0';
OEbar <= '0';
D <= "000000000000";

wait for 4 ns;

clk <= '0';

wait for 4 ns;

wait for 1 ns;

--------------------------

clk <= '1'; --	 Cycle No: 1275

wait for 1 ns;

I <= "0001";
CCEN_BAR <= '1';
CC_BAR <= '1';
RLD_BAR <= '1';
CI <= '0';
OEbar <= '0';

wait for 4 ns;

clk <= '0';

wait for 4 ns;

assert (PL_BAR = '0')
report
"Assert 684 : < PL_BAR /= 0 >" -- 	Vector No: 518
severity warning;

assert (VECT_BAR = '1')
report
"Assert 685 : < VECT_BAR /= 1 >"
severity warning;

assert (MAP_BAR = '1')
report
"Assert 686 : < MAP_BAR /= 1 >"
severity warning;

wait for 1 ns;

-- **************************************************
--------------------------

clk <= '1'; --	 Cycle No: 1276

wait for 1 ns;

I <= "0000";
CCEN_BAR <= '0';
CC_BAR <= '0';
RLD_BAR <= '0';
CI <= '0';
OEbar <= '0';
D <= "000000000000";

wait for 4 ns;

clk <= '0';

wait for 4 ns;

wait for 1 ns;

--------------------------

clk <= '1'; --	 Cycle No: 1277

wait for 1 ns;

I <= "0001";
CCEN_BAR <= '1';
CC_BAR <= '0';
RLD_BAR <= '1';
CI <= '0';
OEbar <= '0';

wait for 4 ns;

clk <= '0';

wait for 4 ns;

assert (PL_BAR = '0')
report
"Assert 687 : < PL_BAR /= 0 >" -- 	Vector No: 519
severity warning;

assert (VECT_BAR = '1')
report
"Assert 688 : < VECT_BAR /= 1 >"
severity warning;

assert (MAP_BAR = '1')
report
"Assert 689 : < MAP_BAR /= 1 >"
severity warning;

wait for 1 ns;

-- **************************************************
-- *** R = 1 ***
--------------------------

clk <= '1'; --	 Cycle No: 1278

wait for 1 ns;

I <= "0000";
CCEN_BAR <= '0';
CC_BAR <= '0';
RLD_BAR <= '0';
CI <= '0';
OEbar <= '0';
D <= "111111111111";

wait for 4 ns;

clk <= '0';

wait for 4 ns;

wait for 1 ns;

--------------------------

clk <= '1'; --	 Cycle No: 1279

wait for 1 ns;

I <= "0001";
CCEN_BAR <= '0';
CC_BAR <= '0';
RLD_BAR <= '1';
CI <= '0';
OEbar <= '0';

wait for 4 ns;

clk <= '0';

wait for 4 ns;

assert (PL_BAR = '0')
report
"Assert 690 : < PL_BAR /= 0 >" -- 	Vector No: 520
severity warning;

assert (VECT_BAR = '1')
report
"Assert 691 : < VECT_BAR /= 1 >"
severity warning;

assert (MAP_BAR = '1')
report
"Assert 692 : < MAP_BAR /= 1 >"
severity warning;

wait for 1 ns;

-- **************************************************
--------------------------

clk <= '1'; --	 Cycle No: 1280

wait for 1 ns;

I <= "0000";
CCEN_BAR <= '0';
CC_BAR <= '0';
RLD_BAR <= '0';
CI <= '0';
OEbar <= '0';
D <= "111111111111";

wait for 4 ns;

clk <= '0';

wait for 4 ns;

wait for 1 ns;

--------------------------

clk <= '1'; --	 Cycle No: 1281

wait for 1 ns;

I <= "0001";
CCEN_BAR <= '0';
CC_BAR <= '1';
RLD_BAR <= '1';
CI <= '0';
OEbar <= '0';

wait for 4 ns;

clk <= '0';

wait for 4 ns;

assert (PL_BAR = '0')
report
"Assert 693 : < PL_BAR /= 0 >" -- 	Vector No: 521
severity warning;

assert (VECT_BAR = '1')
report
"Assert 694 : < VECT_BAR /= 1 >"
severity warning;

assert (MAP_BAR = '1')
report
"Assert 695 : < MAP_BAR /= 1 >"
severity warning;

wait for 1 ns;

-- **************************************************
--------------------------

clk <= '1'; --	 Cycle No: 1282

wait for 1 ns;

I <= "0000";
CCEN_BAR <= '0';
CC_BAR <= '0';
RLD_BAR <= '0';
CI <= '0';
OEbar <= '0';
D <= "111111111111";

wait for 4 ns;

clk <= '0';

wait for 4 ns;

wait for 1 ns;

--------------------------

clk <= '1'; --	 Cycle No: 1283

wait for 1 ns;

I <= "0001";
CCEN_BAR <= '1';
CC_BAR <= '1';
RLD_BAR <= '1';
CI <= '0';
OEbar <= '0';

wait for 4 ns;

clk <= '0';

wait for 4 ns;

assert (PL_BAR = '0')
report
"Assert 696 : < PL_BAR /= 0 >" -- 	Vector No: 522
severity warning;

assert (VECT_BAR = '1')
report
"Assert 697 : < VECT_BAR /= 1 >"
severity warning;

assert (MAP_BAR = '1')
report
"Assert 698 : < MAP_BAR /= 1 >"
severity warning;

wait for 1 ns;

-- **************************************************
--------------------------

clk <= '1'; --	 Cycle No: 1284

wait for 1 ns;

I <= "0000";
CCEN_BAR <= '0';
CC_BAR <= '0';
RLD_BAR <= '0';
CI <= '0';
OEbar <= '0';
D <= "111111111111";

wait for 4 ns;

clk <= '0';

wait for 4 ns;

wait for 1 ns;

--------------------------

clk <= '1'; --	 Cycle No: 1285

wait for 1 ns;

I <= "0001";
CCEN_BAR <= '1';
CC_BAR <= '0';
RLD_BAR <= '1';
CI <= '0';
OEbar <= '0';

wait for 4 ns;

clk <= '0';

wait for 4 ns;

assert (PL_BAR = '0')
report
"Assert 699 : < PL_BAR /= 0 >" -- 	Vector No: 523
severity warning;

assert (VECT_BAR = '1')
report
"Assert 700 : < VECT_BAR /= 1 >"
severity warning;

assert (MAP_BAR = '1')
report
"Assert 701 : < MAP_BAR /= 1 >"
severity warning;

wait for 1 ns;

-- **************************************************
-- ****** I = 2 *******
-- *** R = 0 ***
--------------------------

clk <= '1'; --	 Cycle No: 1286

wait for 1 ns;

I <= "0000";
CCEN_BAR <= '0';
CC_BAR <= '0';
RLD_BAR <= '0';
CI <= '0';
OEbar <= '0';
D <= "000000000000";

wait for 4 ns;

clk <= '0';

wait for 4 ns;

wait for 1 ns;

--------------------------

clk <= '1'; --	 Cycle No: 1287

wait for 1 ns;

I <= "0010";
CCEN_BAR <= '0';
CC_BAR <= '0';
RLD_BAR <= '1';
CI <= '0';
OEbar <= '0';

wait for 4 ns;

clk <= '0';

wait for 4 ns;

assert (PL_BAR = '1')
report
"Assert 702 : < PL_BAR /= 1 >" -- 	Vector No: 524
severity warning;

assert (VECT_BAR = '1')
report
"Assert 703 : < VECT_BAR /= 1 >"
severity warning;

assert (MAP_BAR = '0')
report
"Assert 704 : < MAP_BAR /= 0 >"
severity warning;

wait for 1 ns;

-- **************************************************
--------------------------

clk <= '1'; --	 Cycle No: 1288

wait for 1 ns;

I <= "0000";
CCEN_BAR <= '0';
CC_BAR <= '0';
RLD_BAR <= '0';
CI <= '0';
OEbar <= '0';
D <= "000000000000";

wait for 4 ns;

clk <= '0';

wait for 4 ns;

wait for 1 ns;

--------------------------

clk <= '1'; --	 Cycle No: 1289

wait for 1 ns;

I <= "0010";
CCEN_BAR <= '0';
CC_BAR <= '1';
RLD_BAR <= '1';
CI <= '0';
OEbar <= '0';

wait for 4 ns;

clk <= '0';

wait for 4 ns;

assert (PL_BAR = '1')
report
"Assert 705 : < PL_BAR /= 1 >" -- 	Vector No: 525
severity warning;

assert (VECT_BAR = '1')
report
"Assert 706 : < VECT_BAR /= 1 >"
severity warning;

assert (MAP_BAR = '0')
report
"Assert 707 : < MAP_BAR /= 0 >"
severity warning;

wait for 1 ns;

-- **************************************************
--------------------------

clk <= '1'; --	 Cycle No: 1290

wait for 1 ns;

I <= "0000";
CCEN_BAR <= '0';
CC_BAR <= '0';
RLD_BAR <= '0';
CI <= '0';
OEbar <= '0';
D <= "000000000000";

wait for 4 ns;

clk <= '0';

wait for 4 ns;

wait for 1 ns;

--------------------------

clk <= '1'; --	 Cycle No: 1291

wait for 1 ns;

I <= "0010";
CCEN_BAR <= '1';
CC_BAR <= '1';
RLD_BAR <= '1';
CI <= '0';
OEbar <= '0';

wait for 4 ns;

clk <= '0';

wait for 4 ns;

assert (PL_BAR = '1')
report
"Assert 708 : < PL_BAR /= 1 >" -- 	Vector No: 526
severity warning;

assert (VECT_BAR = '1')
report
"Assert 709 : < VECT_BAR /= 1 >"
severity warning;

assert (MAP_BAR = '0')
report
"Assert 710 : < MAP_BAR /= 0 >"
severity warning;

wait for 1 ns;

-- **************************************************
--------------------------

clk <= '1'; --	 Cycle No: 1292

wait for 1 ns;

I <= "0000";
CCEN_BAR <= '0';
CC_BAR <= '0';
RLD_BAR <= '0';
CI <= '0';
OEbar <= '0';
D <= "000000000000";

wait for 4 ns;

clk <= '0';

wait for 4 ns;

wait for 1 ns;

--------------------------

clk <= '1'; --	 Cycle No: 1293

wait for 1 ns;

I <= "0010";
CCEN_BAR <= '1';
CC_BAR <= '0';
RLD_BAR <= '1';
CI <= '0';
OEbar <= '0';

wait for 4 ns;

clk <= '0';

wait for 4 ns;

assert (PL_BAR = '1')
report
"Assert 711 : < PL_BAR /= 1 >" -- 	Vector No: 527
severity warning;

assert (VECT_BAR = '1')
report
"Assert 712 : < VECT_BAR /= 1 >"
severity warning;

assert (MAP_BAR = '0')
report
"Assert 713 : < MAP_BAR /= 0 >"
severity warning;

wait for 1 ns;

-- **************************************************
-- *** R = 1 ***
--------------------------

clk <= '1'; --	 Cycle No: 1294

wait for 1 ns;

I <= "0000";
CCEN_BAR <= '0';
CC_BAR <= '0';
RLD_BAR <= '0';
CI <= '0';
OEbar <= '0';
D <= "111111111111";

wait for 4 ns;

clk <= '0';

wait for 4 ns;

wait for 1 ns;

--------------------------

clk <= '1'; --	 Cycle No: 1295

wait for 1 ns;

I <= "0010";
CCEN_BAR <= '0';
CC_BAR <= '0';
RLD_BAR <= '1';
CI <= '0';
OEbar <= '0';

wait for 4 ns;

clk <= '0';

wait for 4 ns;

assert (PL_BAR = '1')
report
"Assert 714 : < PL_BAR /= 1 >" -- 	Vector No: 528
severity warning;

assert (VECT_BAR = '1')
report
"Assert 715 : < VECT_BAR /= 1 >"
severity warning;

assert (MAP_BAR = '0')
report
"Assert 716 : < MAP_BAR /= 0 >"
severity warning;

wait for 1 ns;

-- **************************************************
--------------------------

clk <= '1'; --	 Cycle No: 1296

wait for 1 ns;

I <= "0000";
CCEN_BAR <= '0';
CC_BAR <= '0';
RLD_BAR <= '0';
CI <= '0';
OEbar <= '0';
D <= "111111111111";

wait for 4 ns;

clk <= '0';

wait for 4 ns;

wait for 1 ns;

--------------------------

clk <= '1'; --	 Cycle No: 1297

wait for 1 ns;

I <= "0010";
CCEN_BAR <= '0';
CC_BAR <= '1';
RLD_BAR <= '1';
CI <= '0';
OEbar <= '0';

wait for 4 ns;

clk <= '0';

wait for 4 ns;

assert (PL_BAR = '1')
report
"Assert 717 : < PL_BAR /= 1 >" -- 	Vector No: 529
severity warning;

assert (VECT_BAR = '1')
report
"Assert 718 : < VECT_BAR /= 1 >"
severity warning;

assert (MAP_BAR = '0')
report
"Assert 719 : < MAP_BAR /= 0 >"
severity warning;

wait for 1 ns;

-- **************************************************
--------------------------

clk <= '1'; --	 Cycle No: 1298

wait for 1 ns;

I <= "0000";
CCEN_BAR <= '0';
CC_BAR <= '0';
RLD_BAR <= '0';
CI <= '0';
OEbar <= '0';
D <= "111111111111";

wait for 4 ns;

clk <= '0';

wait for 4 ns;

wait for 1 ns;

--------------------------

clk <= '1'; --	 Cycle No: 1299

wait for 1 ns;

I <= "0010";
CCEN_BAR <= '1';
CC_BAR <= '1';
RLD_BAR <= '1';
CI <= '0';
OEbar <= '0';

wait for 4 ns;

clk <= '0';

wait for 4 ns;

assert (PL_BAR = '1')
report
"Assert 720 : < PL_BAR /= 1 >" -- 	Vector No: 530
severity warning;

assert (VECT_BAR = '1')
report
"Assert 721 : < VECT_BAR /= 1 >"
severity warning;

assert (MAP_BAR = '0')
report
"Assert 722 : < MAP_BAR /= 0 >"
severity warning;

wait for 1 ns;

-- **************************************************
--------------------------

clk <= '1'; --	 Cycle No: 1300

wait for 1 ns;

I <= "0000";
CCEN_BAR <= '0';
CC_BAR <= '0';
RLD_BAR <= '0';
CI <= '0';
OEbar <= '0';
D <= "111111111111";

wait for 4 ns;

clk <= '0';

wait for 4 ns;

wait for 1 ns;

--------------------------

clk <= '1'; --	 Cycle No: 1301

wait for 1 ns;

I <= "0010";
CCEN_BAR <= '1';
CC_BAR <= '0';
RLD_BAR <= '1';
CI <= '0';
OEbar <= '0';

wait for 4 ns;

clk <= '0';

wait for 4 ns;

assert (PL_BAR = '1')
report
"Assert 723 : < PL_BAR /= 1 >" -- 	Vector No: 531
severity warning;

assert (VECT_BAR = '1')
report
"Assert 724 : < VECT_BAR /= 1 >"
severity warning;

assert (MAP_BAR = '0')
report
"Assert 725 : < MAP_BAR /= 0 >"
severity warning;

wait for 1 ns;

-- **************************************************
-- ****** I = 3 *******
-- *** R = 0 ***
--------------------------

clk <= '1'; --	 Cycle No: 1302

wait for 1 ns;

I <= "0000";
CCEN_BAR <= '0';
CC_BAR <= '0';
RLD_BAR <= '0';
CI <= '0';
OEbar <= '0';
D <= "000000000000";

wait for 4 ns;

clk <= '0';

wait for 4 ns;

wait for 1 ns;

--------------------------

clk <= '1'; --	 Cycle No: 1303

wait for 1 ns;

I <= "0011";
CCEN_BAR <= '0';
CC_BAR <= '0';
RLD_BAR <= '1';
CI <= '0';
OEbar <= '0';

wait for 4 ns;

clk <= '0';

wait for 4 ns;

assert (PL_BAR = '0')
report
"Assert 726 : < PL_BAR /= 0 >" -- 	Vector No: 532
severity warning;

assert (VECT_BAR = '1')
report
"Assert 727 : < VECT_BAR /= 1 >"
severity warning;

assert (MAP_BAR = '1')
report
"Assert 728 : < MAP_BAR /= 1 >"
severity warning;

wait for 1 ns;

-- **************************************************
--------------------------

clk <= '1'; --	 Cycle No: 1304

wait for 1 ns;

I <= "0000";
CCEN_BAR <= '0';
CC_BAR <= '0';
RLD_BAR <= '0';
CI <= '0';
OEbar <= '0';
D <= "000000000000";

wait for 4 ns;

clk <= '0';

wait for 4 ns;

wait for 1 ns;

--------------------------

clk <= '1'; --	 Cycle No: 1305

wait for 1 ns;

I <= "0011";
CCEN_BAR <= '0';
CC_BAR <= '1';
RLD_BAR <= '1';
CI <= '0';
OEbar <= '0';

wait for 4 ns;

clk <= '0';

wait for 4 ns;

assert (PL_BAR = '0')
report
"Assert 729 : < PL_BAR /= 0 >" -- 	Vector No: 533
severity warning;

assert (VECT_BAR = '1')
report
"Assert 730 : < VECT_BAR /= 1 >"
severity warning;

assert (MAP_BAR = '1')
report
"Assert 731 : < MAP_BAR /= 1 >"
severity warning;

wait for 1 ns;

-- **************************************************
--------------------------

clk <= '1'; --	 Cycle No: 1306

wait for 1 ns;

I <= "0000";
CCEN_BAR <= '0';
CC_BAR <= '0';
RLD_BAR <= '0';
CI <= '0';
OEbar <= '0';
D <= "000000000000";

wait for 4 ns;

clk <= '0';

wait for 4 ns;

wait for 1 ns;

--------------------------

clk <= '1'; --	 Cycle No: 1307

wait for 1 ns;

I <= "0011";
CCEN_BAR <= '1';
CC_BAR <= '1';
RLD_BAR <= '1';
CI <= '0';
OEbar <= '0';

wait for 4 ns;

clk <= '0';

wait for 4 ns;

assert (PL_BAR = '0')
report
"Assert 732 : < PL_BAR /= 0 >" -- 	Vector No: 534
severity warning;

assert (VECT_BAR = '1')
report
"Assert 733 : < VECT_BAR /= 1 >"
severity warning;

assert (MAP_BAR = '1')
report
"Assert 734 : < MAP_BAR /= 1 >"
severity warning;

wait for 1 ns;

-- **************************************************
--------------------------

clk <= '1'; --	 Cycle No: 1308

wait for 1 ns;

I <= "0000";
CCEN_BAR <= '0';
CC_BAR <= '0';
RLD_BAR <= '0';
CI <= '0';
OEbar <= '0';
D <= "000000000000";

wait for 4 ns;

clk <= '0';

wait for 4 ns;

wait for 1 ns;

--------------------------

clk <= '1'; --	 Cycle No: 1309

wait for 1 ns;

I <= "0011";
CCEN_BAR <= '1';
CC_BAR <= '0';
RLD_BAR <= '1';
CI <= '0';
OEbar <= '0';

wait for 4 ns;

clk <= '0';

wait for 4 ns;

assert (PL_BAR = '0')
report
"Assert 735 : < PL_BAR /= 0 >" -- 	Vector No: 535
severity warning;

assert (VECT_BAR = '1')
report
"Assert 736 : < VECT_BAR /= 1 >"
severity warning;

assert (MAP_BAR = '1')
report
"Assert 737 : < MAP_BAR /= 1 >"
severity warning;

wait for 1 ns;

-- **************************************************
-- *** R = 1 ***
--------------------------

clk <= '1'; --	 Cycle No: 1310

wait for 1 ns;

I <= "0000";
CCEN_BAR <= '0';
CC_BAR <= '0';
RLD_BAR <= '0';
CI <= '0';
OEbar <= '0';
D <= "111111111111";

wait for 4 ns;

clk <= '0';

wait for 4 ns;

wait for 1 ns;

--------------------------

clk <= '1'; --	 Cycle No: 1311

wait for 1 ns;

I <= "0011";
CCEN_BAR <= '0';
CC_BAR <= '0';
RLD_BAR <= '1';
CI <= '0';
OEbar <= '0';

wait for 4 ns;

clk <= '0';

wait for 4 ns;

assert (PL_BAR = '0')
report
"Assert 738 : < PL_BAR /= 0 >" -- 	Vector No: 536
severity warning;

assert (VECT_BAR = '1')
report
"Assert 739 : < VECT_BAR /= 1 >"
severity warning;

assert (MAP_BAR = '1')
report
"Assert 740 : < MAP_BAR /= 1 >"
severity warning;

wait for 1 ns;

-- **************************************************
--------------------------

clk <= '1'; --	 Cycle No: 1312

wait for 1 ns;

I <= "0000";
CCEN_BAR <= '0';
CC_BAR <= '0';
RLD_BAR <= '0';
CI <= '0';
OEbar <= '0';
D <= "111111111111";

wait for 4 ns;

clk <= '0';

wait for 4 ns;

wait for 1 ns;

--------------------------

clk <= '1'; --	 Cycle No: 1313

wait for 1 ns;

I <= "0011";
CCEN_BAR <= '0';
CC_BAR <= '1';
RLD_BAR <= '1';
CI <= '0';
OEbar <= '0';

wait for 4 ns;

clk <= '0';

wait for 4 ns;

assert (PL_BAR = '0')
report
"Assert 741 : < PL_BAR /= 0 >" -- 	Vector No: 537
severity warning;

assert (VECT_BAR = '1')
report
"Assert 742 : < VECT_BAR /= 1 >"
severity warning;

assert (MAP_BAR = '1')
report
"Assert 743 : < MAP_BAR /= 1 >"
severity warning;

wait for 1 ns;

-- **************************************************
--------------------------

clk <= '1'; --	 Cycle No: 1314

wait for 1 ns;

I <= "0000";
CCEN_BAR <= '0';
CC_BAR <= '0';
RLD_BAR <= '0';
CI <= '0';
OEbar <= '0';
D <= "111111111111";

wait for 4 ns;

clk <= '0';

wait for 4 ns;

wait for 1 ns;

--------------------------

clk <= '1'; --	 Cycle No: 1315

wait for 1 ns;

I <= "0011";
CCEN_BAR <= '1';
CC_BAR <= '1';
RLD_BAR <= '1';
CI <= '0';
OEbar <= '0';

wait for 4 ns;

clk <= '0';

wait for 4 ns;

assert (PL_BAR = '0')
report
"Assert 744 : < PL_BAR /= 0 >" -- 	Vector No: 538
severity warning;

assert (VECT_BAR = '1')
report
"Assert 745 : < VECT_BAR /= 1 >"
severity warning;

assert (MAP_BAR = '1')
report
"Assert 746 : < MAP_BAR /= 1 >"
severity warning;

wait for 1 ns;

-- **************************************************
--------------------------

clk <= '1'; --	 Cycle No: 1316

wait for 1 ns;

I <= "0000";
CCEN_BAR <= '0';
CC_BAR <= '0';
RLD_BAR <= '0';
CI <= '0';
OEbar <= '0';
D <= "111111111111";

wait for 4 ns;

clk <= '0';

wait for 4 ns;

wait for 1 ns;

--------------------------

clk <= '1'; --	 Cycle No: 1317

wait for 1 ns;

I <= "0011";
CCEN_BAR <= '1';
CC_BAR <= '0';
RLD_BAR <= '1';
CI <= '0';
OEbar <= '0';

wait for 4 ns;

clk <= '0';

wait for 4 ns;

assert (PL_BAR = '0')
report
"Assert 747 : < PL_BAR /= 0 >" -- 	Vector No: 539
severity warning;

assert (VECT_BAR = '1')
report
"Assert 748 : < VECT_BAR /= 1 >"
severity warning;

assert (MAP_BAR = '1')
report
"Assert 749 : < MAP_BAR /= 1 >"
severity warning;

wait for 1 ns;

-- **************************************************
-- ****** I = 4 *******
-- *** R = 0 ***
--------------------------

clk <= '1'; --	 Cycle No: 1318

wait for 1 ns;

I <= "0000";
CCEN_BAR <= '0';
CC_BAR <= '0';
RLD_BAR <= '0';
CI <= '0';
OEbar <= '0';
D <= "000000000000";

wait for 4 ns;

clk <= '0';

wait for 4 ns;

wait for 1 ns;

--------------------------

clk <= '1'; --	 Cycle No: 1319

wait for 1 ns;

I <= "0100";
CCEN_BAR <= '0';
CC_BAR <= '0';
RLD_BAR <= '1';
CI <= '0';
OEbar <= '0';

wait for 4 ns;

clk <= '0';

wait for 4 ns;

assert (PL_BAR = '0')
report
"Assert 750 : < PL_BAR /= 0 >" -- 	Vector No: 540
severity warning;

assert (VECT_BAR = '1')
report
"Assert 751 : < VECT_BAR /= 1 >"
severity warning;

assert (MAP_BAR = '1')
report
"Assert 752 : < MAP_BAR /= 1 >"
severity warning;

wait for 1 ns;

-- **************************************************
--------------------------

clk <= '1'; --	 Cycle No: 1320

wait for 1 ns;

I <= "0000";
CCEN_BAR <= '0';
CC_BAR <= '0';
RLD_BAR <= '0';
CI <= '0';
OEbar <= '0';
D <= "000000000000";

wait for 4 ns;

clk <= '0';

wait for 4 ns;

wait for 1 ns;

--------------------------

clk <= '1'; --	 Cycle No: 1321

wait for 1 ns;

I <= "0100";
CCEN_BAR <= '0';
CC_BAR <= '1';
RLD_BAR <= '1';
CI <= '0';
OEbar <= '0';

wait for 4 ns;

clk <= '0';

wait for 4 ns;

assert (PL_BAR = '0')
report
"Assert 753 : < PL_BAR /= 0 >" -- 	Vector No: 541
severity warning;

assert (VECT_BAR = '1')
report
"Assert 754 : < VECT_BAR /= 1 >"
severity warning;

assert (MAP_BAR = '1')
report
"Assert 755 : < MAP_BAR /= 1 >"
severity warning;

wait for 1 ns;

-- **************************************************
--------------------------

clk <= '1'; --	 Cycle No: 1322

wait for 1 ns;

I <= "0000";
CCEN_BAR <= '0';
CC_BAR <= '0';
RLD_BAR <= '0';
CI <= '0';
OEbar <= '0';
D <= "000000000000";

wait for 4 ns;

clk <= '0';

wait for 4 ns;

wait for 1 ns;

--------------------------

clk <= '1'; --	 Cycle No: 1323

wait for 1 ns;

I <= "0100";
CCEN_BAR <= '1';
CC_BAR <= '1';
RLD_BAR <= '1';
CI <= '0';
OEbar <= '0';

wait for 4 ns;

clk <= '0';

wait for 4 ns;

assert (PL_BAR = '0')
report
"Assert 756 : < PL_BAR /= 0 >" -- 	Vector No: 542
severity warning;

assert (VECT_BAR = '1')
report
"Assert 757 : < VECT_BAR /= 1 >"
severity warning;

assert (MAP_BAR = '1')
report
"Assert 758 : < MAP_BAR /= 1 >"
severity warning;

wait for 1 ns;

-- **************************************************
--------------------------

clk <= '1'; --	 Cycle No: 1324

wait for 1 ns;

I <= "0000";
CCEN_BAR <= '0';
CC_BAR <= '0';
RLD_BAR <= '0';
CI <= '0';
OEbar <= '0';
D <= "000000000000";

wait for 4 ns;

clk <= '0';

wait for 4 ns;

wait for 1 ns;

--------------------------

clk <= '1'; --	 Cycle No: 1325

wait for 1 ns;

I <= "0100";
CCEN_BAR <= '1';
CC_BAR <= '0';
RLD_BAR <= '1';
CI <= '0';
OEbar <= '0';

wait for 4 ns;

clk <= '0';

wait for 4 ns;

assert (PL_BAR = '0')
report
"Assert 759 : < PL_BAR /= 0 >" -- 	Vector No: 543
severity warning;

assert (VECT_BAR = '1')
report
"Assert 760 : < VECT_BAR /= 1 >"
severity warning;

assert (MAP_BAR = '1')
report
"Assert 761 : < MAP_BAR /= 1 >"
severity warning;

wait for 1 ns;

-- **************************************************
-- *** R = 1 ***
--------------------------

clk <= '1'; --	 Cycle No: 1326

wait for 1 ns;

I <= "0000";
CCEN_BAR <= '0';
CC_BAR <= '0';
RLD_BAR <= '0';
CI <= '0';
OEbar <= '0';
D <= "111111111111";

wait for 4 ns;

clk <= '0';

wait for 4 ns;

wait for 1 ns;

--------------------------

clk <= '1'; --	 Cycle No: 1327

wait for 1 ns;

I <= "0100";
CCEN_BAR <= '0';
CC_BAR <= '0';
RLD_BAR <= '1';
CI <= '0';
OEbar <= '0';

wait for 4 ns;

clk <= '0';

wait for 4 ns;

assert (PL_BAR = '0')
report
"Assert 762 : < PL_BAR /= 0 >" -- 	Vector No: 544
severity warning;

assert (VECT_BAR = '1')
report
"Assert 763 : < VECT_BAR /= 1 >"
severity warning;

assert (MAP_BAR = '1')
report
"Assert 764 : < MAP_BAR /= 1 >"
severity warning;

wait for 1 ns;

-- **************************************************
--------------------------

clk <= '1'; --	 Cycle No: 1328

wait for 1 ns;

I <= "0000";
CCEN_BAR <= '0';
CC_BAR <= '0';
RLD_BAR <= '0';
CI <= '0';
OEbar <= '0';
D <= "111111111111";

wait for 4 ns;

clk <= '0';

wait for 4 ns;

wait for 1 ns;

--------------------------

clk <= '1'; --	 Cycle No: 1329

wait for 1 ns;

I <= "0100";
CCEN_BAR <= '0';
CC_BAR <= '1';
RLD_BAR <= '1';
CI <= '0';
OEbar <= '0';

wait for 4 ns;

clk <= '0';

wait for 4 ns;

assert (PL_BAR = '0')
report
"Assert 765 : < PL_BAR /= 0 >" -- 	Vector No: 545
severity warning;

assert (VECT_BAR = '1')
report
"Assert 766 : < VECT_BAR /= 1 >"
severity warning;

assert (MAP_BAR = '1')
report
"Assert 767 : < MAP_BAR /= 1 >"
severity warning;

wait for 1 ns;

-- **************************************************
--------------------------

clk <= '1'; --	 Cycle No: 1330

wait for 1 ns;

I <= "0000";
CCEN_BAR <= '0';
CC_BAR <= '0';
RLD_BAR <= '0';
CI <= '0';
OEbar <= '0';
D <= "111111111111";

wait for 4 ns;

clk <= '0';

wait for 4 ns;

wait for 1 ns;

--------------------------

clk <= '1'; --	 Cycle No: 1331

wait for 1 ns;

I <= "0100";
CCEN_BAR <= '1';
CC_BAR <= '1';
RLD_BAR <= '1';
CI <= '0';
OEbar <= '0';

wait for 4 ns;

clk <= '0';

wait for 4 ns;

assert (PL_BAR = '0')
report
"Assert 768 : < PL_BAR /= 0 >" -- 	Vector No: 546
severity warning;

assert (VECT_BAR = '1')
report
"Assert 769 : < VECT_BAR /= 1 >"
severity warning;

assert (MAP_BAR = '1')
report
"Assert 770 : < MAP_BAR /= 1 >"
severity warning;

wait for 1 ns;

-- **************************************************
--------------------------

clk <= '1'; --	 Cycle No: 1332

wait for 1 ns;

I <= "0000";
CCEN_BAR <= '0';
CC_BAR <= '0';
RLD_BAR <= '0';
CI <= '0';
OEbar <= '0';
D <= "111111111111";

wait for 4 ns;

clk <= '0';

wait for 4 ns;

wait for 1 ns;

--------------------------

clk <= '1'; --	 Cycle No: 1333

wait for 1 ns;

I <= "0100";
CCEN_BAR <= '1';
CC_BAR <= '0';
RLD_BAR <= '1';
CI <= '0';
OEbar <= '0';

wait for 4 ns;

clk <= '0';

wait for 4 ns;

assert (PL_BAR = '0')
report
"Assert 771 : < PL_BAR /= 0 >" -- 	Vector No: 547
severity warning;

assert (VECT_BAR = '1')
report
"Assert 772 : < VECT_BAR /= 1 >"
severity warning;

assert (MAP_BAR = '1')
report
"Assert 773 : < MAP_BAR /= 1 >"
severity warning;

wait for 1 ns;

-- **************************************************
-- ****** I = 5 *******
-- *** R = 0 ***
--------------------------

clk <= '1'; --	 Cycle No: 1334

wait for 1 ns;

I <= "0000";
CCEN_BAR <= '0';
CC_BAR <= '0';
RLD_BAR <= '0';
CI <= '0';
OEbar <= '0';
D <= "000000000000";

wait for 4 ns;

clk <= '0';

wait for 4 ns;

wait for 1 ns;

--------------------------

clk <= '1'; --	 Cycle No: 1335

wait for 1 ns;

I <= "0101";
CCEN_BAR <= '0';
CC_BAR <= '0';
RLD_BAR <= '1';
CI <= '0';
OEbar <= '0';

wait for 4 ns;

clk <= '0';

wait for 4 ns;

assert (PL_BAR = '0')
report
"Assert 774 : < PL_BAR /= 0 >" -- 	Vector No: 548
severity warning;

assert (VECT_BAR = '1')
report
"Assert 775 : < VECT_BAR /= 1 >"
severity warning;

assert (MAP_BAR = '1')
report
"Assert 776 : < MAP_BAR /= 1 >"
severity warning;

wait for 1 ns;

-- **************************************************
--------------------------

clk <= '1'; --	 Cycle No: 1336

wait for 1 ns;

I <= "0000";
CCEN_BAR <= '0';
CC_BAR <= '0';
RLD_BAR <= '0';
CI <= '0';
OEbar <= '0';
D <= "000000000000";

wait for 4 ns;

clk <= '0';

wait for 4 ns;

wait for 1 ns;

--------------------------

clk <= '1'; --	 Cycle No: 1337

wait for 1 ns;

I <= "0101";
CCEN_BAR <= '0';
CC_BAR <= '1';
RLD_BAR <= '1';
CI <= '0';
OEbar <= '0';

wait for 4 ns;

clk <= '0';

wait for 4 ns;

assert (PL_BAR = '0')
report
"Assert 777 : < PL_BAR /= 0 >" -- 	Vector No: 549
severity warning;

assert (VECT_BAR = '1')
report
"Assert 778 : < VECT_BAR /= 1 >"
severity warning;

assert (MAP_BAR = '1')
report
"Assert 779 : < MAP_BAR /= 1 >"
severity warning;

wait for 1 ns;

-- **************************************************
--------------------------

clk <= '1'; --	 Cycle No: 1338

wait for 1 ns;

I <= "0000";
CCEN_BAR <= '0';
CC_BAR <= '0';
RLD_BAR <= '0';
CI <= '0';
OEbar <= '0';
D <= "000000000000";

wait for 4 ns;

clk <= '0';

wait for 4 ns;

wait for 1 ns;

--------------------------

clk <= '1'; --	 Cycle No: 1339

wait for 1 ns;

I <= "0101";
CCEN_BAR <= '1';
CC_BAR <= '1';
RLD_BAR <= '1';
CI <= '0';
OEbar <= '0';

wait for 4 ns;

clk <= '0';

wait for 4 ns;

assert (PL_BAR = '0')
report
"Assert 780 : < PL_BAR /= 0 >" -- 	Vector No: 550
severity warning;

assert (VECT_BAR = '1')
report
"Assert 781 : < VECT_BAR /= 1 >"
severity warning;

assert (MAP_BAR = '1')
report
"Assert 782 : < MAP_BAR /= 1 >"
severity warning;

wait for 1 ns;

-- **************************************************
--------------------------

clk <= '1'; --	 Cycle No: 1340

wait for 1 ns;

I <= "0000";
CCEN_BAR <= '0';
CC_BAR <= '0';
RLD_BAR <= '0';
CI <= '0';
OEbar <= '0';
D <= "000000000000";

wait for 4 ns;

clk <= '0';

wait for 4 ns;

wait for 1 ns;

--------------------------

clk <= '1'; --	 Cycle No: 1341

wait for 1 ns;

I <= "0101";
CCEN_BAR <= '1';
CC_BAR <= '0';
RLD_BAR <= '1';
CI <= '0';
OEbar <= '0';

wait for 4 ns;

clk <= '0';

wait for 4 ns;

assert (PL_BAR = '0')
report
"Assert 783 : < PL_BAR /= 0 >" -- 	Vector No: 551
severity warning;

assert (VECT_BAR = '1')
report
"Assert 784 : < VECT_BAR /= 1 >"
severity warning;

assert (MAP_BAR = '1')
report
"Assert 785 : < MAP_BAR /= 1 >"
severity warning;

wait for 1 ns;

-- **************************************************
-- *** R = 1 ***
--------------------------

clk <= '1'; --	 Cycle No: 1342

wait for 1 ns;

I <= "0000";
CCEN_BAR <= '0';
CC_BAR <= '0';
RLD_BAR <= '0';
CI <= '0';
OEbar <= '0';
D <= "111111111111";

wait for 4 ns;

clk <= '0';

wait for 4 ns;

wait for 1 ns;

--------------------------

clk <= '1'; --	 Cycle No: 1343

wait for 1 ns;

I <= "0101";
CCEN_BAR <= '0';
CC_BAR <= '0';
RLD_BAR <= '1';
CI <= '0';
OEbar <= '0';

wait for 4 ns;

clk <= '0';

wait for 4 ns;

assert (PL_BAR = '0')
report
"Assert 786 : < PL_BAR /= 0 >" -- 	Vector No: 552
severity warning;

assert (VECT_BAR = '1')
report
"Assert 787 : < VECT_BAR /= 1 >"
severity warning;

assert (MAP_BAR = '1')
report
"Assert 788 : < MAP_BAR /= 1 >"
severity warning;

wait for 1 ns;

-- **************************************************
--------------------------

clk <= '1'; --	 Cycle No: 1344

wait for 1 ns;

I <= "0000";
CCEN_BAR <= '0';
CC_BAR <= '0';
RLD_BAR <= '0';
CI <= '0';
OEbar <= '0';
D <= "111111111111";

wait for 4 ns;

clk <= '0';

wait for 4 ns;

wait for 1 ns;

--------------------------

clk <= '1'; --	 Cycle No: 1345

wait for 1 ns;

I <= "0101";
CCEN_BAR <= '0';
CC_BAR <= '1';
RLD_BAR <= '1';
CI <= '0';
OEbar <= '0';

wait for 4 ns;

clk <= '0';

wait for 4 ns;

assert (PL_BAR = '0')
report
"Assert 789 : < PL_BAR /= 0 >" -- 	Vector No: 553
severity warning;

assert (VECT_BAR = '1')
report
"Assert 790 : < VECT_BAR /= 1 >"
severity warning;

assert (MAP_BAR = '1')
report
"Assert 791 : < MAP_BAR /= 1 >"
severity warning;

wait for 1 ns;

-- **************************************************
--------------------------

clk <= '1'; --	 Cycle No: 1346

wait for 1 ns;

I <= "0000";
CCEN_BAR <= '0';
CC_BAR <= '0';
RLD_BAR <= '0';
CI <= '0';
OEbar <= '0';
D <= "111111111111";

wait for 4 ns;

clk <= '0';

wait for 4 ns;

wait for 1 ns;

--------------------------

clk <= '1'; --	 Cycle No: 1347

wait for 1 ns;

I <= "0101";
CCEN_BAR <= '1';
CC_BAR <= '1';
RLD_BAR <= '1';
CI <= '0';
OEbar <= '0';

wait for 4 ns;

clk <= '0';

wait for 4 ns;

assert (PL_BAR = '0')
report
"Assert 792 : < PL_BAR /= 0 >" -- 	Vector No: 554
severity warning;

assert (VECT_BAR = '1')
report
"Assert 793 : < VECT_BAR /= 1 >"
severity warning;

assert (MAP_BAR = '1')
report
"Assert 794 : < MAP_BAR /= 1 >"
severity warning;

wait for 1 ns;

-- **************************************************
--------------------------

clk <= '1'; --	 Cycle No: 1348

wait for 1 ns;

I <= "0000";
CCEN_BAR <= '0';
CC_BAR <= '0';
RLD_BAR <= '0';
CI <= '0';
OEbar <= '0';
D <= "111111111111";

wait for 4 ns;

clk <= '0';

wait for 4 ns;

wait for 1 ns;

--------------------------

clk <= '1'; --	 Cycle No: 1349

wait for 1 ns;

I <= "0101";
CCEN_BAR <= '1';
CC_BAR <= '0';
RLD_BAR <= '1';
CI <= '0';
OEbar <= '0';

wait for 4 ns;

clk <= '0';

wait for 4 ns;

assert (PL_BAR = '0')
report
"Assert 795 : < PL_BAR /= 0 >" -- 	Vector No: 555
severity warning;

assert (VECT_BAR = '1')
report
"Assert 796 : < VECT_BAR /= 1 >"
severity warning;

assert (MAP_BAR = '1')
report
"Assert 797 : < MAP_BAR /= 1 >"
severity warning;

wait for 1 ns;

-- **************************************************
-- ****** I = 6 *******
-- *** R = 0 ***
--------------------------

clk <= '1'; --	 Cycle No: 1350

wait for 1 ns;

I <= "0000";
CCEN_BAR <= '0';
CC_BAR <= '0';
RLD_BAR <= '0';
CI <= '0';
OEbar <= '0';
D <= "000000000000";

wait for 4 ns;

clk <= '0';

wait for 4 ns;

wait for 1 ns;

--------------------------

clk <= '1'; --	 Cycle No: 1351

wait for 1 ns;

I <= "0110";
CCEN_BAR <= '0';
CC_BAR <= '0';
RLD_BAR <= '1';
CI <= '0';
OEbar <= '0';

wait for 4 ns;

clk <= '0';

wait for 4 ns;

assert (PL_BAR = '1')
report
"Assert 798 : < PL_BAR /= 1 >" -- 	Vector No: 556
severity warning;

assert (VECT_BAR = '0')
report
"Assert 799 : < VECT_BAR /= 0 >"
severity warning;

assert (MAP_BAR = '1')
report
"Assert 800 : < MAP_BAR /= 1 >"
severity warning;

wait for 1 ns;

-- **************************************************
--------------------------

clk <= '1'; --	 Cycle No: 1352

wait for 1 ns;

I <= "0000";
CCEN_BAR <= '0';
CC_BAR <= '0';
RLD_BAR <= '0';
CI <= '0';
OEbar <= '0';
D <= "000000000000";

wait for 4 ns;

clk <= '0';

wait for 4 ns;

wait for 1 ns;

--------------------------

clk <= '1'; --	 Cycle No: 1353

wait for 1 ns;

I <= "0110";
CCEN_BAR <= '0';
CC_BAR <= '1';
RLD_BAR <= '1';
CI <= '0';
OEbar <= '0';

wait for 4 ns;

clk <= '0';

wait for 4 ns;

assert (PL_BAR = '1')
report
"Assert 801 : < PL_BAR /= 1 >" -- 	Vector No: 557
severity warning;

assert (VECT_BAR = '0')
report
"Assert 802 : < VECT_BAR /= 0 >"
severity warning;

assert (MAP_BAR = '1')
report
"Assert 803 : < MAP_BAR /= 1 >"
severity warning;

wait for 1 ns;

-- **************************************************
--------------------------

clk <= '1'; --	 Cycle No: 1354

wait for 1 ns;

I <= "0000";
CCEN_BAR <= '0';
CC_BAR <= '0';
RLD_BAR <= '0';
CI <= '0';
OEbar <= '0';
D <= "000000000000";

wait for 4 ns;

clk <= '0';

wait for 4 ns;

wait for 1 ns;

--------------------------

clk <= '1'; --	 Cycle No: 1355

wait for 1 ns;

I <= "0110";
CCEN_BAR <= '1';
CC_BAR <= '1';
RLD_BAR <= '1';
CI <= '0';
OEbar <= '0';

wait for 4 ns;

clk <= '0';

wait for 4 ns;

assert (PL_BAR = '1')
report
"Assert 804 : < PL_BAR /= 1 >" -- 	Vector No: 558
severity warning;

assert (VECT_BAR = '0')
report
"Assert 805 : < VECT_BAR /= 0 >"
severity warning;

assert (MAP_BAR = '1')
report
"Assert 806 : < MAP_BAR /= 1 >"
severity warning;

wait for 1 ns;

-- **************************************************
--------------------------

clk <= '1'; --	 Cycle No: 1356

wait for 1 ns;

I <= "0000";
CCEN_BAR <= '0';
CC_BAR <= '0';
RLD_BAR <= '0';
CI <= '0';
OEbar <= '0';
D <= "000000000000";

wait for 4 ns;

clk <= '0';

wait for 4 ns;

wait for 1 ns;

--------------------------

clk <= '1'; --	 Cycle No: 1357

wait for 1 ns;

I <= "0110";
CCEN_BAR <= '1';
CC_BAR <= '0';
RLD_BAR <= '1';
CI <= '0';
OEbar <= '0';

wait for 4 ns;

clk <= '0';

wait for 4 ns;

assert (PL_BAR = '1')
report
"Assert 807 : < PL_BAR /= 1 >" -- 	Vector No: 559
severity warning;

assert (VECT_BAR = '0')
report
"Assert 808 : < VECT_BAR /= 0 >"
severity warning;

assert (MAP_BAR = '1')
report
"Assert 809 : < MAP_BAR /= 1 >"
severity warning;

wait for 1 ns;

-- **************************************************
-- *** R = 1 ***
--------------------------

clk <= '1'; --	 Cycle No: 1358

wait for 1 ns;

I <= "0000";
CCEN_BAR <= '0';
CC_BAR <= '0';
RLD_BAR <= '0';
CI <= '0';
OEbar <= '0';
D <= "111111111111";

wait for 4 ns;

clk <= '0';

wait for 4 ns;

wait for 1 ns;

--------------------------

clk <= '1'; --	 Cycle No: 1359

wait for 1 ns;

I <= "0110";
CCEN_BAR <= '0';
CC_BAR <= '0';
RLD_BAR <= '1';
CI <= '0';
OEbar <= '0';

wait for 4 ns;

clk <= '0';

wait for 4 ns;

assert (PL_BAR = '1')
report
"Assert 810 : < PL_BAR /= 1 >" -- 	Vector No: 560
severity warning;

assert (VECT_BAR = '0')
report
"Assert 811 : < VECT_BAR /= 0 >"
severity warning;

assert (MAP_BAR = '1')
report
"Assert 812 : < MAP_BAR /= 1 >"
severity warning;

wait for 1 ns;

-- **************************************************
--------------------------

clk <= '1'; --	 Cycle No: 1360

wait for 1 ns;

I <= "0000";
CCEN_BAR <= '0';
CC_BAR <= '0';
RLD_BAR <= '0';
CI <= '0';
OEbar <= '0';
D <= "111111111111";

wait for 4 ns;

clk <= '0';

wait for 4 ns;

wait for 1 ns;

--------------------------

clk <= '1'; --	 Cycle No: 1361

wait for 1 ns;

I <= "0110";
CCEN_BAR <= '0';
CC_BAR <= '1';
RLD_BAR <= '1';
CI <= '0';
OEbar <= '0';

wait for 4 ns;

clk <= '0';

wait for 4 ns;

assert (PL_BAR = '1')
report
"Assert 813 : < PL_BAR /= 1 >" -- 	Vector No: 561
severity warning;

assert (VECT_BAR = '0')
report
"Assert 814 : < VECT_BAR /= 0 >"
severity warning;

assert (MAP_BAR = '1')
report
"Assert 815 : < MAP_BAR /= 1 >"
severity warning;

wait for 1 ns;

-- **************************************************
--------------------------

clk <= '1'; --	 Cycle No: 1362

wait for 1 ns;

I <= "0000";
CCEN_BAR <= '0';
CC_BAR <= '0';
RLD_BAR <= '0';
CI <= '0';
OEbar <= '0';
D <= "111111111111";

wait for 4 ns;

clk <= '0';

wait for 4 ns;

wait for 1 ns;

--------------------------

clk <= '1'; --	 Cycle No: 1363

wait for 1 ns;

I <= "0110";
CCEN_BAR <= '1';
CC_BAR <= '1';
RLD_BAR <= '1';
CI <= '0';
OEbar <= '0';

wait for 4 ns;

clk <= '0';

wait for 4 ns;

assert (PL_BAR = '1')
report
"Assert 816 : < PL_BAR /= 1 >" -- 	Vector No: 562
severity warning;

assert (VECT_BAR = '0')
report
"Assert 817 : < VECT_BAR /= 0 >"
severity warning;

assert (MAP_BAR = '1')
report
"Assert 818 : < MAP_BAR /= 1 >"
severity warning;

wait for 1 ns;

-- **************************************************
--------------------------

clk <= '1'; --	 Cycle No: 1364

wait for 1 ns;

I <= "0000";
CCEN_BAR <= '0';
CC_BAR <= '0';
RLD_BAR <= '0';
CI <= '0';
OEbar <= '0';
D <= "111111111111";

wait for 4 ns;

clk <= '0';

wait for 4 ns;

wait for 1 ns;

--------------------------

clk <= '1'; --	 Cycle No: 1365

wait for 1 ns;

I <= "0110";
CCEN_BAR <= '1';
CC_BAR <= '0';
RLD_BAR <= '1';
CI <= '0';
OEbar <= '0';

wait for 4 ns;

clk <= '0';

wait for 4 ns;

assert (PL_BAR = '1')
report
"Assert 819 : < PL_BAR /= 1 >" -- 	Vector No: 563
severity warning;

assert (VECT_BAR = '0')
report
"Assert 820 : < VECT_BAR /= 0 >"
severity warning;

assert (MAP_BAR = '1')
report
"Assert 821 : < MAP_BAR /= 1 >"
severity warning;

wait for 1 ns;

-- **************************************************
-- ****** I = 7 *******
-- *** R = 0 ***
--------------------------

clk <= '1'; --	 Cycle No: 1366

wait for 1 ns;

I <= "0000";
CCEN_BAR <= '0';
CC_BAR <= '0';
RLD_BAR <= '0';
CI <= '0';
OEbar <= '0';
D <= "000000000000";

wait for 4 ns;

clk <= '0';

wait for 4 ns;

wait for 1 ns;

--------------------------

clk <= '1'; --	 Cycle No: 1367

wait for 1 ns;

I <= "0111";
CCEN_BAR <= '0';
CC_BAR <= '0';
RLD_BAR <= '1';
CI <= '0';
OEbar <= '0';

wait for 4 ns;

clk <= '0';

wait for 4 ns;

assert (PL_BAR = '0')
report
"Assert 822 : < PL_BAR /= 0 >" -- 	Vector No: 564
severity warning;

assert (VECT_BAR = '1')
report
"Assert 823 : < VECT_BAR /= 1 >"
severity warning;

assert (MAP_BAR = '1')
report
"Assert 824 : < MAP_BAR /= 1 >"
severity warning;

wait for 1 ns;

-- **************************************************
--------------------------

clk <= '1'; --	 Cycle No: 1368

wait for 1 ns;

I <= "0000";
CCEN_BAR <= '0';
CC_BAR <= '0';
RLD_BAR <= '0';
CI <= '0';
OEbar <= '0';
D <= "000000000000";

wait for 4 ns;

clk <= '0';

wait for 4 ns;

wait for 1 ns;

--------------------------

clk <= '1'; --	 Cycle No: 1369

wait for 1 ns;

I <= "0111";
CCEN_BAR <= '0';
CC_BAR <= '1';
RLD_BAR <= '1';
CI <= '0';
OEbar <= '0';

wait for 4 ns;

clk <= '0';

wait for 4 ns;

assert (PL_BAR = '0')
report
"Assert 825 : < PL_BAR /= 0 >" -- 	Vector No: 565
severity warning;

assert (VECT_BAR = '1')
report
"Assert 826 : < VECT_BAR /= 1 >"
severity warning;

assert (MAP_BAR = '1')
report
"Assert 827 : < MAP_BAR /= 1 >"
severity warning;

wait for 1 ns;

-- **************************************************
--------------------------

clk <= '1'; --	 Cycle No: 1370

wait for 1 ns;

I <= "0000";
CCEN_BAR <= '0';
CC_BAR <= '0';
RLD_BAR <= '0';
CI <= '0';
OEbar <= '0';
D <= "000000000000";

wait for 4 ns;

clk <= '0';

wait for 4 ns;

wait for 1 ns;

--------------------------

clk <= '1'; --	 Cycle No: 1371

wait for 1 ns;

I <= "0111";
CCEN_BAR <= '1';
CC_BAR <= '1';
RLD_BAR <= '1';
CI <= '0';
OEbar <= '0';

wait for 4 ns;

clk <= '0';

wait for 4 ns;

assert (PL_BAR = '0')
report
"Assert 828 : < PL_BAR /= 0 >" -- 	Vector No: 566
severity warning;

assert (VECT_BAR = '1')
report
"Assert 829 : < VECT_BAR /= 1 >"
severity warning;

assert (MAP_BAR = '1')
report
"Assert 830 : < MAP_BAR /= 1 >"
severity warning;

wait for 1 ns;

-- **************************************************
--------------------------

clk <= '1'; --	 Cycle No: 1372

wait for 1 ns;

I <= "0000";
CCEN_BAR <= '0';
CC_BAR <= '0';
RLD_BAR <= '0';
CI <= '0';
OEbar <= '0';
D <= "000000000000";

wait for 4 ns;

clk <= '0';

wait for 4 ns;

wait for 1 ns;

--------------------------

clk <= '1'; --	 Cycle No: 1373

wait for 1 ns;

I <= "0111";
CCEN_BAR <= '1';
CC_BAR <= '0';
RLD_BAR <= '1';
CI <= '0';
OEbar <= '0';

wait for 4 ns;

clk <= '0';

wait for 4 ns;

assert (PL_BAR = '0')
report
"Assert 831 : < PL_BAR /= 0 >" -- 	Vector No: 567
severity warning;

assert (VECT_BAR = '1')
report
"Assert 832 : < VECT_BAR /= 1 >"
severity warning;

assert (MAP_BAR = '1')
report
"Assert 833 : < MAP_BAR /= 1 >"
severity warning;

wait for 1 ns;

-- **************************************************
-- *** R = 1 ***
--------------------------

clk <= '1'; --	 Cycle No: 1374

wait for 1 ns;

I <= "0000";
CCEN_BAR <= '0';
CC_BAR <= '0';
RLD_BAR <= '0';
CI <= '0';
OEbar <= '0';
D <= "111111111111";

wait for 4 ns;

clk <= '0';

wait for 4 ns;

wait for 1 ns;

--------------------------

clk <= '1'; --	 Cycle No: 1375

wait for 1 ns;

I <= "0111";
CCEN_BAR <= '0';
CC_BAR <= '0';
RLD_BAR <= '1';
CI <= '0';
OEbar <= '0';

wait for 4 ns;

clk <= '0';

wait for 4 ns;

assert (PL_BAR = '0')
report
"Assert 834 : < PL_BAR /= 0 >" -- 	Vector No: 568
severity warning;

assert (VECT_BAR = '1')
report
"Assert 835 : < VECT_BAR /= 1 >"
severity warning;

assert (MAP_BAR = '1')
report
"Assert 836 : < MAP_BAR /= 1 >"
severity warning;

wait for 1 ns;

-- **************************************************
--------------------------

clk <= '1'; --	 Cycle No: 1376

wait for 1 ns;

I <= "0000";
CCEN_BAR <= '0';
CC_BAR <= '0';
RLD_BAR <= '0';
CI <= '0';
OEbar <= '0';
D <= "111111111111";

wait for 4 ns;

clk <= '0';

wait for 4 ns;

wait for 1 ns;

--------------------------

clk <= '1'; --	 Cycle No: 1377

wait for 1 ns;

I <= "0111";
CCEN_BAR <= '0';
CC_BAR <= '1';
RLD_BAR <= '1';
CI <= '0';
OEbar <= '0';

wait for 4 ns;

clk <= '0';

wait for 4 ns;

assert (PL_BAR = '0')
report
"Assert 837 : < PL_BAR /= 0 >" -- 	Vector No: 569
severity warning;

assert (VECT_BAR = '1')
report
"Assert 838 : < VECT_BAR /= 1 >"
severity warning;

assert (MAP_BAR = '1')
report
"Assert 839 : < MAP_BAR /= 1 >"
severity warning;

wait for 1 ns;

-- **************************************************
--------------------------

clk <= '1'; --	 Cycle No: 1378

wait for 1 ns;

I <= "0000";
CCEN_BAR <= '0';
CC_BAR <= '0';
RLD_BAR <= '0';
CI <= '0';
OEbar <= '0';
D <= "111111111111";

wait for 4 ns;

clk <= '0';

wait for 4 ns;

wait for 1 ns;

--------------------------

clk <= '1'; --	 Cycle No: 1379

wait for 1 ns;

I <= "0111";
CCEN_BAR <= '1';
CC_BAR <= '1';
RLD_BAR <= '1';
CI <= '0';
OEbar <= '0';

wait for 4 ns;

clk <= '0';

wait for 4 ns;

assert (PL_BAR = '0')
report
"Assert 840 : < PL_BAR /= 0 >" -- 	Vector No: 570
severity warning;

assert (VECT_BAR = '1')
report
"Assert 841 : < VECT_BAR /= 1 >"
severity warning;

assert (MAP_BAR = '1')
report
"Assert 842 : < MAP_BAR /= 1 >"
severity warning;

wait for 1 ns;

-- **************************************************
--------------------------

clk <= '1'; --	 Cycle No: 1380

wait for 1 ns;

I <= "0000";
CCEN_BAR <= '0';
CC_BAR <= '0';
RLD_BAR <= '0';
CI <= '0';
OEbar <= '0';
D <= "111111111111";

wait for 4 ns;

clk <= '0';

wait for 4 ns;

wait for 1 ns;

--------------------------

clk <= '1'; --	 Cycle No: 1381

wait for 1 ns;

I <= "0111";
CCEN_BAR <= '1';
CC_BAR <= '0';
RLD_BAR <= '1';
CI <= '0';
OEbar <= '0';

wait for 4 ns;

clk <= '0';

wait for 4 ns;

assert (PL_BAR = '0')
report
"Assert 843 : < PL_BAR /= 0 >" -- 	Vector No: 571
severity warning;

assert (VECT_BAR = '1')
report
"Assert 844 : < VECT_BAR /= 1 >"
severity warning;

assert (MAP_BAR = '1')
report
"Assert 845 : < MAP_BAR /= 1 >"
severity warning;

wait for 1 ns;

-- **************************************************
-- ****** I = 8 *******
-- *** R = 0 ***
--------------------------

clk <= '1'; --	 Cycle No: 1382

wait for 1 ns;

I <= "0000";
CCEN_BAR <= '0';
CC_BAR <= '0';
RLD_BAR <= '0';
CI <= '0';
OEbar <= '0';
D <= "000000000000";

wait for 4 ns;

clk <= '0';

wait for 4 ns;

wait for 1 ns;

--------------------------

clk <= '1'; --	 Cycle No: 1383

wait for 1 ns;

I <= "1000";
CCEN_BAR <= '0';
CC_BAR <= '0';
RLD_BAR <= '1';
CI <= '0';
OEbar <= '0';

wait for 4 ns;

clk <= '0';

wait for 4 ns;

assert (PL_BAR = '0')
report
"Assert 846 : < PL_BAR /= 0 >" -- 	Vector No: 572
severity warning;

assert (VECT_BAR = '1')
report
"Assert 847 : < VECT_BAR /= 1 >"
severity warning;

assert (MAP_BAR = '1')
report
"Assert 848 : < MAP_BAR /= 1 >"
severity warning;

wait for 1 ns;

-- **************************************************
--------------------------

clk <= '1'; --	 Cycle No: 1384

wait for 1 ns;

I <= "0000";
CCEN_BAR <= '0';
CC_BAR <= '0';
RLD_BAR <= '0';
CI <= '0';
OEbar <= '0';
D <= "000000000000";

wait for 4 ns;

clk <= '0';

wait for 4 ns;

wait for 1 ns;

--------------------------

clk <= '1'; --	 Cycle No: 1385

wait for 1 ns;

I <= "1000";
CCEN_BAR <= '0';
CC_BAR <= '1';
RLD_BAR <= '1';
CI <= '0';
OEbar <= '0';

wait for 4 ns;

clk <= '0';

wait for 4 ns;

assert (PL_BAR = '0')
report
"Assert 849 : < PL_BAR /= 0 >" -- 	Vector No: 573
severity warning;

assert (VECT_BAR = '1')
report
"Assert 850 : < VECT_BAR /= 1 >"
severity warning;

assert (MAP_BAR = '1')
report
"Assert 851 : < MAP_BAR /= 1 >"
severity warning;

wait for 1 ns;

-- **************************************************
--------------------------

clk <= '1'; --	 Cycle No: 1386

wait for 1 ns;

I <= "0000";
CCEN_BAR <= '0';
CC_BAR <= '0';
RLD_BAR <= '0';
CI <= '0';
OEbar <= '0';
D <= "000000000000";

wait for 4 ns;

clk <= '0';

wait for 4 ns;

wait for 1 ns;

--------------------------

clk <= '1'; --	 Cycle No: 1387

wait for 1 ns;

I <= "1000";
CCEN_BAR <= '1';
CC_BAR <= '1';
RLD_BAR <= '1';
CI <= '0';
OEbar <= '0';

wait for 4 ns;

clk <= '0';

wait for 4 ns;

assert (PL_BAR = '0')
report
"Assert 852 : < PL_BAR /= 0 >" -- 	Vector No: 574
severity warning;

assert (VECT_BAR = '1')
report
"Assert 853 : < VECT_BAR /= 1 >"
severity warning;

assert (MAP_BAR = '1')
report
"Assert 854 : < MAP_BAR /= 1 >"
severity warning;

wait for 1 ns;

-- **************************************************
--------------------------

clk <= '1'; --	 Cycle No: 1388

wait for 1 ns;

I <= "0000";
CCEN_BAR <= '0';
CC_BAR <= '0';
RLD_BAR <= '0';
CI <= '0';
OEbar <= '0';
D <= "000000000000";

wait for 4 ns;

clk <= '0';

wait for 4 ns;

wait for 1 ns;

--------------------------

clk <= '1'; --	 Cycle No: 1389

wait for 1 ns;

I <= "1000";
CCEN_BAR <= '1';
CC_BAR <= '0';
RLD_BAR <= '1';
CI <= '0';
OEbar <= '0';

wait for 4 ns;

clk <= '0';

wait for 4 ns;

assert (PL_BAR = '0')
report
"Assert 855 : < PL_BAR /= 0 >" -- 	Vector No: 575
severity warning;

assert (VECT_BAR = '1')
report
"Assert 856 : < VECT_BAR /= 1 >"
severity warning;

assert (MAP_BAR = '1')
report
"Assert 857 : < MAP_BAR /= 1 >"
severity warning;

wait for 1 ns;

-- **************************************************
-- *** R = 1 ***
--------------------------

clk <= '1'; --	 Cycle No: 1390

wait for 1 ns;

I <= "0000";
CCEN_BAR <= '0';
CC_BAR <= '0';
RLD_BAR <= '0';
CI <= '0';
OEbar <= '0';
D <= "111111111111";

wait for 4 ns;

clk <= '0';

wait for 4 ns;

wait for 1 ns;

--------------------------

clk <= '1'; --	 Cycle No: 1391

wait for 1 ns;

I <= "1000";
CCEN_BAR <= '0';
CC_BAR <= '0';
RLD_BAR <= '1';
CI <= '0';
OEbar <= '0';

wait for 4 ns;

clk <= '0';

wait for 4 ns;

assert (PL_BAR = '0')
report
"Assert 858 : < PL_BAR /= 0 >" -- 	Vector No: 576
severity warning;

assert (VECT_BAR = '1')
report
"Assert 859 : < VECT_BAR /= 1 >"
severity warning;

assert (MAP_BAR = '1')
report
"Assert 860 : < MAP_BAR /= 1 >"
severity warning;

wait for 1 ns;

-- **************************************************
--------------------------

clk <= '1'; --	 Cycle No: 1392

wait for 1 ns;

I <= "0000";
CCEN_BAR <= '0';
CC_BAR <= '0';
RLD_BAR <= '0';
CI <= '0';
OEbar <= '0';
D <= "111111111111";

wait for 4 ns;

clk <= '0';

wait for 4 ns;

wait for 1 ns;

--------------------------

clk <= '1'; --	 Cycle No: 1393

wait for 1 ns;

I <= "1000";
CCEN_BAR <= '0';
CC_BAR <= '1';
RLD_BAR <= '1';
CI <= '0';
OEbar <= '0';

wait for 4 ns;

clk <= '0';

wait for 4 ns;

assert (PL_BAR = '0')
report
"Assert 861 : < PL_BAR /= 0 >" -- 	Vector No: 577
severity warning;

assert (VECT_BAR = '1')
report
"Assert 862 : < VECT_BAR /= 1 >"
severity warning;

assert (MAP_BAR = '1')
report
"Assert 863 : < MAP_BAR /= 1 >"
severity warning;

wait for 1 ns;

-- **************************************************
--------------------------

clk <= '1'; --	 Cycle No: 1394

wait for 1 ns;

I <= "0000";
CCEN_BAR <= '0';
CC_BAR <= '0';
RLD_BAR <= '0';
CI <= '0';
OEbar <= '0';
D <= "111111111111";

wait for 4 ns;

clk <= '0';

wait for 4 ns;

wait for 1 ns;

--------------------------

clk <= '1'; --	 Cycle No: 1395

wait for 1 ns;

I <= "1000";
CCEN_BAR <= '1';
CC_BAR <= '1';
RLD_BAR <= '1';
CI <= '0';
OEbar <= '0';

wait for 4 ns;

clk <= '0';

wait for 4 ns;

assert (PL_BAR = '0')
report
"Assert 864 : < PL_BAR /= 0 >" -- 	Vector No: 578
severity warning;

assert (VECT_BAR = '1')
report
"Assert 865 : < VECT_BAR /= 1 >"
severity warning;

assert (MAP_BAR = '1')
report
"Assert 866 : < MAP_BAR /= 1 >"
severity warning;

wait for 1 ns;

-- **************************************************
--------------------------

clk <= '1'; --	 Cycle No: 1396

wait for 1 ns;

I <= "0000";
CCEN_BAR <= '0';
CC_BAR <= '0';
RLD_BAR <= '0';
CI <= '0';
OEbar <= '0';
D <= "111111111111";

wait for 4 ns;

clk <= '0';

wait for 4 ns;

wait for 1 ns;

--------------------------

clk <= '1'; --	 Cycle No: 1397

wait for 1 ns;

I <= "1000";
CCEN_BAR <= '1';
CC_BAR <= '0';
RLD_BAR <= '1';
CI <= '0';
OEbar <= '0';

wait for 4 ns;

clk <= '0';

wait for 4 ns;

assert (PL_BAR = '0')
report
"Assert 867 : < PL_BAR /= 0 >" -- 	Vector No: 579
severity warning;

assert (VECT_BAR = '1')
report
"Assert 868 : < VECT_BAR /= 1 >"
severity warning;

assert (MAP_BAR = '1')
report
"Assert 869 : < MAP_BAR /= 1 >"
severity warning;

wait for 1 ns;

-- **************************************************
-- ****** I = 9 *******
-- *** R = 0 ***
--------------------------

clk <= '1'; --	 Cycle No: 1398

wait for 1 ns;

I <= "0000";
CCEN_BAR <= '0';
CC_BAR <= '0';
RLD_BAR <= '0';
CI <= '0';
OEbar <= '0';
D <= "000000000000";

wait for 4 ns;

clk <= '0';

wait for 4 ns;

wait for 1 ns;

--------------------------

clk <= '1'; --	 Cycle No: 1399

wait for 1 ns;

I <= "1001";
CCEN_BAR <= '0';
CC_BAR <= '0';
RLD_BAR <= '1';
CI <= '0';
OEbar <= '0';

wait for 4 ns;

clk <= '0';

wait for 4 ns;

assert (PL_BAR = '0')
report
"Assert 870 : < PL_BAR /= 0 >" -- 	Vector No: 580
severity warning;

assert (VECT_BAR = '1')
report
"Assert 871 : < VECT_BAR /= 1 >"
severity warning;

assert (MAP_BAR = '1')
report
"Assert 872 : < MAP_BAR /= 1 >"
severity warning;

wait for 1 ns;

-- **************************************************
--------------------------

clk <= '1'; --	 Cycle No: 1400

wait for 1 ns;

I <= "0000";
CCEN_BAR <= '0';
CC_BAR <= '0';
RLD_BAR <= '0';
CI <= '0';
OEbar <= '0';
D <= "000000000000";

wait for 4 ns;

clk <= '0';

wait for 4 ns;

wait for 1 ns;

--------------------------

clk <= '1'; --	 Cycle No: 1401

wait for 1 ns;

I <= "1001";
CCEN_BAR <= '0';
CC_BAR <= '1';
RLD_BAR <= '1';
CI <= '0';
OEbar <= '0';

wait for 4 ns;

clk <= '0';

wait for 4 ns;

assert (PL_BAR = '0')
report
"Assert 873 : < PL_BAR /= 0 >" -- 	Vector No: 581
severity warning;

assert (VECT_BAR = '1')
report
"Assert 874 : < VECT_BAR /= 1 >"
severity warning;

assert (MAP_BAR = '1')
report
"Assert 875 : < MAP_BAR /= 1 >"
severity warning;

wait for 1 ns;

-- **************************************************
--------------------------

clk <= '1'; --	 Cycle No: 1402

wait for 1 ns;

I <= "0000";
CCEN_BAR <= '0';
CC_BAR <= '0';
RLD_BAR <= '0';
CI <= '0';
OEbar <= '0';
D <= "000000000000";

wait for 4 ns;

clk <= '0';

wait for 4 ns;

wait for 1 ns;

--------------------------

clk <= '1'; --	 Cycle No: 1403

wait for 1 ns;

I <= "1001";
CCEN_BAR <= '1';
CC_BAR <= '1';
RLD_BAR <= '1';
CI <= '0';
OEbar <= '0';

wait for 4 ns;

clk <= '0';

wait for 4 ns;

assert (PL_BAR = '0')
report
"Assert 876 : < PL_BAR /= 0 >" -- 	Vector No: 582
severity warning;

assert (VECT_BAR = '1')
report
"Assert 877 : < VECT_BAR /= 1 >"
severity warning;

assert (MAP_BAR = '1')
report
"Assert 878 : < MAP_BAR /= 1 >"
severity warning;

wait for 1 ns;

-- **************************************************
--------------------------

clk <= '1'; --	 Cycle No: 1404

wait for 1 ns;

I <= "0000";
CCEN_BAR <= '0';
CC_BAR <= '0';
RLD_BAR <= '0';
CI <= '0';
OEbar <= '0';
D <= "000000000000";

wait for 4 ns;

clk <= '0';

wait for 4 ns;

wait for 1 ns;

--------------------------

clk <= '1'; --	 Cycle No: 1405

wait for 1 ns;

I <= "1001";
CCEN_BAR <= '1';
CC_BAR <= '0';
RLD_BAR <= '1';
CI <= '0';
OEbar <= '0';

wait for 4 ns;

clk <= '0';

wait for 4 ns;

assert (PL_BAR = '0')
report
"Assert 879 : < PL_BAR /= 0 >" -- 	Vector No: 583
severity warning;

assert (VECT_BAR = '1')
report
"Assert 880 : < VECT_BAR /= 1 >"
severity warning;

assert (MAP_BAR = '1')
report
"Assert 881 : < MAP_BAR /= 1 >"
severity warning;

wait for 1 ns;

-- **************************************************
-- *** R = 1 ***
--------------------------

clk <= '1'; --	 Cycle No: 1406

wait for 1 ns;

I <= "0000";
CCEN_BAR <= '0';
CC_BAR <= '0';
RLD_BAR <= '0';
CI <= '0';
OEbar <= '0';
D <= "111111111111";

wait for 4 ns;

clk <= '0';

wait for 4 ns;

wait for 1 ns;

--------------------------

clk <= '1'; --	 Cycle No: 1407

wait for 1 ns;

I <= "1001";
CCEN_BAR <= '0';
CC_BAR <= '0';
RLD_BAR <= '1';
CI <= '0';
OEbar <= '0';

wait for 4 ns;

clk <= '0';

wait for 4 ns;

assert (PL_BAR = '0')
report
"Assert 882 : < PL_BAR /= 0 >" -- 	Vector No: 584
severity warning;

assert (VECT_BAR = '1')
report
"Assert 883 : < VECT_BAR /= 1 >"
severity warning;

assert (MAP_BAR = '1')
report
"Assert 884 : < MAP_BAR /= 1 >"
severity warning;

wait for 1 ns;

-- **************************************************
--------------------------

clk <= '1'; --	 Cycle No: 1408

wait for 1 ns;

I <= "0000";
CCEN_BAR <= '0';
CC_BAR <= '0';
RLD_BAR <= '0';
CI <= '0';
OEbar <= '0';
D <= "111111111111";

wait for 4 ns;

clk <= '0';

wait for 4 ns;

wait for 1 ns;

--------------------------

clk <= '1'; --	 Cycle No: 1409

wait for 1 ns;

I <= "1001";
CCEN_BAR <= '0';
CC_BAR <= '1';
RLD_BAR <= '1';
CI <= '0';
OEbar <= '0';

wait for 4 ns;

clk <= '0';

wait for 4 ns;

assert (PL_BAR = '0')
report
"Assert 885 : < PL_BAR /= 0 >" -- 	Vector No: 585
severity warning;

assert (VECT_BAR = '1')
report
"Assert 886 : < VECT_BAR /= 1 >"
severity warning;

assert (MAP_BAR = '1')
report
"Assert 887 : < MAP_BAR /= 1 >"
severity warning;

wait for 1 ns;

-- **************************************************
--------------------------

clk <= '1'; --	 Cycle No: 1410

wait for 1 ns;

I <= "0000";
CCEN_BAR <= '0';
CC_BAR <= '0';
RLD_BAR <= '0';
CI <= '0';
OEbar <= '0';
D <= "111111111111";

wait for 4 ns;

clk <= '0';

wait for 4 ns;

wait for 1 ns;

--------------------------

clk <= '1'; --	 Cycle No: 1411

wait for 1 ns;

I <= "1001";
CCEN_BAR <= '1';
CC_BAR <= '1';
RLD_BAR <= '1';
CI <= '0';
OEbar <= '0';

wait for 4 ns;

clk <= '0';

wait for 4 ns;

assert (PL_BAR = '0')
report
"Assert 888 : < PL_BAR /= 0 >" -- 	Vector No: 586
severity warning;

assert (VECT_BAR = '1')
report
"Assert 889 : < VECT_BAR /= 1 >"
severity warning;

assert (MAP_BAR = '1')
report
"Assert 890 : < MAP_BAR /= 1 >"
severity warning;

wait for 1 ns;

-- **************************************************
--------------------------

clk <= '1'; --	 Cycle No: 1412

wait for 1 ns;

I <= "0000";
CCEN_BAR <= '0';
CC_BAR <= '0';
RLD_BAR <= '0';
CI <= '0';
OEbar <= '0';
D <= "111111111111";

wait for 4 ns;

clk <= '0';

wait for 4 ns;

wait for 1 ns;

--------------------------

clk <= '1'; --	 Cycle No: 1413

wait for 1 ns;

I <= "1001";
CCEN_BAR <= '1';
CC_BAR <= '0';
RLD_BAR <= '1';
CI <= '0';
OEbar <= '0';

wait for 4 ns;

clk <= '0';

wait for 4 ns;

assert (PL_BAR = '0')
report
"Assert 891 : < PL_BAR /= 0 >" -- 	Vector No: 587
severity warning;

assert (VECT_BAR = '1')
report
"Assert 892 : < VECT_BAR /= 1 >"
severity warning;

assert (MAP_BAR = '1')
report
"Assert 893 : < MAP_BAR /= 1 >"
severity warning;

wait for 1 ns;

-- **************************************************
-- ****** I = 10 *******
-- *** R = 0 ***
--------------------------

clk <= '1'; --	 Cycle No: 1414

wait for 1 ns;

I <= "0000";
CCEN_BAR <= '0';
CC_BAR <= '0';
RLD_BAR <= '0';
CI <= '0';
OEbar <= '0';
D <= "000000000000";

wait for 4 ns;

clk <= '0';

wait for 4 ns;

wait for 1 ns;

--------------------------

clk <= '1'; --	 Cycle No: 1415

wait for 1 ns;

I <= "1010";
CCEN_BAR <= '0';
CC_BAR <= '0';
RLD_BAR <= '1';
CI <= '0';
OEbar <= '0';

wait for 4 ns;

clk <= '0';

wait for 4 ns;

assert (PL_BAR = '0')
report
"Assert 894 : < PL_BAR /= 0 >" -- 	Vector No: 588
severity warning;

assert (VECT_BAR = '1')
report
"Assert 895 : < VECT_BAR /= 1 >"
severity warning;

assert (MAP_BAR = '1')
report
"Assert 896 : < MAP_BAR /= 1 >"
severity warning;

wait for 1 ns;

-- **************************************************
--------------------------

clk <= '1'; --	 Cycle No: 1416

wait for 1 ns;

I <= "0000";
CCEN_BAR <= '0';
CC_BAR <= '0';
RLD_BAR <= '0';
CI <= '0';
OEbar <= '0';
D <= "000000000000";

wait for 4 ns;

clk <= '0';

wait for 4 ns;

wait for 1 ns;

--------------------------

clk <= '1'; --	 Cycle No: 1417

wait for 1 ns;

I <= "1010";
CCEN_BAR <= '0';
CC_BAR <= '1';
RLD_BAR <= '1';
CI <= '0';
OEbar <= '0';

wait for 4 ns;

clk <= '0';

wait for 4 ns;

assert (PL_BAR = '0')
report
"Assert 897 : < PL_BAR /= 0 >" -- 	Vector No: 589
severity warning;

assert (VECT_BAR = '1')
report
"Assert 898 : < VECT_BAR /= 1 >"
severity warning;

assert (MAP_BAR = '1')
report
"Assert 899 : < MAP_BAR /= 1 >"
severity warning;

wait for 1 ns;

-- **************************************************
--------------------------

clk <= '1'; --	 Cycle No: 1418

wait for 1 ns;

I <= "0000";
CCEN_BAR <= '0';
CC_BAR <= '0';
RLD_BAR <= '0';
CI <= '0';
OEbar <= '0';
D <= "000000000000";

wait for 4 ns;

clk <= '0';

wait for 4 ns;

wait for 1 ns;

--------------------------

clk <= '1'; --	 Cycle No: 1419

wait for 1 ns;

I <= "1010";
CCEN_BAR <= '1';
CC_BAR <= '1';
RLD_BAR <= '1';
CI <= '0';
OEbar <= '0';

wait for 4 ns;

clk <= '0';

wait for 4 ns;

assert (PL_BAR = '0')
report
"Assert 900 : < PL_BAR /= 0 >" -- 	Vector No: 590
severity warning;

assert (VECT_BAR = '1')
report
"Assert 901 : < VECT_BAR /= 1 >"
severity warning;

assert (MAP_BAR = '1')
report
"Assert 902 : < MAP_BAR /= 1 >"
severity warning;

wait for 1 ns;

-- **************************************************
--------------------------

clk <= '1'; --	 Cycle No: 1420

wait for 1 ns;

I <= "0000";
CCEN_BAR <= '0';
CC_BAR <= '0';
RLD_BAR <= '0';
CI <= '0';
OEbar <= '0';
D <= "000000000000";

wait for 4 ns;

clk <= '0';

wait for 4 ns;

wait for 1 ns;

--------------------------

clk <= '1'; --	 Cycle No: 1421

wait for 1 ns;

I <= "1010";
CCEN_BAR <= '1';
CC_BAR <= '0';
RLD_BAR <= '1';
CI <= '0';
OEbar <= '0';

wait for 4 ns;

clk <= '0';

wait for 4 ns;

assert (PL_BAR = '0')
report
"Assert 903 : < PL_BAR /= 0 >" -- 	Vector No: 591
severity warning;

assert (VECT_BAR = '1')
report
"Assert 904 : < VECT_BAR /= 1 >"
severity warning;

assert (MAP_BAR = '1')
report
"Assert 905 : < MAP_BAR /= 1 >"
severity warning;

wait for 1 ns;

-- **************************************************
-- *** R = 1 ***
--------------------------

clk <= '1'; --	 Cycle No: 1422

wait for 1 ns;

I <= "0000";
CCEN_BAR <= '0';
CC_BAR <= '0';
RLD_BAR <= '0';
CI <= '0';
OEbar <= '0';
D <= "111111111111";

wait for 4 ns;

clk <= '0';

wait for 4 ns;

wait for 1 ns;

--------------------------

clk <= '1'; --	 Cycle No: 1423

wait for 1 ns;

I <= "1010";
CCEN_BAR <= '0';
CC_BAR <= '0';
RLD_BAR <= '1';
CI <= '0';
OEbar <= '0';

wait for 4 ns;

clk <= '0';

wait for 4 ns;

assert (PL_BAR = '0')
report
"Assert 906 : < PL_BAR /= 0 >" -- 	Vector No: 592
severity warning;

assert (VECT_BAR = '1')
report
"Assert 907 : < VECT_BAR /= 1 >"
severity warning;

assert (MAP_BAR = '1')
report
"Assert 908 : < MAP_BAR /= 1 >"
severity warning;

wait for 1 ns;

-- **************************************************
--------------------------

clk <= '1'; --	 Cycle No: 1424

wait for 1 ns;

I <= "0000";
CCEN_BAR <= '0';
CC_BAR <= '0';
RLD_BAR <= '0';
CI <= '0';
OEbar <= '0';
D <= "111111111111";

wait for 4 ns;

clk <= '0';

wait for 4 ns;

wait for 1 ns;

--------------------------

clk <= '1'; --	 Cycle No: 1425

wait for 1 ns;

I <= "1010";
CCEN_BAR <= '0';
CC_BAR <= '1';
RLD_BAR <= '1';
CI <= '0';
OEbar <= '0';

wait for 4 ns;

clk <= '0';

wait for 4 ns;

assert (PL_BAR = '0')
report
"Assert 909 : < PL_BAR /= 0 >" -- 	Vector No: 593
severity warning;

assert (VECT_BAR = '1')
report
"Assert 910 : < VECT_BAR /= 1 >"
severity warning;

assert (MAP_BAR = '1')
report
"Assert 911 : < MAP_BAR /= 1 >"
severity warning;

wait for 1 ns;

-- **************************************************
--------------------------

clk <= '1'; --	 Cycle No: 1426

wait for 1 ns;

I <= "0000";
CCEN_BAR <= '0';
CC_BAR <= '0';
RLD_BAR <= '0';
CI <= '0';
OEbar <= '0';
D <= "111111111111";

wait for 4 ns;

clk <= '0';

wait for 4 ns;

wait for 1 ns;

--------------------------

clk <= '1'; --	 Cycle No: 1427

wait for 1 ns;

I <= "1010";
CCEN_BAR <= '1';
CC_BAR <= '1';
RLD_BAR <= '1';
CI <= '0';
OEbar <= '0';

wait for 4 ns;

clk <= '0';

wait for 4 ns;

assert (PL_BAR = '0')
report
"Assert 912 : < PL_BAR /= 0 >" -- 	Vector No: 594
severity warning;

assert (VECT_BAR = '1')
report
"Assert 913 : < VECT_BAR /= 1 >"
severity warning;

assert (MAP_BAR = '1')
report
"Assert 914 : < MAP_BAR /= 1 >"
severity warning;

wait for 1 ns;

-- **************************************************
--------------------------

clk <= '1'; --	 Cycle No: 1428

wait for 1 ns;

I <= "0000";
CCEN_BAR <= '0';
CC_BAR <= '0';
RLD_BAR <= '0';
CI <= '0';
OEbar <= '0';
D <= "111111111111";

wait for 4 ns;

clk <= '0';

wait for 4 ns;

wait for 1 ns;

--------------------------

clk <= '1'; --	 Cycle No: 1429

wait for 1 ns;

I <= "1010";
CCEN_BAR <= '1';
CC_BAR <= '0';
RLD_BAR <= '1';
CI <= '0';
OEbar <= '0';

wait for 4 ns;

clk <= '0';

wait for 4 ns;

assert (PL_BAR = '0')
report
"Assert 915 : < PL_BAR /= 0 >" -- 	Vector No: 595
severity warning;

assert (VECT_BAR = '1')
report
"Assert 916 : < VECT_BAR /= 1 >"
severity warning;

assert (MAP_BAR = '1')
report
"Assert 917 : < MAP_BAR /= 1 >"
severity warning;

wait for 1 ns;

-- **************************************************
-- ****** I = 11 *******
-- *** R = 0 ***
--------------------------

clk <= '1'; --	 Cycle No: 1430

wait for 1 ns;

I <= "0000";
CCEN_BAR <= '0';
CC_BAR <= '0';
RLD_BAR <= '0';
CI <= '0';
OEbar <= '0';
D <= "000000000000";

wait for 4 ns;

clk <= '0';

wait for 4 ns;

wait for 1 ns;

--------------------------

clk <= '1'; --	 Cycle No: 1431

wait for 1 ns;

I <= "1011";
CCEN_BAR <= '0';
CC_BAR <= '0';
RLD_BAR <= '1';
CI <= '0';
OEbar <= '0';

wait for 4 ns;

clk <= '0';

wait for 4 ns;

assert (PL_BAR = '0')
report
"Assert 918 : < PL_BAR /= 0 >" -- 	Vector No: 596
severity warning;

assert (VECT_BAR = '1')
report
"Assert 919 : < VECT_BAR /= 1 >"
severity warning;

assert (MAP_BAR = '1')
report
"Assert 920 : < MAP_BAR /= 1 >"
severity warning;

wait for 1 ns;

-- **************************************************
--------------------------

clk <= '1'; --	 Cycle No: 1432

wait for 1 ns;

I <= "0000";
CCEN_BAR <= '0';
CC_BAR <= '0';
RLD_BAR <= '0';
CI <= '0';
OEbar <= '0';
D <= "000000000000";

wait for 4 ns;

clk <= '0';

wait for 4 ns;

wait for 1 ns;

--------------------------

clk <= '1'; --	 Cycle No: 1433

wait for 1 ns;

I <= "1011";
CCEN_BAR <= '0';
CC_BAR <= '1';
RLD_BAR <= '1';
CI <= '0';
OEbar <= '0';

wait for 4 ns;

clk <= '0';

wait for 4 ns;

assert (PL_BAR = '0')
report
"Assert 921 : < PL_BAR /= 0 >" -- 	Vector No: 597
severity warning;

assert (VECT_BAR = '1')
report
"Assert 922 : < VECT_BAR /= 1 >"
severity warning;

assert (MAP_BAR = '1')
report
"Assert 923 : < MAP_BAR /= 1 >"
severity warning;

wait for 1 ns;

-- **************************************************
--------------------------

clk <= '1'; --	 Cycle No: 1434

wait for 1 ns;

I <= "0000";
CCEN_BAR <= '0';
CC_BAR <= '0';
RLD_BAR <= '0';
CI <= '0';
OEbar <= '0';
D <= "000000000000";

wait for 4 ns;

clk <= '0';

wait for 4 ns;

wait for 1 ns;

--------------------------

clk <= '1'; --	 Cycle No: 1435

wait for 1 ns;

I <= "1011";
CCEN_BAR <= '1';
CC_BAR <= '1';
RLD_BAR <= '1';
CI <= '0';
OEbar <= '0';

wait for 4 ns;

clk <= '0';

wait for 4 ns;

assert (PL_BAR = '0')
report
"Assert 924 : < PL_BAR /= 0 >" -- 	Vector No: 598
severity warning;

assert (VECT_BAR = '1')
report
"Assert 925 : < VECT_BAR /= 1 >"
severity warning;

assert (MAP_BAR = '1')
report
"Assert 926 : < MAP_BAR /= 1 >"
severity warning;

wait for 1 ns;

-- **************************************************
--------------------------

clk <= '1'; --	 Cycle No: 1436

wait for 1 ns;

I <= "0000";
CCEN_BAR <= '0';
CC_BAR <= '0';
RLD_BAR <= '0';
CI <= '0';
OEbar <= '0';
D <= "000000000000";

wait for 4 ns;

clk <= '0';

wait for 4 ns;

wait for 1 ns;

--------------------------

clk <= '1'; --	 Cycle No: 1437

wait for 1 ns;

I <= "1011";
CCEN_BAR <= '1';
CC_BAR <= '0';
RLD_BAR <= '1';
CI <= '0';
OEbar <= '0';

wait for 4 ns;

clk <= '0';

wait for 4 ns;

assert (PL_BAR = '0')
report
"Assert 927 : < PL_BAR /= 0 >" -- 	Vector No: 599
severity warning;

assert (VECT_BAR = '1')
report
"Assert 928 : < VECT_BAR /= 1 >"
severity warning;

assert (MAP_BAR = '1')
report
"Assert 929 : < MAP_BAR /= 1 >"
severity warning;

wait for 1 ns;

-- **************************************************
-- *** R = 1 ***
--------------------------

clk <= '1'; --	 Cycle No: 1438

wait for 1 ns;

I <= "0000";
CCEN_BAR <= '0';
CC_BAR <= '0';
RLD_BAR <= '0';
CI <= '0';
OEbar <= '0';
D <= "111111111111";

wait for 4 ns;

clk <= '0';

wait for 4 ns;

wait for 1 ns;

--------------------------

clk <= '1'; --	 Cycle No: 1439

wait for 1 ns;

I <= "1011";
CCEN_BAR <= '0';
CC_BAR <= '0';
RLD_BAR <= '1';
CI <= '0';
OEbar <= '0';

wait for 4 ns;

clk <= '0';

wait for 4 ns;

assert (PL_BAR = '0')
report
"Assert 930 : < PL_BAR /= 0 >" -- 	Vector No: 600
severity warning;

assert (VECT_BAR = '1')
report
"Assert 931 : < VECT_BAR /= 1 >"
severity warning;

assert (MAP_BAR = '1')
report
"Assert 932 : < MAP_BAR /= 1 >"
severity warning;

wait for 1 ns;

-- **************************************************
--------------------------

clk <= '1'; --	 Cycle No: 1440

wait for 1 ns;

I <= "0000";
CCEN_BAR <= '0';
CC_BAR <= '0';
RLD_BAR <= '0';
CI <= '0';
OEbar <= '0';
D <= "111111111111";

wait for 4 ns;

clk <= '0';

wait for 4 ns;

wait for 1 ns;

--------------------------

clk <= '1'; --	 Cycle No: 1441

wait for 1 ns;

I <= "1011";
CCEN_BAR <= '0';
CC_BAR <= '1';
RLD_BAR <= '1';
CI <= '0';
OEbar <= '0';

wait for 4 ns;

clk <= '0';

wait for 4 ns;

assert (PL_BAR = '0')
report
"Assert 933 : < PL_BAR /= 0 >" -- 	Vector No: 601
severity warning;

assert (VECT_BAR = '1')
report
"Assert 934 : < VECT_BAR /= 1 >"
severity warning;

assert (MAP_BAR = '1')
report
"Assert 935 : < MAP_BAR /= 1 >"
severity warning;

wait for 1 ns;

-- **************************************************
--------------------------

clk <= '1'; --	 Cycle No: 1442

wait for 1 ns;

I <= "0000";
CCEN_BAR <= '0';
CC_BAR <= '0';
RLD_BAR <= '0';
CI <= '0';
OEbar <= '0';
D <= "111111111111";

wait for 4 ns;

clk <= '0';

wait for 4 ns;

wait for 1 ns;

--------------------------

clk <= '1'; --	 Cycle No: 1443

wait for 1 ns;

I <= "1011";
CCEN_BAR <= '1';
CC_BAR <= '1';
RLD_BAR <= '1';
CI <= '0';
OEbar <= '0';

wait for 4 ns;

clk <= '0';

wait for 4 ns;

assert (PL_BAR = '0')
report
"Assert 936 : < PL_BAR /= 0 >" -- 	Vector No: 602
severity warning;

assert (VECT_BAR = '1')
report
"Assert 937 : < VECT_BAR /= 1 >"
severity warning;

assert (MAP_BAR = '1')
report
"Assert 938 : < MAP_BAR /= 1 >"
severity warning;

wait for 1 ns;

-- **************************************************
--------------------------

clk <= '1'; --	 Cycle No: 1444

wait for 1 ns;

I <= "0000";
CCEN_BAR <= '0';
CC_BAR <= '0';
RLD_BAR <= '0';
CI <= '0';
OEbar <= '0';
D <= "111111111111";

wait for 4 ns;

clk <= '0';

wait for 4 ns;

wait for 1 ns;

--------------------------

clk <= '1'; --	 Cycle No: 1445

wait for 1 ns;

I <= "1011";
CCEN_BAR <= '1';
CC_BAR <= '0';
RLD_BAR <= '1';
CI <= '0';
OEbar <= '0';

wait for 4 ns;

clk <= '0';

wait for 4 ns;

assert (PL_BAR = '0')
report
"Assert 939 : < PL_BAR /= 0 >" -- 	Vector No: 603
severity warning;

assert (VECT_BAR = '1')
report
"Assert 940 : < VECT_BAR /= 1 >"
severity warning;

assert (MAP_BAR = '1')
report
"Assert 941 : < MAP_BAR /= 1 >"
severity warning;

wait for 1 ns;

-- **************************************************
-- ****** I = 12 *******
-- *** R = 0 ***
--------------------------

clk <= '1'; --	 Cycle No: 1446

wait for 1 ns;

I <= "0000";
CCEN_BAR <= '0';
CC_BAR <= '0';
RLD_BAR <= '0';
CI <= '0';
OEbar <= '0';
D <= "000000000000";

wait for 4 ns;

clk <= '0';

wait for 4 ns;

wait for 1 ns;

--------------------------

clk <= '1'; --	 Cycle No: 1447

wait for 1 ns;

I <= "1100";
CCEN_BAR <= '0';
CC_BAR <= '0';
RLD_BAR <= '1';
CI <= '0';
OEbar <= '0';

wait for 4 ns;

clk <= '0';

wait for 4 ns;

assert (PL_BAR = '0')
report
"Assert 942 : < PL_BAR /= 0 >" -- 	Vector No: 604
severity warning;

assert (VECT_BAR = '1')
report
"Assert 943 : < VECT_BAR /= 1 >"
severity warning;

assert (MAP_BAR = '1')
report
"Assert 944 : < MAP_BAR /= 1 >"
severity warning;

wait for 1 ns;

-- **************************************************
--------------------------

clk <= '1'; --	 Cycle No: 1448

wait for 1 ns;

I <= "0000";
CCEN_BAR <= '0';
CC_BAR <= '0';
RLD_BAR <= '0';
CI <= '0';
OEbar <= '0';
D <= "000000000000";

wait for 4 ns;

clk <= '0';

wait for 4 ns;

wait for 1 ns;

--------------------------

clk <= '1'; --	 Cycle No: 1449

wait for 1 ns;

I <= "1100";
CCEN_BAR <= '0';
CC_BAR <= '1';
RLD_BAR <= '1';
CI <= '0';
OEbar <= '0';

wait for 4 ns;

clk <= '0';

wait for 4 ns;

assert (PL_BAR = '0')
report
"Assert 945 : < PL_BAR /= 0 >" -- 	Vector No: 605
severity warning;

assert (VECT_BAR = '1')
report
"Assert 946 : < VECT_BAR /= 1 >"
severity warning;

assert (MAP_BAR = '1')
report
"Assert 947 : < MAP_BAR /= 1 >"
severity warning;

wait for 1 ns;

-- **************************************************
--------------------------

clk <= '1'; --	 Cycle No: 1450

wait for 1 ns;

I <= "0000";
CCEN_BAR <= '0';
CC_BAR <= '0';
RLD_BAR <= '0';
CI <= '0';
OEbar <= '0';
D <= "000000000000";

wait for 4 ns;

clk <= '0';

wait for 4 ns;

wait for 1 ns;

--------------------------

clk <= '1'; --	 Cycle No: 1451

wait for 1 ns;

I <= "1100";
CCEN_BAR <= '1';
CC_BAR <= '1';
RLD_BAR <= '1';
CI <= '0';
OEbar <= '0';

wait for 4 ns;

clk <= '0';

wait for 4 ns;

assert (PL_BAR = '0')
report
"Assert 948 : < PL_BAR /= 0 >" -- 	Vector No: 606
severity warning;

assert (VECT_BAR = '1')
report
"Assert 949 : < VECT_BAR /= 1 >"
severity warning;

assert (MAP_BAR = '1')
report
"Assert 950 : < MAP_BAR /= 1 >"
severity warning;

wait for 1 ns;

-- **************************************************
--------------------------

clk <= '1'; --	 Cycle No: 1452

wait for 1 ns;

I <= "0000";
CCEN_BAR <= '0';
CC_BAR <= '0';
RLD_BAR <= '0';
CI <= '0';
OEbar <= '0';
D <= "000000000000";

wait for 4 ns;

clk <= '0';

wait for 4 ns;

wait for 1 ns;

--------------------------

clk <= '1'; --	 Cycle No: 1453

wait for 1 ns;

I <= "1100";
CCEN_BAR <= '1';
CC_BAR <= '0';
RLD_BAR <= '1';
CI <= '0';
OEbar <= '0';

wait for 4 ns;

clk <= '0';

wait for 4 ns;

assert (PL_BAR = '0')
report
"Assert 951 : < PL_BAR /= 0 >" -- 	Vector No: 607
severity warning;

assert (VECT_BAR = '1')
report
"Assert 952 : < VECT_BAR /= 1 >"
severity warning;

assert (MAP_BAR = '1')
report
"Assert 953 : < MAP_BAR /= 1 >"
severity warning;

wait for 1 ns;

-- **************************************************
-- *** R = 1 ***
--------------------------

clk <= '1'; --	 Cycle No: 1454

wait for 1 ns;

I <= "0000";
CCEN_BAR <= '0';
CC_BAR <= '0';
RLD_BAR <= '0';
CI <= '0';
OEbar <= '0';
D <= "111111111111";

wait for 4 ns;

clk <= '0';

wait for 4 ns;

wait for 1 ns;

--------------------------

clk <= '1'; --	 Cycle No: 1455

wait for 1 ns;

I <= "1100";
CCEN_BAR <= '0';
CC_BAR <= '0';
RLD_BAR <= '1';
CI <= '0';
OEbar <= '0';

wait for 4 ns;

clk <= '0';

wait for 4 ns;

assert (PL_BAR = '0')
report
"Assert 954 : < PL_BAR /= 0 >" -- 	Vector No: 608
severity warning;

assert (VECT_BAR = '1')
report
"Assert 955 : < VECT_BAR /= 1 >"
severity warning;

assert (MAP_BAR = '1')
report
"Assert 956 : < MAP_BAR /= 1 >"
severity warning;

wait for 1 ns;

-- **************************************************
--------------------------

clk <= '1'; --	 Cycle No: 1456

wait for 1 ns;

I <= "0000";
CCEN_BAR <= '0';
CC_BAR <= '0';
RLD_BAR <= '0';
CI <= '0';
OEbar <= '0';
D <= "111111111111";

wait for 4 ns;

clk <= '0';

wait for 4 ns;

wait for 1 ns;

--------------------------

clk <= '1'; --	 Cycle No: 1457

wait for 1 ns;

I <= "1100";
CCEN_BAR <= '0';
CC_BAR <= '1';
RLD_BAR <= '1';
CI <= '0';
OEbar <= '0';

wait for 4 ns;

clk <= '0';

wait for 4 ns;

assert (PL_BAR = '0')
report
"Assert 957 : < PL_BAR /= 0 >" -- 	Vector No: 609
severity warning;

assert (VECT_BAR = '1')
report
"Assert 958 : < VECT_BAR /= 1 >"
severity warning;

assert (MAP_BAR = '1')
report
"Assert 959 : < MAP_BAR /= 1 >"
severity warning;

wait for 1 ns;

-- **************************************************
--------------------------

clk <= '1'; --	 Cycle No: 1458

wait for 1 ns;

I <= "0000";
CCEN_BAR <= '0';
CC_BAR <= '0';
RLD_BAR <= '0';
CI <= '0';
OEbar <= '0';
D <= "111111111111";

wait for 4 ns;

clk <= '0';

wait for 4 ns;

wait for 1 ns;

--------------------------

clk <= '1'; --	 Cycle No: 1459

wait for 1 ns;

I <= "1100";
CCEN_BAR <= '1';
CC_BAR <= '1';
RLD_BAR <= '1';
CI <= '0';
OEbar <= '0';

wait for 4 ns;

clk <= '0';

wait for 4 ns;

assert (PL_BAR = '0')
report
"Assert 960 : < PL_BAR /= 0 >" -- 	Vector No: 610
severity warning;

assert (VECT_BAR = '1')
report
"Assert 961 : < VECT_BAR /= 1 >"
severity warning;

assert (MAP_BAR = '1')
report
"Assert 962 : < MAP_BAR /= 1 >"
severity warning;

wait for 1 ns;

-- **************************************************
--------------------------

clk <= '1'; --	 Cycle No: 1460

wait for 1 ns;

I <= "0000";
CCEN_BAR <= '0';
CC_BAR <= '0';
RLD_BAR <= '0';
CI <= '0';
OEbar <= '0';
D <= "111111111111";

wait for 4 ns;

clk <= '0';

wait for 4 ns;

wait for 1 ns;

--------------------------

clk <= '1'; --	 Cycle No: 1461

wait for 1 ns;

I <= "1100";
CCEN_BAR <= '1';
CC_BAR <= '0';
RLD_BAR <= '1';
CI <= '0';
OEbar <= '0';

wait for 4 ns;

clk <= '0';

wait for 4 ns;

assert (PL_BAR = '0')
report
"Assert 963 : < PL_BAR /= 0 >" -- 	Vector No: 611
severity warning;

assert (VECT_BAR = '1')
report
"Assert 964 : < VECT_BAR /= 1 >"
severity warning;

assert (MAP_BAR = '1')
report
"Assert 965 : < MAP_BAR /= 1 >"
severity warning;

wait for 1 ns;

-- **************************************************
-- ****** I = 13 *******
-- *** R = 0 ***
--------------------------

clk <= '1'; --	 Cycle No: 1462

wait for 1 ns;

I <= "0000";
CCEN_BAR <= '0';
CC_BAR <= '0';
RLD_BAR <= '0';
CI <= '0';
OEbar <= '0';
D <= "000000000000";

wait for 4 ns;

clk <= '0';

wait for 4 ns;

wait for 1 ns;

--------------------------

clk <= '1'; --	 Cycle No: 1463

wait for 1 ns;

I <= "1101";
CCEN_BAR <= '0';
CC_BAR <= '0';
RLD_BAR <= '1';
CI <= '0';
OEbar <= '0';

wait for 4 ns;

clk <= '0';

wait for 4 ns;

assert (PL_BAR = '0')
report
"Assert 966 : < PL_BAR /= 0 >" -- 	Vector No: 612
severity warning;

assert (VECT_BAR = '1')
report
"Assert 967 : < VECT_BAR /= 1 >"
severity warning;

assert (MAP_BAR = '1')
report
"Assert 968 : < MAP_BAR /= 1 >"
severity warning;

wait for 1 ns;

-- **************************************************
--------------------------

clk <= '1'; --	 Cycle No: 1464

wait for 1 ns;

I <= "0000";
CCEN_BAR <= '0';
CC_BAR <= '0';
RLD_BAR <= '0';
CI <= '0';
OEbar <= '0';
D <= "000000000000";

wait for 4 ns;

clk <= '0';

wait for 4 ns;

wait for 1 ns;

--------------------------

clk <= '1'; --	 Cycle No: 1465

wait for 1 ns;

I <= "1101";
CCEN_BAR <= '0';
CC_BAR <= '1';
RLD_BAR <= '1';
CI <= '0';
OEbar <= '0';

wait for 4 ns;

clk <= '0';

wait for 4 ns;

assert (PL_BAR = '0')
report
"Assert 969 : < PL_BAR /= 0 >" -- 	Vector No: 613
severity warning;

assert (VECT_BAR = '1')
report
"Assert 970 : < VECT_BAR /= 1 >"
severity warning;

assert (MAP_BAR = '1')
report
"Assert 971 : < MAP_BAR /= 1 >"
severity warning;

wait for 1 ns;

-- **************************************************
--------------------------

clk <= '1'; --	 Cycle No: 1466

wait for 1 ns;

I <= "0000";
CCEN_BAR <= '0';
CC_BAR <= '0';
RLD_BAR <= '0';
CI <= '0';
OEbar <= '0';
D <= "000000000000";

wait for 4 ns;

clk <= '0';

wait for 4 ns;

wait for 1 ns;

--------------------------

clk <= '1'; --	 Cycle No: 1467

wait for 1 ns;

I <= "1101";
CCEN_BAR <= '1';
CC_BAR <= '1';
RLD_BAR <= '1';
CI <= '0';
OEbar <= '0';

wait for 4 ns;

clk <= '0';

wait for 4 ns;

assert (PL_BAR = '0')
report
"Assert 972 : < PL_BAR /= 0 >" -- 	Vector No: 614
severity warning;

assert (VECT_BAR = '1')
report
"Assert 973 : < VECT_BAR /= 1 >"
severity warning;

assert (MAP_BAR = '1')
report
"Assert 974 : < MAP_BAR /= 1 >"
severity warning;

wait for 1 ns;

-- **************************************************
--------------------------

clk <= '1'; --	 Cycle No: 1468

wait for 1 ns;

I <= "0000";
CCEN_BAR <= '0';
CC_BAR <= '0';
RLD_BAR <= '0';
CI <= '0';
OEbar <= '0';
D <= "000000000000";

wait for 4 ns;

clk <= '0';

wait for 4 ns;

wait for 1 ns;

--------------------------

clk <= '1'; --	 Cycle No: 1469

wait for 1 ns;

I <= "1101";
CCEN_BAR <= '1';
CC_BAR <= '0';
RLD_BAR <= '1';
CI <= '0';
OEbar <= '0';

wait for 4 ns;

clk <= '0';

wait for 4 ns;

assert (PL_BAR = '0')
report
"Assert 975 : < PL_BAR /= 0 >" -- 	Vector No: 615
severity warning;

assert (VECT_BAR = '1')
report
"Assert 976 : < VECT_BAR /= 1 >"
severity warning;

assert (MAP_BAR = '1')
report
"Assert 977 : < MAP_BAR /= 1 >"
severity warning;

wait for 1 ns;

-- **************************************************
-- *** R = 1 ***
--------------------------

clk <= '1'; --	 Cycle No: 1470

wait for 1 ns;

I <= "0000";
CCEN_BAR <= '0';
CC_BAR <= '0';
RLD_BAR <= '0';
CI <= '0';
OEbar <= '0';
D <= "111111111111";

wait for 4 ns;

clk <= '0';

wait for 4 ns;

wait for 1 ns;

--------------------------

clk <= '1'; --	 Cycle No: 1471

wait for 1 ns;

I <= "1101";
CCEN_BAR <= '0';
CC_BAR <= '0';
RLD_BAR <= '1';
CI <= '0';
OEbar <= '0';

wait for 4 ns;

clk <= '0';

wait for 4 ns;

assert (PL_BAR = '0')
report
"Assert 978 : < PL_BAR /= 0 >" -- 	Vector No: 616
severity warning;

assert (VECT_BAR = '1')
report
"Assert 979 : < VECT_BAR /= 1 >"
severity warning;

assert (MAP_BAR = '1')
report
"Assert 980 : < MAP_BAR /= 1 >"
severity warning;

wait for 1 ns;

-- **************************************************
--------------------------

clk <= '1'; --	 Cycle No: 1472

wait for 1 ns;

I <= "0000";
CCEN_BAR <= '0';
CC_BAR <= '0';
RLD_BAR <= '0';
CI <= '0';
OEbar <= '0';
D <= "111111111111";

wait for 4 ns;

clk <= '0';

wait for 4 ns;

wait for 1 ns;

--------------------------

clk <= '1'; --	 Cycle No: 1473

wait for 1 ns;

I <= "1101";
CCEN_BAR <= '0';
CC_BAR <= '1';
RLD_BAR <= '1';
CI <= '0';
OEbar <= '0';

wait for 4 ns;

clk <= '0';

wait for 4 ns;

assert (PL_BAR = '0')
report
"Assert 981 : < PL_BAR /= 0 >" -- 	Vector No: 617
severity warning;

assert (VECT_BAR = '1')
report
"Assert 982 : < VECT_BAR /= 1 >"
severity warning;

assert (MAP_BAR = '1')
report
"Assert 983 : < MAP_BAR /= 1 >"
severity warning;

wait for 1 ns;

-- **************************************************
--------------------------

clk <= '1'; --	 Cycle No: 1474

wait for 1 ns;

I <= "0000";
CCEN_BAR <= '0';
CC_BAR <= '0';
RLD_BAR <= '0';
CI <= '0';
OEbar <= '0';
D <= "111111111111";

wait for 4 ns;

clk <= '0';

wait for 4 ns;

wait for 1 ns;

--------------------------

clk <= '1'; --	 Cycle No: 1475

wait for 1 ns;

I <= "1101";
CCEN_BAR <= '1';
CC_BAR <= '1';
RLD_BAR <= '1';
CI <= '0';
OEbar <= '0';

wait for 4 ns;

clk <= '0';

wait for 4 ns;

assert (PL_BAR = '0')
report
"Assert 984 : < PL_BAR /= 0 >" -- 	Vector No: 618
severity warning;

assert (VECT_BAR = '1')
report
"Assert 985 : < VECT_BAR /= 1 >"
severity warning;

assert (MAP_BAR = '1')
report
"Assert 986 : < MAP_BAR /= 1 >"
severity warning;

wait for 1 ns;

-- **************************************************
--------------------------

clk <= '1'; --	 Cycle No: 1476

wait for 1 ns;

I <= "0000";
CCEN_BAR <= '0';
CC_BAR <= '0';
RLD_BAR <= '0';
CI <= '0';
OEbar <= '0';
D <= "111111111111";

wait for 4 ns;

clk <= '0';

wait for 4 ns;

wait for 1 ns;

--------------------------

clk <= '1'; --	 Cycle No: 1477

wait for 1 ns;

I <= "1101";
CCEN_BAR <= '1';
CC_BAR <= '0';
RLD_BAR <= '1';
CI <= '0';
OEbar <= '0';

wait for 4 ns;

clk <= '0';

wait for 4 ns;

assert (PL_BAR = '0')
report
"Assert 987 : < PL_BAR /= 0 >" -- 	Vector No: 619
severity warning;

assert (VECT_BAR = '1')
report
"Assert 988 : < VECT_BAR /= 1 >"
severity warning;

assert (MAP_BAR = '1')
report
"Assert 989 : < MAP_BAR /= 1 >"
severity warning;

wait for 1 ns;

-- **************************************************
-- ****** I = 14 *******
-- *** R = 0 ***
--------------------------

clk <= '1'; --	 Cycle No: 1478

wait for 1 ns;

I <= "0000";
CCEN_BAR <= '0';
CC_BAR <= '0';
RLD_BAR <= '0';
CI <= '0';
OEbar <= '0';
D <= "000000000000";

wait for 4 ns;

clk <= '0';

wait for 4 ns;

wait for 1 ns;

--------------------------

clk <= '1'; --	 Cycle No: 1479

wait for 1 ns;

I <= "1110";
CCEN_BAR <= '0';
CC_BAR <= '0';
RLD_BAR <= '1';
CI <= '0';
OEbar <= '0';

wait for 4 ns;

clk <= '0';

wait for 4 ns;

assert (PL_BAR = '0')
report
"Assert 990 : < PL_BAR /= 0 >" -- 	Vector No: 620
severity warning;

assert (VECT_BAR = '1')
report
"Assert 991 : < VECT_BAR /= 1 >"
severity warning;

assert (MAP_BAR = '1')
report
"Assert 992 : < MAP_BAR /= 1 >"
severity warning;

wait for 1 ns;

-- **************************************************
--------------------------

clk <= '1'; --	 Cycle No: 1480

wait for 1 ns;

I <= "0000";
CCEN_BAR <= '0';
CC_BAR <= '0';
RLD_BAR <= '0';
CI <= '0';
OEbar <= '0';
D <= "000000000000";

wait for 4 ns;

clk <= '0';

wait for 4 ns;

wait for 1 ns;

--------------------------

clk <= '1'; --	 Cycle No: 1481

wait for 1 ns;

I <= "1110";
CCEN_BAR <= '0';
CC_BAR <= '1';
RLD_BAR <= '1';
CI <= '0';
OEbar <= '0';

wait for 4 ns;

clk <= '0';

wait for 4 ns;

assert (PL_BAR = '0')
report
"Assert 993 : < PL_BAR /= 0 >" -- 	Vector No: 621
severity warning;

assert (VECT_BAR = '1')
report
"Assert 994 : < VECT_BAR /= 1 >"
severity warning;

assert (MAP_BAR = '1')
report
"Assert 995 : < MAP_BAR /= 1 >"
severity warning;

wait for 1 ns;

-- **************************************************
--------------------------

clk <= '1'; --	 Cycle No: 1482

wait for 1 ns;

I <= "0000";
CCEN_BAR <= '0';
CC_BAR <= '0';
RLD_BAR <= '0';
CI <= '0';
OEbar <= '0';
D <= "000000000000";

wait for 4 ns;

clk <= '0';

wait for 4 ns;

wait for 1 ns;

--------------------------

clk <= '1'; --	 Cycle No: 1483

wait for 1 ns;

I <= "1110";
CCEN_BAR <= '1';
CC_BAR <= '1';
RLD_BAR <= '1';
CI <= '0';
OEbar <= '0';

wait for 4 ns;

clk <= '0';

wait for 4 ns;

assert (PL_BAR = '0')
report
"Assert 996 : < PL_BAR /= 0 >" -- 	Vector No: 622
severity warning;

assert (VECT_BAR = '1')
report
"Assert 997 : < VECT_BAR /= 1 >"
severity warning;

assert (MAP_BAR = '1')
report
"Assert 998 : < MAP_BAR /= 1 >"
severity warning;

wait for 1 ns;

-- **************************************************
--------------------------

clk <= '1'; --	 Cycle No: 1484

wait for 1 ns;

I <= "0000";
CCEN_BAR <= '0';
CC_BAR <= '0';
RLD_BAR <= '0';
CI <= '0';
OEbar <= '0';
D <= "000000000000";

wait for 4 ns;

clk <= '0';

wait for 4 ns;

wait for 1 ns;

--------------------------

clk <= '1'; --	 Cycle No: 1485

wait for 1 ns;

I <= "1110";
CCEN_BAR <= '1';
CC_BAR <= '0';
RLD_BAR <= '1';
CI <= '0';
OEbar <= '0';

wait for 4 ns;

clk <= '0';

wait for 4 ns;

assert (PL_BAR = '0')
report
"Assert 999 : < PL_BAR /= 0 >" -- 	Vector No: 623
severity warning;

assert (VECT_BAR = '1')
report
"Assert 1000 : < VECT_BAR /= 1 >"
severity warning;

assert (MAP_BAR = '1')
report
"Assert 1001 : < MAP_BAR /= 1 >"
severity warning;

wait for 1 ns;

-- **************************************************
-- *** R = 1 ***
--------------------------

clk <= '1'; --	 Cycle No: 1486

wait for 1 ns;

I <= "0000";
CCEN_BAR <= '0';
CC_BAR <= '0';
RLD_BAR <= '0';
CI <= '0';
OEbar <= '0';
D <= "111111111111";

wait for 4 ns;

clk <= '0';

wait for 4 ns;

wait for 1 ns;

--------------------------

clk <= '1'; --	 Cycle No: 1487

wait for 1 ns;

I <= "1110";
CCEN_BAR <= '0';
CC_BAR <= '0';
RLD_BAR <= '1';
CI <= '0';
OEbar <= '0';

wait for 4 ns;

clk <= '0';

wait for 4 ns;

assert (PL_BAR = '0')
report
"Assert 1002 : < PL_BAR /= 0 >" -- 	Vector No: 624
severity warning;

assert (VECT_BAR = '1')
report
"Assert 1003 : < VECT_BAR /= 1 >"
severity warning;

assert (MAP_BAR = '1')
report
"Assert 1004 : < MAP_BAR /= 1 >"
severity warning;

wait for 1 ns;

-- **************************************************
--------------------------

clk <= '1'; --	 Cycle No: 1488

wait for 1 ns;

I <= "0000";
CCEN_BAR <= '0';
CC_BAR <= '0';
RLD_BAR <= '0';
CI <= '0';
OEbar <= '0';
D <= "111111111111";

wait for 4 ns;

clk <= '0';

wait for 4 ns;

wait for 1 ns;

--------------------------

clk <= '1'; --	 Cycle No: 1489

wait for 1 ns;

I <= "1110";
CCEN_BAR <= '0';
CC_BAR <= '1';
RLD_BAR <= '1';
CI <= '0';
OEbar <= '0';

wait for 4 ns;

clk <= '0';

wait for 4 ns;

assert (PL_BAR = '0')
report
"Assert 1005 : < PL_BAR /= 0 >" -- 	Vector No: 625
severity warning;

assert (VECT_BAR = '1')
report
"Assert 1006 : < VECT_BAR /= 1 >"
severity warning;

assert (MAP_BAR = '1')
report
"Assert 1007 : < MAP_BAR /= 1 >"
severity warning;

wait for 1 ns;

-- **************************************************
--------------------------

clk <= '1'; --	 Cycle No: 1490

wait for 1 ns;

I <= "0000";
CCEN_BAR <= '0';
CC_BAR <= '0';
RLD_BAR <= '0';
CI <= '0';
OEbar <= '0';
D <= "111111111111";

wait for 4 ns;

clk <= '0';

wait for 4 ns;

wait for 1 ns;

--------------------------

clk <= '1'; --	 Cycle No: 1491

wait for 1 ns;

I <= "1110";
CCEN_BAR <= '1';
CC_BAR <= '1';
RLD_BAR <= '1';
CI <= '0';
OEbar <= '0';

wait for 4 ns;

clk <= '0';

wait for 4 ns;

assert (PL_BAR = '0')
report
"Assert 1008 : < PL_BAR /= 0 >" -- 	Vector No: 626
severity warning;

assert (VECT_BAR = '1')
report
"Assert 1009 : < VECT_BAR /= 1 >"
severity warning;

assert (MAP_BAR = '1')
report
"Assert 1010 : < MAP_BAR /= 1 >"
severity warning;

wait for 1 ns;

-- **************************************************
--------------------------

clk <= '1'; --	 Cycle No: 1492

wait for 1 ns;

I <= "0000";
CCEN_BAR <= '0';
CC_BAR <= '0';
RLD_BAR <= '0';
CI <= '0';
OEbar <= '0';
D <= "111111111111";

wait for 4 ns;

clk <= '0';

wait for 4 ns;

wait for 1 ns;

--------------------------

clk <= '1'; --	 Cycle No: 1493

wait for 1 ns;

I <= "1110";
CCEN_BAR <= '1';
CC_BAR <= '0';
RLD_BAR <= '1';
CI <= '0';
OEbar <= '0';

wait for 4 ns;

clk <= '0';

wait for 4 ns;

assert (PL_BAR = '0')
report
"Assert 1011 : < PL_BAR /= 0 >" -- 	Vector No: 627
severity warning;

assert (VECT_BAR = '1')
report
"Assert 1012 : < VECT_BAR /= 1 >"
severity warning;

assert (MAP_BAR = '1')
report
"Assert 1013 : < MAP_BAR /= 1 >"
severity warning;

wait for 1 ns;

-- **************************************************
-- ****** I = 15 *******
-- *** R = 0 ***
--------------------------

clk <= '1'; --	 Cycle No: 1494

wait for 1 ns;

I <= "0000";
CCEN_BAR <= '0';
CC_BAR <= '0';
RLD_BAR <= '0';
CI <= '0';
OEbar <= '0';
D <= "000000000000";

wait for 4 ns;

clk <= '0';

wait for 4 ns;

wait for 1 ns;

--------------------------

clk <= '1'; --	 Cycle No: 1495

wait for 1 ns;

I <= "1111";
CCEN_BAR <= '0';
CC_BAR <= '0';
RLD_BAR <= '1';
CI <= '0';
OEbar <= '0';

wait for 4 ns;

clk <= '0';

wait for 4 ns;

assert (PL_BAR = '0')
report
"Assert 1014 : < PL_BAR /= 0 >" -- 	Vector No: 628
severity warning;

assert (VECT_BAR = '1')
report
"Assert 1015 : < VECT_BAR /= 1 >"
severity warning;

assert (MAP_BAR = '1')
report
"Assert 1016 : < MAP_BAR /= 1 >"
severity warning;

wait for 1 ns;

-- **************************************************
--------------------------

clk <= '1'; --	 Cycle No: 1496

wait for 1 ns;

I <= "0000";
CCEN_BAR <= '0';
CC_BAR <= '0';
RLD_BAR <= '0';
CI <= '0';
OEbar <= '0';
D <= "000000000000";

wait for 4 ns;

clk <= '0';

wait for 4 ns;

wait for 1 ns;

--------------------------

clk <= '1'; --	 Cycle No: 1497

wait for 1 ns;

I <= "1111";
CCEN_BAR <= '0';
CC_BAR <= '1';
RLD_BAR <= '1';
CI <= '0';
OEbar <= '0';

wait for 4 ns;

clk <= '0';

wait for 4 ns;

assert (PL_BAR = '0')
report
"Assert 1017 : < PL_BAR /= 0 >" -- 	Vector No: 629
severity warning;

assert (VECT_BAR = '1')
report
"Assert 1018 : < VECT_BAR /= 1 >"
severity warning;

assert (MAP_BAR = '1')
report
"Assert 1019 : < MAP_BAR /= 1 >"
severity warning;

wait for 1 ns;

-- **************************************************
--------------------------

clk <= '1'; --	 Cycle No: 1498

wait for 1 ns;

I <= "0000";
CCEN_BAR <= '0';
CC_BAR <= '0';
RLD_BAR <= '0';
CI <= '0';
OEbar <= '0';
D <= "000000000000";

wait for 4 ns;

clk <= '0';

wait for 4 ns;

wait for 1 ns;

--------------------------

clk <= '1'; --	 Cycle No: 1499

wait for 1 ns;

I <= "1111";
CCEN_BAR <= '1';
CC_BAR <= '1';
RLD_BAR <= '1';
CI <= '0';
OEbar <= '0';

wait for 4 ns;

clk <= '0';

wait for 4 ns;

assert (PL_BAR = '0')
report
"Assert 1020 : < PL_BAR /= 0 >" -- 	Vector No: 630
severity warning;

assert (VECT_BAR = '1')
report
"Assert 1021 : < VECT_BAR /= 1 >"
severity warning;

assert (MAP_BAR = '1')
report
"Assert 1022 : < MAP_BAR /= 1 >"
severity warning;

wait for 1 ns;

-- **************************************************
--------------------------

clk <= '1'; --	 Cycle No: 1500

wait for 1 ns;

I <= "0000";
CCEN_BAR <= '0';
CC_BAR <= '0';
RLD_BAR <= '0';
CI <= '0';
OEbar <= '0';
D <= "000000000000";

wait for 4 ns;

clk <= '0';

wait for 4 ns;

wait for 1 ns;

--------------------------

clk <= '1'; --	 Cycle No: 1501

wait for 1 ns;

I <= "1111";
CCEN_BAR <= '1';
CC_BAR <= '0';
RLD_BAR <= '1';
CI <= '0';
OEbar <= '0';

wait for 4 ns;

clk <= '0';

wait for 4 ns;

assert (PL_BAR = '0')
report
"Assert 1023 : < PL_BAR /= 0 >" -- 	Vector No: 631
severity warning;

assert (VECT_BAR = '1')
report
"Assert 1024 : < VECT_BAR /= 1 >"
severity warning;

assert (MAP_BAR = '1')
report
"Assert 1025 : < MAP_BAR /= 1 >"
severity warning;

wait for 1 ns;

-- **************************************************
-- *** R = 1 ***
--------------------------

clk <= '1'; --	 Cycle No: 1502

wait for 1 ns;

I <= "0000";
CCEN_BAR <= '0';
CC_BAR <= '0';
RLD_BAR <= '0';
CI <= '0';
OEbar <= '0';
D <= "111111111111";

wait for 4 ns;

clk <= '0';

wait for 4 ns;

wait for 1 ns;

--------------------------

clk <= '1'; --	 Cycle No: 1503

wait for 1 ns;

I <= "1111";
CCEN_BAR <= '0';
CC_BAR <= '0';
RLD_BAR <= '1';
CI <= '0';
OEbar <= '0';

wait for 4 ns;

clk <= '0';

wait for 4 ns;

assert (PL_BAR = '0')
report
"Assert 1026 : < PL_BAR /= 0 >" -- 	Vector No: 632
severity warning;

assert (VECT_BAR = '1')
report
"Assert 1027 : < VECT_BAR /= 1 >"
severity warning;

assert (MAP_BAR = '1')
report
"Assert 1028 : < MAP_BAR /= 1 >"
severity warning;

wait for 1 ns;

-- **************************************************
--------------------------

clk <= '1'; --	 Cycle No: 1504

wait for 1 ns;

I <= "0000";
CCEN_BAR <= '0';
CC_BAR <= '0';
RLD_BAR <= '0';
CI <= '0';
OEbar <= '0';
D <= "111111111111";

wait for 4 ns;

clk <= '0';

wait for 4 ns;

wait for 1 ns;

--------------------------

clk <= '1'; --	 Cycle No: 1505

wait for 1 ns;

I <= "1111";
CCEN_BAR <= '0';
CC_BAR <= '1';
RLD_BAR <= '1';
CI <= '0';
OEbar <= '0';

wait for 4 ns;

clk <= '0';

wait for 4 ns;

assert (PL_BAR = '0')
report
"Assert 1029 : < PL_BAR /= 0 >" -- 	Vector No: 633
severity warning;

assert (VECT_BAR = '1')
report
"Assert 1030 : < VECT_BAR /= 1 >"
severity warning;

assert (MAP_BAR = '1')
report
"Assert 1031 : < MAP_BAR /= 1 >"
severity warning;

wait for 1 ns;

-- **************************************************
--------------------------

clk <= '1'; --	 Cycle No: 1506

wait for 1 ns;

I <= "0000";
CCEN_BAR <= '0';
CC_BAR <= '0';
RLD_BAR <= '0';
CI <= '0';
OEbar <= '0';
D <= "111111111111";

wait for 4 ns;

clk <= '0';

wait for 4 ns;

wait for 1 ns;

--------------------------

clk <= '1'; --	 Cycle No: 1507

wait for 1 ns;

I <= "1111";
CCEN_BAR <= '1';
CC_BAR <= '1';
RLD_BAR <= '1';
CI <= '0';
OEbar <= '0';

wait for 4 ns;

clk <= '0';

wait for 4 ns;

assert (PL_BAR = '0')
report
"Assert 1032 : < PL_BAR /= 0 >" -- 	Vector No: 634
severity warning;

assert (VECT_BAR = '1')
report
"Assert 1033 : < VECT_BAR /= 1 >"
severity warning;

assert (MAP_BAR = '1')
report
"Assert 1034 : < MAP_BAR /= 1 >"
severity warning;

wait for 1 ns;

-- **************************************************
--------------------------

clk <= '1'; --	 Cycle No: 1508

wait for 1 ns;

I <= "0000";
CCEN_BAR <= '0';
CC_BAR <= '0';
RLD_BAR <= '0';
CI <= '0';
OEbar <= '0';
D <= "111111111111";

wait for 4 ns;

clk <= '0';

wait for 4 ns;

wait for 1 ns;

--------------------------

clk <= '1'; --	 Cycle No: 1509

wait for 1 ns;

I <= "1111";
CCEN_BAR <= '1';
CC_BAR <= '0';
RLD_BAR <= '1';
CI <= '0';
OEbar <= '0';

wait for 4 ns;

clk <= '0';

wait for 4 ns;

assert (PL_BAR = '0')
report
"Assert 1035 : < PL_BAR /= 0 >" -- 	Vector No: 635
severity warning;

assert (VECT_BAR = '1')
report
"Assert 1036 : < VECT_BAR /= 1 >"
severity warning;

assert (MAP_BAR = '1')
report
"Assert 1037 : < MAP_BAR /= 1 >"
severity warning;

wait for 1 ns;

-- **************************************************
-- *I:CCEN:CC:RLD:CI:OE:D:Y:PL:VECT:MAP:FULL:
--------------------------

end process;

end AA;
