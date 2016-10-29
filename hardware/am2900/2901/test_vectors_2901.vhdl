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
-- Modified by : Champaka Ramachandran
--
-- Modified on : Sept 16, 1992
--
-- Verification Information:
--
--                  Verified     By whom?           Date         Simulator
--                  --------   ------------        --------     ------------
--  Syntax            yes     Champaka Ramachandran  16 Sept,92    ZYCAD
--  Functionality     yes     Champaka Ramachandran  16 Sept,92    ZYCAD
--------------------------------------------------------------------------------

use work.types.all;
use work.MVL7_functions.all;	--some binary functions
use work.synthesis_types.all;	--hints for synthesis

entity E is
end;

architecture A of E is
	 component AM2901
	  port (
		I : in MVL7_vector(8 downto 0);
		Aadd, Badd : in integer range 0 to 15;
		D :  in MVL7_vector(3 downto 0);
		Y : out MVL7_vector(3 downto 0);
		RAM0, RAM3, Q0, Q3 : inout MVL7;
		CLK : in clock;
		C0 : in MVL7;
		OEbar : in MVL7;
		C4, Gbar, Pbar, OVR, F3, F30 : out MVL7
	  );
	 end component;

		signal I : MVL7_vector(8 downto 0);
		signal Aadd, Badd : integer range 0 to 15;
		signal D : MVL7_vector(3 downto 0);
		signal Y : MVL7_vector(3 downto 0);
		signal RAM0, RAM3, Q0, Q3 : WiredOr MVL7;
		signal CLK : clock;
		signal C0 : MVL7;
		signal OEbar : MVL7;
		signal C4, Gbar, Pbar, OVR, F3, F30 : MVL7;

for all : AM2901 use entity work.a2901(a2901);

begin

AM1 : AM2901 port map(
		I,
		Aadd, Badd,
		D,
		Y,
		RAM0, RAM3, Q0, Q3,
		CLK,
		C0,
		OEbar,
		C4, Gbar, Pbar, OVR, F3, F30
           );

process

begin

-- ************************
-- *                      *
-- * TEST VECTORS FOR ALU *
-- *                      *
-- ************************
-- ******** ADDITION  R + S ********
--------------------------

clk <= '1'; --	 Cycle No: 0

wait for 1 ns;

I <= "000000111";
D <= "0000";
C0 <= '0';
OEbar <= '0';

wait for 4 ns;

clk <= '0';

wait for 4 ns;

wait for 1 ns;

--------------------------

clk <= '1'; --	 Cycle No: 1

wait for 1 ns;

I <= "001000110";
D <= "0000";
C0 <= '0';
OEbar <= '0';

wait for 4 ns;

clk <= '0';

wait for 4 ns;

assert (Y = "0000")
report
"Assert 0 : < Y /= 0000 >" -- 	Vector No: 0
severity warning;

assert (C4 = '0')
report
"Assert 1 : < C4 /= 0 >"
severity warning;

assert (Gbar = '1')
report
"Assert 2 : < Gbar /= 1 >"
severity warning;

assert (Pbar = '1')
report
"Assert 3 : < Pbar /= 1 >"
severity warning;

assert (OVR = '0')
report
"Assert 4 : < OVR /= 0 >"
severity warning;

assert (F3 = '0')
report
"Assert 5 : < F3 /= 0 >"
severity warning;

assert (F30 = '1')
report
"Assert 6 : < F30 /= 1 >"
severity warning;

wait for 1 ns;

-- ************************
--------------------------

clk <= '1'; --	 Cycle No: 2

wait for 1 ns;

I <= "000000111";
D <= "0000";
C0 <= '0';
OEbar <= '0';

wait for 4 ns;

clk <= '0';

wait for 4 ns;

wait for 1 ns;

--------------------------

clk <= '1'; --	 Cycle No: 3

wait for 1 ns;

I <= "001000110";
D <= "0000";
C0 <= '1';
OEbar <= '0';

wait for 4 ns;

clk <= '0';

wait for 4 ns;

assert (Y = "0001")
report
"Assert 7 : < Y /= 0001 >" -- 	Vector No: 1
severity warning;

assert (C4 = '0')
report
"Assert 8 : < C4 /= 0 >"
severity warning;

assert (Gbar = '1')
report
"Assert 9 : < Gbar /= 1 >"
severity warning;

assert (Pbar = '1')
report
"Assert 10 : < Pbar /= 1 >"
severity warning;

assert (OVR = '0')
report
"Assert 11 : < OVR /= 0 >"
severity warning;

assert (F3 = '0')
report
"Assert 12 : < F3 /= 0 >"
severity warning;

assert (F30 = '0')
report
"Assert 13 : < F30 /= 0 >"
severity warning;

wait for 1 ns;

-- ************************		
--------------------------

clk <= '1'; --	 Cycle No: 4

wait for 1 ns;

I <= "000000111";
D <= "1111";
C0 <= '0';
OEbar <= '0';

wait for 4 ns;

clk <= '0';

wait for 4 ns;

wait for 1 ns;

--------------------------

clk <= '1'; --	 Cycle No: 5

wait for 1 ns;

I <= "001000110";
D <= "0000";
C0 <= '0';
OEbar <= '0';

wait for 4 ns;

clk <= '0';

wait for 4 ns;

assert (Y = "1111")
report
"Assert 14 : < Y /= 1111 >" -- 	Vector No: 2
severity warning;

assert (C4 = '0')
report
"Assert 15 : < C4 /= 0 >"
severity warning;

assert (Gbar = '1')
report
"Assert 16 : < Gbar /= 1 >"
severity warning;

assert (Pbar = '0')
report
"Assert 17 : < Pbar /= 0 >"
severity warning;

assert (OVR = '0')
report
"Assert 18 : < OVR /= 0 >"
severity warning;

assert (F3 = '1')
report
"Assert 19 : < F3 /= 1 >"
severity warning;

assert (F30 = '0')
report
"Assert 20 : < F30 /= 0 >"
severity warning;

wait for 1 ns;

-- ************************		
--------------------------

clk <= '1'; --	 Cycle No: 6

wait for 1 ns;

I <= "000000111";
D <= "1111";
C0 <= '0';
OEbar <= '0';

wait for 4 ns;

clk <= '0';

wait for 4 ns;

wait for 1 ns;

--------------------------

clk <= '1'; --	 Cycle No: 7

wait for 1 ns;

I <= "001000110";
D <= "0000";
C0 <= '1';
OEbar <= '0';

wait for 4 ns;

clk <= '0';

wait for 4 ns;

assert (Y = "0000")
report
"Assert 21 : < Y /= 0000 >" -- 	Vector No: 3
severity warning;

assert (C4 = '1')
report
"Assert 22 : < C4 /= 1 >"
severity warning;

assert (Gbar = '1')
report
"Assert 23 : < Gbar /= 1 >"
severity warning;

assert (Pbar = '0')
report
"Assert 24 : < Pbar /= 0 >"
severity warning;

assert (OVR = '0')
report
"Assert 25 : < OVR /= 0 >"
severity warning;

assert (F3 = '0')
report
"Assert 26 : < F3 /= 0 >"
severity warning;

assert (F30 = '1')
report
"Assert 27 : < F30 /= 1 >"
severity warning;

wait for 1 ns;

-- ************************		
--------------------------

clk <= '1'; --	 Cycle No: 8

wait for 1 ns;

I <= "000000111";
D <= "1111";
C0 <= '0';
OEbar <= '0';

wait for 4 ns;

clk <= '0';

wait for 4 ns;

wait for 1 ns;

--------------------------

clk <= '1'; --	 Cycle No: 9

wait for 1 ns;

I <= "001000110";
D <= "1111";
C0 <= '0';
OEbar <= '0';

wait for 4 ns;

clk <= '0';

wait for 4 ns;

assert (Y = "1110")
report
"Assert 28 : < Y /= 1110 >" -- 	Vector No: 4
severity warning;

assert (C4 = '1')
report
"Assert 29 : < C4 /= 1 >"
severity warning;

assert (Gbar = '0')
report
"Assert 30 : < Gbar /= 0 >"
severity warning;

assert (Pbar = '0')
report
"Assert 31 : < Pbar /= 0 >"
severity warning;

assert (OVR = '0')
report
"Assert 32 : < OVR /= 0 >"
severity warning;

assert (F3 = '1')
report
"Assert 33 : < F3 /= 1 >"
severity warning;

assert (F30 = '0')
report
"Assert 34 : < F30 /= 0 >"
severity warning;

wait for 1 ns;

-- ************************		
--------------------------

clk <= '1'; --	 Cycle No: 10

wait for 1 ns;

I <= "000000111";
D <= "1111";
C0 <= '0';
OEbar <= '0';

wait for 4 ns;

clk <= '0';

wait for 4 ns;

wait for 1 ns;

--------------------------

clk <= '1'; --	 Cycle No: 11

wait for 1 ns;

I <= "001000110";
D <= "1111";
C0 <= '1';
OEbar <= '0';

wait for 4 ns;

clk <= '0';

wait for 4 ns;

assert (Y = "1111")
report
"Assert 35 : < Y /= 1111 >" -- 	Vector No: 5
severity warning;

assert (C4 = '1')
report
"Assert 36 : < C4 /= 1 >"
severity warning;

assert (Gbar = '0')
report
"Assert 37 : < Gbar /= 0 >"
severity warning;

assert (Pbar = '0')
report
"Assert 38 : < Pbar /= 0 >"
severity warning;

assert (OVR = '0')
report
"Assert 39 : < OVR /= 0 >"
severity warning;

assert (F3 = '1')
report
"Assert 40 : < F3 /= 1 >"
severity warning;

assert (F30 = '0')
report
"Assert 41 : < F30 /= 0 >"
severity warning;

wait for 1 ns;

-- ************************		
--------------------------

clk <= '1'; --	 Cycle No: 12

wait for 1 ns;

I <= "000000111";
D <= "0000";
C0 <= '0';
OEbar <= '0';

wait for 4 ns;

clk <= '0';

wait for 4 ns;

wait for 1 ns;

--------------------------

clk <= '1'; --	 Cycle No: 13

wait for 1 ns;

I <= "001000110";
D <= "1111";
C0 <= '0';
OEbar <= '0';

wait for 4 ns;

clk <= '0';

wait for 4 ns;

assert (Y = "1111")
report
"Assert 42 : < Y /= 1111 >" -- 	Vector No: 6
severity warning;

assert (C4 = '0')
report
"Assert 43 : < C4 /= 0 >"
severity warning;

assert (Gbar = '1')
report
"Assert 44 : < Gbar /= 1 >"
severity warning;

assert (Pbar = '0')
report
"Assert 45 : < Pbar /= 0 >"
severity warning;

assert (OVR = '0')
report
"Assert 46 : < OVR /= 0 >"
severity warning;

assert (F3 = '1')
report
"Assert 47 : < F3 /= 1 >"
severity warning;

assert (F30 = '0')
report
"Assert 48 : < F30 /= 0 >"
severity warning;

wait for 1 ns;

-- ************************		
--------------------------

clk <= '1'; --	 Cycle No: 14

wait for 1 ns;

I <= "000000111";
D <= "0000";
C0 <= '0';
OEbar <= '0';

wait for 4 ns;

clk <= '0';

wait for 4 ns;

wait for 1 ns;

--------------------------

clk <= '1'; --	 Cycle No: 15

wait for 1 ns;

I <= "001000110";
D <= "1111";
C0 <= '1';
OEbar <= '0';

wait for 4 ns;

clk <= '0';

wait for 4 ns;

assert (Y = "0000")
report
"Assert 49 : < Y /= 0000 >" -- 	Vector No: 7
severity warning;

assert (C4 = '1')
report
"Assert 50 : < C4 /= 1 >"
severity warning;

assert (Gbar = '1')
report
"Assert 51 : < Gbar /= 1 >"
severity warning;

assert (Pbar = '0')
report
"Assert 52 : < Pbar /= 0 >"
severity warning;

assert (OVR = '0')
report
"Assert 53 : < OVR /= 0 >"
severity warning;

assert (F3 = '0')
report
"Assert 54 : < F3 /= 0 >"
severity warning;

assert (F30 = '1')
report
"Assert 55 : < F30 /= 1 >"
severity warning;

wait for 1 ns;

-- ************************		
--------------------------

clk <= '1'; --	 Cycle No: 16

wait for 1 ns;

I <= "000000111";
D <= "0001";
C0 <= '0';
OEbar <= '0';

wait for 4 ns;

clk <= '0';

wait for 4 ns;

wait for 1 ns;

--------------------------

clk <= '1'; --	 Cycle No: 17

wait for 1 ns;

I <= "000000110";
D <= "0001";
C0 <= '0';
OEbar <= '0';

wait for 4 ns;

clk <= '0';

wait for 4 ns;

assert (Y = "0010")
report
"Assert 56 : < Y /= 0010 >" -- 	Vector No: 8
severity warning;

assert (C4 = '0')
report
"Assert 57 : < C4 /= 0 >"
severity warning;

assert (Gbar = '1')
report
"Assert 58 : < Gbar /= 1 >"
severity warning;

assert (Pbar = '1')
report
"Assert 59 : < Pbar /= 1 >"
severity warning;

assert (OVR = '0')
report
"Assert 60 : < OVR /= 0 >"
severity warning;

assert (F3 = '0')
report
"Assert 61 : < F3 /= 0 >"
severity warning;

assert (F30 = '0')
report
"Assert 62 : < F30 /= 0 >"
severity warning;

wait for 1 ns;

-- ************************		
--------------------------

clk <= '1'; --	 Cycle No: 18

wait for 1 ns;

I <= "000000111";
D <= "0010";
C0 <= '0';
OEbar <= '0';

wait for 4 ns;

clk <= '0';

wait for 4 ns;

wait for 1 ns;

--------------------------

clk <= '1'; --	 Cycle No: 19

wait for 1 ns;

I <= "001000110";
D <= "0010";
C0 <= '0';
OEbar <= '0';

wait for 4 ns;

clk <= '0';

wait for 4 ns;

assert (Y = "0100")
report
"Assert 63 : < Y /= 0100 >" -- 	Vector No: 9
severity warning;

assert (C4 = '0')
report
"Assert 64 : < C4 /= 0 >"
severity warning;

assert (Gbar = '1')
report
"Assert 65 : < Gbar /= 1 >"
severity warning;

assert (Pbar = '1')
report
"Assert 66 : < Pbar /= 1 >"
severity warning;

assert (OVR = '0')
report
"Assert 67 : < OVR /= 0 >"
severity warning;

assert (F3 = '0')
report
"Assert 68 : < F3 /= 0 >"
severity warning;

assert (F30 = '0')
report
"Assert 69 : < F30 /= 0 >"
severity warning;

wait for 1 ns;

-- ************************		
--------------------------

clk <= '1'; --	 Cycle No: 20

wait for 1 ns;

I <= "000000111";
D <= "0100";
C0 <= '0';
OEbar <= '0';

wait for 4 ns;

clk <= '0';

wait for 4 ns;

wait for 1 ns;

--------------------------

clk <= '1'; --	 Cycle No: 21

wait for 1 ns;

I <= "001000110";
D <= "0100";
C0 <= '0';
OEbar <= '0';

wait for 4 ns;

clk <= '0';

wait for 4 ns;

assert (Y = "1000")
report
"Assert 70 : < Y /= 1000 >" -- 	Vector No: 10
severity warning;

assert (C4 = '0')
report
"Assert 71 : < C4 /= 0 >"
severity warning;

assert (Gbar = '1')
report
"Assert 72 : < Gbar /= 1 >"
severity warning;

assert (Pbar = '1')
report
"Assert 73 : < Pbar /= 1 >"
severity warning;

assert (OVR = '1')
report
"Assert 74 : < OVR /= 1 >"
severity warning;

assert (F3 = '1')
report
"Assert 75 : < F3 /= 1 >"
severity warning;

assert (F30 = '0')
report
"Assert 76 : < F30 /= 0 >"
severity warning;

wait for 1 ns;

-- ************************		
--------------------------

clk <= '1'; --	 Cycle No: 22

wait for 1 ns;

I <= "000000111";
D <= "1000";
C0 <= '0';
OEbar <= '0';

wait for 4 ns;

clk <= '0';

wait for 4 ns;

wait for 1 ns;

--------------------------

clk <= '1'; --	 Cycle No: 23

wait for 1 ns;

I <= "001000110";
D <= "1000";
C0 <= '0';
OEbar <= '0';

wait for 4 ns;

clk <= '0';

wait for 4 ns;

assert (Y = "0000")
report
"Assert 77 : < Y /= 0000 >" -- 	Vector No: 11
severity warning;

assert (C4 = '1')
report
"Assert 78 : < C4 /= 1 >"
severity warning;

assert (Gbar = '0')
report
"Assert 79 : < Gbar /= 0 >"
severity warning;

assert (Pbar = '1')
report
"Assert 80 : < Pbar /= 1 >"
severity warning;

assert (OVR = '1')
report
"Assert 81 : < OVR /= 1 >"
severity warning;

assert (F3 = '0')
report
"Assert 82 : < F3 /= 0 >"
severity warning;

assert (F30 = '1')
report
"Assert 83 : < F30 /= 1 >"
severity warning;

wait for 1 ns;

-- ************************		
-- ******** SUBTRACTION S - R ********
--------------------------

clk <= '1'; --	 Cycle No: 24

wait for 1 ns;

I <= "000000111";
D <= "0000";
C0 <= '0';
OEbar <= '0';

wait for 4 ns;

clk <= '0';

wait for 4 ns;

wait for 1 ns;

--------------------------

clk <= '1'; --	 Cycle No: 25

wait for 1 ns;

I <= "001001110";
D <= "0000";
C0 <= '0';
OEbar <= '0';

wait for 4 ns;

clk <= '0';

wait for 4 ns;

assert (Y = "1111")
report
"Assert 84 : < Y /= 1111 >" -- 	Vector No: 12
severity warning;

assert (C4 = '0')
report
"Assert 85 : < C4 /= 0 >"
severity warning;

assert (Gbar = '1')
report
"Assert 86 : < Gbar /= 1 >"
severity warning;

assert (Pbar = '0')
report
"Assert 87 : < Pbar /= 0 >"
severity warning;

assert (OVR = '0')
report
"Assert 88 : < OVR /= 0 >"
severity warning;

assert (F3 = '1')
report
"Assert 89 : < F3 /= 1 >"
severity warning;

assert (F30 = '0')
report
"Assert 90 : < F30 /= 0 >"
severity warning;

wait for 1 ns;

-- ************************
--------------------------

clk <= '1'; --	 Cycle No: 26

wait for 1 ns;

I <= "000000111";
D <= "0000";
C0 <= '0';
OEbar <= '0';

wait for 4 ns;

clk <= '0';

wait for 4 ns;

wait for 1 ns;

--------------------------

clk <= '1'; --	 Cycle No: 27

wait for 1 ns;

I <= "001001110";
D <= "0000";
C0 <= '1';
OEbar <= '0';

wait for 4 ns;

clk <= '0';

wait for 4 ns;

assert (Y = "0000")
report
"Assert 91 : < Y /= 0000 >" -- 	Vector No: 13
severity warning;

assert (C4 = '1')
report
"Assert 92 : < C4 /= 1 >"
severity warning;

assert (Gbar = '1')
report
"Assert 93 : < Gbar /= 1 >"
severity warning;

assert (Pbar = '0')
report
"Assert 94 : < Pbar /= 0 >"
severity warning;

assert (OVR = '0')
report
"Assert 95 : < OVR /= 0 >"
severity warning;

assert (F3 = '0')
report
"Assert 96 : < F3 /= 0 >"
severity warning;

assert (F30 = '1')
report
"Assert 97 : < F30 /= 1 >"
severity warning;

wait for 1 ns;

-- ************************		
--------------------------

clk <= '1'; --	 Cycle No: 28

wait for 1 ns;

I <= "000000111";
D <= "0000";
C0 <= '0';
OEbar <= '0';

wait for 4 ns;

clk <= '0';

wait for 4 ns;

wait for 1 ns;

--------------------------

clk <= '1'; --	 Cycle No: 29

wait for 1 ns;

I <= "001001110";
D <= "1111";
C0 <= '0';
OEbar <= '0';

wait for 4 ns;

clk <= '0';

wait for 4 ns;

assert (Y = "0000")
report
"Assert 98 : < Y /= 0000 >" -- 	Vector No: 14
severity warning;

assert (C4 = '0')
report
"Assert 99 : < C4 /= 0 >"
severity warning;

assert (Gbar = '1')
report
"Assert 100 : < Gbar /= 1 >"
severity warning;

assert (Pbar = '1')
report
"Assert 101 : < Pbar /= 1 >"
severity warning;

assert (OVR = '0')
report
"Assert 102 : < OVR /= 0 >"
severity warning;

assert (F3 = '0')
report
"Assert 103 : < F3 /= 0 >"
severity warning;

assert (F30 = '1')
report
"Assert 104 : < F30 /= 1 >"
severity warning;

wait for 1 ns;

-- ************************		
--------------------------

clk <= '1'; --	 Cycle No: 30

wait for 1 ns;

I <= "000000111";
D <= "0000";
C0 <= '0';
OEbar <= '0';

wait for 4 ns;

clk <= '0';

wait for 4 ns;

wait for 1 ns;

--------------------------

clk <= '1'; --	 Cycle No: 31

wait for 1 ns;

I <= "001001110";
D <= "1111";
C0 <= '1';
OEbar <= '0';

wait for 4 ns;

clk <= '0';

wait for 4 ns;

assert (Y = "0001")
report
"Assert 105 : < Y /= 0001 >" -- 	Vector No: 15
severity warning;

assert (C4 = '0')
report
"Assert 106 : < C4 /= 0 >"
severity warning;

assert (Gbar = '1')
report
"Assert 107 : < Gbar /= 1 >"
severity warning;

assert (Pbar = '1')
report
"Assert 108 : < Pbar /= 1 >"
severity warning;

assert (OVR = '0')
report
"Assert 109 : < OVR /= 0 >"
severity warning;

assert (F3 = '0')
report
"Assert 110 : < F3 /= 0 >"
severity warning;

assert (F30 = '0')
report
"Assert 111 : < F30 /= 0 >"
severity warning;

wait for 1 ns;

-- ************************		
--------------------------

clk <= '1'; --	 Cycle No: 32

wait for 1 ns;

I <= "000000111";
D <= "1111";
C0 <= '0';
OEbar <= '0';

wait for 4 ns;

clk <= '0';

wait for 4 ns;

wait for 1 ns;

--------------------------

clk <= '1'; --	 Cycle No: 33

wait for 1 ns;

I <= "001001110";
D <= "1111";
C0 <= '0';
OEbar <= '0';

wait for 4 ns;

clk <= '0';

wait for 4 ns;

assert (Y = "1111")
report
"Assert 112 : < Y /= 1111 >" -- 	Vector No: 16
severity warning;

assert (C4 = '0')
report
"Assert 113 : < C4 /= 0 >"
severity warning;

assert (Gbar = '1')
report
"Assert 114 : < Gbar /= 1 >"
severity warning;

assert (Pbar = '0')
report
"Assert 115 : < Pbar /= 0 >"
severity warning;

assert (OVR = '0')
report
"Assert 116 : < OVR /= 0 >"
severity warning;

assert (F3 = '1')
report
"Assert 117 : < F3 /= 1 >"
severity warning;

assert (F30 = '0')
report
"Assert 118 : < F30 /= 0 >"
severity warning;

wait for 1 ns;

-- ************************		
--------------------------

clk <= '1'; --	 Cycle No: 34

wait for 1 ns;

I <= "000000111";
D <= "1111";
C0 <= '0';
OEbar <= '0';

wait for 4 ns;

clk <= '0';

wait for 4 ns;

wait for 1 ns;

--------------------------

clk <= '1'; --	 Cycle No: 35

wait for 1 ns;

I <= "001001110";
D <= "1111";
C0 <= '1';
OEbar <= '0';

wait for 4 ns;

clk <= '0';

wait for 4 ns;

assert (Y = "0000")
report
"Assert 119 : < Y /= 0000 >" -- 	Vector No: 17
severity warning;

assert (C4 = '1')
report
"Assert 120 : < C4 /= 1 >"
severity warning;

assert (Gbar = '1')
report
"Assert 121 : < Gbar /= 1 >"
severity warning;

assert (Pbar = '0')
report
"Assert 122 : < Pbar /= 0 >"
severity warning;

assert (OVR = '0')
report
"Assert 123 : < OVR /= 0 >"
severity warning;

assert (F3 = '0')
report
"Assert 124 : < F3 /= 0 >"
severity warning;

assert (F30 = '1')
report
"Assert 125 : < F30 /= 1 >"
severity warning;

wait for 1 ns;

-- ************************		
--------------------------

clk <= '1'; --	 Cycle No: 36

wait for 1 ns;

I <= "000000111";
D <= "1111";
C0 <= '0';
OEbar <= '0';

wait for 4 ns;

clk <= '0';

wait for 4 ns;

wait for 1 ns;

--------------------------

clk <= '1'; --	 Cycle No: 37

wait for 1 ns;

I <= "001001110";
D <= "0000";
C0 <= '0';
OEbar <= '0';

wait for 4 ns;

clk <= '0';

wait for 4 ns;

assert (Y = "1110")
report
"Assert 126 : < Y /= 1110 >" -- 	Vector No: 18
severity warning;

assert (C4 = '1')
report
"Assert 127 : < C4 /= 1 >"
severity warning;

assert (Gbar = '0')
report
"Assert 128 : < Gbar /= 0 >"
severity warning;

assert (Pbar = '0')
report
"Assert 129 : < Pbar /= 0 >"
severity warning;

assert (OVR = '0')
report
"Assert 130 : < OVR /= 0 >"
severity warning;

assert (F3 = '1')
report
"Assert 131 : < F3 /= 1 >"
severity warning;

assert (F30 = '0')
report
"Assert 132 : < F30 /= 0 >"
severity warning;

wait for 1 ns;

-- ************************		
--------------------------

clk <= '1'; --	 Cycle No: 38

wait for 1 ns;

I <= "000000111";
D <= "1111";
C0 <= '0';
OEbar <= '0';

wait for 4 ns;

clk <= '0';

wait for 4 ns;

wait for 1 ns;

--------------------------

clk <= '1'; --	 Cycle No: 39

wait for 1 ns;

I <= "001001110";
D <= "0000";
C0 <= '1';
OEbar <= '0';

wait for 4 ns;

clk <= '0';

wait for 4 ns;

assert (Y = "1111")
report
"Assert 133 : < Y /= 1111 >" -- 	Vector No: 19
severity warning;

assert (C4 = '1')
report
"Assert 134 : < C4 /= 1 >"
severity warning;

assert (Gbar = '0')
report
"Assert 135 : < Gbar /= 0 >"
severity warning;

assert (Pbar = '0')
report
"Assert 136 : < Pbar /= 0 >"
severity warning;

assert (OVR = '0')
report
"Assert 137 : < OVR /= 0 >"
severity warning;

assert (F3 = '1')
report
"Assert 138 : < F3 /= 1 >"
severity warning;

assert (F30 = '0')
report
"Assert 139 : < F30 /= 0 >"
severity warning;

wait for 1 ns;

-- ************************		
--------------------------

clk <= '1'; --	 Cycle No: 40

wait for 1 ns;

I <= "000000111";
D <= "0001";
C0 <= '0';
OEbar <= '0';

wait for 4 ns;

clk <= '0';

wait for 4 ns;

wait for 1 ns;

--------------------------

clk <= '1'; --	 Cycle No: 41

wait for 1 ns;

I <= "001001110";
D <= "0000";
C0 <= '0';
OEbar <= '0';

wait for 4 ns;

clk <= '0';

wait for 4 ns;

assert (Y = "0000")
report
"Assert 140 : < Y /= 0000 >" -- 	Vector No: 20
severity warning;

assert (C4 = '1')
report
"Assert 141 : < C4 /= 1 >"
severity warning;

assert (Gbar = '0')
report
"Assert 142 : < Gbar /= 0 >"
severity warning;

assert (Pbar = '0')
report
"Assert 143 : < Pbar /= 0 >"
severity warning;

assert (OVR = '0')
report
"Assert 144 : < OVR /= 0 >"
severity warning;

assert (F3 = '0')
report
"Assert 145 : < F3 /= 0 >"
severity warning;

assert (F30 = '1')
report
"Assert 146 : < F30 /= 1 >"
severity warning;

wait for 1 ns;

-- ************************		
--------------------------

clk <= '1'; --	 Cycle No: 42

wait for 1 ns;

I <= "000000111";
D <= "0010";
C0 <= '0';
OEbar <= '0';

wait for 4 ns;

clk <= '0';

wait for 4 ns;

wait for 1 ns;

--------------------------

clk <= '1'; --	 Cycle No: 43

wait for 1 ns;

I <= "001001110";
D <= "0001";
C0 <= '1';
OEbar <= '0';

wait for 4 ns;

clk <= '0';

wait for 4 ns;

assert (Y = "0001")
report
"Assert 147 : < Y /= 0001 >" -- 	Vector No: 21
severity warning;

assert (C4 = '1')
report
"Assert 148 : < C4 /= 1 >"
severity warning;

assert (Gbar = '0')
report
"Assert 149 : < Gbar /= 0 >"
severity warning;

assert (Pbar = '1')
report
"Assert 150 : < Pbar /= 1 >"
severity warning;

assert (OVR = '0')
report
"Assert 151 : < OVR /= 0 >"
severity warning;

assert (F3 = '0')
report
"Assert 152 : < F3 /= 0 >"
severity warning;

assert (F30 = '0')
report
"Assert 153 : < F30 /= 0 >"
severity warning;

wait for 1 ns;

-- ************************		
--------------------------

clk <= '1'; --	 Cycle No: 44

wait for 1 ns;

I <= "000000111";
D <= "0100";
C0 <= '0';
OEbar <= '0';

wait for 4 ns;

clk <= '0';

wait for 4 ns;

wait for 1 ns;

--------------------------

clk <= '1'; --	 Cycle No: 45

wait for 1 ns;

I <= "001001110";
D <= "0010";
C0 <= '1';
OEbar <= '0';

wait for 4 ns;

clk <= '0';

wait for 4 ns;

assert (Y = "0010")
report
"Assert 154 : < Y /= 0010 >" -- 	Vector No: 22
severity warning;

assert (C4 = '1')
report
"Assert 155 : < C4 /= 1 >"
severity warning;

assert (Gbar = '0')
report
"Assert 156 : < Gbar /= 0 >"
severity warning;

assert (Pbar = '1')
report
"Assert 157 : < Pbar /= 1 >"
severity warning;

assert (OVR = '0')
report
"Assert 158 : < OVR /= 0 >"
severity warning;

assert (F3 = '0')
report
"Assert 159 : < F3 /= 0 >"
severity warning;

assert (F30 = '0')
report
"Assert 160 : < F30 /= 0 >"
severity warning;

wait for 1 ns;

-- ************************		
--------------------------

clk <= '1'; --	 Cycle No: 46

wait for 1 ns;

I <= "000000111";
D <= "1000";
C0 <= '0';
OEbar <= '0';

wait for 4 ns;

clk <= '0';

wait for 4 ns;

wait for 1 ns;

--------------------------

clk <= '1'; --	 Cycle No: 47

wait for 1 ns;

I <= "001001110";
D <= "0100";
C0 <= '1';
OEbar <= '0';

wait for 4 ns;

clk <= '0';

wait for 4 ns;

assert (Y = "0100")
report
"Assert 161 : < Y /= 0100 >" -- 	Vector No: 23
severity warning;

assert (C4 = '1')
report
"Assert 162 : < C4 /= 1 >"
severity warning;

assert (Gbar = '0')
report
"Assert 163 : < Gbar /= 0 >"
severity warning;

assert (Pbar = '1')
report
"Assert 164 : < Pbar /= 1 >"
severity warning;

assert (OVR = '1')
report
"Assert 165 : < OVR /= 1 >"
severity warning;

assert (F3 = '0')
report
"Assert 166 : < F3 /= 0 >"
severity warning;

assert (F30 = '0')
report
"Assert 167 : < F30 /= 0 >"
severity warning;

wait for 1 ns;

-- ************************		
-- ******** SUBTRACTION R - S ********
--------------------------

clk <= '1'; --	 Cycle No: 48

wait for 1 ns;

I <= "000000111";
D <= "0000";
C0 <= '0';
OEbar <= '0';

wait for 4 ns;

clk <= '0';

wait for 4 ns;

wait for 1 ns;

--------------------------

clk <= '1'; --	 Cycle No: 49

wait for 1 ns;

I <= "001010110";
D <= "0000";
C0 <= '0';
OEbar <= '0';

wait for 4 ns;

clk <= '0';

wait for 4 ns;

assert (Y = "1111")
report
"Assert 168 : < Y /= 1111 >" -- 	Vector No: 24
severity warning;

assert (C4 = '0')
report
"Assert 169 : < C4 /= 0 >"
severity warning;

assert (Gbar = '1')
report
"Assert 170 : < Gbar /= 1 >"
severity warning;

assert (Pbar = '0')
report
"Assert 171 : < Pbar /= 0 >"
severity warning;

assert (OVR = '0')
report
"Assert 172 : < OVR /= 0 >"
severity warning;

assert (F3 = '1')
report
"Assert 173 : < F3 /= 1 >"
severity warning;

assert (F30 = '0')
report
"Assert 174 : < F30 /= 0 >"
severity warning;

wait for 1 ns;

-- ************************
--------------------------

clk <= '1'; --	 Cycle No: 50

wait for 1 ns;

I <= "000000111";
D <= "0000";
C0 <= '0';
OEbar <= '0';

wait for 4 ns;

clk <= '0';

wait for 4 ns;

wait for 1 ns;

--------------------------

clk <= '1'; --	 Cycle No: 51

wait for 1 ns;

I <= "001010110";
D <= "0000";
C0 <= '1';
OEbar <= '0';

wait for 4 ns;

clk <= '0';

wait for 4 ns;

assert (Y = "0000")
report
"Assert 175 : < Y /= 0000 >" -- 	Vector No: 25
severity warning;

assert (C4 = '1')
report
"Assert 176 : < C4 /= 1 >"
severity warning;

assert (Gbar = '1')
report
"Assert 177 : < Gbar /= 1 >"
severity warning;

assert (Pbar = '0')
report
"Assert 178 : < Pbar /= 0 >"
severity warning;

assert (OVR = '0')
report
"Assert 179 : < OVR /= 0 >"
severity warning;

assert (F3 = '0')
report
"Assert 180 : < F3 /= 0 >"
severity warning;

assert (F30 = '1')
report
"Assert 181 : < F30 /= 1 >"
severity warning;

wait for 1 ns;

-- ************************		
--------------------------

clk <= '1'; --	 Cycle No: 52

wait for 1 ns;

I <= "000000111";
D <= "1111";
C0 <= '0';
OEbar <= '0';

wait for 4 ns;

clk <= '0';

wait for 4 ns;

wait for 1 ns;

--------------------------

clk <= '1'; --	 Cycle No: 53

wait for 1 ns;

I <= "001010110";
D <= "0000";
C0 <= '0';
OEbar <= '0';

wait for 4 ns;

clk <= '0';

wait for 4 ns;

assert (Y = "0000")
report
"Assert 182 : < Y /= 0000 >" -- 	Vector No: 26
severity warning;

assert (C4 = '0')
report
"Assert 183 : < C4 /= 0 >"
severity warning;

assert (Gbar = '1')
report
"Assert 184 : < Gbar /= 1 >"
severity warning;

assert (Pbar = '1')
report
"Assert 185 : < Pbar /= 1 >"
severity warning;

assert (OVR = '0')
report
"Assert 186 : < OVR /= 0 >"
severity warning;

assert (F3 = '0')
report
"Assert 187 : < F3 /= 0 >"
severity warning;

assert (F30 = '1')
report
"Assert 188 : < F30 /= 1 >"
severity warning;

wait for 1 ns;

-- ************************		
--------------------------

clk <= '1'; --	 Cycle No: 54

wait for 1 ns;

I <= "000000111";
D <= "1111";
C0 <= '0';
OEbar <= '0';

wait for 4 ns;

clk <= '0';

wait for 4 ns;

wait for 1 ns;

--------------------------

clk <= '1'; --	 Cycle No: 55

wait for 1 ns;

I <= "001010110";
D <= "0000";
C0 <= '1';
OEbar <= '0';

wait for 4 ns;

clk <= '0';

wait for 4 ns;

assert (Y = "0001")
report
"Assert 189 : < Y /= 0001 >" -- 	Vector No: 27
severity warning;

assert (C4 = '0')
report
"Assert 190 : < C4 /= 0 >"
severity warning;

assert (Gbar = '1')
report
"Assert 191 : < Gbar /= 1 >"
severity warning;

assert (Pbar = '1')
report
"Assert 192 : < Pbar /= 1 >"
severity warning;

assert (OVR = '0')
report
"Assert 193 : < OVR /= 0 >"
severity warning;

assert (F3 = '0')
report
"Assert 194 : < F3 /= 0 >"
severity warning;

assert (F30 = '0')
report
"Assert 195 : < F30 /= 0 >"
severity warning;

wait for 1 ns;

-- ************************		
--------------------------

clk <= '1'; --	 Cycle No: 56

wait for 1 ns;

I <= "000000111";
D <= "1111";
C0 <= '0';
OEbar <= '0';

wait for 4 ns;

clk <= '0';

wait for 4 ns;

wait for 1 ns;

--------------------------

clk <= '1'; --	 Cycle No: 57

wait for 1 ns;

I <= "001010110";
D <= "1111";
C0 <= '0';
OEbar <= '0';

wait for 4 ns;

clk <= '0';

wait for 4 ns;

assert (Y = "1111")
report
"Assert 196 : < Y /= 1111 >" -- 	Vector No: 28
severity warning;

assert (C4 = '0')
report
"Assert 197 : < C4 /= 0 >"
severity warning;

assert (Gbar = '1')
report
"Assert 198 : < Gbar /= 1 >"
severity warning;

assert (Pbar = '0')
report
"Assert 199 : < Pbar /= 0 >"
severity warning;

assert (OVR = '0')
report
"Assert 200 : < OVR /= 0 >"
severity warning;

assert (F3 = '1')
report
"Assert 201 : < F3 /= 1 >"
severity warning;

assert (F30 = '0')
report
"Assert 202 : < F30 /= 0 >"
severity warning;

wait for 1 ns;

-- ************************		
--------------------------

clk <= '1'; --	 Cycle No: 58

wait for 1 ns;

I <= "000000111";
D <= "1111";
C0 <= '0';
OEbar <= '0';

wait for 4 ns;

clk <= '0';

wait for 4 ns;

wait for 1 ns;

--------------------------

clk <= '1'; --	 Cycle No: 59

wait for 1 ns;

I <= "001010110";
D <= "1111";
C0 <= '1';
OEbar <= '0';

wait for 4 ns;

clk <= '0';

wait for 4 ns;

assert (Y = "0000")
report
"Assert 203 : < Y /= 0000 >" -- 	Vector No: 29
severity warning;

assert (C4 = '1')
report
"Assert 204 : < C4 /= 1 >"
severity warning;

assert (Gbar = '1')
report
"Assert 205 : < Gbar /= 1 >"
severity warning;

assert (Pbar = '0')
report
"Assert 206 : < Pbar /= 0 >"
severity warning;

assert (OVR = '0')
report
"Assert 207 : < OVR /= 0 >"
severity warning;

assert (F3 = '0')
report
"Assert 208 : < F3 /= 0 >"
severity warning;

assert (F30 = '1')
report
"Assert 209 : < F30 /= 1 >"
severity warning;

wait for 1 ns;

-- ************************		
--------------------------

clk <= '1'; --	 Cycle No: 60

wait for 1 ns;

I <= "000000111";
D <= "0000";
C0 <= '0';
OEbar <= '0';

wait for 4 ns;

clk <= '0';

wait for 4 ns;

wait for 1 ns;

--------------------------

clk <= '1'; --	 Cycle No: 61

wait for 1 ns;

I <= "001010110";
D <= "1111";
C0 <= '0';
OEbar <= '0';

wait for 4 ns;

clk <= '0';

wait for 4 ns;

assert (Y = "1110")
report
"Assert 210 : < Y /= 1110 >" -- 	Vector No: 30
severity warning;

assert (C4 = '1')
report
"Assert 211 : < C4 /= 1 >"
severity warning;

assert (Gbar = '0')
report
"Assert 212 : < Gbar /= 0 >"
severity warning;

assert (Pbar = '0')
report
"Assert 213 : < Pbar /= 0 >"
severity warning;

assert (OVR = '0')
report
"Assert 214 : < OVR /= 0 >"
severity warning;

assert (F3 = '1')
report
"Assert 215 : < F3 /= 1 >"
severity warning;

assert (F30 = '0')
report
"Assert 216 : < F30 /= 0 >"
severity warning;

wait for 1 ns;

-- ************************		
--------------------------

clk <= '1'; --	 Cycle No: 62

wait for 1 ns;

I <= "000000111";
D <= "0000";
C0 <= '0';
OEbar <= '0';

wait for 4 ns;

clk <= '0';

wait for 4 ns;

wait for 1 ns;

--------------------------

clk <= '1'; --	 Cycle No: 63

wait for 1 ns;

I <= "001010110";
D <= "1111";
C0 <= '1';
OEbar <= '0';

wait for 4 ns;

clk <= '0';

wait for 4 ns;

assert (Y = "1111")
report
"Assert 217 : < Y /= 1111 >" -- 	Vector No: 31
severity warning;

assert (C4 = '1')
report
"Assert 218 : < C4 /= 1 >"
severity warning;

assert (Gbar = '0')
report
"Assert 219 : < Gbar /= 0 >"
severity warning;

assert (Pbar = '0')
report
"Assert 220 : < Pbar /= 0 >"
severity warning;

assert (OVR = '0')
report
"Assert 221 : < OVR /= 0 >"
severity warning;

assert (F3 = '1')
report
"Assert 222 : < F3 /= 1 >"
severity warning;

assert (F30 = '0')
report
"Assert 223 : < F30 /= 0 >"
severity warning;

wait for 1 ns;

-- ************************		
--------------------------

clk <= '1'; --	 Cycle No: 64

wait for 1 ns;

I <= "000000111";
D <= "0000";
C0 <= '0';
OEbar <= '0';

wait for 4 ns;

clk <= '0';

wait for 4 ns;

wait for 1 ns;

--------------------------

clk <= '1'; --	 Cycle No: 65

wait for 1 ns;

I <= "001010110";
D <= "0001";
C0 <= '0';
OEbar <= '0';

wait for 4 ns;

clk <= '0';

wait for 4 ns;

assert (Y = "0000")
report
"Assert 224 : < Y /= 0000 >" -- 	Vector No: 32
severity warning;

assert (C4 = '1')
report
"Assert 225 : < C4 /= 1 >"
severity warning;

assert (Gbar = '0')
report
"Assert 226 : < Gbar /= 0 >"
severity warning;

assert (Pbar = '0')
report
"Assert 227 : < Pbar /= 0 >"
severity warning;

assert (OVR = '0')
report
"Assert 228 : < OVR /= 0 >"
severity warning;

assert (F3 = '0')
report
"Assert 229 : < F3 /= 0 >"
severity warning;

assert (F30 = '1')
report
"Assert 230 : < F30 /= 1 >"
severity warning;

wait for 1 ns;

-- ************************		
--------------------------

clk <= '1'; --	 Cycle No: 66

wait for 1 ns;

I <= "000000111";
D <= "0001";
C0 <= '0';
OEbar <= '0';

wait for 4 ns;

clk <= '0';

wait for 4 ns;

wait for 1 ns;

--------------------------

clk <= '1'; --	 Cycle No: 67

wait for 1 ns;

I <= "001010110";
D <= "0010";
C0 <= '1';
OEbar <= '0';

wait for 4 ns;

clk <= '0';

wait for 4 ns;

assert (Y = "0001")
report
"Assert 231 : < Y /= 0001 >" -- 	Vector No: 33
severity warning;

assert (C4 = '1')
report
"Assert 232 : < C4 /= 1 >"
severity warning;

assert (Gbar = '0')
report
"Assert 233 : < Gbar /= 0 >"
severity warning;

assert (Pbar = '1')
report
"Assert 234 : < Pbar /= 1 >"
severity warning;

assert (OVR = '0')
report
"Assert 235 : < OVR /= 0 >"
severity warning;

assert (F3 = '0')
report
"Assert 236 : < F3 /= 0 >"
severity warning;

assert (F30 = '0')
report
"Assert 237 : < F30 /= 0 >"
severity warning;

wait for 1 ns;

-- ************************		
--------------------------

clk <= '1'; --	 Cycle No: 68

wait for 1 ns;

I <= "000000111";
D <= "0010";
C0 <= '0';
OEbar <= '0';

wait for 4 ns;

clk <= '0';

wait for 4 ns;

wait for 1 ns;

--------------------------

clk <= '1'; --	 Cycle No: 69

wait for 1 ns;

I <= "001010110";
D <= "0100";
C0 <= '1';
OEbar <= '0';

wait for 4 ns;

clk <= '0';

wait for 4 ns;

assert (Y = "0010")
report
"Assert 238 : < Y /= 0010 >" -- 	Vector No: 34
severity warning;

assert (C4 = '1')
report
"Assert 239 : < C4 /= 1 >"
severity warning;

assert (Gbar = '0')
report
"Assert 240 : < Gbar /= 0 >"
severity warning;

assert (Pbar = '1')
report
"Assert 241 : < Pbar /= 1 >"
severity warning;

assert (OVR = '0')
report
"Assert 242 : < OVR /= 0 >"
severity warning;

assert (F3 = '0')
report
"Assert 243 : < F3 /= 0 >"
severity warning;

assert (F30 = '0')
report
"Assert 244 : < F30 /= 0 >"
severity warning;

wait for 1 ns;

-- ************************		
--------------------------

clk <= '1'; --	 Cycle No: 70

wait for 1 ns;

I <= "000000111";
D <= "0100";
C0 <= '0';
OEbar <= '0';

wait for 4 ns;

clk <= '0';

wait for 4 ns;

wait for 1 ns;

--------------------------

clk <= '1'; --	 Cycle No: 71

wait for 1 ns;

I <= "001010110";
D <= "1000";
C0 <= '1';
OEbar <= '0';

wait for 4 ns;

clk <= '0';

wait for 4 ns;

assert (Y = "0100")
report
"Assert 245 : < Y /= 0100 >" -- 	Vector No: 35
severity warning;

assert (C4 = '1')
report
"Assert 246 : < C4 /= 1 >"
severity warning;

assert (Gbar = '0')
report
"Assert 247 : < Gbar /= 0 >"
severity warning;

assert (Pbar = '1')
report
"Assert 248 : < Pbar /= 1 >"
severity warning;

assert (OVR = '1')
report
"Assert 249 : < OVR /= 1 >"
severity warning;

assert (F3 = '0')
report
"Assert 250 : < F3 /= 0 >"
severity warning;

assert (F30 = '0')
report
"Assert 251 : < F30 /= 0 >"
severity warning;

wait for 1 ns;

-- ************************		
-- ******** OR (R or S) ********		
--------------------------

clk <= '1'; --	 Cycle No: 72

wait for 1 ns;

I <= "000000111";
D <= "0000";
C0 <= '0';
OEbar <= '0';

wait for 4 ns;

clk <= '0';

wait for 4 ns;

wait for 1 ns;

--------------------------

clk <= '1'; --	 Cycle No: 73

wait for 1 ns;

I <= "001011110";
D <= "0000";
OEbar <= '0';

wait for 4 ns;

clk <= '0';

wait for 4 ns;

assert (Y = "0000")
report
"Assert 252 : < Y /= 0000 >" -- 	Vector No: 36
severity warning;

assert (F3 = '0')
report
"Assert 253 : < F3 /= 0 >"
severity warning;

assert (F30 = '1')
report
"Assert 254 : < F30 /= 1 >"
severity warning;

wait for 1 ns;

-- ************************		
--------------------------

clk <= '1'; --	 Cycle No: 74

wait for 1 ns;

I <= "000000111";
D <= "1111";
C0 <= '0';
OEbar <= '0';

wait for 4 ns;

clk <= '0';

wait for 4 ns;

wait for 1 ns;

--------------------------

clk <= '1'; --	 Cycle No: 75

wait for 1 ns;

I <= "001011110";
D <= "0000";
OEbar <= '0';

wait for 4 ns;

clk <= '0';

wait for 4 ns;

assert (Y = "1111")
report
"Assert 255 : < Y /= 1111 >" -- 	Vector No: 37
severity warning;

assert (F3 = '1')
report
"Assert 256 : < F3 /= 1 >"
severity warning;

assert (F30 = '0')
report
"Assert 257 : < F30 /= 0 >"
severity warning;

wait for 1 ns;

-- ************************		
--------------------------

clk <= '1'; --	 Cycle No: 76

wait for 1 ns;

I <= "000000111";
D <= "1111";
C0 <= '0';
OEbar <= '0';

wait for 4 ns;

clk <= '0';

wait for 4 ns;

wait for 1 ns;

--------------------------

clk <= '1'; --	 Cycle No: 77

wait for 1 ns;

I <= "001011110";
D <= "1111";
OEbar <= '0';

wait for 4 ns;

clk <= '0';

wait for 4 ns;

assert (Y = "1111")
report
"Assert 258 : < Y /= 1111 >" -- 	Vector No: 38
severity warning;

assert (F3 = '1')
report
"Assert 259 : < F3 /= 1 >"
severity warning;

assert (F30 = '0')
report
"Assert 260 : < F30 /= 0 >"
severity warning;

wait for 1 ns;

-- ************************		
--------------------------

clk <= '1'; --	 Cycle No: 78

wait for 1 ns;

I <= "000000111";
D <= "0000";
C0 <= '0';
OEbar <= '0';

wait for 4 ns;

clk <= '0';

wait for 4 ns;

wait for 1 ns;

--------------------------

clk <= '1'; --	 Cycle No: 79

wait for 1 ns;

I <= "001011110";
D <= "1111";
OEbar <= '0';

wait for 4 ns;

clk <= '0';

wait for 4 ns;

assert (Y = "1111")
report
"Assert 261 : < Y /= 1111 >" -- 	Vector No: 39
severity warning;

assert (F3 = '1')
report
"Assert 262 : < F3 /= 1 >"
severity warning;

assert (F30 = '0')
report
"Assert 263 : < F30 /= 0 >"
severity warning;

wait for 1 ns;

-- ************************		
-- ******** AND (R and S) ********		
--------------------------

clk <= '1'; --	 Cycle No: 80

wait for 1 ns;

I <= "000000111";
D <= "0000";
C0 <= '0';
OEbar <= '0';

wait for 4 ns;

clk <= '0';

wait for 4 ns;

wait for 1 ns;

--------------------------

clk <= '1'; --	 Cycle No: 81

wait for 1 ns;

I <= "001100110";
D <= "0000";
OEbar <= '0';

wait for 4 ns;

clk <= '0';

wait for 4 ns;

assert (Y = "0000")
report
"Assert 264 : < Y /= 0000 >" -- 	Vector No: 40
severity warning;

assert (F3 = '0')
report
"Assert 265 : < F3 /= 0 >"
severity warning;

assert (F30 = '1')
report
"Assert 266 : < F30 /= 1 >"
severity warning;

wait for 1 ns;

-- ************************		
--------------------------

clk <= '1'; --	 Cycle No: 82

wait for 1 ns;

I <= "000000111";
D <= "1111";
C0 <= '0';
OEbar <= '0';

wait for 4 ns;

clk <= '0';

wait for 4 ns;

wait for 1 ns;

--------------------------

clk <= '1'; --	 Cycle No: 83

wait for 1 ns;

I <= "001100110";
D <= "0000";
OEbar <= '0';

wait for 4 ns;

clk <= '0';

wait for 4 ns;

assert (Y = "0000")
report
"Assert 267 : < Y /= 0000 >" -- 	Vector No: 41
severity warning;

assert (F3 = '0')
report
"Assert 268 : < F3 /= 0 >"
severity warning;

assert (F30 = '1')
report
"Assert 269 : < F30 /= 1 >"
severity warning;

wait for 1 ns;

-- ************************		
--------------------------

clk <= '1'; --	 Cycle No: 84

wait for 1 ns;

I <= "000000111";
D <= "1111";
C0 <= '0';
OEbar <= '0';

wait for 4 ns;

clk <= '0';

wait for 4 ns;

wait for 1 ns;

--------------------------

clk <= '1'; --	 Cycle No: 85

wait for 1 ns;

I <= "001100110";
D <= "1111";
OEbar <= '0';

wait for 4 ns;

clk <= '0';

wait for 4 ns;

assert (Y = "1111")
report
"Assert 270 : < Y /= 1111 >" -- 	Vector No: 42
severity warning;

assert (F3 = '1')
report
"Assert 271 : < F3 /= 1 >"
severity warning;

assert (F30 = '0')
report
"Assert 272 : < F30 /= 0 >"
severity warning;

wait for 1 ns;

-- ************************		
--------------------------

clk <= '1'; --	 Cycle No: 86

wait for 1 ns;

I <= "000000111";
D <= "0000";
C0 <= '0';
OEbar <= '0';

wait for 4 ns;

clk <= '0';

wait for 4 ns;

wait for 1 ns;

--------------------------

clk <= '1'; --	 Cycle No: 87

wait for 1 ns;

I <= "001100110";
D <= "1111";
OEbar <= '0';

wait for 4 ns;

clk <= '0';

wait for 4 ns;

assert (Y = "0000")
report
"Assert 273 : < Y /= 0000 >" -- 	Vector No: 43
severity warning;

assert (F3 = '0')
report
"Assert 274 : < F3 /= 0 >"
severity warning;

assert (F30 = '1')
report
"Assert 275 : < F30 /= 1 >"
severity warning;

wait for 1 ns;

-- ************************		
-- ******** AND (not(R) and S) ********		
--------------------------

clk <= '1'; --	 Cycle No: 88

wait for 1 ns;

I <= "000000111";
D <= "0000";
C0 <= '0';
OEbar <= '0';

wait for 4 ns;

clk <= '0';

wait for 4 ns;

wait for 1 ns;

--------------------------

clk <= '1'; --	 Cycle No: 89

wait for 1 ns;

I <= "001101110";
D <= "0000";
OEbar <= '0';

wait for 4 ns;

clk <= '0';

wait for 4 ns;

assert (Y = "0000")
report
"Assert 276 : < Y /= 0000 >" -- 	Vector No: 44
severity warning;

assert (F3 = '0')
report
"Assert 277 : < F3 /= 0 >"
severity warning;

assert (F30 = '1')
report
"Assert 278 : < F30 /= 1 >"
severity warning;

wait for 1 ns;

-- ************************		
--------------------------

clk <= '1'; --	 Cycle No: 90

wait for 1 ns;

I <= "000000111";
D <= "1111";
C0 <= '0';
OEbar <= '0';

wait for 4 ns;

clk <= '0';

wait for 4 ns;

wait for 1 ns;

--------------------------

clk <= '1'; --	 Cycle No: 91

wait for 1 ns;

I <= "001101110";
D <= "0000";
OEbar <= '0';

wait for 4 ns;

clk <= '0';

wait for 4 ns;

assert (Y = "1111")
report
"Assert 279 : < Y /= 1111 >" -- 	Vector No: 45
severity warning;

assert (F3 = '1')
report
"Assert 280 : < F3 /= 1 >"
severity warning;

assert (F30 = '0')
report
"Assert 281 : < F30 /= 0 >"
severity warning;

wait for 1 ns;

-- ************************		
--------------------------

clk <= '1'; --	 Cycle No: 92

wait for 1 ns;

I <= "000000111";
D <= "1111";
C0 <= '0';
OEbar <= '0';

wait for 4 ns;

clk <= '0';

wait for 4 ns;

wait for 1 ns;

--------------------------

clk <= '1'; --	 Cycle No: 93

wait for 1 ns;

I <= "001101110";
D <= "1111";
OEbar <= '0';

wait for 4 ns;

clk <= '0';

wait for 4 ns;

assert (Y = "0000")
report
"Assert 282 : < Y /= 0000 >" -- 	Vector No: 46
severity warning;

assert (F3 = '0')
report
"Assert 283 : < F3 /= 0 >"
severity warning;

assert (F30 = '1')
report
"Assert 284 : < F30 /= 1 >"
severity warning;

wait for 1 ns;

-- ************************		
--------------------------

clk <= '1'; --	 Cycle No: 94

wait for 1 ns;

I <= "000000111";
D <= "0000";
C0 <= '0';
OEbar <= '0';

wait for 4 ns;

clk <= '0';

wait for 4 ns;

wait for 1 ns;

--------------------------

clk <= '1'; --	 Cycle No: 95

wait for 1 ns;

I <= "001101110";
D <= "1111";
OEbar <= '0';

wait for 4 ns;

clk <= '0';

wait for 4 ns;

assert (Y = "0000")
report
"Assert 285 : < Y /= 0000 >" -- 	Vector No: 47
severity warning;

assert (F3 = '0')
report
"Assert 286 : < F3 /= 0 >"
severity warning;

assert (F30 = '1')
report
"Assert 287 : < F30 /= 1 >"
severity warning;

wait for 1 ns;

-- ************************		
-- ******** XOR (R xor S) ********		
--------------------------

clk <= '1'; --	 Cycle No: 96

wait for 1 ns;

I <= "000000111";
D <= "0000";
C0 <= '0';
OEbar <= '0';

wait for 4 ns;

clk <= '0';

wait for 4 ns;

wait for 1 ns;

--------------------------

clk <= '1'; --	 Cycle No: 97

wait for 1 ns;

I <= "001110110";
D <= "0000";
OEbar <= '0';

wait for 4 ns;

clk <= '0';

wait for 4 ns;

assert (Y = "0000")
report
"Assert 288 : < Y /= 0000 >" -- 	Vector No: 48
severity warning;

assert (F3 = '0')
report
"Assert 289 : < F3 /= 0 >"
severity warning;

assert (F30 = '1')
report
"Assert 290 : < F30 /= 1 >"
severity warning;

wait for 1 ns;

-- ************************		
--------------------------

clk <= '1'; --	 Cycle No: 98

wait for 1 ns;

I <= "000000111";
D <= "1111";
C0 <= '0';
OEbar <= '0';

wait for 4 ns;

clk <= '0';

wait for 4 ns;

wait for 1 ns;

--------------------------

clk <= '1'; --	 Cycle No: 99

wait for 1 ns;

I <= "001110110";
D <= "0000";
OEbar <= '0';

wait for 4 ns;

clk <= '0';

wait for 4 ns;

assert (Y = "1111")
report
"Assert 291 : < Y /= 1111 >" -- 	Vector No: 49
severity warning;

assert (F3 = '1')
report
"Assert 292 : < F3 /= 1 >"
severity warning;

assert (F30 = '0')
report
"Assert 293 : < F30 /= 0 >"
severity warning;

wait for 1 ns;

-- ************************		
--------------------------

clk <= '1'; --	 Cycle No: 100

wait for 1 ns;

I <= "000000111";
D <= "1111";
C0 <= '0';
OEbar <= '0';

wait for 4 ns;

clk <= '0';

wait for 4 ns;

wait for 1 ns;

--------------------------

clk <= '1'; --	 Cycle No: 101

wait for 1 ns;

I <= "001110110";
D <= "1111";
OEbar <= '0';

wait for 4 ns;

clk <= '0';

wait for 4 ns;

assert (Y = "0000")
report
"Assert 294 : < Y /= 0000 >" -- 	Vector No: 50
severity warning;

assert (F3 = '0')
report
"Assert 295 : < F3 /= 0 >"
severity warning;

assert (F30 = '1')
report
"Assert 296 : < F30 /= 1 >"
severity warning;

wait for 1 ns;

-- ************************		
--------------------------

clk <= '1'; --	 Cycle No: 102

wait for 1 ns;

I <= "000000111";
D <= "0000";
C0 <= '0';
OEbar <= '0';

wait for 4 ns;

clk <= '0';

wait for 4 ns;

wait for 1 ns;

--------------------------

clk <= '1'; --	 Cycle No: 103

wait for 1 ns;

I <= "001110110";
D <= "1111";
OEbar <= '0';

wait for 4 ns;

clk <= '0';

wait for 4 ns;

assert (Y = "1111")
report
"Assert 297 : < Y /= 1111 >" -- 	Vector No: 51
severity warning;

assert (F3 = '1')
report
"Assert 298 : < F3 /= 1 >"
severity warning;

assert (F30 = '0')
report
"Assert 299 : < F30 /= 0 >"
severity warning;

wait for 1 ns;

-- ************************		
-- ******** XNOR (R xnor S) ********		
--------------------------

clk <= '1'; --	 Cycle No: 104

wait for 1 ns;

I <= "000000111";
D <= "0000";
C0 <= '0';
OEbar <= '0';

wait for 4 ns;

clk <= '0';

wait for 4 ns;

wait for 1 ns;

--------------------------

clk <= '1'; --	 Cycle No: 105

wait for 1 ns;

I <= "001111110";
D <= "0000";
OEbar <= '0';

wait for 4 ns;

clk <= '0';

wait for 4 ns;

assert (Y = "1111")
report
"Assert 300 : < Y /= 1111 >" -- 	Vector No: 52
severity warning;

assert (F3 = '1')
report
"Assert 301 : < F3 /= 1 >"
severity warning;

assert (F30 = '0')
report
"Assert 302 : < F30 /= 0 >"
severity warning;

wait for 1 ns;

-- ************************		
--------------------------

clk <= '1'; --	 Cycle No: 106

wait for 1 ns;

I <= "000000111";
D <= "1111";
C0 <= '0';
OEbar <= '0';

wait for 4 ns;

clk <= '0';

wait for 4 ns;

wait for 1 ns;

--------------------------

clk <= '1'; --	 Cycle No: 107

wait for 1 ns;

I <= "001111110";
D <= "0000";
OEbar <= '0';

wait for 4 ns;

clk <= '0';

wait for 4 ns;

assert (Y = "0000")
report
"Assert 303 : < Y /= 0000 >" -- 	Vector No: 53
severity warning;

assert (F3 = '0')
report
"Assert 304 : < F3 /= 0 >"
severity warning;

assert (F30 = '1')
report
"Assert 305 : < F30 /= 1 >"
severity warning;

wait for 1 ns;

-- ************************		
--------------------------

clk <= '1'; --	 Cycle No: 108

wait for 1 ns;

I <= "000000111";
D <= "1111";
C0 <= '0';
OEbar <= '0';

wait for 4 ns;

clk <= '0';

wait for 4 ns;

wait for 1 ns;

--------------------------

clk <= '1'; --	 Cycle No: 109

wait for 1 ns;

I <= "001111110";
D <= "1111";
OEbar <= '0';

wait for 4 ns;

clk <= '0';

wait for 4 ns;

assert (Y = "1111")
report
"Assert 306 : < Y /= 1111 >" -- 	Vector No: 54
severity warning;

assert (F3 = '1')
report
"Assert 307 : < F3 /= 1 >"
severity warning;

assert (F30 = '0')
report
"Assert 308 : < F30 /= 0 >"
severity warning;

wait for 1 ns;

-- ************************
-- ************************
-- *                      *		
-- * TEST VECTORS FOR RAM *
-- *                      *
-- ************************
-- ******** WRITING 0's AND 1's IN RAM LOCATIONS ********		
--------------------------

clk <= '1'; --	 Cycle No: 110

wait for 1 ns;

I <= "010000111";
D <= "0000";
Badd <=  0 ;
C0 <= '0';
OEbar <= '0';

wait for 4 ns;

clk <= '0';

wait for 4 ns;

wait for 1 ns;

--------------------------

clk <= '1'; --	 Cycle No: 111

wait for 1 ns;

I <= "001000100";
Aadd <=  0 ;
C0 <= '0';
OEbar <= '0';

wait for 4 ns;

clk <= '0';

wait for 4 ns;

assert (Y = "0000")
report
"Assert 309 : < Y /= 0000 >" -- 	Vector No: 55
severity warning;

wait for 1 ns;

-- ************************		
--------------------------

clk <= '1'; --	 Cycle No: 112

wait for 1 ns;

I <= "011000111";
D <= "0000";
Badd <=  0 ;
C0 <= '0';
OEbar <= '0';

wait for 4 ns;

clk <= '0';

wait for 4 ns;

wait for 1 ns;

--------------------------

clk <= '1'; --	 Cycle No: 113

wait for 1 ns;

I <= "001000100";
Badd <=  0 ;
C0 <= '0';
OEbar <= '0';

wait for 4 ns;

clk <= '0';

wait for 4 ns;

assert (Y = "0000")
report
"Assert 310 : < Y /= 0000 >" -- 	Vector No: 56
severity warning;

wait for 1 ns;

-- ************************		
--------------------------

clk <= '1'; --	 Cycle No: 114

wait for 1 ns;

I <= "010000111";
D <= "1111";
Badd <=  0 ;
C0 <= '0';
OEbar <= '0';

wait for 4 ns;

clk <= '0';

wait for 4 ns;

wait for 1 ns;

--------------------------

clk <= '1'; --	 Cycle No: 115

wait for 1 ns;

I <= "001000100";
Aadd <=  0 ;
C0 <= '0';
OEbar <= '0';

wait for 4 ns;

clk <= '0';

wait for 4 ns;

assert (Y = "1111")
report
"Assert 311 : < Y /= 1111 >" -- 	Vector No: 57
severity warning;

wait for 1 ns;

-- ************************		
--------------------------

clk <= '1'; --	 Cycle No: 116

wait for 1 ns;

I <= "011000111";
D <= "1111";
Badd <=  0 ;
C0 <= '0';
OEbar <= '0';

wait for 4 ns;

clk <= '0';

wait for 4 ns;

wait for 1 ns;

--------------------------

clk <= '1'; --	 Cycle No: 117

wait for 1 ns;

I <= "001000100";
Badd <=  0 ;
C0 <= '0';
OEbar <= '0';

wait for 4 ns;

clk <= '0';

wait for 4 ns;

assert (Y = "1111")
report
"Assert 312 : < Y /= 1111 >" -- 	Vector No: 58
severity warning;

wait for 1 ns;

-- ************************		
--------------------------

clk <= '1'; --	 Cycle No: 118

wait for 1 ns;

I <= "010000111";
D <= "0000";
Badd <=  1 ;
C0 <= '0';
OEbar <= '0';

wait for 4 ns;

clk <= '0';

wait for 4 ns;

wait for 1 ns;

--------------------------

clk <= '1'; --	 Cycle No: 119

wait for 1 ns;

I <= "001000100";
Aadd <=  1 ;
C0 <= '0';
OEbar <= '0';

wait for 4 ns;

clk <= '0';

wait for 4 ns;

assert (Y = "0000")
report
"Assert 313 : < Y /= 0000 >" -- 	Vector No: 59
severity warning;

wait for 1 ns;

-- ************************		
--------------------------

clk <= '1'; --	 Cycle No: 120

wait for 1 ns;

I <= "011000111";
D <= "0000";
Badd <=  1 ;
C0 <= '0';
OEbar <= '0';

wait for 4 ns;

clk <= '0';

wait for 4 ns;

wait for 1 ns;

--------------------------

clk <= '1'; --	 Cycle No: 121

wait for 1 ns;

I <= "001000100";
Badd <=  1 ;
C0 <= '0';
OEbar <= '0';

wait for 4 ns;

clk <= '0';

wait for 4 ns;

assert (Y = "0000")
report
"Assert 314 : < Y /= 0000 >" -- 	Vector No: 60
severity warning;

wait for 1 ns;

-- ************************		
--------------------------

clk <= '1'; --	 Cycle No: 122

wait for 1 ns;

I <= "010000111";
D <= "1111";
Badd <=  1 ;
C0 <= '0';
OEbar <= '0';

wait for 4 ns;

clk <= '0';

wait for 4 ns;

wait for 1 ns;

--------------------------

clk <= '1'; --	 Cycle No: 123

wait for 1 ns;

I <= "001000100";
Aadd <=  1 ;
C0 <= '0';
OEbar <= '0';

wait for 4 ns;

clk <= '0';

wait for 4 ns;

assert (Y = "1111")
report
"Assert 315 : < Y /= 1111 >" -- 	Vector No: 61
severity warning;

wait for 1 ns;

-- ************************		
--------------------------

clk <= '1'; --	 Cycle No: 124

wait for 1 ns;

I <= "011000111";
D <= "1111";
Badd <=  1 ;
C0 <= '0';
OEbar <= '0';

wait for 4 ns;

clk <= '0';

wait for 4 ns;

wait for 1 ns;

--------------------------

clk <= '1'; --	 Cycle No: 125

wait for 1 ns;

I <= "001000100";
Badd <=  1 ;
C0 <= '0';
OEbar <= '0';

wait for 4 ns;

clk <= '0';

wait for 4 ns;

assert (Y = "1111")
report
"Assert 316 : < Y /= 1111 >" -- 	Vector No: 62
severity warning;

wait for 1 ns;

-- ************************	
--------------------------

clk <= '1'; --	 Cycle No: 126

wait for 1 ns;

I <= "010000111";
D <= "0000";
Badd <=  2 ;
C0 <= '0';
OEbar <= '0';

wait for 4 ns;

clk <= '0';

wait for 4 ns;

wait for 1 ns;

--------------------------

clk <= '1'; --	 Cycle No: 127

wait for 1 ns;

I <= "001000100";
Aadd <=  2 ;
C0 <= '0';
OEbar <= '0';

wait for 4 ns;

clk <= '0';

wait for 4 ns;

assert (Y = "0000")
report
"Assert 317 : < Y /= 0000 >" -- 	Vector No: 63
severity warning;

wait for 1 ns;

-- ************************		
--------------------------

clk <= '1'; --	 Cycle No: 128

wait for 1 ns;

I <= "011000111";
D <= "0000";
Badd <=  2 ;
C0 <= '0';
OEbar <= '0';

wait for 4 ns;

clk <= '0';

wait for 4 ns;

wait for 1 ns;

--------------------------

clk <= '1'; --	 Cycle No: 129

wait for 1 ns;

I <= "001000100";
Badd <=  2 ;
C0 <= '0';
OEbar <= '0';

wait for 4 ns;

clk <= '0';

wait for 4 ns;

assert (Y = "0000")
report
"Assert 318 : < Y /= 0000 >" -- 	Vector No: 64
severity warning;

wait for 1 ns;

-- ************************		
--------------------------

clk <= '1'; --	 Cycle No: 130

wait for 1 ns;

I <= "010000111";
D <= "1111";
Badd <=  2 ;
C0 <= '0';
OEbar <= '0';

wait for 4 ns;

clk <= '0';

wait for 4 ns;

wait for 1 ns;

--------------------------

clk <= '1'; --	 Cycle No: 131

wait for 1 ns;

I <= "001000100";
Aadd <=  2 ;
C0 <= '0';
OEbar <= '0';

wait for 4 ns;

clk <= '0';

wait for 4 ns;

assert (Y = "1111")
report
"Assert 319 : < Y /= 1111 >" -- 	Vector No: 65
severity warning;

wait for 1 ns;

-- ************************		
--------------------------

clk <= '1'; --	 Cycle No: 132

wait for 1 ns;

I <= "011000111";
D <= "1111";
Badd <=  2 ;
C0 <= '0';
OEbar <= '0';

wait for 4 ns;

clk <= '0';

wait for 4 ns;

wait for 1 ns;

--------------------------

clk <= '1'; --	 Cycle No: 133

wait for 1 ns;

I <= "001000100";
Badd <=  2 ;
C0 <= '0';
OEbar <= '0';

wait for 4 ns;

clk <= '0';

wait for 4 ns;

assert (Y = "1111")
report
"Assert 320 : < Y /= 1111 >" -- 	Vector No: 66
severity warning;

wait for 1 ns;

-- ************************	
--------------------------

clk <= '1'; --	 Cycle No: 134

wait for 1 ns;

I <= "010000111";
D <= "0000";
Badd <=  3 ;
C0 <= '0';
OEbar <= '0';

wait for 4 ns;

clk <= '0';

wait for 4 ns;

wait for 1 ns;

--------------------------

clk <= '1'; --	 Cycle No: 135

wait for 1 ns;

I <= "001000100";
Aadd <=  3 ;
C0 <= '0';
OEbar <= '0';

wait for 4 ns;

clk <= '0';

wait for 4 ns;

assert (Y = "0000")
report
"Assert 321 : < Y /= 0000 >" -- 	Vector No: 67
severity warning;

wait for 1 ns;

-- ************************		
--------------------------

clk <= '1'; --	 Cycle No: 136

wait for 1 ns;

I <= "011000111";
D <= "0000";
Badd <=  3 ;
C0 <= '0';
OEbar <= '0';

wait for 4 ns;

clk <= '0';

wait for 4 ns;

wait for 1 ns;

--------------------------

clk <= '1'; --	 Cycle No: 137

wait for 1 ns;

I <= "001000100";
Badd <=  3 ;
C0 <= '0';
OEbar <= '0';

wait for 4 ns;

clk <= '0';

wait for 4 ns;

assert (Y = "0000")
report
"Assert 322 : < Y /= 0000 >" -- 	Vector No: 68
severity warning;

wait for 1 ns;

-- ************************		
--------------------------

clk <= '1'; --	 Cycle No: 138

wait for 1 ns;

I <= "010000111";
D <= "1111";
Badd <=  3 ;
C0 <= '0';
OEbar <= '0';

wait for 4 ns;

clk <= '0';

wait for 4 ns;

wait for 1 ns;

--------------------------

clk <= '1'; --	 Cycle No: 139

wait for 1 ns;

I <= "001000100";
Aadd <=  3 ;
C0 <= '0';
OEbar <= '0';

wait for 4 ns;

clk <= '0';

wait for 4 ns;

assert (Y = "1111")
report
"Assert 323 : < Y /= 1111 >" -- 	Vector No: 69
severity warning;

wait for 1 ns;

-- ************************		
--------------------------

clk <= '1'; --	 Cycle No: 140

wait for 1 ns;

I <= "011000111";
D <= "1111";
Badd <=  3 ;
C0 <= '0';
OEbar <= '0';

wait for 4 ns;

clk <= '0';

wait for 4 ns;

wait for 1 ns;

--------------------------

clk <= '1'; --	 Cycle No: 141

wait for 1 ns;

I <= "001000100";
Badd <=  3 ;
C0 <= '0';
OEbar <= '0';

wait for 4 ns;

clk <= '0';

wait for 4 ns;

assert (Y = "1111")
report
"Assert 324 : < Y /= 1111 >" -- 	Vector No: 70
severity warning;

wait for 1 ns;

-- ************************	
--------------------------

clk <= '1'; --	 Cycle No: 142

wait for 1 ns;

I <= "010000111";
D <= "0000";
Badd <=  4 ;
C0 <= '0';
OEbar <= '0';

wait for 4 ns;

clk <= '0';

wait for 4 ns;

wait for 1 ns;

--------------------------

clk <= '1'; --	 Cycle No: 143

wait for 1 ns;

I <= "001000100";
Aadd <=  4 ;
C0 <= '0';
OEbar <= '0';

wait for 4 ns;

clk <= '0';

wait for 4 ns;

assert (Y = "0000")
report
"Assert 325 : < Y /= 0000 >" -- 	Vector No: 71
severity warning;

wait for 1 ns;

-- ************************		
--------------------------

clk <= '1'; --	 Cycle No: 144

wait for 1 ns;

I <= "011000111";
D <= "0000";
Badd <=  4 ;
C0 <= '0';
OEbar <= '0';

wait for 4 ns;

clk <= '0';

wait for 4 ns;

wait for 1 ns;

--------------------------

clk <= '1'; --	 Cycle No: 145

wait for 1 ns;

I <= "001000100";
Badd <=  4 ;
C0 <= '0';
OEbar <= '0';

wait for 4 ns;

clk <= '0';

wait for 4 ns;

assert (Y = "0000")
report
"Assert 326 : < Y /= 0000 >" -- 	Vector No: 72
severity warning;

wait for 1 ns;

-- ************************		
--------------------------

clk <= '1'; --	 Cycle No: 146

wait for 1 ns;

I <= "010000111";
D <= "1111";
Badd <=  4 ;
C0 <= '0';
OEbar <= '0';

wait for 4 ns;

clk <= '0';

wait for 4 ns;

wait for 1 ns;

--------------------------

clk <= '1'; --	 Cycle No: 147

wait for 1 ns;

I <= "001000100";
Aadd <=  4 ;
C0 <= '0';
OEbar <= '0';

wait for 4 ns;

clk <= '0';

wait for 4 ns;

assert (Y = "1111")
report
"Assert 327 : < Y /= 1111 >" -- 	Vector No: 73
severity warning;

wait for 1 ns;

-- ************************		
--------------------------

clk <= '1'; --	 Cycle No: 148

wait for 1 ns;

I <= "011000111";
D <= "1111";
Badd <=  4 ;
C0 <= '0';
OEbar <= '0';

wait for 4 ns;

clk <= '0';

wait for 4 ns;

wait for 1 ns;

--------------------------

clk <= '1'; --	 Cycle No: 149

wait for 1 ns;

I <= "001000100";
Badd <=  4 ;
C0 <= '0';
OEbar <= '0';

wait for 4 ns;

clk <= '0';

wait for 4 ns;

assert (Y = "1111")
report
"Assert 328 : < Y /= 1111 >" -- 	Vector No: 74
severity warning;

wait for 1 ns;

-- ************************					
--------------------------

clk <= '1'; --	 Cycle No: 150

wait for 1 ns;

I <= "010000111";
D <= "0000";
Badd <=  5 ;
C0 <= '0';
OEbar <= '0';

wait for 4 ns;

clk <= '0';

wait for 4 ns;

wait for 1 ns;

--------------------------

clk <= '1'; --	 Cycle No: 151

wait for 1 ns;

I <= "001000100";
Aadd <=  5 ;
C0 <= '0';
OEbar <= '0';

wait for 4 ns;

clk <= '0';

wait for 4 ns;

assert (Y = "0000")
report
"Assert 329 : < Y /= 0000 >" -- 	Vector No: 75
severity warning;

wait for 1 ns;

-- ************************		
--------------------------

clk <= '1'; --	 Cycle No: 152

wait for 1 ns;

I <= "011000111";
D <= "0000";
Badd <=  5 ;
C0 <= '0';
OEbar <= '0';

wait for 4 ns;

clk <= '0';

wait for 4 ns;

wait for 1 ns;

--------------------------

clk <= '1'; --	 Cycle No: 153

wait for 1 ns;

I <= "001000100";
Badd <=  5 ;
C0 <= '0';
OEbar <= '0';

wait for 4 ns;

clk <= '0';

wait for 4 ns;

assert (Y = "0000")
report
"Assert 330 : < Y /= 0000 >" -- 	Vector No: 76
severity warning;

wait for 1 ns;

-- ************************		
--------------------------

clk <= '1'; --	 Cycle No: 154

wait for 1 ns;

I <= "010000111";
D <= "1111";
Badd <=  5 ;
C0 <= '0';
OEbar <= '0';

wait for 4 ns;

clk <= '0';

wait for 4 ns;

wait for 1 ns;

--------------------------

clk <= '1'; --	 Cycle No: 155

wait for 1 ns;

I <= "001000100";
Aadd <=  5 ;
C0 <= '0';
OEbar <= '0';

wait for 4 ns;

clk <= '0';

wait for 4 ns;

assert (Y = "1111")
report
"Assert 331 : < Y /= 1111 >" -- 	Vector No: 77
severity warning;

wait for 1 ns;

-- ************************		
--------------------------

clk <= '1'; --	 Cycle No: 156

wait for 1 ns;

I <= "011000111";
D <= "1111";
Badd <=  5 ;
C0 <= '0';
OEbar <= '0';

wait for 4 ns;

clk <= '0';

wait for 4 ns;

wait for 1 ns;

--------------------------

clk <= '1'; --	 Cycle No: 157

wait for 1 ns;

I <= "001000100";
Badd <=  5 ;
C0 <= '0';
OEbar <= '0';

wait for 4 ns;

clk <= '0';

wait for 4 ns;

assert (Y = "1111")
report
"Assert 332 : < Y /= 1111 >" -- 	Vector No: 78
severity warning;

wait for 1 ns;

-- ************************	
--------------------------

clk <= '1'; --	 Cycle No: 158

wait for 1 ns;

I <= "010000111";
D <= "0000";
Badd <=  6 ;
C0 <= '0';
OEbar <= '0';

wait for 4 ns;

clk <= '0';

wait for 4 ns;

wait for 1 ns;

--------------------------

clk <= '1'; --	 Cycle No: 159

wait for 1 ns;

I <= "001000100";
Aadd <=  6 ;
C0 <= '0';
OEbar <= '0';

wait for 4 ns;

clk <= '0';

wait for 4 ns;

assert (Y = "0000")
report
"Assert 333 : < Y /= 0000 >" -- 	Vector No: 79
severity warning;

wait for 1 ns;

-- ************************		
--------------------------

clk <= '1'; --	 Cycle No: 160

wait for 1 ns;

I <= "011000111";
D <= "0000";
Badd <=  6 ;
C0 <= '0';
OEbar <= '0';

wait for 4 ns;

clk <= '0';

wait for 4 ns;

wait for 1 ns;

--------------------------

clk <= '1'; --	 Cycle No: 161

wait for 1 ns;

I <= "001000100";
Badd <=  6 ;
C0 <= '0';
OEbar <= '0';

wait for 4 ns;

clk <= '0';

wait for 4 ns;

assert (Y = "0000")
report
"Assert 334 : < Y /= 0000 >" -- 	Vector No: 80
severity warning;

wait for 1 ns;

-- ************************		
--------------------------

clk <= '1'; --	 Cycle No: 162

wait for 1 ns;

I <= "010000111";
D <= "1111";
Badd <=  6 ;
C0 <= '0';
OEbar <= '0';

wait for 4 ns;

clk <= '0';

wait for 4 ns;

wait for 1 ns;

--------------------------

clk <= '1'; --	 Cycle No: 163

wait for 1 ns;

I <= "001000100";
Aadd <=  6 ;
C0 <= '0';
OEbar <= '0';

wait for 4 ns;

clk <= '0';

wait for 4 ns;

assert (Y = "1111")
report
"Assert 335 : < Y /= 1111 >" -- 	Vector No: 81
severity warning;

wait for 1 ns;

-- ************************		
--------------------------

clk <= '1'; --	 Cycle No: 164

wait for 1 ns;

I <= "011000111";
D <= "1111";
Badd <=  6 ;
C0 <= '0';
OEbar <= '0';

wait for 4 ns;

clk <= '0';

wait for 4 ns;

wait for 1 ns;

--------------------------

clk <= '1'; --	 Cycle No: 165

wait for 1 ns;

I <= "001000100";
Badd <=  6 ;
C0 <= '0';
OEbar <= '0';

wait for 4 ns;

clk <= '0';

wait for 4 ns;

assert (Y = "1111")
report
"Assert 336 : < Y /= 1111 >" -- 	Vector No: 82
severity warning;

wait for 1 ns;

-- ************************			
--------------------------

clk <= '1'; --	 Cycle No: 166

wait for 1 ns;

I <= "010000111";
D <= "0000";
Badd <=  7 ;
C0 <= '0';
OEbar <= '0';

wait for 4 ns;

clk <= '0';

wait for 4 ns;

wait for 1 ns;

--------------------------

clk <= '1'; --	 Cycle No: 167

wait for 1 ns;

I <= "001000100";
Aadd <=  7 ;
C0 <= '0';
OEbar <= '0';

wait for 4 ns;

clk <= '0';

wait for 4 ns;

assert (Y = "0000")
report
"Assert 337 : < Y /= 0000 >" -- 	Vector No: 83
severity warning;

wait for 1 ns;

-- ************************		
--------------------------

clk <= '1'; --	 Cycle No: 168

wait for 1 ns;

I <= "011000111";
D <= "0000";
Badd <=  7 ;
C0 <= '0';
OEbar <= '0';

wait for 4 ns;

clk <= '0';

wait for 4 ns;

wait for 1 ns;

--------------------------

clk <= '1'; --	 Cycle No: 169

wait for 1 ns;

I <= "001000100";
Badd <=  7 ;
C0 <= '0';
OEbar <= '0';

wait for 4 ns;

clk <= '0';

wait for 4 ns;

assert (Y = "0000")
report
"Assert 338 : < Y /= 0000 >" -- 	Vector No: 84
severity warning;

wait for 1 ns;

-- ************************		
--------------------------

clk <= '1'; --	 Cycle No: 170

wait for 1 ns;

I <= "010000111";
D <= "1111";
Badd <=  7 ;
C0 <= '0';
OEbar <= '0';

wait for 4 ns;

clk <= '0';

wait for 4 ns;

wait for 1 ns;

--------------------------

clk <= '1'; --	 Cycle No: 171

wait for 1 ns;

I <= "001000100";
Aadd <=  7 ;
C0 <= '0';
OEbar <= '0';

wait for 4 ns;

clk <= '0';

wait for 4 ns;

assert (Y = "1111")
report
"Assert 339 : < Y /= 1111 >" -- 	Vector No: 85
severity warning;

wait for 1 ns;

-- ************************		
--------------------------

clk <= '1'; --	 Cycle No: 172

wait for 1 ns;

I <= "011000111";
D <= "1111";
Badd <=  7 ;
C0 <= '0';
OEbar <= '0';

wait for 4 ns;

clk <= '0';

wait for 4 ns;

wait for 1 ns;

--------------------------

clk <= '1'; --	 Cycle No: 173

wait for 1 ns;

I <= "001000100";
Badd <=  7 ;
C0 <= '0';
OEbar <= '0';

wait for 4 ns;

clk <= '0';

wait for 4 ns;

assert (Y = "1111")
report
"Assert 340 : < Y /= 1111 >" -- 	Vector No: 86
severity warning;

wait for 1 ns;

-- ************************		
--------------------------

clk <= '1'; --	 Cycle No: 174

wait for 1 ns;

I <= "010000111";
D <= "0000";
Badd <=  8 ;
C0 <= '0';
OEbar <= '0';

wait for 4 ns;

clk <= '0';

wait for 4 ns;

wait for 1 ns;

--------------------------

clk <= '1'; --	 Cycle No: 175

wait for 1 ns;

I <= "001000100";
Aadd <=  8 ;
C0 <= '0';
OEbar <= '0';

wait for 4 ns;

clk <= '0';

wait for 4 ns;

assert (Y = "0000")
report
"Assert 341 : < Y /= 0000 >" -- 	Vector No: 87
severity warning;

wait for 1 ns;

-- ************************		
--------------------------

clk <= '1'; --	 Cycle No: 176

wait for 1 ns;

I <= "011000111";
D <= "0000";
Badd <=  8 ;
C0 <= '0';
OEbar <= '0';

wait for 4 ns;

clk <= '0';

wait for 4 ns;

wait for 1 ns;

--------------------------

clk <= '1'; --	 Cycle No: 177

wait for 1 ns;

I <= "001000100";
Badd <=  8 ;
C0 <= '0';
OEbar <= '0';

wait for 4 ns;

clk <= '0';

wait for 4 ns;

assert (Y = "0000")
report
"Assert 342 : < Y /= 0000 >" -- 	Vector No: 88
severity warning;

wait for 1 ns;

-- ************************		
--------------------------

clk <= '1'; --	 Cycle No: 178

wait for 1 ns;

I <= "010000111";
D <= "1111";
Badd <=  8 ;
C0 <= '0';
OEbar <= '0';

wait for 4 ns;

clk <= '0';

wait for 4 ns;

wait for 1 ns;

--------------------------

clk <= '1'; --	 Cycle No: 179

wait for 1 ns;

I <= "001000100";
Aadd <=  8 ;
C0 <= '0';
OEbar <= '0';

wait for 4 ns;

clk <= '0';

wait for 4 ns;

assert (Y = "1111")
report
"Assert 343 : < Y /= 1111 >" -- 	Vector No: 89
severity warning;

wait for 1 ns;

-- ************************		
--------------------------

clk <= '1'; --	 Cycle No: 180

wait for 1 ns;

I <= "011000111";
D <= "1111";
Badd <=  8 ;
C0 <= '0';
OEbar <= '0';

wait for 4 ns;

clk <= '0';

wait for 4 ns;

wait for 1 ns;

--------------------------

clk <= '1'; --	 Cycle No: 181

wait for 1 ns;

I <= "001000100";
Badd <=  8 ;
C0 <= '0';
OEbar <= '0';

wait for 4 ns;

clk <= '0';

wait for 4 ns;

assert (Y = "1111")
report
"Assert 344 : < Y /= 1111 >" -- 	Vector No: 90
severity warning;

wait for 1 ns;

-- ************************		
--------------------------

clk <= '1'; --	 Cycle No: 182

wait for 1 ns;

I <= "010000111";
D <= "0000";
Badd <=  9 ;
C0 <= '0';
OEbar <= '0';

wait for 4 ns;

clk <= '0';

wait for 4 ns;

wait for 1 ns;

--------------------------

clk <= '1'; --	 Cycle No: 183

wait for 1 ns;

I <= "001000100";
Aadd <=  9 ;
C0 <= '0';
OEbar <= '0';

wait for 4 ns;

clk <= '0';

wait for 4 ns;

assert (Y = "0000")
report
"Assert 345 : < Y /= 0000 >" -- 	Vector No: 91
severity warning;

wait for 1 ns;

-- ************************		
--------------------------

clk <= '1'; --	 Cycle No: 184

wait for 1 ns;

I <= "011000111";
D <= "0000";
Badd <=  9 ;
C0 <= '0';
OEbar <= '0';

wait for 4 ns;

clk <= '0';

wait for 4 ns;

wait for 1 ns;

--------------------------

clk <= '1'; --	 Cycle No: 185

wait for 1 ns;

I <= "001000100";
Badd <=  9 ;
C0 <= '0';
OEbar <= '0';

wait for 4 ns;

clk <= '0';

wait for 4 ns;

assert (Y = "0000")
report
"Assert 346 : < Y /= 0000 >" -- 	Vector No: 92
severity warning;

wait for 1 ns;

-- ************************		
--------------------------

clk <= '1'; --	 Cycle No: 186

wait for 1 ns;

I <= "010000111";
D <= "1111";
Badd <=  9 ;
C0 <= '0';
OEbar <= '0';

wait for 4 ns;

clk <= '0';

wait for 4 ns;

wait for 1 ns;

--------------------------

clk <= '1'; --	 Cycle No: 187

wait for 1 ns;

I <= "001000100";
Aadd <=  9 ;
C0 <= '0';
OEbar <= '0';

wait for 4 ns;

clk <= '0';

wait for 4 ns;

assert (Y = "1111")
report
"Assert 347 : < Y /= 1111 >" -- 	Vector No: 93
severity warning;

wait for 1 ns;

-- ************************		
--------------------------

clk <= '1'; --	 Cycle No: 188

wait for 1 ns;

I <= "011000111";
D <= "1111";
Badd <=  9 ;
C0 <= '0';
OEbar <= '0';

wait for 4 ns;

clk <= '0';

wait for 4 ns;

wait for 1 ns;

--------------------------

clk <= '1'; --	 Cycle No: 189

wait for 1 ns;

I <= "001000100";
Badd <=  9 ;
C0 <= '0';
OEbar <= '0';

wait for 4 ns;

clk <= '0';

wait for 4 ns;

assert (Y = "1111")
report
"Assert 348 : < Y /= 1111 >" -- 	Vector No: 94
severity warning;

wait for 1 ns;

-- ************************		
--------------------------

clk <= '1'; --	 Cycle No: 190

wait for 1 ns;

I <= "010000111";
D <= "0000";
Badd <=  10 ;
C0 <= '0';
OEbar <= '0';

wait for 4 ns;

clk <= '0';

wait for 4 ns;

wait for 1 ns;

--------------------------

clk <= '1'; --	 Cycle No: 191

wait for 1 ns;

I <= "001000100";
Aadd <=  10 ;
C0 <= '0';
OEbar <= '0';

wait for 4 ns;

clk <= '0';

wait for 4 ns;

assert (Y = "0000")
report
"Assert 349 : < Y /= 0000 >" -- 	Vector No: 95
severity warning;

wait for 1 ns;

-- ************************		
--------------------------

clk <= '1'; --	 Cycle No: 192

wait for 1 ns;

I <= "011000111";
D <= "0000";
Badd <=  10 ;
C0 <= '0';
OEbar <= '0';

wait for 4 ns;

clk <= '0';

wait for 4 ns;

wait for 1 ns;

--------------------------

clk <= '1'; --	 Cycle No: 193

wait for 1 ns;

I <= "001000100";
Badd <=  10 ;
C0 <= '0';
OEbar <= '0';

wait for 4 ns;

clk <= '0';

wait for 4 ns;

assert (Y = "0000")
report
"Assert 350 : < Y /= 0000 >" -- 	Vector No: 96
severity warning;

wait for 1 ns;

-- ************************		
--------------------------

clk <= '1'; --	 Cycle No: 194

wait for 1 ns;

I <= "010000111";
D <= "1111";
Badd <=  10 ;
C0 <= '0';
OEbar <= '0';

wait for 4 ns;

clk <= '0';

wait for 4 ns;

wait for 1 ns;

--------------------------

clk <= '1'; --	 Cycle No: 195

wait for 1 ns;

I <= "001000100";
Aadd <=  10 ;
C0 <= '0';
OEbar <= '0';

wait for 4 ns;

clk <= '0';

wait for 4 ns;

assert (Y = "1111")
report
"Assert 351 : < Y /= 1111 >" -- 	Vector No: 97
severity warning;

wait for 1 ns;

-- ************************		
--------------------------

clk <= '1'; --	 Cycle No: 196

wait for 1 ns;

I <= "011000111";
D <= "1111";
Badd <=  10 ;
C0 <= '0';
OEbar <= '0';

wait for 4 ns;

clk <= '0';

wait for 4 ns;

wait for 1 ns;

--------------------------

clk <= '1'; --	 Cycle No: 197

wait for 1 ns;

I <= "001000100";
Badd <=  10 ;
C0 <= '0';
OEbar <= '0';

wait for 4 ns;

clk <= '0';

wait for 4 ns;

assert (Y = "1111")
report
"Assert 352 : < Y /= 1111 >" -- 	Vector No: 98
severity warning;

wait for 1 ns;

-- ************************		
--------------------------

clk <= '1'; --	 Cycle No: 198

wait for 1 ns;

I <= "010000111";
D <= "0000";
Badd <=  11 ;
C0 <= '0';
OEbar <= '0';

wait for 4 ns;

clk <= '0';

wait for 4 ns;

wait for 1 ns;

--------------------------

clk <= '1'; --	 Cycle No: 199

wait for 1 ns;

I <= "001000100";
Aadd <=  11 ;
C0 <= '0';
OEbar <= '0';

wait for 4 ns;

clk <= '0';

wait for 4 ns;

assert (Y = "0000")
report
"Assert 353 : < Y /= 0000 >" -- 	Vector No: 99
severity warning;

wait for 1 ns;

-- ************************		
--------------------------

clk <= '1'; --	 Cycle No: 200

wait for 1 ns;

I <= "011000111";
D <= "0000";
Badd <=  11 ;
C0 <= '0';
OEbar <= '0';

wait for 4 ns;

clk <= '0';

wait for 4 ns;

wait for 1 ns;

--------------------------

clk <= '1'; --	 Cycle No: 201

wait for 1 ns;

I <= "001000100";
Badd <=  11 ;
C0 <= '0';
OEbar <= '0';

wait for 4 ns;

clk <= '0';

wait for 4 ns;

assert (Y = "0000")
report
"Assert 354 : < Y /= 0000 >" -- 	Vector No: 100
severity warning;

wait for 1 ns;

-- ************************		
--------------------------

clk <= '1'; --	 Cycle No: 202

wait for 1 ns;

I <= "010000111";
D <= "1111";
Badd <=  11 ;
C0 <= '0';
OEbar <= '0';

wait for 4 ns;

clk <= '0';

wait for 4 ns;

wait for 1 ns;

--------------------------

clk <= '1'; --	 Cycle No: 203

wait for 1 ns;

I <= "001000100";
Aadd <=  11 ;
C0 <= '0';
OEbar <= '0';

wait for 4 ns;

clk <= '0';

wait for 4 ns;

assert (Y = "1111")
report
"Assert 355 : < Y /= 1111 >" -- 	Vector No: 101
severity warning;

wait for 1 ns;

-- ************************		
--------------------------

clk <= '1'; --	 Cycle No: 204

wait for 1 ns;

I <= "011000111";
D <= "1111";
Badd <=  11 ;
C0 <= '0';
OEbar <= '0';

wait for 4 ns;

clk <= '0';

wait for 4 ns;

wait for 1 ns;

--------------------------

clk <= '1'; --	 Cycle No: 205

wait for 1 ns;

I <= "001000100";
Badd <=  11 ;
C0 <= '0';
OEbar <= '0';

wait for 4 ns;

clk <= '0';

wait for 4 ns;

assert (Y = "1111")
report
"Assert 356 : < Y /= 1111 >" -- 	Vector No: 102
severity warning;

wait for 1 ns;

-- ************************		
--------------------------

clk <= '1'; --	 Cycle No: 206

wait for 1 ns;

I <= "010000111";
D <= "0000";
Badd <=  12 ;
C0 <= '0';
OEbar <= '0';

wait for 4 ns;

clk <= '0';

wait for 4 ns;

wait for 1 ns;

--------------------------

clk <= '1'; --	 Cycle No: 207

wait for 1 ns;

I <= "001000100";
Aadd <=  12 ;
C0 <= '0';
OEbar <= '0';

wait for 4 ns;

clk <= '0';

wait for 4 ns;

assert (Y = "0000")
report
"Assert 357 : < Y /= 0000 >" -- 	Vector No: 103
severity warning;

wait for 1 ns;

-- ************************		
--------------------------

clk <= '1'; --	 Cycle No: 208

wait for 1 ns;

I <= "011000111";
D <= "0000";
Badd <=  12 ;
C0 <= '0';
OEbar <= '0';

wait for 4 ns;

clk <= '0';

wait for 4 ns;

wait for 1 ns;

--------------------------

clk <= '1'; --	 Cycle No: 209

wait for 1 ns;

I <= "001000100";
Badd <=  12 ;
C0 <= '0';
OEbar <= '0';

wait for 4 ns;

clk <= '0';

wait for 4 ns;

assert (Y = "0000")
report
"Assert 358 : < Y /= 0000 >" -- 	Vector No: 104
severity warning;

wait for 1 ns;

-- ************************		
--------------------------

clk <= '1'; --	 Cycle No: 210

wait for 1 ns;

I <= "010000111";
D <= "1111";
Badd <=  12 ;
C0 <= '0';
OEbar <= '0';

wait for 4 ns;

clk <= '0';

wait for 4 ns;

wait for 1 ns;

--------------------------

clk <= '1'; --	 Cycle No: 211

wait for 1 ns;

I <= "001000100";
Aadd <=  12 ;
C0 <= '0';
OEbar <= '0';

wait for 4 ns;

clk <= '0';

wait for 4 ns;

assert (Y = "1111")
report
"Assert 359 : < Y /= 1111 >" -- 	Vector No: 105
severity warning;

wait for 1 ns;

-- ************************		
--------------------------

clk <= '1'; --	 Cycle No: 212

wait for 1 ns;

I <= "011000111";
D <= "1111";
Badd <=  12 ;
C0 <= '0';
OEbar <= '0';

wait for 4 ns;

clk <= '0';

wait for 4 ns;

wait for 1 ns;

--------------------------

clk <= '1'; --	 Cycle No: 213

wait for 1 ns;

I <= "001000100";
Badd <=  12 ;
C0 <= '0';
OEbar <= '0';

wait for 4 ns;

clk <= '0';

wait for 4 ns;

assert (Y = "1111")
report
"Assert 360 : < Y /= 1111 >" -- 	Vector No: 106
severity warning;

wait for 1 ns;

-- ************************		
--------------------------

clk <= '1'; --	 Cycle No: 214

wait for 1 ns;

I <= "010000111";
D <= "0000";
Badd <=  13 ;
C0 <= '0';
OEbar <= '0';

wait for 4 ns;

clk <= '0';

wait for 4 ns;

wait for 1 ns;

--------------------------

clk <= '1'; --	 Cycle No: 215

wait for 1 ns;

I <= "001000100";
Aadd <=  13 ;
C0 <= '0';
OEbar <= '0';

wait for 4 ns;

clk <= '0';

wait for 4 ns;

assert (Y = "0000")
report
"Assert 361 : < Y /= 0000 >" -- 	Vector No: 107
severity warning;

wait for 1 ns;

-- ************************		
--------------------------

clk <= '1'; --	 Cycle No: 216

wait for 1 ns;

I <= "011000111";
D <= "0000";
Badd <=  13 ;
C0 <= '0';
OEbar <= '0';

wait for 4 ns;

clk <= '0';

wait for 4 ns;

wait for 1 ns;

--------------------------

clk <= '1'; --	 Cycle No: 217

wait for 1 ns;

I <= "001000100";
Badd <=  13 ;
C0 <= '0';
OEbar <= '0';

wait for 4 ns;

clk <= '0';

wait for 4 ns;

assert (Y = "0000")
report
"Assert 362 : < Y /= 0000 >" -- 	Vector No: 108
severity warning;

wait for 1 ns;

-- ************************		
--------------------------

clk <= '1'; --	 Cycle No: 218

wait for 1 ns;

I <= "010000111";
D <= "1111";
Badd <=  13 ;
C0 <= '0';
OEbar <= '0';

wait for 4 ns;

clk <= '0';

wait for 4 ns;

wait for 1 ns;

--------------------------

clk <= '1'; --	 Cycle No: 219

wait for 1 ns;

I <= "001000100";
Aadd <=  13 ;
C0 <= '0';
OEbar <= '0';

wait for 4 ns;

clk <= '0';

wait for 4 ns;

assert (Y = "1111")
report
"Assert 363 : < Y /= 1111 >" -- 	Vector No: 109
severity warning;

wait for 1 ns;

-- ************************		
--------------------------

clk <= '1'; --	 Cycle No: 220

wait for 1 ns;

I <= "011000111";
D <= "1111";
Badd <=  13 ;
C0 <= '0';
OEbar <= '0';

wait for 4 ns;

clk <= '0';

wait for 4 ns;

wait for 1 ns;

--------------------------

clk <= '1'; --	 Cycle No: 221

wait for 1 ns;

I <= "001000100";
Badd <=  13 ;
C0 <= '0';
OEbar <= '0';

wait for 4 ns;

clk <= '0';

wait for 4 ns;

assert (Y = "1111")
report
"Assert 364 : < Y /= 1111 >" -- 	Vector No: 110
severity warning;

wait for 1 ns;

-- ************************		
--------------------------

clk <= '1'; --	 Cycle No: 222

wait for 1 ns;

I <= "010000111";
D <= "0000";
Badd <=  14 ;
C0 <= '0';
OEbar <= '0';

wait for 4 ns;

clk <= '0';

wait for 4 ns;

wait for 1 ns;

--------------------------

clk <= '1'; --	 Cycle No: 223

wait for 1 ns;

I <= "001000100";
Aadd <=  14 ;
C0 <= '0';
OEbar <= '0';

wait for 4 ns;

clk <= '0';

wait for 4 ns;

assert (Y = "0000")
report
"Assert 365 : < Y /= 0000 >" -- 	Vector No: 111
severity warning;

wait for 1 ns;

-- ************************		
--------------------------

clk <= '1'; --	 Cycle No: 224

wait for 1 ns;

I <= "011000111";
D <= "0000";
Badd <=  14 ;
C0 <= '0';
OEbar <= '0';

wait for 4 ns;

clk <= '0';

wait for 4 ns;

wait for 1 ns;

--------------------------

clk <= '1'; --	 Cycle No: 225

wait for 1 ns;

I <= "001000100";
Badd <=  14 ;
C0 <= '0';
OEbar <= '0';

wait for 4 ns;

clk <= '0';

wait for 4 ns;

assert (Y = "0000")
report
"Assert 366 : < Y /= 0000 >" -- 	Vector No: 112
severity warning;

wait for 1 ns;

-- ************************		
--------------------------

clk <= '1'; --	 Cycle No: 226

wait for 1 ns;

I <= "010000111";
D <= "1111";
Badd <=  14 ;
C0 <= '0';
OEbar <= '0';

wait for 4 ns;

clk <= '0';

wait for 4 ns;

wait for 1 ns;

--------------------------

clk <= '1'; --	 Cycle No: 227

wait for 1 ns;

I <= "001000100";
Aadd <=  14 ;
C0 <= '0';
OEbar <= '0';

wait for 4 ns;

clk <= '0';

wait for 4 ns;

assert (Y = "1111")
report
"Assert 367 : < Y /= 1111 >" -- 	Vector No: 113
severity warning;

wait for 1 ns;

-- ************************		
--------------------------

clk <= '1'; --	 Cycle No: 228

wait for 1 ns;

I <= "011000111";
D <= "1111";
Badd <=  14 ;
C0 <= '0';
OEbar <= '0';

wait for 4 ns;

clk <= '0';

wait for 4 ns;

wait for 1 ns;

--------------------------

clk <= '1'; --	 Cycle No: 229

wait for 1 ns;

I <= "001000100";
Badd <=  14 ;
C0 <= '0';
OEbar <= '0';

wait for 4 ns;

clk <= '0';

wait for 4 ns;

assert (Y = "1111")
report
"Assert 368 : < Y /= 1111 >" -- 	Vector No: 114
severity warning;

wait for 1 ns;

-- ************************		
--------------------------

clk <= '1'; --	 Cycle No: 230

wait for 1 ns;

I <= "010000111";
D <= "0000";
Badd <=  15 ;
C0 <= '0';
OEbar <= '0';

wait for 4 ns;

clk <= '0';

wait for 4 ns;

wait for 1 ns;

--------------------------

clk <= '1'; --	 Cycle No: 231

wait for 1 ns;

I <= "001000100";
Aadd <=  15 ;
C0 <= '0';
OEbar <= '0';

wait for 4 ns;

clk <= '0';

wait for 4 ns;

assert (Y = "0000")
report
"Assert 369 : < Y /= 0000 >" -- 	Vector No: 115
severity warning;

wait for 1 ns;

-- ************************		
--------------------------

clk <= '1'; --	 Cycle No: 232

wait for 1 ns;

I <= "011000111";
D <= "0000";
Badd <=  15 ;
C0 <= '0';
OEbar <= '0';

wait for 4 ns;

clk <= '0';

wait for 4 ns;

wait for 1 ns;

--------------------------

clk <= '1'; --	 Cycle No: 233

wait for 1 ns;

I <= "001000100";
Badd <=  15 ;
C0 <= '0';
OEbar <= '0';

wait for 4 ns;

clk <= '0';

wait for 4 ns;

assert (Y = "0000")
report
"Assert 370 : < Y /= 0000 >" -- 	Vector No: 116
severity warning;

wait for 1 ns;

-- ************************		
--------------------------

clk <= '1'; --	 Cycle No: 234

wait for 1 ns;

I <= "010000111";
D <= "1111";
Badd <=  15 ;
C0 <= '0';
OEbar <= '0';

wait for 4 ns;

clk <= '0';

wait for 4 ns;

wait for 1 ns;

--------------------------

clk <= '1'; --	 Cycle No: 235

wait for 1 ns;

I <= "001000100";
Aadd <=  15 ;
C0 <= '0';
OEbar <= '0';

wait for 4 ns;

clk <= '0';

wait for 4 ns;

assert (Y = "1111")
report
"Assert 371 : < Y /= 1111 >" -- 	Vector No: 117
severity warning;

wait for 1 ns;

-- ************************		
--------------------------

clk <= '1'; --	 Cycle No: 236

wait for 1 ns;

I <= "011000111";
D <= "1111";
Badd <=  15 ;
C0 <= '0';
OEbar <= '0';

wait for 4 ns;

clk <= '0';

wait for 4 ns;

wait for 1 ns;

--------------------------

clk <= '1'; --	 Cycle No: 237

wait for 1 ns;

I <= "001000100";
Badd <=  15 ;
C0 <= '0';
OEbar <= '0';

wait for 4 ns;

clk <= '0';

wait for 4 ns;

assert (Y = "1111")
report
"Assert 372 : < Y /= 1111 >" -- 	Vector No: 118
severity warning;

wait for 1 ns;

-- ************************		
-- ******** TESTING RAM SHIFTERS ********		
--------------------------

clk <= '1'; --	 Cycle No: 238

wait for 1 ns;

I <= "100000111";
D <= "0000";
Badd <=  0 ;
C0 <= '0';
OEbar <= '0';
RAM0 <= 'Z';
RAM3 <= '1';

wait for 4 ns;

clk <= '0';

wait for 4 ns;

assert (RAM0 = '0')
report
"Assert 373 : < RAM0 /= 0 >" -- 	Vector No: 119
severity warning;

wait for 1 ns;

--------------------------

clk <= '1'; --	 Cycle No: 239

wait for 1 ns;

I <= "001000100";
Aadd <=  0 ;
C0 <= '0';
OEbar <= '0';

wait for 4 ns;

clk <= '0';

wait for 4 ns;

assert (Y = "1000")
report
"Assert 374 : < Y /= 1000 >" -- 	Vector No: 120
severity warning;

wait for 1 ns;

-- ************************		
--------------------------

clk <= '1'; --	 Cycle No: 240

wait for 1 ns;

I <= "100000111";
D <= "1111";
Badd <=  0 ;
C0 <= '0';
OEbar <= '0';
RAM0 <= 'Z';
RAM3 <= '0';

wait for 4 ns;

clk <= '0';

wait for 4 ns;

assert (RAM0 = '1')
report
"Assert 375 : < RAM0 /= 1 >" -- 	Vector No: 121
severity warning;

wait for 1 ns;

--------------------------

clk <= '1'; --	 Cycle No: 241

wait for 1 ns;

I <= "001000100";
Aadd <=  0 ;
C0 <= '0';
OEbar <= '0';

wait for 4 ns;

clk <= '0';

wait for 4 ns;

assert (Y = "0111")
report
"Assert 376 : < Y /= 0111 >" -- 	Vector No: 122
severity warning;

wait for 1 ns;

-- ************************		
--------------------------

clk <= '1'; --	 Cycle No: 242

wait for 1 ns;

I <= "101000111";
D <= "0000";
Badd <=  0 ;
C0 <= '0';
OEbar <= '0';
RAM0 <= 'Z';
RAM3 <= '1';

wait for 4 ns;

clk <= '0';

wait for 4 ns;

assert (RAM0 = '0')
report
"Assert 377 : < RAM0 /= 0 >" -- 	Vector No: 123
severity warning;

wait for 1 ns;

--------------------------

clk <= '1'; --	 Cycle No: 243

wait for 1 ns;

I <= "001000100";
Aadd <=  0 ;
C0 <= '0';
OEbar <= '0';

wait for 4 ns;

clk <= '0';

wait for 4 ns;

assert (Y = "1000")
report
"Assert 378 : < Y /= 1000 >" -- 	Vector No: 124
severity warning;

wait for 1 ns;

-- ************************		
--------------------------

clk <= '1'; --	 Cycle No: 244

wait for 1 ns;

I <= "101000111";
D <= "1111";
Badd <=  0 ;
C0 <= '0';
OEbar <= '0';
RAM0 <= 'Z';
RAM3 <= '0';

wait for 4 ns;

clk <= '0';

wait for 4 ns;

assert (RAM0 = '1')
report
"Assert 379 : < RAM0 /= 1 >" -- 	Vector No: 125
severity warning;

wait for 1 ns;

--------------------------

clk <= '1'; --	 Cycle No: 245

wait for 1 ns;

I <= "001000100";
Aadd <=  0 ;
C0 <= '0';
OEbar <= '0';

wait for 4 ns;

clk <= '0';

wait for 4 ns;

assert (Y = "0111")
report
"Assert 380 : < Y /= 0111 >" -- 	Vector No: 126
severity warning;

wait for 1 ns;

-- ************************		
--------------------------

clk <= '1'; --	 Cycle No: 246

wait for 1 ns;

I <= "110000111";
D <= "0000";
Badd <=  0 ;
C0 <= '0';
OEbar <= '0';
RAM0 <= '1';
RAM3 <= 'Z';

wait for 4 ns;

clk <= '0';

wait for 4 ns;

assert (RAM3 = '0')
report
"Assert 381 : < RAM3 /= 0 >" -- 	Vector No: 127
severity warning;

wait for 1 ns;

--------------------------

clk <= '1'; --	 Cycle No: 247

wait for 1 ns;

I <= "001000100";
Aadd <=  0 ;
C0 <= '0';
OEbar <= '0';

wait for 4 ns;

clk <= '0';

wait for 4 ns;

assert (Y = "0001")
report
"Assert 382 : < Y /= 0001 >" -- 	Vector No: 128
severity warning;

wait for 1 ns;

-- ************************		
--------------------------

clk <= '1'; --	 Cycle No: 248

wait for 1 ns;

I <= "110000111";
D <= "1111";
Badd <=  0 ;
C0 <= '0';
OEbar <= '0';
RAM0 <= '0';
RAM3 <= 'Z';

wait for 4 ns;

clk <= '0';

wait for 4 ns;

assert (RAM3 = '1')
report
"Assert 383 : < RAM3 /= 1 >" -- 	Vector No: 129
severity warning;

wait for 1 ns;

--------------------------

clk <= '1'; --	 Cycle No: 249

wait for 1 ns;

I <= "001000100";
Aadd <=  0 ;
C0 <= '0';
OEbar <= '0';

wait for 4 ns;

clk <= '0';

wait for 4 ns;

assert (Y = "1110")
report
"Assert 384 : < Y /= 1110 >" -- 	Vector No: 130
severity warning;

wait for 1 ns;

-- ************************		
--------------------------

clk <= '1'; --	 Cycle No: 250

wait for 1 ns;

I <= "111000111";
D <= "0000";
Badd <=  0 ;
C0 <= '0';
OEbar <= '0';
RAM0 <= '1';
RAM3 <= 'Z';

wait for 4 ns;

clk <= '0';

wait for 4 ns;

assert (RAM3 = '0')
report
"Assert 385 : < RAM3 /= 0 >" -- 	Vector No: 131
severity warning;

wait for 1 ns;

--------------------------

clk <= '1'; --	 Cycle No: 251

wait for 1 ns;

I <= "001000100";
Aadd <=  0 ;
C0 <= '0';
OEbar <= '0';

wait for 4 ns;

clk <= '0';

wait for 4 ns;

assert (Y = "0001")
report
"Assert 386 : < Y /= 0001 >" -- 	Vector No: 132
severity warning;

wait for 1 ns;

-- ************************		
--------------------------

clk <= '1'; --	 Cycle No: 252

wait for 1 ns;

I <= "111000111";
D <= "1111";
Badd <=  0 ;
C0 <= '0';
OEbar <= '0';
RAM0 <= '0';
RAM3 <= 'Z';

wait for 4 ns;

clk <= '0';

wait for 4 ns;

assert (RAM3 = '1')
report
"Assert 387 : < RAM3 /= 1 >" -- 	Vector No: 133
severity warning;

wait for 1 ns;

--------------------------

clk <= '1'; --	 Cycle No: 253

wait for 1 ns;

I <= "001000100";
Aadd <=  0 ;
C0 <= '0';
OEbar <= '0';

wait for 4 ns;

clk <= '0';

wait for 4 ns;

assert (Y = "1110")
report
"Assert 388 : < Y /= 1110 >" -- 	Vector No: 134
severity warning;

wait for 1 ns;

-- ************************
-- *******************************		
-- *                             *
-- * TEST VECTORS FOR Q-REGISTER *
-- *                             *
-- *******************************		
-- ******** WRITING 0's AND 1's ********		
--------------------------

clk <= '1'; --	 Cycle No: 254

wait for 1 ns;

I <= "000000111";
D <= "1111";
C0 <= '0';
OEbar <= '0';

wait for 4 ns;

clk <= '0';

wait for 4 ns;

wait for 1 ns;

--------------------------

clk <= '1'; --	 Cycle No: 255

wait for 1 ns;

I <= "001000010";
C0 <= '0';
OEbar <= '0';

wait for 4 ns;

clk <= '0';

wait for 4 ns;

assert (Y = "1111")
report
"Assert 389 : < Y /= 1111 >" -- 	Vector No: 135
severity warning;

wait for 1 ns;

-- ************************		
--------------------------

clk <= '1'; --	 Cycle No: 256

wait for 1 ns;

I <= "000000111";
D <= "0000";
C0 <= '0';
OEbar <= '0';

wait for 4 ns;

clk <= '0';

wait for 4 ns;

wait for 1 ns;

--------------------------

clk <= '1'; --	 Cycle No: 257

wait for 1 ns;

I <= "001000010";
C0 <= '0';
OEbar <= '0';

wait for 4 ns;

clk <= '0';

wait for 4 ns;

assert (Y = "0000")
report
"Assert 390 : < Y /= 0000 >" -- 	Vector No: 136
severity warning;

wait for 1 ns;

-- ************************		
-- ******** TESTING Q-REGISTER SHIFTERS ********		
--------------------------

clk <= '1'; --	 Cycle No: 258

wait for 1 ns;

I <= "000000111";
D <= "0000";
C0 <= '0';
OEbar <= '0';

wait for 4 ns;

clk <= '0';

wait for 4 ns;

wait for 1 ns;

--------------------------

clk <= '1'; --	 Cycle No: 259

wait for 1 ns;

I <= "100000111";
C0 <= '0';
OEbar <= '0';
Q0 <= 'Z';
Q3 <= '1';

wait for 4 ns;

clk <= '0';

wait for 4 ns;

assert (Q0 = '0')
report
"Assert 391 : < Q0 /= 0 >" -- 	Vector No: 137
severity warning;

wait for 1 ns;

--------------------------

clk <= '1'; --	 Cycle No: 260

wait for 1 ns;

I <= "001000010";
C0 <= '0';
OEbar <= '0';

wait for 4 ns;

clk <= '0';

wait for 4 ns;

assert (Y = "1000")
report
"Assert 392 : < Y /= 1000 >" -- 	Vector No: 138
severity warning;

wait for 1 ns;

-- ************************		
--------------------------

clk <= '1'; --	 Cycle No: 261

wait for 1 ns;

I <= "000000111";
D <= "1111";
C0 <= '0';
OEbar <= '0';

wait for 4 ns;

clk <= '0';

wait for 4 ns;

wait for 1 ns;

--------------------------

clk <= '1'; --	 Cycle No: 262

wait for 1 ns;

I <= "100000111";
C0 <= '0';
OEbar <= '0';
Q0 <= 'Z';
Q3 <= '0';

wait for 4 ns;

clk <= '0';

wait for 4 ns;

assert (Q0 = '1')
report
"Assert 393 : < Q0 /= 1 >" -- 	Vector No: 139
severity warning;

wait for 1 ns;

--------------------------

clk <= '1'; --	 Cycle No: 263

wait for 1 ns;

I <= "001000010";
C0 <= '0';
OEbar <= '0';

wait for 4 ns;

clk <= '0';

wait for 4 ns;

assert (Y = "0111")
report
"Assert 394 : < Y /= 0111 >" -- 	Vector No: 140
severity warning;

wait for 1 ns;

-- ************************		
--------------------------

clk <= '1'; --	 Cycle No: 264

wait for 1 ns;

I <= "000000111";
D <= "0000";
C0 <= '0';
OEbar <= '0';

wait for 4 ns;

clk <= '0';

wait for 4 ns;

wait for 1 ns;

--------------------------

clk <= '1'; --	 Cycle No: 265

wait for 1 ns;

I <= "110000111";
C0 <= '0';
OEbar <= '0';
Q0 <= '1';
Q3 <= 'Z';

wait for 4 ns;

clk <= '0';

wait for 4 ns;

assert (Q3 = '0')
report
"Assert 395 : < Q3 /= 0 >" -- 	Vector No: 141
severity warning;

wait for 1 ns;

--------------------------

clk <= '1'; --	 Cycle No: 266

wait for 1 ns;

I <= "001000010";
C0 <= '0';
OEbar <= '0';

wait for 4 ns;

clk <= '0';

wait for 4 ns;

assert (Y = "0001")
report
"Assert 396 : < Y /= 0001 >" -- 	Vector No: 142
severity warning;

wait for 1 ns;

-- ************************		
--------------------------

clk <= '1'; --	 Cycle No: 267

wait for 1 ns;

I <= "000000111";
D <= "1111";
C0 <= '0';
OEbar <= '0';

wait for 4 ns;

clk <= '0';

wait for 4 ns;

wait for 1 ns;

--------------------------

clk <= '1'; --	 Cycle No: 268

wait for 1 ns;

I <= "110000111";
C0 <= '0';
OEbar <= '0';
Q0 <= '0';
Q3 <= 'Z';

wait for 4 ns;

clk <= '0';

wait for 4 ns;

assert (Q3 = '1')
report
"Assert 397 : < Q3 /= 1 >" -- 	Vector No: 143
severity warning;

wait for 1 ns;

--------------------------

clk <= '1'; --	 Cycle No: 269

wait for 1 ns;

I <= "001000010";
C0 <= '0';
OEbar <= '0';

wait for 4 ns;

clk <= '0';

wait for 4 ns;

assert (Y = "1110")
report
"Assert 398 : < Y /= 1110 >" -- 	Vector No: 144
severity warning;

wait for 1 ns;

-- ************************		
-- ****************************************
-- *                                      *
-- * TEST VECTORS FOR ALU SOURCE-SELECTOR *
-- *                                      *
-- ****************************************
-- ******** TESTING FOR ALU SOURCE-SELECT ( R = A, S = Q) ********		
--------------------------

clk <= '1'; --	 Cycle No: 270

wait for 1 ns;

I <= "000000111";
D <= "0000";
C0 <= '0';
OEbar <= '0';

wait for 4 ns;

clk <= '0';

wait for 4 ns;

wait for 1 ns;

--------------------------

clk <= '1'; --	 Cycle No: 271

wait for 1 ns;

I <= "011000111";
D <= "1111";
Badd <=  0 ;
C0 <= '0';
OEbar <= '0';

wait for 4 ns;

clk <= '0';

wait for 4 ns;

wait for 1 ns;

--------------------------

clk <= '1'; --	 Cycle No: 272

wait for 1 ns;

I <= "001000000";
Aadd <=  0 ;
C0 <= '0';
OEbar <= '0';

wait for 4 ns;

clk <= '0';

wait for 4 ns;

assert (Y = "1111")
report
"Assert 399 : < Y /= 1111 >" -- 	Vector No: 145
severity warning;

assert (C4 = '0')
report
"Assert 400 : < C4 /= 0 >"
severity warning;

wait for 1 ns;

-- ************************
--------------------------

clk <= '1'; --	 Cycle No: 273

wait for 1 ns;

I <= "000000111";
D <= "1111";
C0 <= '0';
OEbar <= '0';

wait for 4 ns;

clk <= '0';

wait for 4 ns;

wait for 1 ns;

--------------------------

clk <= '1'; --	 Cycle No: 274

wait for 1 ns;

I <= "011000111";
D <= "0000";
Badd <=  0 ;
C0 <= '0';
OEbar <= '0';

wait for 4 ns;

clk <= '0';

wait for 4 ns;

wait for 1 ns;

--------------------------

clk <= '1'; --	 Cycle No: 275

wait for 1 ns;

I <= "001000000";
Aadd <=  0 ;
C0 <= '0';
OEbar <= '0';

wait for 4 ns;

clk <= '0';

wait for 4 ns;

assert (Y = "1111")
report
"Assert 401 : < Y /= 1111 >" -- 	Vector No: 146
severity warning;

assert (C4 = '0')
report
"Assert 402 : < C4 /= 0 >"
severity warning;

wait for 1 ns;

-- ************************
-- ******** TESTING FOR ALU SOURCE-SELECT ( R = A, S = B) ********		
--------------------------

clk <= '1'; --	 Cycle No: 276

wait for 1 ns;

I <= "011000111";
D <= "0000";
Badd <=  0 ;
C0 <= '0';
OEbar <= '0';

wait for 4 ns;

clk <= '0';

wait for 4 ns;

wait for 1 ns;

--------------------------

clk <= '1'; --	 Cycle No: 277

wait for 1 ns;

I <= "011000111";
D <= "1111";
Badd <=  1 ;
C0 <= '0';
OEbar <= '0';

wait for 4 ns;

clk <= '0';

wait for 4 ns;

wait for 1 ns;

--------------------------

clk <= '1'; --	 Cycle No: 278

wait for 1 ns;

I <= "001000001";
Aadd <=  0 ;
Badd <=  1 ;
C0 <= '0';
OEbar <= '0';

wait for 4 ns;

clk <= '0';

wait for 4 ns;

assert (Y = "1111")
report
"Assert 403 : < Y /= 1111 >" -- 	Vector No: 147
severity warning;

assert (C4 = '0')
report
"Assert 404 : < C4 /= 0 >"
severity warning;

wait for 1 ns;

-- ************************
--------------------------

clk <= '1'; --	 Cycle No: 279

wait for 1 ns;

I <= "011000111";
D <= "1111";
Badd <=  0 ;
C0 <= '0';
OEbar <= '0';

wait for 4 ns;

clk <= '0';

wait for 4 ns;

wait for 1 ns;

--------------------------

clk <= '1'; --	 Cycle No: 280

wait for 1 ns;

I <= "011000111";
D <= "0000";
Badd <=  1 ;
C0 <= '0';
OEbar <= '0';

wait for 4 ns;

clk <= '0';

wait for 4 ns;

wait for 1 ns;

--------------------------

clk <= '1'; --	 Cycle No: 281

wait for 1 ns;

I <= "001000001";
Aadd <=  0 ;
Badd <=  1 ;
C0 <= '0';
OEbar <= '0';

wait for 4 ns;

clk <= '0';

wait for 4 ns;

assert (Y = "1111")
report
"Assert 405 : < Y /= 1111 >" -- 	Vector No: 148
severity warning;

assert (C4 = '0')
report
"Assert 406 : < C4 /= 0 >"
severity warning;

wait for 1 ns;

-- ************************
-- ******** TESTING FOR ALU SOURCE-SELECT ( R = 0, S = Q) ********		
--------------------------

clk <= '1'; --	 Cycle No: 282

wait for 1 ns;

I <= "000000111";
D <= "0000";
C0 <= '0';
OEbar <= '0';

wait for 4 ns;

clk <= '0';

wait for 4 ns;

wait for 1 ns;

--------------------------

clk <= '1'; --	 Cycle No: 283

wait for 1 ns;

I <= "001000010";
C0 <= '0';
OEbar <= '0';

wait for 4 ns;

clk <= '0';

wait for 4 ns;

assert (Y = "0000")
report
"Assert 407 : < Y /= 0000 >" -- 	Vector No: 149
severity warning;

assert (C4 = '0')
report
"Assert 408 : < C4 /= 0 >"
severity warning;

wait for 1 ns;

-- ************************
--------------------------

clk <= '1'; --	 Cycle No: 284

wait for 1 ns;

I <= "000000111";
D <= "1111";
C0 <= '0';
OEbar <= '0';

wait for 4 ns;

clk <= '0';

wait for 4 ns;

wait for 1 ns;

--------------------------

clk <= '1'; --	 Cycle No: 285

wait for 1 ns;

I <= "001000010";
C0 <= '0';
OEbar <= '0';

wait for 4 ns;

clk <= '0';

wait for 4 ns;

assert (Y = "1111")
report
"Assert 409 : < Y /= 1111 >" -- 	Vector No: 150
severity warning;

assert (C4 = '0')
report
"Assert 410 : < C4 /= 0 >"
severity warning;

wait for 1 ns;

-- ************************
-- ******** TESTING FOR ALU SOURCE-SELECT ( R = 0, S = B) ********		
--------------------------

clk <= '1'; --	 Cycle No: 286

wait for 1 ns;

I <= "011000111";
D <= "0000";
Badd <=  0 ;
C0 <= '0';
OEbar <= '0';

wait for 4 ns;

clk <= '0';

wait for 4 ns;

wait for 1 ns;

--------------------------

clk <= '1'; --	 Cycle No: 287

wait for 1 ns;

I <= "001000011";
Badd <=  0 ;
C0 <= '0';
OEbar <= '0';

wait for 4 ns;

clk <= '0';

wait for 4 ns;

assert (Y = "0000")
report
"Assert 411 : < Y /= 0000 >" -- 	Vector No: 151
severity warning;

assert (C4 = '0')
report
"Assert 412 : < C4 /= 0 >"
severity warning;

wait for 1 ns;

-- ************************
--------------------------

clk <= '1'; --	 Cycle No: 288

wait for 1 ns;

I <= "011000111";
D <= "1111";
Badd <=  0 ;
C0 <= '0';
OEbar <= '0';

wait for 4 ns;

clk <= '0';

wait for 4 ns;

wait for 1 ns;

--------------------------

clk <= '1'; --	 Cycle No: 289

wait for 1 ns;

I <= "001000011";
Badd <=  0 ;
C0 <= '0';
OEbar <= '0';

wait for 4 ns;

clk <= '0';

wait for 4 ns;

assert (Y = "1111")
report
"Assert 413 : < Y /= 1111 >" -- 	Vector No: 152
severity warning;

assert (C4 = '0')
report
"Assert 414 : < C4 /= 0 >"
severity warning;

wait for 1 ns;

-- ************************
-- ******** TESTING FOR ALU SOURCE-SELECT ( R = 0, S = A) ********		
--------------------------

clk <= '1'; --	 Cycle No: 290

wait for 1 ns;

I <= "011000111";
D <= "0000";
Badd <=  0 ;
C0 <= '0';
OEbar <= '0';

wait for 4 ns;

clk <= '0';

wait for 4 ns;

wait for 1 ns;

--------------------------

clk <= '1'; --	 Cycle No: 291

wait for 1 ns;

I <= "001000100";
Badd <=  0 ;
C0 <= '0';
OEbar <= '0';

wait for 4 ns;

clk <= '0';

wait for 4 ns;

assert (Y = "0000")
report
"Assert 415 : < Y /= 0000 >" -- 	Vector No: 153
severity warning;

assert (C4 = '0')
report
"Assert 416 : < C4 /= 0 >"
severity warning;

wait for 1 ns;

-- ************************
--------------------------

clk <= '1'; --	 Cycle No: 292

wait for 1 ns;

I <= "011000111";
D <= "1111";
Badd <=  0 ;
C0 <= '0';
OEbar <= '0';

wait for 4 ns;

clk <= '0';

wait for 4 ns;

wait for 1 ns;

--------------------------

clk <= '1'; --	 Cycle No: 293

wait for 1 ns;

I <= "001000100";
Aadd <=  0 ;
C0 <= '0';
OEbar <= '0';

wait for 4 ns;

clk <= '0';

wait for 4 ns;

assert (Y = "1111")
report
"Assert 417 : < Y /= 1111 >" -- 	Vector No: 154
severity warning;

assert (C4 = '0')
report
"Assert 418 : < C4 /= 0 >"
severity warning;

wait for 1 ns;

-- ************************
-- ******** TESTING FOR ALU SOURCE-SELECT ( R = D, S = A) ********		
--------------------------

clk <= '1'; --	 Cycle No: 294

wait for 1 ns;

I <= "011000111";
D <= "0000";
Badd <=  0 ;
C0 <= '0';
OEbar <= '0';

wait for 4 ns;

clk <= '0';

wait for 4 ns;

wait for 1 ns;

--------------------------

clk <= '1'; --	 Cycle No: 295

wait for 1 ns;

I <= "001000101";
D <= "1111";
Aadd <=  0 ;
C0 <= '0';
OEbar <= '0';

wait for 4 ns;

clk <= '0';

wait for 4 ns;

assert (Y = "1111")
report
"Assert 419 : < Y /= 1111 >" -- 	Vector No: 155
severity warning;

assert (C4 = '0')
report
"Assert 420 : < C4 /= 0 >"
severity warning;

wait for 1 ns;

-- ************************
--------------------------

clk <= '1'; --	 Cycle No: 296

wait for 1 ns;

I <= "011000111";
D <= "1111";
Badd <=  0 ;
C0 <= '0';
OEbar <= '0';

wait for 4 ns;

clk <= '0';

wait for 4 ns;

wait for 1 ns;

--------------------------

clk <= '1'; --	 Cycle No: 297

wait for 1 ns;

I <= "001000101";
D <= "0000";
Aadd <=  0 ;
C0 <= '0';
OEbar <= '0';

wait for 4 ns;

clk <= '0';

wait for 4 ns;

assert (Y = "1111")
report
"Assert 421 : < Y /= 1111 >" -- 	Vector No: 156
severity warning;

assert (C4 = '0')
report
"Assert 422 : < C4 /= 0 >"
severity warning;

wait for 1 ns;

-- ************************
-- ******** TESTING FOR ALU SOURCE-SELECT ( R = D, S = Q) ********		
--------------------------

clk <= '1'; --	 Cycle No: 298

wait for 1 ns;

I <= "000000111";
D <= "1111";
C0 <= '0';
OEbar <= '0';

wait for 4 ns;

clk <= '0';

wait for 4 ns;

wait for 1 ns;

--------------------------

clk <= '1'; --	 Cycle No: 299

wait for 1 ns;

I <= "001000110";
D <= "0000";
C0 <= '0';
OEbar <= '0';

wait for 4 ns;

clk <= '0';

wait for 4 ns;

assert (Y = "1111")
report
"Assert 423 : < Y /= 1111 >" -- 	Vector No: 157
severity warning;

assert (C4 = '0')
report
"Assert 424 : < C4 /= 0 >"
severity warning;

wait for 1 ns;

-- ************************
--------------------------

clk <= '1'; --	 Cycle No: 300

wait for 1 ns;

I <= "000000111";
D <= "0000";
C0 <= '0';
OEbar <= '0';

wait for 4 ns;

clk <= '0';

wait for 4 ns;

wait for 1 ns;

--------------------------

clk <= '1'; --	 Cycle No: 301

wait for 1 ns;

I <= "001000110";
D <= "1111";
C0 <= '0';
OEbar <= '0';

wait for 4 ns;

clk <= '0';

wait for 4 ns;

assert (Y = "1111")
report
"Assert 425 : < Y /= 1111 >" -- 	Vector No: 158
severity warning;

assert (C4 = '0')
report
"Assert 426 : < C4 /= 0 >"
severity warning;

wait for 1 ns;

-- ************************
-- ******** TESTING FOR ALU SOURCE-SELECT ( R = D, S = 0) ********		
--------------------------

clk <= '1'; --	 Cycle No: 302

wait for 1 ns;

I <= "001000111";
D <= "0000";
C0 <= '0';
OEbar <= '0';

wait for 4 ns;

clk <= '0';

wait for 4 ns;

assert (Y = "0000")
report
"Assert 427 : < Y /= 0000 >" -- 	Vector No: 159
severity warning;

wait for 1 ns;

-- ************************
--------------------------

clk <= '1'; --	 Cycle No: 303

wait for 1 ns;

I <= "001000111";
D <= "1111";
C0 <= '0';
OEbar <= '0';

wait for 4 ns;

clk <= '0';

wait for 4 ns;

assert (Y = "1111")
report
"Assert 428 : < Y /= 1111 >" -- 	Vector No: 160
severity warning;

wait for 1 ns;

-- ************************
-- ************************************
-- *                                  *
-- * TEST VECTORS FOR OUTPUT-SELECTOR *
-- *                                  *
-- ************************************
-- ******** TESTING OUTPUT FOR (I8 I7 I6 = 000) ********		
--------------------------

clk <= '1'; --	 Cycle No: 304

wait for 1 ns;

I <= "000000111";
D <= "0000";
C0 <= '0';
OEbar <= '0';

wait for 4 ns;

clk <= '0';

wait for 4 ns;

assert (Y = "0000")
report
"Assert 429 : < Y /= 0000 >" -- 	Vector No: 161
severity warning;

wait for 1 ns;

-- ************************
--------------------------

clk <= '1'; --	 Cycle No: 305

wait for 1 ns;

I <= "000000111";
D <= "1111";
C0 <= '0';
OEbar <= '0';

wait for 4 ns;

clk <= '0';

wait for 4 ns;

assert (Y = "1111")
report
"Assert 430 : < Y /= 1111 >" -- 	Vector No: 162
severity warning;

wait for 1 ns;

-- ************************
--------------------------

clk <= '1'; --	 Cycle No: 306

wait for 1 ns;

I <= "000000111";
D <= "1111";
C0 <= '0';
OEbar <= '1';

wait for 4 ns;

clk <= '0';

wait for 4 ns;

assert (Y = "ZZZZ")
report
"Assert 431 : < Y /= ZZZZ >" -- 	Vector No: 163
severity warning;

wait for 1 ns;

-- ************************
-- ******** TESTING OUTPUT FOR (I8 I7 I6 = 001) ********		
--------------------------

clk <= '1'; --	 Cycle No: 307

wait for 1 ns;

I <= "001000111";
D <= "0000";
C0 <= '0';
OEbar <= '0';

wait for 4 ns;

clk <= '0';

wait for 4 ns;

assert (Y = "0000")
report
"Assert 432 : < Y /= 0000 >" -- 	Vector No: 164
severity warning;

wait for 1 ns;

-- ************************
--------------------------

clk <= '1'; --	 Cycle No: 308

wait for 1 ns;

I <= "001000111";
D <= "1111";
C0 <= '0';
OEbar <= '0';

wait for 4 ns;

clk <= '0';

wait for 4 ns;

assert (Y = "1111")
report
"Assert 433 : < Y /= 1111 >" -- 	Vector No: 165
severity warning;

wait for 1 ns;

-- ************************
--------------------------

clk <= '1'; --	 Cycle No: 309

wait for 1 ns;

I <= "001000111";
D <= "1111";
C0 <= '0';
OEbar <= '1';

wait for 4 ns;

clk <= '0';

wait for 4 ns;

assert (Y = "ZZZZ")
report
"Assert 434 : < Y /= ZZZZ >" -- 	Vector No: 166
severity warning;

wait for 1 ns;

-- ************************
-- ******** TESTING OUTPUT FOR (I8 I7 I6 = 010) ********		
--------------------------

clk <= '1'; --	 Cycle No: 310

wait for 1 ns;

I <= "011000111";
D <= "0000";
Badd <=  0 ;
C0 <= '0';
OEbar <= '0';

wait for 4 ns;

clk <= '0';

wait for 4 ns;

wait for 1 ns;

--------------------------

clk <= '1'; --	 Cycle No: 311

wait for 1 ns;

I <= "010000111";
Aadd <=  0 ;
C0 <= '0';
OEbar <= '0';

wait for 4 ns;

clk <= '0';

wait for 4 ns;

assert (Y = "0000")
report
"Assert 435 : < Y /= 0000 >" -- 	Vector No: 167
severity warning;

wait for 1 ns;

-- ************************
--------------------------

clk <= '1'; --	 Cycle No: 312

wait for 1 ns;

I <= "011000111";
D <= "1111";
Badd <=  0 ;
C0 <= '0';
OEbar <= '0';

wait for 4 ns;

clk <= '0';

wait for 4 ns;

wait for 1 ns;

--------------------------

clk <= '1'; --	 Cycle No: 313

wait for 1 ns;

I <= "010000111";
Aadd <=  0 ;
C0 <= '0';
OEbar <= '0';

wait for 4 ns;

clk <= '0';

wait for 4 ns;

assert (Y = "1111")
report
"Assert 436 : < Y /= 1111 >" -- 	Vector No: 168
severity warning;

wait for 1 ns;

-- ************************
--------------------------

clk <= '1'; --	 Cycle No: 314

wait for 1 ns;

I <= "011000111";
D <= "1111";
Badd <=  0 ;
C0 <= '0';
OEbar <= '0';

wait for 4 ns;

clk <= '0';

wait for 4 ns;

wait for 1 ns;

--------------------------

clk <= '1'; --	 Cycle No: 315

wait for 1 ns;

I <= "010000111";
Aadd <=  0 ;
C0 <= '0';
OEbar <= '1';

wait for 4 ns;

clk <= '0';

wait for 4 ns;

assert (Y = "ZZZZ")
report
"Assert 437 : < Y /= ZZZZ >" -- 	Vector No: 169
severity warning;

wait for 1 ns;

-- ************************
-- ******** TESTING OUTPUT FOR (I8 I7 I6 = 011) ********		
--------------------------

clk <= '1'; --	 Cycle No: 316

wait for 1 ns;

I <= "011000111";
D <= "0000";
C0 <= '0';
OEbar <= '0';

wait for 4 ns;

clk <= '0';

wait for 4 ns;

assert (Y = "0000")
report
"Assert 438 : < Y /= 0000 >" -- 	Vector No: 170
severity warning;

wait for 1 ns;

-- ************************
--------------------------

clk <= '1'; --	 Cycle No: 317

wait for 1 ns;

I <= "011000111";
D <= "1111";
C0 <= '0';
OEbar <= '0';

wait for 4 ns;

clk <= '0';

wait for 4 ns;

assert (Y = "1111")
report
"Assert 439 : < Y /= 1111 >" -- 	Vector No: 171
severity warning;

wait for 1 ns;

-- ************************
--------------------------

clk <= '1'; --	 Cycle No: 318

wait for 1 ns;

I <= "011000111";
D <= "1111";
C0 <= '0';
OEbar <= '1';

wait for 4 ns;

clk <= '0';

wait for 4 ns;

assert (Y = "ZZZZ")
report
"Assert 440 : < Y /= ZZZZ >" -- 	Vector No: 172
severity warning;

wait for 1 ns;

-- ************************
-- ******** TESTING OUTPUT FOR (I8 I7 I6 = 100) ********		
--------------------------

clk <= '1'; --	 Cycle No: 319

wait for 1 ns;

I <= "100000111";
D <= "0000";
C0 <= '0';
OEbar <= '0';

wait for 4 ns;

clk <= '0';

wait for 4 ns;

assert (Y = "0000")
report
"Assert 441 : < Y /= 0000 >" -- 	Vector No: 173
severity warning;

wait for 1 ns;

-- ************************
--------------------------

clk <= '1'; --	 Cycle No: 320

wait for 1 ns;

I <= "100000111";
D <= "1111";
C0 <= '0';
OEbar <= '0';

wait for 4 ns;

clk <= '0';

wait for 4 ns;

assert (Y = "1111")
report
"Assert 442 : < Y /= 1111 >" -- 	Vector No: 174
severity warning;

wait for 1 ns;

-- ************************
--------------------------

clk <= '1'; --	 Cycle No: 321

wait for 1 ns;

I <= "100000111";
D <= "1111";
C0 <= '0';
OEbar <= '1';

wait for 4 ns;

clk <= '0';

wait for 4 ns;

assert (Y = "ZZZZ")
report
"Assert 443 : < Y /= ZZZZ >" -- 	Vector No: 175
severity warning;

wait for 1 ns;

-- ************************
-- ******** TESTING OUTPUT FOR (I8 I7 I6 = 101) ********		
--------------------------

clk <= '1'; --	 Cycle No: 322

wait for 1 ns;

I <= "101000111";
D <= "0000";
C0 <= '0';
OEbar <= '0';

wait for 4 ns;

clk <= '0';

wait for 4 ns;

assert (Y = "0000")
report
"Assert 444 : < Y /= 0000 >" -- 	Vector No: 176
severity warning;

wait for 1 ns;

-- ************************
--------------------------

clk <= '1'; --	 Cycle No: 323

wait for 1 ns;

I <= "101000111";
D <= "1111";
C0 <= '0';
OEbar <= '0';

wait for 4 ns;

clk <= '0';

wait for 4 ns;

assert (Y = "1111")
report
"Assert 445 : < Y /= 1111 >" -- 	Vector No: 177
severity warning;

wait for 1 ns;

-- ************************
--------------------------

clk <= '1'; --	 Cycle No: 324

wait for 1 ns;

I <= "101000111";
D <= "1111";
C0 <= '0';
OEbar <= '1';

wait for 4 ns;

clk <= '0';

wait for 4 ns;

assert (Y = "ZZZZ")
report
"Assert 446 : < Y /= ZZZZ >" -- 	Vector No: 178
severity warning;

wait for 1 ns;

-- ************************
-- ******** TESTING OUTPUT FOR (I8 I7 I6 = 110) ********		
--------------------------

clk <= '1'; --	 Cycle No: 325

wait for 1 ns;

I <= "110000111";
D <= "0000";
C0 <= '0';
OEbar <= '0';

wait for 4 ns;

clk <= '0';

wait for 4 ns;

assert (Y = "0000")
report
"Assert 447 : < Y /= 0000 >" -- 	Vector No: 179
severity warning;

wait for 1 ns;

-- ************************
--------------------------

clk <= '1'; --	 Cycle No: 326

wait for 1 ns;

I <= "110000111";
D <= "1111";
C0 <= '0';
OEbar <= '0';

wait for 4 ns;

clk <= '0';

wait for 4 ns;

assert (Y = "1111")
report
"Assert 448 : < Y /= 1111 >" -- 	Vector No: 180
severity warning;

wait for 1 ns;

-- ************************
--------------------------

clk <= '1'; --	 Cycle No: 327

wait for 1 ns;

I <= "110000111";
D <= "1111";
C0 <= '0';
OEbar <= '1';

wait for 4 ns;

clk <= '0';

wait for 4 ns;

assert (Y = "ZZZZ")
report
"Assert 449 : < Y /= ZZZZ >" -- 	Vector No: 181
severity warning;

wait for 1 ns;

-- ************************
-- ******** TESTING OUTPUT FOR (I8 I7 I6 = 111) ********		
--------------------------

clk <= '1'; --	 Cycle No: 328

wait for 1 ns;

I <= "111000111";
D <= "0000";
C0 <= '0';
OEbar <= '0';

wait for 4 ns;

clk <= '0';

wait for 4 ns;

assert (Y = "0000")
report
"Assert 450 : < Y /= 0000 >" -- 	Vector No: 182
severity warning;

wait for 1 ns;

-- ************************
--------------------------

clk <= '1'; --	 Cycle No: 329

wait for 1 ns;

I <= "111000111";
D <= "1111";
C0 <= '0';
OEbar <= '0';

wait for 4 ns;

clk <= '0';

wait for 4 ns;

assert (Y = "1111")
report
"Assert 451 : < Y /= 1111 >" -- 	Vector No: 183
severity warning;

wait for 1 ns;

-- ************************
--------------------------

clk <= '1'; --	 Cycle No: 330

wait for 1 ns;

I <= "111000111";
D <= "1111";
C0 <= '0';
OEbar <= '1';

wait for 4 ns;

clk <= '0';

wait for 4 ns;

assert (Y = "ZZZZ")
report
"Assert 452 : < Y /= ZZZZ >" -- 	Vector No: 184
severity warning;

wait for 1 ns;

-- ************************
-- **************************
-- *                        *
-- * TEST VECTORS FOR PATHS *
-- *                        *
-- **************************
-- ******** TESTING PATHS ( "ALU-SOURCES" --> F --> Q ) ********
-- * * * * PATH TEST WITH ALU-SOURCE ( R = A , S = Q ) * * * *
--------------------------

clk <= '1'; --	 Cycle No: 331

wait for 1 ns;

I <= "011000111";
D <= "0000";
Badd <=  0 ;
C0 <= '0';
OEbar <= '0';

wait for 4 ns;

clk <= '0';

wait for 4 ns;

wait for 1 ns;

--------------------------

clk <= '1'; --	 Cycle No: 332

wait for 1 ns;

I <= "000000111";
D <= "0000";
C0 <= '0';
OEbar <= '0';

wait for 4 ns;

clk <= '0';

wait for 4 ns;

wait for 1 ns;

--------------------------

clk <= '1'; --	 Cycle No: 333

wait for 1 ns;

I <= "000011000";
Aadd <=  0 ;
C0 <= '0';
OEbar <= '0';

wait for 4 ns;

clk <= '0';

wait for 4 ns;

wait for 1 ns;

--------------------------

clk <= '1'; --	 Cycle No: 334

wait for 1 ns;

I <= "001000010";
C0 <= '0';
OEbar <= '0';

wait for 4 ns;

clk <= '0';

wait for 4 ns;

assert (Y = "0000")
report
"Assert 453 : < Y /= 0000 >" -- 	Vector No: 185
severity warning;

wait for 1 ns;

-- ************************
--------------------------

clk <= '1'; --	 Cycle No: 335

wait for 1 ns;

I <= "011000111";
D <= "1111";
Badd <=  0 ;
C0 <= '0';
OEbar <= '0';

wait for 4 ns;

clk <= '0';

wait for 4 ns;

wait for 1 ns;

--------------------------

clk <= '1'; --	 Cycle No: 336

wait for 1 ns;

I <= "000000111";
D <= "1111";
C0 <= '0';
OEbar <= '0';

wait for 4 ns;

clk <= '0';

wait for 4 ns;

wait for 1 ns;

--------------------------

clk <= '1'; --	 Cycle No: 337

wait for 1 ns;

I <= "000100000";
Aadd <=  0 ;
C0 <= '0';
OEbar <= '0';

wait for 4 ns;

clk <= '0';

wait for 4 ns;

wait for 1 ns;

--------------------------

clk <= '1'; --	 Cycle No: 338

wait for 1 ns;

I <= "001000010";
C0 <= '0';
OEbar <= '0';

wait for 4 ns;

clk <= '0';

wait for 4 ns;

assert (Y = "1111")
report
"Assert 454 : < Y /= 1111 >" -- 	Vector No: 186
severity warning;

wait for 1 ns;

-- ************************
-- * * * * PATH TEST WITH ALU-SOURCE ( R = A , S = B ) * * * *
--------------------------

clk <= '1'; --	 Cycle No: 339

wait for 1 ns;

I <= "011000111";
D <= "0000";
Badd <=  0 ;
C0 <= '0';
OEbar <= '0';

wait for 4 ns;

clk <= '0';

wait for 4 ns;

wait for 1 ns;

--------------------------

clk <= '1'; --	 Cycle No: 340

wait for 1 ns;

I <= "011000111";
D <= "0000";
Badd <=  1 ;
C0 <= '0';
OEbar <= '0';

wait for 4 ns;

clk <= '0';

wait for 4 ns;

wait for 1 ns;

--------------------------

clk <= '1'; --	 Cycle No: 341

wait for 1 ns;

I <= "000011001";
Aadd <=  0 ;
Badd <=  1 ;
C0 <= '0';
OEbar <= '0';

wait for 4 ns;

clk <= '0';

wait for 4 ns;

wait for 1 ns;

--------------------------

clk <= '1'; --	 Cycle No: 342

wait for 1 ns;

I <= "001000010";
C0 <= '0';
OEbar <= '0';

wait for 4 ns;

clk <= '0';

wait for 4 ns;

assert (Y = "0000")
report
"Assert 455 : < Y /= 0000 >" -- 	Vector No: 187
severity warning;

wait for 1 ns;

-- ************************
--------------------------

clk <= '1'; --	 Cycle No: 343

wait for 1 ns;

I <= "011000111";
D <= "1111";
Badd <=  0 ;
C0 <= '0';
OEbar <= '0';

wait for 4 ns;

clk <= '0';

wait for 4 ns;

wait for 1 ns;

--------------------------

clk <= '1'; --	 Cycle No: 344

wait for 1 ns;

I <= "011000111";
D <= "1111";
Badd <=  1 ;
C0 <= '0';
OEbar <= '0';

wait for 4 ns;

clk <= '0';

wait for 4 ns;

wait for 1 ns;

--------------------------

clk <= '1'; --	 Cycle No: 345

wait for 1 ns;

I <= "000100001";
Aadd <=  0 ;
Badd <=  1 ;
C0 <= '0';
OEbar <= '0';

wait for 4 ns;

clk <= '0';

wait for 4 ns;

wait for 1 ns;

--------------------------

clk <= '1'; --	 Cycle No: 346

wait for 1 ns;

I <= "001000010";
C0 <= '0';
OEbar <= '0';

wait for 4 ns;

clk <= '0';

wait for 4 ns;

assert (Y = "1111")
report
"Assert 456 : < Y /= 1111 >" -- 	Vector No: 188
severity warning;

wait for 1 ns;

-- ************************
-- * * * * PATH TEST WITH ALU-SOURCE ( R = 0 , S = Q ) * * * *
--------------------------

clk <= '1'; --	 Cycle No: 347

wait for 1 ns;

I <= "000000111";
D <= "0000";
C0 <= '0';
OEbar <= '0';

wait for 4 ns;

clk <= '0';

wait for 4 ns;

wait for 1 ns;

--------------------------

clk <= '1'; --	 Cycle No: 348

wait for 1 ns;

I <= "000011010";
C0 <= '0';
OEbar <= '0';

wait for 4 ns;

clk <= '0';

wait for 4 ns;

wait for 1 ns;

--------------------------

clk <= '1'; --	 Cycle No: 349

wait for 1 ns;

I <= "001000010";
C0 <= '0';
OEbar <= '0';

wait for 4 ns;

clk <= '0';

wait for 4 ns;

assert (Y = "0000")
report
"Assert 457 : < Y /= 0000 >" -- 	Vector No: 189
severity warning;

wait for 1 ns;

-- ************************
--------------------------

clk <= '1'; --	 Cycle No: 350

wait for 1 ns;

I <= "000000111";
D <= "1111";
C0 <= '0';
OEbar <= '0';

wait for 4 ns;

clk <= '0';

wait for 4 ns;

wait for 1 ns;

--------------------------

clk <= '1'; --	 Cycle No: 351

wait for 1 ns;

I <= "000011010";
C0 <= '0';
OEbar <= '0';

wait for 4 ns;

clk <= '0';

wait for 4 ns;

wait for 1 ns;

--------------------------

clk <= '1'; --	 Cycle No: 352

wait for 1 ns;

I <= "001000010";
C0 <= '0';
OEbar <= '0';

wait for 4 ns;

clk <= '0';

wait for 4 ns;

assert (Y = "1111")
report
"Assert 458 : < Y /= 1111 >" -- 	Vector No: 190
severity warning;

wait for 1 ns;

-- ************************
-- * * * * PATH TEST WITH ALU-SOURCE ( R = 0 , S = B ) * * * *
--------------------------

clk <= '1'; --	 Cycle No: 353

wait for 1 ns;

I <= "011000111";
D <= "0000";
Badd <=  0 ;
C0 <= '0';
OEbar <= '0';

wait for 4 ns;

clk <= '0';

wait for 4 ns;

wait for 1 ns;

--------------------------

clk <= '1'; --	 Cycle No: 354

wait for 1 ns;

I <= "000011011";
Badd <=  0 ;
C0 <= '0';
OEbar <= '0';

wait for 4 ns;

clk <= '0';

wait for 4 ns;

wait for 1 ns;

--------------------------

clk <= '1'; --	 Cycle No: 355

wait for 1 ns;

I <= "001000010";
C0 <= '0';
OEbar <= '0';

wait for 4 ns;

clk <= '0';

wait for 4 ns;

assert (Y = "0000")
report
"Assert 459 : < Y /= 0000 >" -- 	Vector No: 191
severity warning;

wait for 1 ns;

-- ************************
--------------------------

clk <= '1'; --	 Cycle No: 356

wait for 1 ns;

I <= "011000111";
D <= "1111";
Badd <=  0 ;
C0 <= '0';
OEbar <= '0';

wait for 4 ns;

clk <= '0';

wait for 4 ns;

wait for 1 ns;

--------------------------

clk <= '1'; --	 Cycle No: 357

wait for 1 ns;

I <= "000011011";
Badd <=  0 ;
C0 <= '0';
OEbar <= '0';

wait for 4 ns;

clk <= '0';

wait for 4 ns;

wait for 1 ns;

--------------------------

clk <= '1'; --	 Cycle No: 358

wait for 1 ns;

I <= "001000010";
C0 <= '0';
OEbar <= '0';

wait for 4 ns;

clk <= '0';

wait for 4 ns;

assert (Y = "1111")
report
"Assert 460 : < Y /= 1111 >" -- 	Vector No: 192
severity warning;

wait for 1 ns;

-- ************************
-- * * * * PATH TEST WITH ALU-SOURCE ( R = 0 , S = A ) * * * *
--------------------------

clk <= '1'; --	 Cycle No: 359

wait for 1 ns;

I <= "011000111";
D <= "0000";
Badd <=  0 ;
C0 <= '0';
OEbar <= '0';

wait for 4 ns;

clk <= '0';

wait for 4 ns;

wait for 1 ns;

--------------------------

clk <= '1'; --	 Cycle No: 360

wait for 1 ns;

I <= "000011100";
Aadd <=  0 ;
C0 <= '0';
OEbar <= '0';

wait for 4 ns;

clk <= '0';

wait for 4 ns;

wait for 1 ns;

--------------------------

clk <= '1'; --	 Cycle No: 361

wait for 1 ns;

I <= "001000010";
C0 <= '0';
OEbar <= '0';

wait for 4 ns;

clk <= '0';

wait for 4 ns;

assert (Y = "0000")
report
"Assert 461 : < Y /= 0000 >" -- 	Vector No: 193
severity warning;

wait for 1 ns;

-- ************************
--------------------------

clk <= '1'; --	 Cycle No: 362

wait for 1 ns;

I <= "011000111";
D <= "1111";
Badd <=  0 ;
C0 <= '0';
OEbar <= '0';

wait for 4 ns;

clk <= '0';

wait for 4 ns;

wait for 1 ns;

--------------------------

clk <= '1'; --	 Cycle No: 363

wait for 1 ns;

I <= "000011100";
Aadd <=  0 ;
C0 <= '0';
OEbar <= '0';

wait for 4 ns;

clk <= '0';

wait for 4 ns;

wait for 1 ns;

--------------------------

clk <= '1'; --	 Cycle No: 364

wait for 1 ns;

I <= "001000010";
C0 <= '0';
OEbar <= '0';

wait for 4 ns;

clk <= '0';

wait for 4 ns;

assert (Y = "1111")
report
"Assert 462 : < Y /= 1111 >" -- 	Vector No: 194
severity warning;

wait for 1 ns;

-- ************************
-- * * * * PATH TEST WITH ALU-SOURCE ( R = D , S = A ) * * * *
--------------------------

clk <= '1'; --	 Cycle No: 365

wait for 1 ns;

I <= "011000111";
D <= "0000";
Badd <=  0 ;
C0 <= '0';
OEbar <= '0';

wait for 4 ns;

clk <= '0';

wait for 4 ns;

wait for 1 ns;

--------------------------

clk <= '1'; --	 Cycle No: 366

wait for 1 ns;

I <= "000011101";
D <= "0000";
Aadd <=  0 ;
C0 <= '0';
OEbar <= '0';

wait for 4 ns;

clk <= '0';

wait for 4 ns;

wait for 1 ns;

--------------------------

clk <= '1'; --	 Cycle No: 367

wait for 1 ns;

I <= "001000010";
C0 <= '0';
OEbar <= '0';

wait for 4 ns;

clk <= '0';

wait for 4 ns;

assert (Y = "0000")
report
"Assert 463 : < Y /= 0000 >" -- 	Vector No: 195
severity warning;

wait for 1 ns;

-- ************************
--------------------------

clk <= '1'; --	 Cycle No: 368

wait for 1 ns;

I <= "011000111";
D <= "1111";
Badd <=  0 ;
C0 <= '0';
OEbar <= '0';

wait for 4 ns;

clk <= '0';

wait for 4 ns;

wait for 1 ns;

--------------------------

clk <= '1'; --	 Cycle No: 369

wait for 1 ns;

I <= "000100101";
D <= "1111";
Aadd <=  0 ;
C0 <= '0';
OEbar <= '0';

wait for 4 ns;

clk <= '0';

wait for 4 ns;

wait for 1 ns;

--------------------------

clk <= '1'; --	 Cycle No: 370

wait for 1 ns;

I <= "001000010";
C0 <= '0';
OEbar <= '0';

wait for 4 ns;

clk <= '0';

wait for 4 ns;

assert (Y = "1111")
report
"Assert 464 : < Y /= 1111 >" -- 	Vector No: 196
severity warning;

wait for 1 ns;

-- ************************
-- * * * * PATH TEST WITH ALU-SOURCE ( R = D , S = Q ) * * * *
--------------------------

clk <= '1'; --	 Cycle No: 371

wait for 1 ns;

I <= "000000111";
D <= "0000";
C0 <= '0';
OEbar <= '0';

wait for 4 ns;

clk <= '0';

wait for 4 ns;

wait for 1 ns;

--------------------------

clk <= '1'; --	 Cycle No: 372

wait for 1 ns;

I <= "000011110";
D <= "0000";
C0 <= '0';
OEbar <= '0';

wait for 4 ns;

clk <= '0';

wait for 4 ns;

wait for 1 ns;

--------------------------

clk <= '1'; --	 Cycle No: 373

wait for 1 ns;

I <= "001000010";
C0 <= '0';
OEbar <= '0';

wait for 4 ns;

clk <= '0';

wait for 4 ns;

assert (Y = "0000")
report
"Assert 465 : < Y /= 0000 >" -- 	Vector No: 197
severity warning;

wait for 1 ns;

-- ************************
--------------------------

clk <= '1'; --	 Cycle No: 374

wait for 1 ns;

I <= "000000111";
D <= "1111";
C0 <= '0';
OEbar <= '0';

wait for 4 ns;

clk <= '0';

wait for 4 ns;

wait for 1 ns;

--------------------------

clk <= '1'; --	 Cycle No: 375

wait for 1 ns;

I <= "000100110";
D <= "1111";
C0 <= '0';
OEbar <= '0';

wait for 4 ns;

clk <= '0';

wait for 4 ns;

wait for 1 ns;

--------------------------

clk <= '1'; --	 Cycle No: 376

wait for 1 ns;

I <= "001000010";
C0 <= '0';
OEbar <= '0';

wait for 4 ns;

clk <= '0';

wait for 4 ns;

assert (Y = "1111")
report
"Assert 466 : < Y /= 1111 >" -- 	Vector No: 198
severity warning;

wait for 1 ns;

-- ************************
-- * * * * PATH TEST WITH ALU-SOURCE ( R = D , S = 0 ) * * * *
--------------------------

clk <= '1'; --	 Cycle No: 377

wait for 1 ns;

I <= "000011111";
D <= "0000";
C0 <= '0';
OEbar <= '0';

wait for 4 ns;

clk <= '0';

wait for 4 ns;

wait for 1 ns;

--------------------------

clk <= '1'; --	 Cycle No: 378

wait for 1 ns;

I <= "001000010";
C0 <= '0';
OEbar <= '0';

wait for 4 ns;

clk <= '0';

wait for 4 ns;

assert (Y = "0000")
report
"Assert 467 : < Y /= 0000 >" -- 	Vector No: 199
severity warning;

wait for 1 ns;

-- ************************
-- * * * * PATH TEST WITH ALU-SOURCE ( R = D , S = 0 ) * * * *
--------------------------

clk <= '1'; --	 Cycle No: 379

wait for 1 ns;

I <= "000011111";
D <= "1111";
C0 <= '0';
OEbar <= '0';

wait for 4 ns;

clk <= '0';

wait for 4 ns;

wait for 1 ns;

--------------------------

clk <= '1'; --	 Cycle No: 380

wait for 1 ns;

I <= "001000010";
C0 <= '0';
OEbar <= '0';

wait for 4 ns;

clk <= '0';

wait for 4 ns;

assert (Y = "1111")
report
"Assert 468 : < Y /= 1111 >" -- 	Vector No: 200
severity warning;

wait for 1 ns;

-- ************************
-- ******** TESTING PATHS ( "ALU-SOURCES" --> F --> B ) ********
-- * * * * PATH TEST WITH ALU-SOURCE ( R = A , S = Q ) * * * *
--------------------------

clk <= '1'; --	 Cycle No: 381

wait for 1 ns;

I <= "011000111";
D <= "0000";
Badd <=  0 ;
C0 <= '0';
OEbar <= '0';

wait for 4 ns;

clk <= '0';

wait for 4 ns;

wait for 1 ns;

--------------------------

clk <= '1'; --	 Cycle No: 382

wait for 1 ns;

I <= "000000111";
D <= "0000";
C0 <= '0';
OEbar <= '0';

wait for 4 ns;

clk <= '0';

wait for 4 ns;

wait for 1 ns;

--------------------------

clk <= '1'; --	 Cycle No: 383

wait for 1 ns;

I <= "011011000";
Aadd <=  0 ;
Badd <=  0 ;
C0 <= '0';
OEbar <= '0';

wait for 4 ns;

clk <= '0';

wait for 4 ns;

wait for 1 ns;

--------------------------

clk <= '1'; --	 Cycle No: 384

wait for 1 ns;

I <= "001000100";
Aadd <=  0 ;
C0 <= '0';
OEbar <= '0';

wait for 4 ns;

clk <= '0';

wait for 4 ns;

assert (Y = "0000")
report
"Assert 469 : < Y /= 0000 >" -- 	Vector No: 201
severity warning;

wait for 1 ns;

-- ************************
--------------------------

clk <= '1'; --	 Cycle No: 385

wait for 1 ns;

I <= "011000111";
D <= "1111";
Badd <=  0 ;
C0 <= '0';
OEbar <= '0';

wait for 4 ns;

clk <= '0';

wait for 4 ns;

wait for 1 ns;

--------------------------

clk <= '1'; --	 Cycle No: 386

wait for 1 ns;

I <= "000000111";
D <= "1111";
C0 <= '0';
OEbar <= '0';

wait for 4 ns;

clk <= '0';

wait for 4 ns;

wait for 1 ns;

--------------------------

clk <= '1'; --	 Cycle No: 387

wait for 1 ns;

I <= "011100000";
Aadd <=  0 ;
Badd <=  0 ;
C0 <= '0';
OEbar <= '0';

wait for 4 ns;

clk <= '0';

wait for 4 ns;

wait for 1 ns;

--------------------------

clk <= '1'; --	 Cycle No: 388

wait for 1 ns;

I <= "001000100";
Aadd <=  0 ;
C0 <= '0';
OEbar <= '0';

wait for 4 ns;

clk <= '0';

wait for 4 ns;

assert (Y = "1111")
report
"Assert 470 : < Y /= 1111 >" -- 	Vector No: 202
severity warning;

wait for 1 ns;

-- ************************
-- * * * * PATH TEST WITH ALU-SOURCE ( R = A , S = B ) * * * *
--------------------------

clk <= '1'; --	 Cycle No: 389

wait for 1 ns;

I <= "011000111";
D <= "0000";
Badd <=  0 ;
C0 <= '0';
OEbar <= '0';

wait for 4 ns;

clk <= '0';

wait for 4 ns;

wait for 1 ns;

--------------------------

clk <= '1'; --	 Cycle No: 390

wait for 1 ns;

I <= "011000111";
D <= "0000";
Badd <=  1 ;
C0 <= '0';
OEbar <= '0';

wait for 4 ns;

clk <= '0';

wait for 4 ns;

wait for 1 ns;

--------------------------

clk <= '1'; --	 Cycle No: 391

wait for 1 ns;

I <= "011011001";
Aadd <=  1 ;
Badd <=  0 ;
C0 <= '0';
OEbar <= '0';

wait for 4 ns;

clk <= '0';

wait for 4 ns;

wait for 1 ns;

--------------------------

clk <= '1'; --	 Cycle No: 392

wait for 1 ns;

I <= "001000100";
Aadd <=  0 ;
C0 <= '0';
OEbar <= '0';

wait for 4 ns;

clk <= '0';

wait for 4 ns;

assert (Y = "0000")
report
"Assert 471 : < Y /= 0000 >" -- 	Vector No: 203
severity warning;

wait for 1 ns;

-- ************************
--------------------------

clk <= '1'; --	 Cycle No: 393

wait for 1 ns;

I <= "011000111";
D <= "1111";
Badd <=  0 ;
C0 <= '0';
OEbar <= '0';

wait for 4 ns;

clk <= '0';

wait for 4 ns;

wait for 1 ns;

--------------------------

clk <= '1'; --	 Cycle No: 394

wait for 1 ns;

I <= "011000111";
D <= "1111";
Badd <=  1 ;
C0 <= '0';
OEbar <= '0';

wait for 4 ns;

clk <= '0';

wait for 4 ns;

wait for 1 ns;

--------------------------

clk <= '1'; --	 Cycle No: 395

wait for 1 ns;

I <= "011100001";
Aadd <=  1 ;
Badd <=  0 ;
C0 <= '0';
OEbar <= '0';

wait for 4 ns;

clk <= '0';

wait for 4 ns;

wait for 1 ns;

--------------------------

clk <= '1'; --	 Cycle No: 396

wait for 1 ns;

I <= "001000100";
Aadd <=  0 ;
C0 <= '0';
OEbar <= '0';

wait for 4 ns;

clk <= '0';

wait for 4 ns;

assert (Y = "1111")
report
"Assert 472 : < Y /= 1111 >" -- 	Vector No: 204
severity warning;

wait for 1 ns;

-- ************************
-- * * * * PATH TEST WITH ALU-SOURCE ( R = 0 , S = Q ) * * * *
--------------------------

clk <= '1'; --	 Cycle No: 397

wait for 1 ns;

I <= "000000111";
D <= "0000";
C0 <= '0';
OEbar <= '0';

wait for 4 ns;

clk <= '0';

wait for 4 ns;

wait for 1 ns;

--------------------------

clk <= '1'; --	 Cycle No: 398

wait for 1 ns;

I <= "011011010";
Badd <=  0 ;
C0 <= '0';
OEbar <= '0';

wait for 4 ns;

clk <= '0';

wait for 4 ns;

wait for 1 ns;

--------------------------

clk <= '1'; --	 Cycle No: 399

wait for 1 ns;

I <= "001000100";
Aadd <=  0 ;
C0 <= '0';
OEbar <= '0';

wait for 4 ns;

clk <= '0';

wait for 4 ns;

assert (Y = "0000")
report
"Assert 473 : < Y /= 0000 >" -- 	Vector No: 205
severity warning;

wait for 1 ns;

-- ************************
--------------------------

clk <= '1'; --	 Cycle No: 400

wait for 1 ns;

I <= "000000111";
D <= "1111";
C0 <= '0';
OEbar <= '0';

wait for 4 ns;

clk <= '0';

wait for 4 ns;

wait for 1 ns;

--------------------------

clk <= '1'; --	 Cycle No: 401

wait for 1 ns;

I <= "011011010";
Badd <=  0 ;
C0 <= '0';
OEbar <= '0';

wait for 4 ns;

clk <= '0';

wait for 4 ns;

wait for 1 ns;

--------------------------

clk <= '1'; --	 Cycle No: 402

wait for 1 ns;

I <= "001000100";
Aadd <=  0 ;
C0 <= '0';
OEbar <= '0';

wait for 4 ns;

clk <= '0';

wait for 4 ns;

assert (Y = "1111")
report
"Assert 474 : < Y /= 1111 >" -- 	Vector No: 206
severity warning;

wait for 1 ns;

-- ************************
-- * * * * PATH TEST WITH ALU-SOURCE ( R = 0 , S = B ) * * * *
--------------------------

clk <= '1'; --	 Cycle No: 403

wait for 1 ns;

I <= "011000111";
D <= "0000";
Badd <=  0 ;
C0 <= '0';
OEbar <= '0';

wait for 4 ns;

clk <= '0';

wait for 4 ns;

wait for 1 ns;

--------------------------

clk <= '1'; --	 Cycle No: 404

wait for 1 ns;

I <= "011011011";
Badd <=  0 ;
C0 <= '0';
OEbar <= '0';

wait for 4 ns;

clk <= '0';

wait for 4 ns;

wait for 1 ns;

--------------------------

clk <= '1'; --	 Cycle No: 405

wait for 1 ns;

I <= "001000100";
Aadd <=  0 ;
C0 <= '0';
OEbar <= '0';

wait for 4 ns;

clk <= '0';

wait for 4 ns;

assert (Y = "0000")
report
"Assert 475 : < Y /= 0000 >" -- 	Vector No: 207
severity warning;

wait for 1 ns;

-- ************************
--------------------------

clk <= '1'; --	 Cycle No: 406

wait for 1 ns;

I <= "011000111";
D <= "1111";
Badd <=  0 ;
C0 <= '0';
OEbar <= '0';

wait for 4 ns;

clk <= '0';

wait for 4 ns;

wait for 1 ns;

--------------------------

clk <= '1'; --	 Cycle No: 407

wait for 1 ns;

I <= "011011011";
Badd <=  0 ;
C0 <= '0';
OEbar <= '0';

wait for 4 ns;

clk <= '0';

wait for 4 ns;

wait for 1 ns;

--------------------------

clk <= '1'; --	 Cycle No: 408

wait for 1 ns;

I <= "001000100";
Aadd <=  0 ;
C0 <= '0';
OEbar <= '0';

wait for 4 ns;

clk <= '0';

wait for 4 ns;

assert (Y = "1111")
report
"Assert 476 : < Y /= 1111 >" -- 	Vector No: 208
severity warning;

wait for 1 ns;

-- ************************
-- * * * * PATH TEST WITH ALU-SOURCE ( R = 0 , S = A ) * * * *
--------------------------

clk <= '1'; --	 Cycle No: 409

wait for 1 ns;

I <= "011000111";
D <= "0000";
Badd <=  0 ;
C0 <= '0';
OEbar <= '0';

wait for 4 ns;

clk <= '0';

wait for 4 ns;

wait for 1 ns;

--------------------------

clk <= '1'; --	 Cycle No: 410

wait for 1 ns;

I <= "011011100";
Aadd <=  0 ;
Badd <=  0 ;
C0 <= '0';
OEbar <= '0';

wait for 4 ns;

clk <= '0';

wait for 4 ns;

wait for 1 ns;

--------------------------

clk <= '1'; --	 Cycle No: 411

wait for 1 ns;

I <= "001000100";
Aadd <=  0 ;
C0 <= '0';
OEbar <= '0';

wait for 4 ns;

clk <= '0';

wait for 4 ns;

assert (Y = "0000")
report
"Assert 477 : < Y /= 0000 >" -- 	Vector No: 209
severity warning;

wait for 1 ns;

-- ************************
--------------------------

clk <= '1'; --	 Cycle No: 412

wait for 1 ns;

I <= "011000111";
D <= "1111";
Badd <=  0 ;
C0 <= '0';
OEbar <= '0';

wait for 4 ns;

clk <= '0';

wait for 4 ns;

wait for 1 ns;

--------------------------

clk <= '1'; --	 Cycle No: 413

wait for 1 ns;

I <= "011011100";
Aadd <=  0 ;
Badd <=  0 ;
C0 <= '0';
OEbar <= '0';

wait for 4 ns;

clk <= '0';

wait for 4 ns;

wait for 1 ns;

--------------------------

clk <= '1'; --	 Cycle No: 414

wait for 1 ns;

I <= "001000100";
Aadd <=  0 ;
C0 <= '0';
OEbar <= '0';

wait for 4 ns;

clk <= '0';

wait for 4 ns;

assert (Y = "1111")
report
"Assert 478 : < Y /= 1111 >" -- 	Vector No: 210
severity warning;

wait for 1 ns;

-- ************************
-- * * * * PATH TEST WITH ALU-SOURCE ( R = D , S = A ) * * * *
--------------------------

clk <= '1'; --	 Cycle No: 415

wait for 1 ns;

I <= "011000111";
D <= "0000";
Badd <=  0 ;
C0 <= '0';
OEbar <= '0';

wait for 4 ns;

clk <= '0';

wait for 4 ns;

wait for 1 ns;

--------------------------

clk <= '1'; --	 Cycle No: 416

wait for 1 ns;

I <= "011011101";
D <= "0000";
Aadd <=  0 ;
Badd <=  0 ;
C0 <= '0';
OEbar <= '0';

wait for 4 ns;

clk <= '0';

wait for 4 ns;

wait for 1 ns;

--------------------------

clk <= '1'; --	 Cycle No: 417

wait for 1 ns;

I <= "001000100";
Aadd <=  0 ;
C0 <= '0';
OEbar <= '0';

wait for 4 ns;

clk <= '0';

wait for 4 ns;

assert (Y = "0000")
report
"Assert 479 : < Y /= 0000 >" -- 	Vector No: 211
severity warning;

wait for 1 ns;

-- ************************
--------------------------

clk <= '1'; --	 Cycle No: 418

wait for 1 ns;

I <= "011000111";
D <= "1111";
Badd <=  0 ;
C0 <= '0';
OEbar <= '0';

wait for 4 ns;

clk <= '0';

wait for 4 ns;

wait for 1 ns;

--------------------------

clk <= '1'; --	 Cycle No: 419

wait for 1 ns;

I <= "011100101";
D <= "1111";
Aadd <=  0 ;
Badd <=  0 ;
C0 <= '0';
OEbar <= '0';

wait for 4 ns;

clk <= '0';

wait for 4 ns;

wait for 1 ns;

--------------------------

clk <= '1'; --	 Cycle No: 420

wait for 1 ns;

I <= "001000100";
Aadd <=  0 ;
C0 <= '0';
OEbar <= '0';

wait for 4 ns;

clk <= '0';

wait for 4 ns;

assert (Y = "1111")
report
"Assert 480 : < Y /= 1111 >" -- 	Vector No: 212
severity warning;

wait for 1 ns;

-- ************************
-- * * * * PATH TEST WITH ALU-SOURCE ( R = D , S = Q ) * * * *
--------------------------

clk <= '1'; --	 Cycle No: 421

wait for 1 ns;

I <= "000000111";
D <= "0000";
C0 <= '0';
OEbar <= '0';

wait for 4 ns;

clk <= '0';

wait for 4 ns;

wait for 1 ns;

--------------------------

clk <= '1'; --	 Cycle No: 422

wait for 1 ns;

I <= "011011110";
D <= "0000";
Badd <=  0 ;
C0 <= '0';
OEbar <= '0';

wait for 4 ns;

clk <= '0';

wait for 4 ns;

wait for 1 ns;

--------------------------

clk <= '1'; --	 Cycle No: 423

wait for 1 ns;

I <= "001000100";
Aadd <=  0 ;
C0 <= '0';
OEbar <= '0';

wait for 4 ns;

clk <= '0';

wait for 4 ns;

assert (Y = "0000")
report
"Assert 481 : < Y /= 0000 >" -- 	Vector No: 213
severity warning;

wait for 1 ns;

-- ************************
--------------------------

clk <= '1'; --	 Cycle No: 424

wait for 1 ns;

I <= "000000111";
D <= "1111";
C0 <= '0';
OEbar <= '0';

wait for 4 ns;

clk <= '0';

wait for 4 ns;

wait for 1 ns;

--------------------------

clk <= '1'; --	 Cycle No: 425

wait for 1 ns;

I <= "011100110";
D <= "1111";
Badd <=  0 ;
C0 <= '0';
OEbar <= '0';

wait for 4 ns;

clk <= '0';

wait for 4 ns;

wait for 1 ns;

--------------------------

clk <= '1'; --	 Cycle No: 426

wait for 1 ns;

I <= "001000100";
Aadd <=  0 ;
C0 <= '0';
OEbar <= '0';

wait for 4 ns;

clk <= '0';

wait for 4 ns;

assert (Y = "1111")
report
"Assert 482 : < Y /= 1111 >" -- 	Vector No: 214
severity warning;

wait for 1 ns;

-- ************************
-- * * * * PATH TEST WITH ALU-SOURCE ( R = D , S = 0 ) * * * *
--------------------------

clk <= '1'; --	 Cycle No: 427

wait for 1 ns;

I <= "011011111";
D <= "0000";
Badd <=  0 ;
C0 <= '0';
OEbar <= '0';

wait for 4 ns;

clk <= '0';

wait for 4 ns;

wait for 1 ns;

--------------------------

clk <= '1'; --	 Cycle No: 428

wait for 1 ns;

I <= "001000100";
Aadd <=  0 ;
C0 <= '0';
OEbar <= '0';

wait for 4 ns;

clk <= '0';

wait for 4 ns;

assert (Y = "0000")
report
"Assert 483 : < Y /= 0000 >" -- 	Vector No: 215
severity warning;

wait for 1 ns;

-- ************************
--------------------------

clk <= '1'; --	 Cycle No: 429

wait for 1 ns;

I <= "011011111";
D <= "1111";
Badd <=  0 ;
C0 <= '0';
OEbar <= '0';

wait for 4 ns;

clk <= '0';

wait for 4 ns;

wait for 1 ns;

--------------------------

clk <= '1'; --	 Cycle No: 430

wait for 1 ns;

I <= "001000100";
Aadd <=  0 ;
C0 <= '0';
OEbar <= '0';

wait for 4 ns;

clk <= '0';

wait for 4 ns;

assert (Y = "1111")
report
"Assert 484 : < Y /= 1111 >" -- 	Vector No: 216
severity warning;

wait for 1 ns;

-- ************************
--------------------------

end process;

end A;
