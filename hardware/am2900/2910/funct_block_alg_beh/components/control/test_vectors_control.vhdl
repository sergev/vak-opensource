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
use work.MVL7_functions.all;	--some binary functions
use work.synthesis_types.all;	--hints for synthesis

entity E is
end;

architecture AA of E is
	 component ccontrol
	  port (
		I : in MVL7_VECTOR(3 downto 0);
		CCEN_BAR : in MVL7;
		CC_BAR : in MVL7;
		Rzero_bar : in MVL7;
		PL_BAR : out MVL7;
		VECT_BAR : out MVL7;
		MAP_BAR : out MVL7;
		R_sel : out MVL7;
		D_sel : out MVL7;
		uPC_sel : out MVL7;
		stack_sel : out MVL7;
		decr : out MVL7;
		load : out MVL7;
		clear : out MVL7;
		push : out MVL7;
		pop : out MVL7
	  );
	 end component;

		signal I : MVL7_VECTOR(3 downto 0);
		signal CCEN_BAR : MVL7;
		signal CC_BAR : MVL7;
		signal Rzero_bar : MVL7;
		signal PL_BAR : MVL7;
		signal VECT_BAR : MVL7;
		signal MAP_BAR : MVL7;
		signal R_sel : MVL7;
		signal D_sel : MVL7;
		signal uPC_sel : MVL7;
		signal stack_sel : MVL7;
		signal decr : MVL7;
		signal load : MVL7;
		signal clear : MVL7;
		signal push : MVL7;
		signal pop : MVL7;
for all : ccontrol use entity work.control(control);

begin

CCONTROL1 : ccontrol port map(
		I,
		CCEN_BAR,
		CC_BAR,
		Rzero_bar,
		PL_BAR,
		VECT_BAR,
		MAP_BAR,
		R_sel,
		D_sel,
		uPC_sel,
		stack_sel,
		decr,
		load,
		clear,
		push,
		pop
           );

process

begin

-- *********
-- * I = 0 *
-- *********
--------------------------

I <= "0000";
CCEN_BAR <= '0';
CC_BAR <= '1';
Rzero_bar <= '1';

wait for 1 ns; --	 Cycle No: 0

assert (PL_BAR = '0')
report
"Assert 0 : < PL_BAR /= 0 >" -- 	Vector No: 0
severity warning;

assert (VECT_BAR = '1')
report
"Assert 1 : < VECT_BAR /= 1 >"
severity warning;

assert (MAP_BAR = '1')
report
"Assert 2 : < MAP_BAR /= 1 >"
severity warning;

assert (R_sel = '0')
report
"Assert 3 : < R_sel /= 0 >"
severity warning;

assert (D_sel = '0')
report
"Assert 4 : < D_sel /= 0 >"
severity warning;

assert (uPC_sel = '0')
report
"Assert 5 : < uPC_sel /= 0 >"
severity warning;

assert (stack_sel = '0')
report
"Assert 6 : < stack_sel /= 0 >"
severity warning;

assert (decr = '0')
report
"Assert 7 : < decr /= 0 >"
severity warning;

assert (load = '0')
report
"Assert 8 : < load /= 0 >"
severity warning;

assert (clear = '1')
report
"Assert 9 : < clear /= 1 >"
severity warning;

assert (push = '0')
report
"Assert 10 : < push /= 0 >"
severity warning;

assert (pop = '0')
report
"Assert 11 : < pop /= 0 >"
severity warning;

wait for 1 ns;

--------------------------

I <= "0000";
CCEN_BAR <= '0';
CC_BAR <= '1';
Rzero_bar <= '0';

wait for 1 ns; --	 Cycle No: 1

assert (PL_BAR = '0')
report
"Assert 12 : < PL_BAR /= 0 >" -- 	Vector No: 1
severity warning;

assert (VECT_BAR = '1')
report
"Assert 13 : < VECT_BAR /= 1 >"
severity warning;

assert (MAP_BAR = '1')
report
"Assert 14 : < MAP_BAR /= 1 >"
severity warning;

assert (R_sel = '0')
report
"Assert 15 : < R_sel /= 0 >"
severity warning;

assert (D_sel = '0')
report
"Assert 16 : < D_sel /= 0 >"
severity warning;

assert (uPC_sel = '0')
report
"Assert 17 : < uPC_sel /= 0 >"
severity warning;

assert (stack_sel = '0')
report
"Assert 18 : < stack_sel /= 0 >"
severity warning;

assert (decr = '0')
report
"Assert 19 : < decr /= 0 >"
severity warning;

assert (load = '0')
report
"Assert 20 : < load /= 0 >"
severity warning;

assert (clear = '1')
report
"Assert 21 : < clear /= 1 >"
severity warning;

assert (push = '0')
report
"Assert 22 : < push /= 0 >"
severity warning;

assert (pop = '0')
report
"Assert 23 : < pop /= 0 >"
severity warning;

wait for 1 ns;

-- ***********************************
--------------------------

I <= "0000";
CCEN_BAR <= '0';
CC_BAR <= '0';
Rzero_bar <= '1';

wait for 1 ns; --	 Cycle No: 2

assert (PL_BAR = '0')
report
"Assert 24 : < PL_BAR /= 0 >" -- 	Vector No: 2
severity warning;

assert (VECT_BAR = '1')
report
"Assert 25 : < VECT_BAR /= 1 >"
severity warning;

assert (MAP_BAR = '1')
report
"Assert 26 : < MAP_BAR /= 1 >"
severity warning;

assert (R_sel = '0')
report
"Assert 27 : < R_sel /= 0 >"
severity warning;

assert (D_sel = '0')
report
"Assert 28 : < D_sel /= 0 >"
severity warning;

assert (uPC_sel = '0')
report
"Assert 29 : < uPC_sel /= 0 >"
severity warning;

assert (stack_sel = '0')
report
"Assert 30 : < stack_sel /= 0 >"
severity warning;

assert (decr = '0')
report
"Assert 31 : < decr /= 0 >"
severity warning;

assert (load = '0')
report
"Assert 32 : < load /= 0 >"
severity warning;

assert (clear = '1')
report
"Assert 33 : < clear /= 1 >"
severity warning;

assert (push = '0')
report
"Assert 34 : < push /= 0 >"
severity warning;

assert (pop = '0')
report
"Assert 35 : < pop /= 0 >"
severity warning;

wait for 1 ns;

--------------------------

I <= "0000";
CCEN_BAR <= '0';
CC_BAR <= '0';
Rzero_bar <= '0';

wait for 1 ns; --	 Cycle No: 3

assert (PL_BAR = '0')
report
"Assert 36 : < PL_BAR /= 0 >" -- 	Vector No: 3
severity warning;

assert (VECT_BAR = '1')
report
"Assert 37 : < VECT_BAR /= 1 >"
severity warning;

assert (MAP_BAR = '1')
report
"Assert 38 : < MAP_BAR /= 1 >"
severity warning;

assert (R_sel = '0')
report
"Assert 39 : < R_sel /= 0 >"
severity warning;

assert (D_sel = '0')
report
"Assert 40 : < D_sel /= 0 >"
severity warning;

assert (uPC_sel = '0')
report
"Assert 41 : < uPC_sel /= 0 >"
severity warning;

assert (stack_sel = '0')
report
"Assert 42 : < stack_sel /= 0 >"
severity warning;

assert (decr = '0')
report
"Assert 43 : < decr /= 0 >"
severity warning;

assert (load = '0')
report
"Assert 44 : < load /= 0 >"
severity warning;

assert (clear = '1')
report
"Assert 45 : < clear /= 1 >"
severity warning;

assert (push = '0')
report
"Assert 46 : < push /= 0 >"
severity warning;

assert (pop = '0')
report
"Assert 47 : < pop /= 0 >"
severity warning;

wait for 1 ns;

-- ***********************************
--------------------------

I <= "0000";
CCEN_BAR <= '1';
CC_BAR <= '1';
Rzero_bar <= '1';

wait for 1 ns; --	 Cycle No: 4

assert (PL_BAR = '0')
report
"Assert 48 : < PL_BAR /= 0 >" -- 	Vector No: 4
severity warning;

assert (VECT_BAR = '1')
report
"Assert 49 : < VECT_BAR /= 1 >"
severity warning;

assert (MAP_BAR = '1')
report
"Assert 50 : < MAP_BAR /= 1 >"
severity warning;

assert (R_sel = '0')
report
"Assert 51 : < R_sel /= 0 >"
severity warning;

assert (D_sel = '0')
report
"Assert 52 : < D_sel /= 0 >"
severity warning;

assert (uPC_sel = '0')
report
"Assert 53 : < uPC_sel /= 0 >"
severity warning;

assert (stack_sel = '0')
report
"Assert 54 : < stack_sel /= 0 >"
severity warning;

assert (decr = '0')
report
"Assert 55 : < decr /= 0 >"
severity warning;

assert (load = '0')
report
"Assert 56 : < load /= 0 >"
severity warning;

assert (clear = '1')
report
"Assert 57 : < clear /= 1 >"
severity warning;

assert (push = '0')
report
"Assert 58 : < push /= 0 >"
severity warning;

assert (pop = '0')
report
"Assert 59 : < pop /= 0 >"
severity warning;

wait for 1 ns;

--------------------------

I <= "0000";
CCEN_BAR <= '1';
CC_BAR <= '1';
Rzero_bar <= '0';

wait for 1 ns; --	 Cycle No: 5

assert (PL_BAR = '0')
report
"Assert 60 : < PL_BAR /= 0 >" -- 	Vector No: 5
severity warning;

assert (VECT_BAR = '1')
report
"Assert 61 : < VECT_BAR /= 1 >"
severity warning;

assert (MAP_BAR = '1')
report
"Assert 62 : < MAP_BAR /= 1 >"
severity warning;

assert (R_sel = '0')
report
"Assert 63 : < R_sel /= 0 >"
severity warning;

assert (D_sel = '0')
report
"Assert 64 : < D_sel /= 0 >"
severity warning;

assert (uPC_sel = '0')
report
"Assert 65 : < uPC_sel /= 0 >"
severity warning;

assert (stack_sel = '0')
report
"Assert 66 : < stack_sel /= 0 >"
severity warning;

assert (decr = '0')
report
"Assert 67 : < decr /= 0 >"
severity warning;

assert (load = '0')
report
"Assert 68 : < load /= 0 >"
severity warning;

assert (clear = '1')
report
"Assert 69 : < clear /= 1 >"
severity warning;

assert (push = '0')
report
"Assert 70 : < push /= 0 >"
severity warning;

assert (pop = '0')
report
"Assert 71 : < pop /= 0 >"
severity warning;

wait for 1 ns;

-- ***********************************
--------------------------

I <= "0000";
CCEN_BAR <= '1';
CC_BAR <= '0';
Rzero_bar <= '1';

wait for 1 ns; --	 Cycle No: 6

assert (PL_BAR = '0')
report
"Assert 72 : < PL_BAR /= 0 >" -- 	Vector No: 6
severity warning;

assert (VECT_BAR = '1')
report
"Assert 73 : < VECT_BAR /= 1 >"
severity warning;

assert (MAP_BAR = '1')
report
"Assert 74 : < MAP_BAR /= 1 >"
severity warning;

assert (R_sel = '0')
report
"Assert 75 : < R_sel /= 0 >"
severity warning;

assert (D_sel = '0')
report
"Assert 76 : < D_sel /= 0 >"
severity warning;

assert (uPC_sel = '0')
report
"Assert 77 : < uPC_sel /= 0 >"
severity warning;

assert (stack_sel = '0')
report
"Assert 78 : < stack_sel /= 0 >"
severity warning;

assert (decr = '0')
report
"Assert 79 : < decr /= 0 >"
severity warning;

assert (load = '0')
report
"Assert 80 : < load /= 0 >"
severity warning;

assert (clear = '1')
report
"Assert 81 : < clear /= 1 >"
severity warning;

assert (push = '0')
report
"Assert 82 : < push /= 0 >"
severity warning;

assert (pop = '0')
report
"Assert 83 : < pop /= 0 >"
severity warning;

wait for 1 ns;

--------------------------

I <= "0000";
CCEN_BAR <= '1';
CC_BAR <= '0';
Rzero_bar <= '0';

wait for 1 ns; --	 Cycle No: 7

assert (PL_BAR = '0')
report
"Assert 84 : < PL_BAR /= 0 >" -- 	Vector No: 7
severity warning;

assert (VECT_BAR = '1')
report
"Assert 85 : < VECT_BAR /= 1 >"
severity warning;

assert (MAP_BAR = '1')
report
"Assert 86 : < MAP_BAR /= 1 >"
severity warning;

assert (R_sel = '0')
report
"Assert 87 : < R_sel /= 0 >"
severity warning;

assert (D_sel = '0')
report
"Assert 88 : < D_sel /= 0 >"
severity warning;

assert (uPC_sel = '0')
report
"Assert 89 : < uPC_sel /= 0 >"
severity warning;

assert (stack_sel = '0')
report
"Assert 90 : < stack_sel /= 0 >"
severity warning;

assert (decr = '0')
report
"Assert 91 : < decr /= 0 >"
severity warning;

assert (load = '0')
report
"Assert 92 : < load /= 0 >"
severity warning;

assert (clear = '1')
report
"Assert 93 : < clear /= 1 >"
severity warning;

assert (push = '0')
report
"Assert 94 : < push /= 0 >"
severity warning;

assert (pop = '0')
report
"Assert 95 : < pop /= 0 >"
severity warning;

wait for 1 ns;

-- ***********************************
-- *********
-- * I = 1 *
-- *********
--------------------------

I <= "0001";
CCEN_BAR <= '0';
CC_BAR <= '1';
Rzero_bar <= '1';

wait for 1 ns; --	 Cycle No: 8

assert (PL_BAR = '0')
report
"Assert 96 : < PL_BAR /= 0 >" -- 	Vector No: 8
severity warning;

assert (VECT_BAR = '1')
report
"Assert 97 : < VECT_BAR /= 1 >"
severity warning;

assert (MAP_BAR = '1')
report
"Assert 98 : < MAP_BAR /= 1 >"
severity warning;

assert (R_sel = '0')
report
"Assert 99 : < R_sel /= 0 >"
severity warning;

assert (D_sel = '0')
report
"Assert 100 : < D_sel /= 0 >"
severity warning;

assert (uPC_sel = '1')
report
"Assert 101 : < uPC_sel /= 1 >"
severity warning;

assert (stack_sel = '0')
report
"Assert 102 : < stack_sel /= 0 >"
severity warning;

assert (decr = '0')
report
"Assert 103 : < decr /= 0 >"
severity warning;

assert (load = '0')
report
"Assert 104 : < load /= 0 >"
severity warning;

assert (clear = '0')
report
"Assert 105 : < clear /= 0 >"
severity warning;

assert (push = '0')
report
"Assert 106 : < push /= 0 >"
severity warning;

assert (pop = '0')
report
"Assert 107 : < pop /= 0 >"
severity warning;

wait for 1 ns;

--------------------------

I <= "0001";
CCEN_BAR <= '0';
CC_BAR <= '1';
Rzero_bar <= '0';

wait for 1 ns; --	 Cycle No: 9

assert (PL_BAR = '0')
report
"Assert 108 : < PL_BAR /= 0 >" -- 	Vector No: 9
severity warning;

assert (VECT_BAR = '1')
report
"Assert 109 : < VECT_BAR /= 1 >"
severity warning;

assert (MAP_BAR = '1')
report
"Assert 110 : < MAP_BAR /= 1 >"
severity warning;

assert (R_sel = '0')
report
"Assert 111 : < R_sel /= 0 >"
severity warning;

assert (D_sel = '0')
report
"Assert 112 : < D_sel /= 0 >"
severity warning;

assert (uPC_sel = '1')
report
"Assert 113 : < uPC_sel /= 1 >"
severity warning;

assert (stack_sel = '0')
report
"Assert 114 : < stack_sel /= 0 >"
severity warning;

assert (decr = '0')
report
"Assert 115 : < decr /= 0 >"
severity warning;

assert (load = '0')
report
"Assert 116 : < load /= 0 >"
severity warning;

assert (clear = '0')
report
"Assert 117 : < clear /= 0 >"
severity warning;

assert (push = '0')
report
"Assert 118 : < push /= 0 >"
severity warning;

assert (pop = '0')
report
"Assert 119 : < pop /= 0 >"
severity warning;

wait for 1 ns;

-- ***********************************
--------------------------

I <= "0001";
CCEN_BAR <= '0';
CC_BAR <= '0';
Rzero_bar <= '1';

wait for 1 ns; --	 Cycle No: 10

assert (PL_BAR = '0')
report
"Assert 120 : < PL_BAR /= 0 >" -- 	Vector No: 10
severity warning;

assert (VECT_BAR = '1')
report
"Assert 121 : < VECT_BAR /= 1 >"
severity warning;

assert (MAP_BAR = '1')
report
"Assert 122 : < MAP_BAR /= 1 >"
severity warning;

assert (R_sel = '0')
report
"Assert 123 : < R_sel /= 0 >"
severity warning;

assert (D_sel = '1')
report
"Assert 124 : < D_sel /= 1 >"
severity warning;

assert (uPC_sel = '0')
report
"Assert 125 : < uPC_sel /= 0 >"
severity warning;

assert (stack_sel = '0')
report
"Assert 126 : < stack_sel /= 0 >"
severity warning;

assert (decr = '0')
report
"Assert 127 : < decr /= 0 >"
severity warning;

assert (load = '0')
report
"Assert 128 : < load /= 0 >"
severity warning;

assert (clear = '0')
report
"Assert 129 : < clear /= 0 >"
severity warning;

assert (push = '1')
report
"Assert 130 : < push /= 1 >"
severity warning;

assert (pop = '0')
report
"Assert 131 : < pop /= 0 >"
severity warning;

wait for 1 ns;

--------------------------

I <= "0001";
CCEN_BAR <= '0';
CC_BAR <= '0';
Rzero_bar <= '0';

wait for 1 ns; --	 Cycle No: 11

assert (PL_BAR = '0')
report
"Assert 132 : < PL_BAR /= 0 >" -- 	Vector No: 11
severity warning;

assert (VECT_BAR = '1')
report
"Assert 133 : < VECT_BAR /= 1 >"
severity warning;

assert (MAP_BAR = '1')
report
"Assert 134 : < MAP_BAR /= 1 >"
severity warning;

assert (R_sel = '0')
report
"Assert 135 : < R_sel /= 0 >"
severity warning;

assert (D_sel = '1')
report
"Assert 136 : < D_sel /= 1 >"
severity warning;

assert (uPC_sel = '0')
report
"Assert 137 : < uPC_sel /= 0 >"
severity warning;

assert (stack_sel = '0')
report
"Assert 138 : < stack_sel /= 0 >"
severity warning;

assert (decr = '0')
report
"Assert 139 : < decr /= 0 >"
severity warning;

assert (load = '0')
report
"Assert 140 : < load /= 0 >"
severity warning;

assert (clear = '0')
report
"Assert 141 : < clear /= 0 >"
severity warning;

assert (push = '1')
report
"Assert 142 : < push /= 1 >"
severity warning;

assert (pop = '0')
report
"Assert 143 : < pop /= 0 >"
severity warning;

wait for 1 ns;

-- ***********************************
--------------------------

I <= "0001";
CCEN_BAR <= '1';
CC_BAR <= '1';
Rzero_bar <= '1';

wait for 1 ns; --	 Cycle No: 12

assert (PL_BAR = '0')
report
"Assert 144 : < PL_BAR /= 0 >" -- 	Vector No: 12
severity warning;

assert (VECT_BAR = '1')
report
"Assert 145 : < VECT_BAR /= 1 >"
severity warning;

assert (MAP_BAR = '1')
report
"Assert 146 : < MAP_BAR /= 1 >"
severity warning;

assert (R_sel = '0')
report
"Assert 147 : < R_sel /= 0 >"
severity warning;

assert (D_sel = '1')
report
"Assert 148 : < D_sel /= 1 >"
severity warning;

assert (uPC_sel = '0')
report
"Assert 149 : < uPC_sel /= 0 >"
severity warning;

assert (stack_sel = '0')
report
"Assert 150 : < stack_sel /= 0 >"
severity warning;

assert (decr = '0')
report
"Assert 151 : < decr /= 0 >"
severity warning;

assert (load = '0')
report
"Assert 152 : < load /= 0 >"
severity warning;

assert (clear = '0')
report
"Assert 153 : < clear /= 0 >"
severity warning;

assert (push = '1')
report
"Assert 154 : < push /= 1 >"
severity warning;

assert (pop = '0')
report
"Assert 155 : < pop /= 0 >"
severity warning;

wait for 1 ns;

--------------------------

I <= "0001";
CCEN_BAR <= '1';
CC_BAR <= '1';
Rzero_bar <= '0';

wait for 1 ns; --	 Cycle No: 13

assert (PL_BAR = '0')
report
"Assert 156 : < PL_BAR /= 0 >" -- 	Vector No: 13
severity warning;

assert (VECT_BAR = '1')
report
"Assert 157 : < VECT_BAR /= 1 >"
severity warning;

assert (MAP_BAR = '1')
report
"Assert 158 : < MAP_BAR /= 1 >"
severity warning;

assert (R_sel = '0')
report
"Assert 159 : < R_sel /= 0 >"
severity warning;

assert (D_sel = '1')
report
"Assert 160 : < D_sel /= 1 >"
severity warning;

assert (uPC_sel = '0')
report
"Assert 161 : < uPC_sel /= 0 >"
severity warning;

assert (stack_sel = '0')
report
"Assert 162 : < stack_sel /= 0 >"
severity warning;

assert (decr = '0')
report
"Assert 163 : < decr /= 0 >"
severity warning;

assert (load = '0')
report
"Assert 164 : < load /= 0 >"
severity warning;

assert (clear = '0')
report
"Assert 165 : < clear /= 0 >"
severity warning;

assert (push = '1')
report
"Assert 166 : < push /= 1 >"
severity warning;

assert (pop = '0')
report
"Assert 167 : < pop /= 0 >"
severity warning;

wait for 1 ns;

-- ***********************************
--------------------------

I <= "0001";
CCEN_BAR <= '1';
CC_BAR <= '0';
Rzero_bar <= '1';

wait for 1 ns; --	 Cycle No: 14

assert (PL_BAR = '0')
report
"Assert 168 : < PL_BAR /= 0 >" -- 	Vector No: 14
severity warning;

assert (VECT_BAR = '1')
report
"Assert 169 : < VECT_BAR /= 1 >"
severity warning;

assert (MAP_BAR = '1')
report
"Assert 170 : < MAP_BAR /= 1 >"
severity warning;

assert (R_sel = '0')
report
"Assert 171 : < R_sel /= 0 >"
severity warning;

assert (D_sel = '1')
report
"Assert 172 : < D_sel /= 1 >"
severity warning;

assert (uPC_sel = '0')
report
"Assert 173 : < uPC_sel /= 0 >"
severity warning;

assert (stack_sel = '0')
report
"Assert 174 : < stack_sel /= 0 >"
severity warning;

assert (decr = '0')
report
"Assert 175 : < decr /= 0 >"
severity warning;

assert (load = '0')
report
"Assert 176 : < load /= 0 >"
severity warning;

assert (clear = '0')
report
"Assert 177 : < clear /= 0 >"
severity warning;

assert (push = '1')
report
"Assert 178 : < push /= 1 >"
severity warning;

assert (pop = '0')
report
"Assert 179 : < pop /= 0 >"
severity warning;

wait for 1 ns;

--------------------------

I <= "0001";
CCEN_BAR <= '1';
CC_BAR <= '0';
Rzero_bar <= '0';

wait for 1 ns; --	 Cycle No: 15

assert (PL_BAR = '0')
report
"Assert 180 : < PL_BAR /= 0 >" -- 	Vector No: 15
severity warning;

assert (VECT_BAR = '1')
report
"Assert 181 : < VECT_BAR /= 1 >"
severity warning;

assert (MAP_BAR = '1')
report
"Assert 182 : < MAP_BAR /= 1 >"
severity warning;

assert (R_sel = '0')
report
"Assert 183 : < R_sel /= 0 >"
severity warning;

assert (D_sel = '1')
report
"Assert 184 : < D_sel /= 1 >"
severity warning;

assert (uPC_sel = '0')
report
"Assert 185 : < uPC_sel /= 0 >"
severity warning;

assert (stack_sel = '0')
report
"Assert 186 : < stack_sel /= 0 >"
severity warning;

assert (decr = '0')
report
"Assert 187 : < decr /= 0 >"
severity warning;

assert (load = '0')
report
"Assert 188 : < load /= 0 >"
severity warning;

assert (clear = '0')
report
"Assert 189 : < clear /= 0 >"
severity warning;

assert (push = '1')
report
"Assert 190 : < push /= 1 >"
severity warning;

assert (pop = '0')
report
"Assert 191 : < pop /= 0 >"
severity warning;

wait for 1 ns;

-- ***********************************
-- *********
-- * I = 2 *
-- *********
--------------------------

I <= "0010";
CCEN_BAR <= '0';
CC_BAR <= '1';
Rzero_bar <= '1';

wait for 1 ns; --	 Cycle No: 16

assert (PL_BAR = '1')
report
"Assert 192 : < PL_BAR /= 1 >" -- 	Vector No: 16
severity warning;

assert (VECT_BAR = '1')
report
"Assert 193 : < VECT_BAR /= 1 >"
severity warning;

assert (MAP_BAR = '0')
report
"Assert 194 : < MAP_BAR /= 0 >"
severity warning;

assert (R_sel = '0')
report
"Assert 195 : < R_sel /= 0 >"
severity warning;

assert (D_sel = '1')
report
"Assert 196 : < D_sel /= 1 >"
severity warning;

assert (uPC_sel = '0')
report
"Assert 197 : < uPC_sel /= 0 >"
severity warning;

assert (stack_sel = '0')
report
"Assert 198 : < stack_sel /= 0 >"
severity warning;

assert (decr = '0')
report
"Assert 199 : < decr /= 0 >"
severity warning;

assert (load = '0')
report
"Assert 200 : < load /= 0 >"
severity warning;

assert (clear = '0')
report
"Assert 201 : < clear /= 0 >"
severity warning;

assert (push = '0')
report
"Assert 202 : < push /= 0 >"
severity warning;

assert (pop = '0')
report
"Assert 203 : < pop /= 0 >"
severity warning;

wait for 1 ns;

--------------------------

I <= "0010";
CCEN_BAR <= '0';
CC_BAR <= '1';
Rzero_bar <= '0';

wait for 1 ns; --	 Cycle No: 17

assert (PL_BAR = '1')
report
"Assert 204 : < PL_BAR /= 1 >" -- 	Vector No: 17
severity warning;

assert (VECT_BAR = '1')
report
"Assert 205 : < VECT_BAR /= 1 >"
severity warning;

assert (MAP_BAR = '0')
report
"Assert 206 : < MAP_BAR /= 0 >"
severity warning;

assert (R_sel = '0')
report
"Assert 207 : < R_sel /= 0 >"
severity warning;

assert (D_sel = '1')
report
"Assert 208 : < D_sel /= 1 >"
severity warning;

assert (uPC_sel = '0')
report
"Assert 209 : < uPC_sel /= 0 >"
severity warning;

assert (stack_sel = '0')
report
"Assert 210 : < stack_sel /= 0 >"
severity warning;

assert (decr = '0')
report
"Assert 211 : < decr /= 0 >"
severity warning;

assert (load = '0')
report
"Assert 212 : < load /= 0 >"
severity warning;

assert (clear = '0')
report
"Assert 213 : < clear /= 0 >"
severity warning;

assert (push = '0')
report
"Assert 214 : < push /= 0 >"
severity warning;

assert (pop = '0')
report
"Assert 215 : < pop /= 0 >"
severity warning;

wait for 1 ns;

-- ***********************************
--------------------------

I <= "0010";
CCEN_BAR <= '0';
CC_BAR <= '0';
Rzero_bar <= '1';

wait for 1 ns; --	 Cycle No: 18

assert (PL_BAR = '1')
report
"Assert 216 : < PL_BAR /= 1 >" -- 	Vector No: 18
severity warning;

assert (VECT_BAR = '1')
report
"Assert 217 : < VECT_BAR /= 1 >"
severity warning;

assert (MAP_BAR = '0')
report
"Assert 218 : < MAP_BAR /= 0 >"
severity warning;

assert (R_sel = '0')
report
"Assert 219 : < R_sel /= 0 >"
severity warning;

assert (D_sel = '1')
report
"Assert 220 : < D_sel /= 1 >"
severity warning;

assert (uPC_sel = '0')
report
"Assert 221 : < uPC_sel /= 0 >"
severity warning;

assert (stack_sel = '0')
report
"Assert 222 : < stack_sel /= 0 >"
severity warning;

assert (decr = '0')
report
"Assert 223 : < decr /= 0 >"
severity warning;

assert (load = '0')
report
"Assert 224 : < load /= 0 >"
severity warning;

assert (clear = '0')
report
"Assert 225 : < clear /= 0 >"
severity warning;

assert (push = '0')
report
"Assert 226 : < push /= 0 >"
severity warning;

assert (pop = '0')
report
"Assert 227 : < pop /= 0 >"
severity warning;

wait for 1 ns;

--------------------------

I <= "0010";
CCEN_BAR <= '0';
CC_BAR <= '0';
Rzero_bar <= '0';

wait for 1 ns; --	 Cycle No: 19

assert (PL_BAR = '1')
report
"Assert 228 : < PL_BAR /= 1 >" -- 	Vector No: 19
severity warning;

assert (VECT_BAR = '1')
report
"Assert 229 : < VECT_BAR /= 1 >"
severity warning;

assert (MAP_BAR = '0')
report
"Assert 230 : < MAP_BAR /= 0 >"
severity warning;

assert (R_sel = '0')
report
"Assert 231 : < R_sel /= 0 >"
severity warning;

assert (D_sel = '1')
report
"Assert 232 : < D_sel /= 1 >"
severity warning;

assert (uPC_sel = '0')
report
"Assert 233 : < uPC_sel /= 0 >"
severity warning;

assert (stack_sel = '0')
report
"Assert 234 : < stack_sel /= 0 >"
severity warning;

assert (decr = '0')
report
"Assert 235 : < decr /= 0 >"
severity warning;

assert (load = '0')
report
"Assert 236 : < load /= 0 >"
severity warning;

assert (clear = '0')
report
"Assert 237 : < clear /= 0 >"
severity warning;

assert (push = '0')
report
"Assert 238 : < push /= 0 >"
severity warning;

assert (pop = '0')
report
"Assert 239 : < pop /= 0 >"
severity warning;

wait for 1 ns;

-- ***********************************
--------------------------

I <= "0010";
CCEN_BAR <= '1';
CC_BAR <= '1';
Rzero_bar <= '1';

wait for 1 ns; --	 Cycle No: 20

assert (PL_BAR = '1')
report
"Assert 240 : < PL_BAR /= 1 >" -- 	Vector No: 20
severity warning;

assert (VECT_BAR = '1')
report
"Assert 241 : < VECT_BAR /= 1 >"
severity warning;

assert (MAP_BAR = '0')
report
"Assert 242 : < MAP_BAR /= 0 >"
severity warning;

assert (R_sel = '0')
report
"Assert 243 : < R_sel /= 0 >"
severity warning;

assert (D_sel = '1')
report
"Assert 244 : < D_sel /= 1 >"
severity warning;

assert (uPC_sel = '0')
report
"Assert 245 : < uPC_sel /= 0 >"
severity warning;

assert (stack_sel = '0')
report
"Assert 246 : < stack_sel /= 0 >"
severity warning;

assert (decr = '0')
report
"Assert 247 : < decr /= 0 >"
severity warning;

assert (load = '0')
report
"Assert 248 : < load /= 0 >"
severity warning;

assert (clear = '0')
report
"Assert 249 : < clear /= 0 >"
severity warning;

assert (push = '0')
report
"Assert 250 : < push /= 0 >"
severity warning;

assert (pop = '0')
report
"Assert 251 : < pop /= 0 >"
severity warning;

wait for 1 ns;

--------------------------

I <= "0010";
CCEN_BAR <= '1';
CC_BAR <= '1';
Rzero_bar <= '0';

wait for 1 ns; --	 Cycle No: 21

assert (PL_BAR = '1')
report
"Assert 252 : < PL_BAR /= 1 >" -- 	Vector No: 21
severity warning;

assert (VECT_BAR = '1')
report
"Assert 253 : < VECT_BAR /= 1 >"
severity warning;

assert (MAP_BAR = '0')
report
"Assert 254 : < MAP_BAR /= 0 >"
severity warning;

assert (R_sel = '0')
report
"Assert 255 : < R_sel /= 0 >"
severity warning;

assert (D_sel = '1')
report
"Assert 256 : < D_sel /= 1 >"
severity warning;

assert (uPC_sel = '0')
report
"Assert 257 : < uPC_sel /= 0 >"
severity warning;

assert (stack_sel = '0')
report
"Assert 258 : < stack_sel /= 0 >"
severity warning;

assert (decr = '0')
report
"Assert 259 : < decr /= 0 >"
severity warning;

assert (load = '0')
report
"Assert 260 : < load /= 0 >"
severity warning;

assert (clear = '0')
report
"Assert 261 : < clear /= 0 >"
severity warning;

assert (push = '0')
report
"Assert 262 : < push /= 0 >"
severity warning;

assert (pop = '0')
report
"Assert 263 : < pop /= 0 >"
severity warning;

wait for 1 ns;

-- ***********************************
--------------------------

I <= "0010";
CCEN_BAR <= '1';
CC_BAR <= '0';
Rzero_bar <= '1';

wait for 1 ns; --	 Cycle No: 22

assert (PL_BAR = '1')
report
"Assert 264 : < PL_BAR /= 1 >" -- 	Vector No: 22
severity warning;

assert (VECT_BAR = '1')
report
"Assert 265 : < VECT_BAR /= 1 >"
severity warning;

assert (MAP_BAR = '0')
report
"Assert 266 : < MAP_BAR /= 0 >"
severity warning;

assert (R_sel = '0')
report
"Assert 267 : < R_sel /= 0 >"
severity warning;

assert (D_sel = '1')
report
"Assert 268 : < D_sel /= 1 >"
severity warning;

assert (uPC_sel = '0')
report
"Assert 269 : < uPC_sel /= 0 >"
severity warning;

assert (stack_sel = '0')
report
"Assert 270 : < stack_sel /= 0 >"
severity warning;

assert (decr = '0')
report
"Assert 271 : < decr /= 0 >"
severity warning;

assert (load = '0')
report
"Assert 272 : < load /= 0 >"
severity warning;

assert (clear = '0')
report
"Assert 273 : < clear /= 0 >"
severity warning;

assert (push = '0')
report
"Assert 274 : < push /= 0 >"
severity warning;

assert (pop = '0')
report
"Assert 275 : < pop /= 0 >"
severity warning;

wait for 1 ns;

--------------------------

I <= "0010";
CCEN_BAR <= '1';
CC_BAR <= '0';
Rzero_bar <= '0';

wait for 1 ns; --	 Cycle No: 23

assert (PL_BAR = '1')
report
"Assert 276 : < PL_BAR /= 1 >" -- 	Vector No: 23
severity warning;

assert (VECT_BAR = '1')
report
"Assert 277 : < VECT_BAR /= 1 >"
severity warning;

assert (MAP_BAR = '0')
report
"Assert 278 : < MAP_BAR /= 0 >"
severity warning;

assert (R_sel = '0')
report
"Assert 279 : < R_sel /= 0 >"
severity warning;

assert (D_sel = '1')
report
"Assert 280 : < D_sel /= 1 >"
severity warning;

assert (uPC_sel = '0')
report
"Assert 281 : < uPC_sel /= 0 >"
severity warning;

assert (stack_sel = '0')
report
"Assert 282 : < stack_sel /= 0 >"
severity warning;

assert (decr = '0')
report
"Assert 283 : < decr /= 0 >"
severity warning;

assert (load = '0')
report
"Assert 284 : < load /= 0 >"
severity warning;

assert (clear = '0')
report
"Assert 285 : < clear /= 0 >"
severity warning;

assert (push = '0')
report
"Assert 286 : < push /= 0 >"
severity warning;

assert (pop = '0')
report
"Assert 287 : < pop /= 0 >"
severity warning;

wait for 1 ns;

-- ***********************************
-- *********
-- * I = 3 *
-- *********
--------------------------

I <= "0011";
CCEN_BAR <= '0';
CC_BAR <= '1';
Rzero_bar <= '1';

wait for 1 ns; --	 Cycle No: 24

assert (PL_BAR = '0')
report
"Assert 288 : < PL_BAR /= 0 >" -- 	Vector No: 24
severity warning;

assert (VECT_BAR = '1')
report
"Assert 289 : < VECT_BAR /= 1 >"
severity warning;

assert (MAP_BAR = '1')
report
"Assert 290 : < MAP_BAR /= 1 >"
severity warning;

assert (R_sel = '0')
report
"Assert 291 : < R_sel /= 0 >"
severity warning;

assert (D_sel = '0')
report
"Assert 292 : < D_sel /= 0 >"
severity warning;

assert (uPC_sel = '1')
report
"Assert 293 : < uPC_sel /= 1 >"
severity warning;

assert (stack_sel = '0')
report
"Assert 294 : < stack_sel /= 0 >"
severity warning;

assert (decr = '0')
report
"Assert 295 : < decr /= 0 >"
severity warning;

assert (load = '0')
report
"Assert 296 : < load /= 0 >"
severity warning;

assert (clear = '0')
report
"Assert 297 : < clear /= 0 >"
severity warning;

assert (push = '0')
report
"Assert 298 : < push /= 0 >"
severity warning;

assert (pop = '0')
report
"Assert 299 : < pop /= 0 >"
severity warning;

wait for 1 ns;

--------------------------

I <= "0011";
CCEN_BAR <= '0';
CC_BAR <= '1';
Rzero_bar <= '0';

wait for 1 ns; --	 Cycle No: 25

assert (PL_BAR = '0')
report
"Assert 300 : < PL_BAR /= 0 >" -- 	Vector No: 25
severity warning;

assert (VECT_BAR = '1')
report
"Assert 301 : < VECT_BAR /= 1 >"
severity warning;

assert (MAP_BAR = '1')
report
"Assert 302 : < MAP_BAR /= 1 >"
severity warning;

assert (R_sel = '0')
report
"Assert 303 : < R_sel /= 0 >"
severity warning;

assert (D_sel = '0')
report
"Assert 304 : < D_sel /= 0 >"
severity warning;

assert (uPC_sel = '1')
report
"Assert 305 : < uPC_sel /= 1 >"
severity warning;

assert (stack_sel = '0')
report
"Assert 306 : < stack_sel /= 0 >"
severity warning;

assert (decr = '0')
report
"Assert 307 : < decr /= 0 >"
severity warning;

assert (load = '0')
report
"Assert 308 : < load /= 0 >"
severity warning;

assert (clear = '0')
report
"Assert 309 : < clear /= 0 >"
severity warning;

assert (push = '0')
report
"Assert 310 : < push /= 0 >"
severity warning;

assert (pop = '0')
report
"Assert 311 : < pop /= 0 >"
severity warning;

wait for 1 ns;

-- ***********************************
--------------------------

I <= "0011";
CCEN_BAR <= '0';
CC_BAR <= '0';
Rzero_bar <= '1';

wait for 1 ns; --	 Cycle No: 26

assert (PL_BAR = '0')
report
"Assert 312 : < PL_BAR /= 0 >" -- 	Vector No: 26
severity warning;

assert (VECT_BAR = '1')
report
"Assert 313 : < VECT_BAR /= 1 >"
severity warning;

assert (MAP_BAR = '1')
report
"Assert 314 : < MAP_BAR /= 1 >"
severity warning;

assert (R_sel = '0')
report
"Assert 315 : < R_sel /= 0 >"
severity warning;

assert (D_sel = '1')
report
"Assert 316 : < D_sel /= 1 >"
severity warning;

assert (uPC_sel = '0')
report
"Assert 317 : < uPC_sel /= 0 >"
severity warning;

assert (stack_sel = '0')
report
"Assert 318 : < stack_sel /= 0 >"
severity warning;

assert (decr = '0')
report
"Assert 319 : < decr /= 0 >"
severity warning;

assert (load = '0')
report
"Assert 320 : < load /= 0 >"
severity warning;

assert (clear = '0')
report
"Assert 321 : < clear /= 0 >"
severity warning;

assert (push = '0')
report
"Assert 322 : < push /= 0 >"
severity warning;

assert (pop = '0')
report
"Assert 323 : < pop /= 0 >"
severity warning;

wait for 1 ns;

--------------------------

I <= "0011";
CCEN_BAR <= '0';
CC_BAR <= '0';
Rzero_bar <= '0';

wait for 1 ns; --	 Cycle No: 27

assert (PL_BAR = '0')
report
"Assert 324 : < PL_BAR /= 0 >" -- 	Vector No: 27
severity warning;

assert (VECT_BAR = '1')
report
"Assert 325 : < VECT_BAR /= 1 >"
severity warning;

assert (MAP_BAR = '1')
report
"Assert 326 : < MAP_BAR /= 1 >"
severity warning;

assert (R_sel = '0')
report
"Assert 327 : < R_sel /= 0 >"
severity warning;

assert (D_sel = '1')
report
"Assert 328 : < D_sel /= 1 >"
severity warning;

assert (uPC_sel = '0')
report
"Assert 329 : < uPC_sel /= 0 >"
severity warning;

assert (stack_sel = '0')
report
"Assert 330 : < stack_sel /= 0 >"
severity warning;

assert (decr = '0')
report
"Assert 331 : < decr /= 0 >"
severity warning;

assert (load = '0')
report
"Assert 332 : < load /= 0 >"
severity warning;

assert (clear = '0')
report
"Assert 333 : < clear /= 0 >"
severity warning;

assert (push = '0')
report
"Assert 334 : < push /= 0 >"
severity warning;

assert (pop = '0')
report
"Assert 335 : < pop /= 0 >"
severity warning;

wait for 1 ns;

-- ***********************************
--------------------------

I <= "0011";
CCEN_BAR <= '1';
CC_BAR <= '1';
Rzero_bar <= '1';

wait for 1 ns; --	 Cycle No: 28

assert (PL_BAR = '0')
report
"Assert 336 : < PL_BAR /= 0 >" -- 	Vector No: 28
severity warning;

assert (VECT_BAR = '1')
report
"Assert 337 : < VECT_BAR /= 1 >"
severity warning;

assert (MAP_BAR = '1')
report
"Assert 338 : < MAP_BAR /= 1 >"
severity warning;

assert (R_sel = '0')
report
"Assert 339 : < R_sel /= 0 >"
severity warning;

assert (D_sel = '1')
report
"Assert 340 : < D_sel /= 1 >"
severity warning;

assert (uPC_sel = '0')
report
"Assert 341 : < uPC_sel /= 0 >"
severity warning;

assert (stack_sel = '0')
report
"Assert 342 : < stack_sel /= 0 >"
severity warning;

assert (decr = '0')
report
"Assert 343 : < decr /= 0 >"
severity warning;

assert (load = '0')
report
"Assert 344 : < load /= 0 >"
severity warning;

assert (clear = '0')
report
"Assert 345 : < clear /= 0 >"
severity warning;

assert (push = '0')
report
"Assert 346 : < push /= 0 >"
severity warning;

assert (pop = '0')
report
"Assert 347 : < pop /= 0 >"
severity warning;

wait for 1 ns;

--------------------------

I <= "0011";
CCEN_BAR <= '1';
CC_BAR <= '1';
Rzero_bar <= '0';

wait for 1 ns; --	 Cycle No: 29

assert (PL_BAR = '0')
report
"Assert 348 : < PL_BAR /= 0 >" -- 	Vector No: 29
severity warning;

assert (VECT_BAR = '1')
report
"Assert 349 : < VECT_BAR /= 1 >"
severity warning;

assert (MAP_BAR = '1')
report
"Assert 350 : < MAP_BAR /= 1 >"
severity warning;

assert (R_sel = '0')
report
"Assert 351 : < R_sel /= 0 >"
severity warning;

assert (D_sel = '1')
report
"Assert 352 : < D_sel /= 1 >"
severity warning;

assert (uPC_sel = '0')
report
"Assert 353 : < uPC_sel /= 0 >"
severity warning;

assert (stack_sel = '0')
report
"Assert 354 : < stack_sel /= 0 >"
severity warning;

assert (decr = '0')
report
"Assert 355 : < decr /= 0 >"
severity warning;

assert (load = '0')
report
"Assert 356 : < load /= 0 >"
severity warning;

assert (clear = '0')
report
"Assert 357 : < clear /= 0 >"
severity warning;

assert (push = '0')
report
"Assert 358 : < push /= 0 >"
severity warning;

assert (pop = '0')
report
"Assert 359 : < pop /= 0 >"
severity warning;

wait for 1 ns;

-- ***********************************
--------------------------

I <= "0011";
CCEN_BAR <= '1';
CC_BAR <= '0';
Rzero_bar <= '1';

wait for 1 ns; --	 Cycle No: 30

assert (PL_BAR = '0')
report
"Assert 360 : < PL_BAR /= 0 >" -- 	Vector No: 30
severity warning;

assert (VECT_BAR = '1')
report
"Assert 361 : < VECT_BAR /= 1 >"
severity warning;

assert (MAP_BAR = '1')
report
"Assert 362 : < MAP_BAR /= 1 >"
severity warning;

assert (R_sel = '0')
report
"Assert 363 : < R_sel /= 0 >"
severity warning;

assert (D_sel = '1')
report
"Assert 364 : < D_sel /= 1 >"
severity warning;

assert (uPC_sel = '0')
report
"Assert 365 : < uPC_sel /= 0 >"
severity warning;

assert (stack_sel = '0')
report
"Assert 366 : < stack_sel /= 0 >"
severity warning;

assert (decr = '0')
report
"Assert 367 : < decr /= 0 >"
severity warning;

assert (load = '0')
report
"Assert 368 : < load /= 0 >"
severity warning;

assert (clear = '0')
report
"Assert 369 : < clear /= 0 >"
severity warning;

assert (push = '0')
report
"Assert 370 : < push /= 0 >"
severity warning;

assert (pop = '0')
report
"Assert 371 : < pop /= 0 >"
severity warning;

wait for 1 ns;

--------------------------

I <= "0011";
CCEN_BAR <= '1';
CC_BAR <= '0';
Rzero_bar <= '0';

wait for 1 ns; --	 Cycle No: 31

assert (PL_BAR = '0')
report
"Assert 372 : < PL_BAR /= 0 >" -- 	Vector No: 31
severity warning;

assert (VECT_BAR = '1')
report
"Assert 373 : < VECT_BAR /= 1 >"
severity warning;

assert (MAP_BAR = '1')
report
"Assert 374 : < MAP_BAR /= 1 >"
severity warning;

assert (R_sel = '0')
report
"Assert 375 : < R_sel /= 0 >"
severity warning;

assert (D_sel = '1')
report
"Assert 376 : < D_sel /= 1 >"
severity warning;

assert (uPC_sel = '0')
report
"Assert 377 : < uPC_sel /= 0 >"
severity warning;

assert (stack_sel = '0')
report
"Assert 378 : < stack_sel /= 0 >"
severity warning;

assert (decr = '0')
report
"Assert 379 : < decr /= 0 >"
severity warning;

assert (load = '0')
report
"Assert 380 : < load /= 0 >"
severity warning;

assert (clear = '0')
report
"Assert 381 : < clear /= 0 >"
severity warning;

assert (push = '0')
report
"Assert 382 : < push /= 0 >"
severity warning;

assert (pop = '0')
report
"Assert 383 : < pop /= 0 >"
severity warning;

wait for 1 ns;

-- ***********************************
-- *********
-- * I = 4 *
-- *********
--------------------------

I <= "0100";
CCEN_BAR <= '0';
CC_BAR <= '1';
Rzero_bar <= '1';

wait for 1 ns; --	 Cycle No: 32

assert (PL_BAR = '0')
report
"Assert 384 : < PL_BAR /= 0 >" -- 	Vector No: 32
severity warning;

assert (VECT_BAR = '1')
report
"Assert 385 : < VECT_BAR /= 1 >"
severity warning;

assert (MAP_BAR = '1')
report
"Assert 386 : < MAP_BAR /= 1 >"
severity warning;

assert (R_sel = '0')
report
"Assert 387 : < R_sel /= 0 >"
severity warning;

assert (D_sel = '0')
report
"Assert 388 : < D_sel /= 0 >"
severity warning;

assert (uPC_sel = '1')
report
"Assert 389 : < uPC_sel /= 1 >"
severity warning;

assert (stack_sel = '0')
report
"Assert 390 : < stack_sel /= 0 >"
severity warning;

assert (decr = '0')
report
"Assert 391 : < decr /= 0 >"
severity warning;

assert (load = '0')
report
"Assert 392 : < load /= 0 >"
severity warning;

assert (clear = '0')
report
"Assert 393 : < clear /= 0 >"
severity warning;

assert (push = '1')
report
"Assert 394 : < push /= 1 >"
severity warning;

assert (pop = '0')
report
"Assert 395 : < pop /= 0 >"
severity warning;

wait for 1 ns;

--------------------------

I <= "0100";
CCEN_BAR <= '0';
CC_BAR <= '1';
Rzero_bar <= '0';

wait for 1 ns; --	 Cycle No: 33

assert (PL_BAR = '0')
report
"Assert 396 : < PL_BAR /= 0 >" -- 	Vector No: 33
severity warning;

assert (VECT_BAR = '1')
report
"Assert 397 : < VECT_BAR /= 1 >"
severity warning;

assert (MAP_BAR = '1')
report
"Assert 398 : < MAP_BAR /= 1 >"
severity warning;

assert (R_sel = '0')
report
"Assert 399 : < R_sel /= 0 >"
severity warning;

assert (D_sel = '0')
report
"Assert 400 : < D_sel /= 0 >"
severity warning;

assert (uPC_sel = '1')
report
"Assert 401 : < uPC_sel /= 1 >"
severity warning;

assert (stack_sel = '0')
report
"Assert 402 : < stack_sel /= 0 >"
severity warning;

assert (decr = '0')
report
"Assert 403 : < decr /= 0 >"
severity warning;

assert (load = '0')
report
"Assert 404 : < load /= 0 >"
severity warning;

assert (clear = '0')
report
"Assert 405 : < clear /= 0 >"
severity warning;

assert (push = '1')
report
"Assert 406 : < push /= 1 >"
severity warning;

assert (pop = '0')
report
"Assert 407 : < pop /= 0 >"
severity warning;

wait for 1 ns;

-- ***********************************
--------------------------

I <= "0100";
CCEN_BAR <= '0';
CC_BAR <= '0';
Rzero_bar <= '1';

wait for 1 ns; --	 Cycle No: 34

assert (PL_BAR = '0')
report
"Assert 408 : < PL_BAR /= 0 >" -- 	Vector No: 34
severity warning;

assert (VECT_BAR = '1')
report
"Assert 409 : < VECT_BAR /= 1 >"
severity warning;

assert (MAP_BAR = '1')
report
"Assert 410 : < MAP_BAR /= 1 >"
severity warning;

assert (R_sel = '0')
report
"Assert 411 : < R_sel /= 0 >"
severity warning;

assert (D_sel = '0')
report
"Assert 412 : < D_sel /= 0 >"
severity warning;

assert (uPC_sel = '1')
report
"Assert 413 : < uPC_sel /= 1 >"
severity warning;

assert (stack_sel = '0')
report
"Assert 414 : < stack_sel /= 0 >"
severity warning;

assert (decr = '0')
report
"Assert 415 : < decr /= 0 >"
severity warning;

assert (load = '1')
report
"Assert 416 : < load /= 1 >"
severity warning;

assert (clear = '0')
report
"Assert 417 : < clear /= 0 >"
severity warning;

assert (push = '1')
report
"Assert 418 : < push /= 1 >"
severity warning;

assert (pop = '0')
report
"Assert 419 : < pop /= 0 >"
severity warning;

wait for 1 ns;

--------------------------

I <= "0100";
CCEN_BAR <= '0';
CC_BAR <= '0';
Rzero_bar <= '0';

wait for 1 ns; --	 Cycle No: 35

assert (PL_BAR = '0')
report
"Assert 420 : < PL_BAR /= 0 >" -- 	Vector No: 35
severity warning;

assert (VECT_BAR = '1')
report
"Assert 421 : < VECT_BAR /= 1 >"
severity warning;

assert (MAP_BAR = '1')
report
"Assert 422 : < MAP_BAR /= 1 >"
severity warning;

assert (R_sel = '0')
report
"Assert 423 : < R_sel /= 0 >"
severity warning;

assert (D_sel = '0')
report
"Assert 424 : < D_sel /= 0 >"
severity warning;

assert (uPC_sel = '1')
report
"Assert 425 : < uPC_sel /= 1 >"
severity warning;

assert (stack_sel = '0')
report
"Assert 426 : < stack_sel /= 0 >"
severity warning;

assert (decr = '0')
report
"Assert 427 : < decr /= 0 >"
severity warning;

assert (load = '1')
report
"Assert 428 : < load /= 1 >"
severity warning;

assert (clear = '0')
report
"Assert 429 : < clear /= 0 >"
severity warning;

assert (push = '1')
report
"Assert 430 : < push /= 1 >"
severity warning;

assert (pop = '0')
report
"Assert 431 : < pop /= 0 >"
severity warning;

wait for 1 ns;

-- ***********************************
--------------------------

I <= "0100";
CCEN_BAR <= '1';
CC_BAR <= '1';
Rzero_bar <= '1';

wait for 1 ns; --	 Cycle No: 36

assert (PL_BAR = '0')
report
"Assert 432 : < PL_BAR /= 0 >" -- 	Vector No: 36
severity warning;

assert (VECT_BAR = '1')
report
"Assert 433 : < VECT_BAR /= 1 >"
severity warning;

assert (MAP_BAR = '1')
report
"Assert 434 : < MAP_BAR /= 1 >"
severity warning;

assert (R_sel = '0')
report
"Assert 435 : < R_sel /= 0 >"
severity warning;

assert (D_sel = '0')
report
"Assert 436 : < D_sel /= 0 >"
severity warning;

assert (uPC_sel = '1')
report
"Assert 437 : < uPC_sel /= 1 >"
severity warning;

assert (stack_sel = '0')
report
"Assert 438 : < stack_sel /= 0 >"
severity warning;

assert (decr = '0')
report
"Assert 439 : < decr /= 0 >"
severity warning;

assert (load = '1')
report
"Assert 440 : < load /= 1 >"
severity warning;

assert (clear = '0')
report
"Assert 441 : < clear /= 0 >"
severity warning;

assert (push = '1')
report
"Assert 442 : < push /= 1 >"
severity warning;

assert (pop = '0')
report
"Assert 443 : < pop /= 0 >"
severity warning;

wait for 1 ns;

--------------------------

I <= "0100";
CCEN_BAR <= '1';
CC_BAR <= '1';
Rzero_bar <= '0';

wait for 1 ns; --	 Cycle No: 37

assert (PL_BAR = '0')
report
"Assert 444 : < PL_BAR /= 0 >" -- 	Vector No: 37
severity warning;

assert (VECT_BAR = '1')
report
"Assert 445 : < VECT_BAR /= 1 >"
severity warning;

assert (MAP_BAR = '1')
report
"Assert 446 : < MAP_BAR /= 1 >"
severity warning;

assert (R_sel = '0')
report
"Assert 447 : < R_sel /= 0 >"
severity warning;

assert (D_sel = '0')
report
"Assert 448 : < D_sel /= 0 >"
severity warning;

assert (uPC_sel = '1')
report
"Assert 449 : < uPC_sel /= 1 >"
severity warning;

assert (stack_sel = '0')
report
"Assert 450 : < stack_sel /= 0 >"
severity warning;

assert (decr = '0')
report
"Assert 451 : < decr /= 0 >"
severity warning;

assert (load = '1')
report
"Assert 452 : < load /= 1 >"
severity warning;

assert (clear = '0')
report
"Assert 453 : < clear /= 0 >"
severity warning;

assert (push = '1')
report
"Assert 454 : < push /= 1 >"
severity warning;

assert (pop = '0')
report
"Assert 455 : < pop /= 0 >"
severity warning;

wait for 1 ns;

-- ***********************************
--------------------------

I <= "0100";
CCEN_BAR <= '1';
CC_BAR <= '0';
Rzero_bar <= '1';

wait for 1 ns; --	 Cycle No: 38

assert (PL_BAR = '0')
report
"Assert 456 : < PL_BAR /= 0 >" -- 	Vector No: 38
severity warning;

assert (VECT_BAR = '1')
report
"Assert 457 : < VECT_BAR /= 1 >"
severity warning;

assert (MAP_BAR = '1')
report
"Assert 458 : < MAP_BAR /= 1 >"
severity warning;

assert (R_sel = '0')
report
"Assert 459 : < R_sel /= 0 >"
severity warning;

assert (D_sel = '0')
report
"Assert 460 : < D_sel /= 0 >"
severity warning;

assert (uPC_sel = '1')
report
"Assert 461 : < uPC_sel /= 1 >"
severity warning;

assert (stack_sel = '0')
report
"Assert 462 : < stack_sel /= 0 >"
severity warning;

assert (decr = '0')
report
"Assert 463 : < decr /= 0 >"
severity warning;

assert (load = '1')
report
"Assert 464 : < load /= 1 >"
severity warning;

assert (clear = '0')
report
"Assert 465 : < clear /= 0 >"
severity warning;

assert (push = '1')
report
"Assert 466 : < push /= 1 >"
severity warning;

assert (pop = '0')
report
"Assert 467 : < pop /= 0 >"
severity warning;

wait for 1 ns;

--------------------------

I <= "0100";
CCEN_BAR <= '1';
CC_BAR <= '0';
Rzero_bar <= '0';

wait for 1 ns; --	 Cycle No: 39

assert (PL_BAR = '0')
report
"Assert 468 : < PL_BAR /= 0 >" -- 	Vector No: 39
severity warning;

assert (VECT_BAR = '1')
report
"Assert 469 : < VECT_BAR /= 1 >"
severity warning;

assert (MAP_BAR = '1')
report
"Assert 470 : < MAP_BAR /= 1 >"
severity warning;

assert (R_sel = '0')
report
"Assert 471 : < R_sel /= 0 >"
severity warning;

assert (D_sel = '0')
report
"Assert 472 : < D_sel /= 0 >"
severity warning;

assert (uPC_sel = '1')
report
"Assert 473 : < uPC_sel /= 1 >"
severity warning;

assert (stack_sel = '0')
report
"Assert 474 : < stack_sel /= 0 >"
severity warning;

assert (decr = '0')
report
"Assert 475 : < decr /= 0 >"
severity warning;

assert (load = '1')
report
"Assert 476 : < load /= 1 >"
severity warning;

assert (clear = '0')
report
"Assert 477 : < clear /= 0 >"
severity warning;

assert (push = '1')
report
"Assert 478 : < push /= 1 >"
severity warning;

assert (pop = '0')
report
"Assert 479 : < pop /= 0 >"
severity warning;

wait for 1 ns;

-- ***********************************
-- *********
-- * I = 5 *
-- *********
--------------------------

I <= "0101";
CCEN_BAR <= '0';
CC_BAR <= '1';
Rzero_bar <= '1';

wait for 1 ns; --	 Cycle No: 40

assert (PL_BAR = '0')
report
"Assert 480 : < PL_BAR /= 0 >" -- 	Vector No: 40
severity warning;

assert (VECT_BAR = '1')
report
"Assert 481 : < VECT_BAR /= 1 >"
severity warning;

assert (MAP_BAR = '1')
report
"Assert 482 : < MAP_BAR /= 1 >"
severity warning;

assert (R_sel = '1')
report
"Assert 483 : < R_sel /= 1 >"
severity warning;

assert (D_sel = '0')
report
"Assert 484 : < D_sel /= 0 >"
severity warning;

assert (uPC_sel = '0')
report
"Assert 485 : < uPC_sel /= 0 >"
severity warning;

assert (stack_sel = '0')
report
"Assert 486 : < stack_sel /= 0 >"
severity warning;

assert (decr = '0')
report
"Assert 487 : < decr /= 0 >"
severity warning;

assert (load = '0')
report
"Assert 488 : < load /= 0 >"
severity warning;

assert (clear = '0')
report
"Assert 489 : < clear /= 0 >"
severity warning;

assert (push = '1')
report
"Assert 490 : < push /= 1 >"
severity warning;

assert (pop = '0')
report
"Assert 491 : < pop /= 0 >"
severity warning;

wait for 1 ns;

--------------------------

I <= "0101";
CCEN_BAR <= '0';
CC_BAR <= '1';
Rzero_bar <= '0';

wait for 1 ns; --	 Cycle No: 41

assert (PL_BAR = '0')
report
"Assert 492 : < PL_BAR /= 0 >" -- 	Vector No: 41
severity warning;

assert (VECT_BAR = '1')
report
"Assert 493 : < VECT_BAR /= 1 >"
severity warning;

assert (MAP_BAR = '1')
report
"Assert 494 : < MAP_BAR /= 1 >"
severity warning;

assert (R_sel = '1')
report
"Assert 495 : < R_sel /= 1 >"
severity warning;

assert (D_sel = '0')
report
"Assert 496 : < D_sel /= 0 >"
severity warning;

assert (uPC_sel = '0')
report
"Assert 497 : < uPC_sel /= 0 >"
severity warning;

assert (stack_sel = '0')
report
"Assert 498 : < stack_sel /= 0 >"
severity warning;

assert (decr = '0')
report
"Assert 499 : < decr /= 0 >"
severity warning;

assert (load = '0')
report
"Assert 500 : < load /= 0 >"
severity warning;

assert (clear = '0')
report
"Assert 501 : < clear /= 0 >"
severity warning;

assert (push = '1')
report
"Assert 502 : < push /= 1 >"
severity warning;

assert (pop = '0')
report
"Assert 503 : < pop /= 0 >"
severity warning;

wait for 1 ns;

-- ***********************************
--------------------------

I <= "0101";
CCEN_BAR <= '0';
CC_BAR <= '0';
Rzero_bar <= '1';

wait for 1 ns; --	 Cycle No: 42

assert (PL_BAR = '0')
report
"Assert 504 : < PL_BAR /= 0 >" -- 	Vector No: 42
severity warning;

assert (VECT_BAR = '1')
report
"Assert 505 : < VECT_BAR /= 1 >"
severity warning;

assert (MAP_BAR = '1')
report
"Assert 506 : < MAP_BAR /= 1 >"
severity warning;

assert (R_sel = '0')
report
"Assert 507 : < R_sel /= 0 >"
severity warning;

assert (D_sel = '1')
report
"Assert 508 : < D_sel /= 1 >"
severity warning;

assert (uPC_sel = '0')
report
"Assert 509 : < uPC_sel /= 0 >"
severity warning;

assert (stack_sel = '0')
report
"Assert 510 : < stack_sel /= 0 >"
severity warning;

assert (decr = '0')
report
"Assert 511 : < decr /= 0 >"
severity warning;

assert (load = '0')
report
"Assert 512 : < load /= 0 >"
severity warning;

assert (clear = '0')
report
"Assert 513 : < clear /= 0 >"
severity warning;

assert (push = '1')
report
"Assert 514 : < push /= 1 >"
severity warning;

assert (pop = '0')
report
"Assert 515 : < pop /= 0 >"
severity warning;

wait for 1 ns;

--------------------------

I <= "0101";
CCEN_BAR <= '0';
CC_BAR <= '0';
Rzero_bar <= '0';

wait for 1 ns; --	 Cycle No: 43

assert (PL_BAR = '0')
report
"Assert 516 : < PL_BAR /= 0 >" -- 	Vector No: 43
severity warning;

assert (VECT_BAR = '1')
report
"Assert 517 : < VECT_BAR /= 1 >"
severity warning;

assert (MAP_BAR = '1')
report
"Assert 518 : < MAP_BAR /= 1 >"
severity warning;

assert (R_sel = '0')
report
"Assert 519 : < R_sel /= 0 >"
severity warning;

assert (D_sel = '1')
report
"Assert 520 : < D_sel /= 1 >"
severity warning;

assert (uPC_sel = '0')
report
"Assert 521 : < uPC_sel /= 0 >"
severity warning;

assert (stack_sel = '0')
report
"Assert 522 : < stack_sel /= 0 >"
severity warning;

assert (decr = '0')
report
"Assert 523 : < decr /= 0 >"
severity warning;

assert (load = '0')
report
"Assert 524 : < load /= 0 >"
severity warning;

assert (clear = '0')
report
"Assert 525 : < clear /= 0 >"
severity warning;

assert (push = '1')
report
"Assert 526 : < push /= 1 >"
severity warning;

assert (pop = '0')
report
"Assert 527 : < pop /= 0 >"
severity warning;

wait for 1 ns;

-- ***********************************
--------------------------

I <= "0101";
CCEN_BAR <= '1';
CC_BAR <= '1';
Rzero_bar <= '1';

wait for 1 ns; --	 Cycle No: 44

assert (PL_BAR = '0')
report
"Assert 528 : < PL_BAR /= 0 >" -- 	Vector No: 44
severity warning;

assert (VECT_BAR = '1')
report
"Assert 529 : < VECT_BAR /= 1 >"
severity warning;

assert (MAP_BAR = '1')
report
"Assert 530 : < MAP_BAR /= 1 >"
severity warning;

assert (R_sel = '0')
report
"Assert 531 : < R_sel /= 0 >"
severity warning;

assert (D_sel = '1')
report
"Assert 532 : < D_sel /= 1 >"
severity warning;

assert (uPC_sel = '0')
report
"Assert 533 : < uPC_sel /= 0 >"
severity warning;

assert (stack_sel = '0')
report
"Assert 534 : < stack_sel /= 0 >"
severity warning;

assert (decr = '0')
report
"Assert 535 : < decr /= 0 >"
severity warning;

assert (load = '0')
report
"Assert 536 : < load /= 0 >"
severity warning;

assert (clear = '0')
report
"Assert 537 : < clear /= 0 >"
severity warning;

assert (push = '1')
report
"Assert 538 : < push /= 1 >"
severity warning;

assert (pop = '0')
report
"Assert 539 : < pop /= 0 >"
severity warning;

wait for 1 ns;

--------------------------

I <= "0101";
CCEN_BAR <= '1';
CC_BAR <= '1';
Rzero_bar <= '0';

wait for 1 ns; --	 Cycle No: 45

assert (PL_BAR = '0')
report
"Assert 540 : < PL_BAR /= 0 >" -- 	Vector No: 45
severity warning;

assert (VECT_BAR = '1')
report
"Assert 541 : < VECT_BAR /= 1 >"
severity warning;

assert (MAP_BAR = '1')
report
"Assert 542 : < MAP_BAR /= 1 >"
severity warning;

assert (R_sel = '0')
report
"Assert 543 : < R_sel /= 0 >"
severity warning;

assert (D_sel = '1')
report
"Assert 544 : < D_sel /= 1 >"
severity warning;

assert (uPC_sel = '0')
report
"Assert 545 : < uPC_sel /= 0 >"
severity warning;

assert (stack_sel = '0')
report
"Assert 546 : < stack_sel /= 0 >"
severity warning;

assert (decr = '0')
report
"Assert 547 : < decr /= 0 >"
severity warning;

assert (load = '0')
report
"Assert 548 : < load /= 0 >"
severity warning;

assert (clear = '0')
report
"Assert 549 : < clear /= 0 >"
severity warning;

assert (push = '1')
report
"Assert 550 : < push /= 1 >"
severity warning;

assert (pop = '0')
report
"Assert 551 : < pop /= 0 >"
severity warning;

wait for 1 ns;

-- ***********************************
--------------------------

I <= "0101";
CCEN_BAR <= '1';
CC_BAR <= '0';
Rzero_bar <= '1';

wait for 1 ns; --	 Cycle No: 46

assert (PL_BAR = '0')
report
"Assert 552 : < PL_BAR /= 0 >" -- 	Vector No: 46
severity warning;

assert (VECT_BAR = '1')
report
"Assert 553 : < VECT_BAR /= 1 >"
severity warning;

assert (MAP_BAR = '1')
report
"Assert 554 : < MAP_BAR /= 1 >"
severity warning;

assert (R_sel = '0')
report
"Assert 555 : < R_sel /= 0 >"
severity warning;

assert (D_sel = '1')
report
"Assert 556 : < D_sel /= 1 >"
severity warning;

assert (uPC_sel = '0')
report
"Assert 557 : < uPC_sel /= 0 >"
severity warning;

assert (stack_sel = '0')
report
"Assert 558 : < stack_sel /= 0 >"
severity warning;

assert (decr = '0')
report
"Assert 559 : < decr /= 0 >"
severity warning;

assert (load = '0')
report
"Assert 560 : < load /= 0 >"
severity warning;

assert (clear = '0')
report
"Assert 561 : < clear /= 0 >"
severity warning;

assert (push = '1')
report
"Assert 562 : < push /= 1 >"
severity warning;

assert (pop = '0')
report
"Assert 563 : < pop /= 0 >"
severity warning;

wait for 1 ns;

--------------------------

I <= "0101";
CCEN_BAR <= '1';
CC_BAR <= '0';
Rzero_bar <= '0';

wait for 1 ns; --	 Cycle No: 47

assert (PL_BAR = '0')
report
"Assert 564 : < PL_BAR /= 0 >" -- 	Vector No: 47
severity warning;

assert (VECT_BAR = '1')
report
"Assert 565 : < VECT_BAR /= 1 >"
severity warning;

assert (MAP_BAR = '1')
report
"Assert 566 : < MAP_BAR /= 1 >"
severity warning;

assert (R_sel = '0')
report
"Assert 567 : < R_sel /= 0 >"
severity warning;

assert (D_sel = '1')
report
"Assert 568 : < D_sel /= 1 >"
severity warning;

assert (uPC_sel = '0')
report
"Assert 569 : < uPC_sel /= 0 >"
severity warning;

assert (stack_sel = '0')
report
"Assert 570 : < stack_sel /= 0 >"
severity warning;

assert (decr = '0')
report
"Assert 571 : < decr /= 0 >"
severity warning;

assert (load = '0')
report
"Assert 572 : < load /= 0 >"
severity warning;

assert (clear = '0')
report
"Assert 573 : < clear /= 0 >"
severity warning;

assert (push = '1')
report
"Assert 574 : < push /= 1 >"
severity warning;

assert (pop = '0')
report
"Assert 575 : < pop /= 0 >"
severity warning;

wait for 1 ns;

-- ***********************************
-- *********
-- * I = 6 *
-- *********
--------------------------

I <= "0110";
CCEN_BAR <= '0';
CC_BAR <= '1';
Rzero_bar <= '1';

wait for 1 ns; --	 Cycle No: 48

assert (PL_BAR = '1')
report
"Assert 576 : < PL_BAR /= 1 >" -- 	Vector No: 48
severity warning;

assert (VECT_BAR = '0')
report
"Assert 577 : < VECT_BAR /= 0 >"
severity warning;

assert (MAP_BAR = '1')
report
"Assert 578 : < MAP_BAR /= 1 >"
severity warning;

assert (R_sel = '0')
report
"Assert 579 : < R_sel /= 0 >"
severity warning;

assert (D_sel = '0')
report
"Assert 580 : < D_sel /= 0 >"
severity warning;

assert (uPC_sel = '1')
report
"Assert 581 : < uPC_sel /= 1 >"
severity warning;

assert (stack_sel = '0')
report
"Assert 582 : < stack_sel /= 0 >"
severity warning;

assert (decr = '0')
report
"Assert 583 : < decr /= 0 >"
severity warning;

assert (load = '0')
report
"Assert 584 : < load /= 0 >"
severity warning;

assert (clear = '0')
report
"Assert 585 : < clear /= 0 >"
severity warning;

assert (push = '0')
report
"Assert 586 : < push /= 0 >"
severity warning;

assert (pop = '0')
report
"Assert 587 : < pop /= 0 >"
severity warning;

wait for 1 ns;

--------------------------

I <= "0110";
CCEN_BAR <= '0';
CC_BAR <= '1';
Rzero_bar <= '0';

wait for 1 ns; --	 Cycle No: 49

assert (PL_BAR = '1')
report
"Assert 588 : < PL_BAR /= 1 >" -- 	Vector No: 49
severity warning;

assert (VECT_BAR = '0')
report
"Assert 589 : < VECT_BAR /= 0 >"
severity warning;

assert (MAP_BAR = '1')
report
"Assert 590 : < MAP_BAR /= 1 >"
severity warning;

assert (R_sel = '0')
report
"Assert 591 : < R_sel /= 0 >"
severity warning;

assert (D_sel = '0')
report
"Assert 592 : < D_sel /= 0 >"
severity warning;

assert (uPC_sel = '1')
report
"Assert 593 : < uPC_sel /= 1 >"
severity warning;

assert (stack_sel = '0')
report
"Assert 594 : < stack_sel /= 0 >"
severity warning;

assert (decr = '0')
report
"Assert 595 : < decr /= 0 >"
severity warning;

assert (load = '0')
report
"Assert 596 : < load /= 0 >"
severity warning;

assert (clear = '0')
report
"Assert 597 : < clear /= 0 >"
severity warning;

assert (push = '0')
report
"Assert 598 : < push /= 0 >"
severity warning;

assert (pop = '0')
report
"Assert 599 : < pop /= 0 >"
severity warning;

wait for 1 ns;

-- ***********************************
--------------------------

I <= "0110";
CCEN_BAR <= '0';
CC_BAR <= '0';
Rzero_bar <= '1';

wait for 1 ns; --	 Cycle No: 50

assert (PL_BAR = '1')
report
"Assert 600 : < PL_BAR /= 1 >" -- 	Vector No: 50
severity warning;

assert (VECT_BAR = '0')
report
"Assert 601 : < VECT_BAR /= 0 >"
severity warning;

assert (MAP_BAR = '1')
report
"Assert 602 : < MAP_BAR /= 1 >"
severity warning;

assert (R_sel = '0')
report
"Assert 603 : < R_sel /= 0 >"
severity warning;

assert (D_sel = '1')
report
"Assert 604 : < D_sel /= 1 >"
severity warning;

assert (uPC_sel = '0')
report
"Assert 605 : < uPC_sel /= 0 >"
severity warning;

assert (stack_sel = '0')
report
"Assert 606 : < stack_sel /= 0 >"
severity warning;

assert (decr = '0')
report
"Assert 607 : < decr /= 0 >"
severity warning;

assert (load = '0')
report
"Assert 608 : < load /= 0 >"
severity warning;

assert (clear = '0')
report
"Assert 609 : < clear /= 0 >"
severity warning;

assert (push = '0')
report
"Assert 610 : < push /= 0 >"
severity warning;

assert (pop = '0')
report
"Assert 611 : < pop /= 0 >"
severity warning;

wait for 1 ns;

--------------------------

I <= "0110";
CCEN_BAR <= '0';
CC_BAR <= '0';
Rzero_bar <= '0';

wait for 1 ns; --	 Cycle No: 51

assert (PL_BAR = '1')
report
"Assert 612 : < PL_BAR /= 1 >" -- 	Vector No: 51
severity warning;

assert (VECT_BAR = '0')
report
"Assert 613 : < VECT_BAR /= 0 >"
severity warning;

assert (MAP_BAR = '1')
report
"Assert 614 : < MAP_BAR /= 1 >"
severity warning;

assert (R_sel = '0')
report
"Assert 615 : < R_sel /= 0 >"
severity warning;

assert (D_sel = '1')
report
"Assert 616 : < D_sel /= 1 >"
severity warning;

assert (uPC_sel = '0')
report
"Assert 617 : < uPC_sel /= 0 >"
severity warning;

assert (stack_sel = '0')
report
"Assert 618 : < stack_sel /= 0 >"
severity warning;

assert (decr = '0')
report
"Assert 619 : < decr /= 0 >"
severity warning;

assert (load = '0')
report
"Assert 620 : < load /= 0 >"
severity warning;

assert (clear = '0')
report
"Assert 621 : < clear /= 0 >"
severity warning;

assert (push = '0')
report
"Assert 622 : < push /= 0 >"
severity warning;

assert (pop = '0')
report
"Assert 623 : < pop /= 0 >"
severity warning;

wait for 1 ns;

-- ***********************************
--------------------------

I <= "0110";
CCEN_BAR <= '1';
CC_BAR <= '1';
Rzero_bar <= '1';

wait for 1 ns; --	 Cycle No: 52

assert (PL_BAR = '1')
report
"Assert 624 : < PL_BAR /= 1 >" -- 	Vector No: 52
severity warning;

assert (VECT_BAR = '0')
report
"Assert 625 : < VECT_BAR /= 0 >"
severity warning;

assert (MAP_BAR = '1')
report
"Assert 626 : < MAP_BAR /= 1 >"
severity warning;

assert (R_sel = '0')
report
"Assert 627 : < R_sel /= 0 >"
severity warning;

assert (D_sel = '1')
report
"Assert 628 : < D_sel /= 1 >"
severity warning;

assert (uPC_sel = '0')
report
"Assert 629 : < uPC_sel /= 0 >"
severity warning;

assert (stack_sel = '0')
report
"Assert 630 : < stack_sel /= 0 >"
severity warning;

assert (decr = '0')
report
"Assert 631 : < decr /= 0 >"
severity warning;

assert (load = '0')
report
"Assert 632 : < load /= 0 >"
severity warning;

assert (clear = '0')
report
"Assert 633 : < clear /= 0 >"
severity warning;

assert (push = '0')
report
"Assert 634 : < push /= 0 >"
severity warning;

assert (pop = '0')
report
"Assert 635 : < pop /= 0 >"
severity warning;

wait for 1 ns;

--------------------------

I <= "0110";
CCEN_BAR <= '1';
CC_BAR <= '1';
Rzero_bar <= '0';

wait for 1 ns; --	 Cycle No: 53

assert (PL_BAR = '1')
report
"Assert 636 : < PL_BAR /= 1 >" -- 	Vector No: 53
severity warning;

assert (VECT_BAR = '0')
report
"Assert 637 : < VECT_BAR /= 0 >"
severity warning;

assert (MAP_BAR = '1')
report
"Assert 638 : < MAP_BAR /= 1 >"
severity warning;

assert (R_sel = '0')
report
"Assert 639 : < R_sel /= 0 >"
severity warning;

assert (D_sel = '1')
report
"Assert 640 : < D_sel /= 1 >"
severity warning;

assert (uPC_sel = '0')
report
"Assert 641 : < uPC_sel /= 0 >"
severity warning;

assert (stack_sel = '0')
report
"Assert 642 : < stack_sel /= 0 >"
severity warning;

assert (decr = '0')
report
"Assert 643 : < decr /= 0 >"
severity warning;

assert (load = '0')
report
"Assert 644 : < load /= 0 >"
severity warning;

assert (clear = '0')
report
"Assert 645 : < clear /= 0 >"
severity warning;

assert (push = '0')
report
"Assert 646 : < push /= 0 >"
severity warning;

assert (pop = '0')
report
"Assert 647 : < pop /= 0 >"
severity warning;

wait for 1 ns;

-- ***********************************
--------------------------

I <= "0110";
CCEN_BAR <= '1';
CC_BAR <= '0';
Rzero_bar <= '1';

wait for 1 ns; --	 Cycle No: 54

assert (PL_BAR = '1')
report
"Assert 648 : < PL_BAR /= 1 >" -- 	Vector No: 54
severity warning;

assert (VECT_BAR = '0')
report
"Assert 649 : < VECT_BAR /= 0 >"
severity warning;

assert (MAP_BAR = '1')
report
"Assert 650 : < MAP_BAR /= 1 >"
severity warning;

assert (R_sel = '0')
report
"Assert 651 : < R_sel /= 0 >"
severity warning;

assert (D_sel = '1')
report
"Assert 652 : < D_sel /= 1 >"
severity warning;

assert (uPC_sel = '0')
report
"Assert 653 : < uPC_sel /= 0 >"
severity warning;

assert (stack_sel = '0')
report
"Assert 654 : < stack_sel /= 0 >"
severity warning;

assert (decr = '0')
report
"Assert 655 : < decr /= 0 >"
severity warning;

assert (load = '0')
report
"Assert 656 : < load /= 0 >"
severity warning;

assert (clear = '0')
report
"Assert 657 : < clear /= 0 >"
severity warning;

assert (push = '0')
report
"Assert 658 : < push /= 0 >"
severity warning;

assert (pop = '0')
report
"Assert 659 : < pop /= 0 >"
severity warning;

wait for 1 ns;

--------------------------

I <= "0110";
CCEN_BAR <= '1';
CC_BAR <= '0';
Rzero_bar <= '0';

wait for 1 ns; --	 Cycle No: 55

assert (PL_BAR = '1')
report
"Assert 660 : < PL_BAR /= 1 >" -- 	Vector No: 55
severity warning;

assert (VECT_BAR = '0')
report
"Assert 661 : < VECT_BAR /= 0 >"
severity warning;

assert (MAP_BAR = '1')
report
"Assert 662 : < MAP_BAR /= 1 >"
severity warning;

assert (R_sel = '0')
report
"Assert 663 : < R_sel /= 0 >"
severity warning;

assert (D_sel = '1')
report
"Assert 664 : < D_sel /= 1 >"
severity warning;

assert (uPC_sel = '0')
report
"Assert 665 : < uPC_sel /= 0 >"
severity warning;

assert (stack_sel = '0')
report
"Assert 666 : < stack_sel /= 0 >"
severity warning;

assert (decr = '0')
report
"Assert 667 : < decr /= 0 >"
severity warning;

assert (load = '0')
report
"Assert 668 : < load /= 0 >"
severity warning;

assert (clear = '0')
report
"Assert 669 : < clear /= 0 >"
severity warning;

assert (push = '0')
report
"Assert 670 : < push /= 0 >"
severity warning;

assert (pop = '0')
report
"Assert 671 : < pop /= 0 >"
severity warning;

wait for 1 ns;

-- ***********************************
-- *********
-- * I = 7 *
-- *********
--------------------------

I <= "0111";
CCEN_BAR <= '0';
CC_BAR <= '1';
Rzero_bar <= '1';

wait for 1 ns; --	 Cycle No: 56

assert (PL_BAR = '0')
report
"Assert 672 : < PL_BAR /= 0 >" -- 	Vector No: 56
severity warning;

assert (VECT_BAR = '1')
report
"Assert 673 : < VECT_BAR /= 1 >"
severity warning;

assert (MAP_BAR = '1')
report
"Assert 674 : < MAP_BAR /= 1 >"
severity warning;

assert (R_sel = '1')
report
"Assert 675 : < R_sel /= 1 >"
severity warning;

assert (D_sel = '0')
report
"Assert 676 : < D_sel /= 0 >"
severity warning;

assert (uPC_sel = '0')
report
"Assert 677 : < uPC_sel /= 0 >"
severity warning;

assert (stack_sel = '0')
report
"Assert 678 : < stack_sel /= 0 >"
severity warning;

assert (decr = '0')
report
"Assert 679 : < decr /= 0 >"
severity warning;

assert (load = '0')
report
"Assert 680 : < load /= 0 >"
severity warning;

assert (clear = '0')
report
"Assert 681 : < clear /= 0 >"
severity warning;

assert (push = '0')
report
"Assert 682 : < push /= 0 >"
severity warning;

assert (pop = '0')
report
"Assert 683 : < pop /= 0 >"
severity warning;

wait for 1 ns;

--------------------------

I <= "0111";
CCEN_BAR <= '0';
CC_BAR <= '1';
Rzero_bar <= '0';

wait for 1 ns; --	 Cycle No: 57

assert (PL_BAR = '0')
report
"Assert 684 : < PL_BAR /= 0 >" -- 	Vector No: 57
severity warning;

assert (VECT_BAR = '1')
report
"Assert 685 : < VECT_BAR /= 1 >"
severity warning;

assert (MAP_BAR = '1')
report
"Assert 686 : < MAP_BAR /= 1 >"
severity warning;

assert (R_sel = '1')
report
"Assert 687 : < R_sel /= 1 >"
severity warning;

assert (D_sel = '0')
report
"Assert 688 : < D_sel /= 0 >"
severity warning;

assert (uPC_sel = '0')
report
"Assert 689 : < uPC_sel /= 0 >"
severity warning;

assert (stack_sel = '0')
report
"Assert 690 : < stack_sel /= 0 >"
severity warning;

assert (decr = '0')
report
"Assert 691 : < decr /= 0 >"
severity warning;

assert (load = '0')
report
"Assert 692 : < load /= 0 >"
severity warning;

assert (clear = '0')
report
"Assert 693 : < clear /= 0 >"
severity warning;

assert (push = '0')
report
"Assert 694 : < push /= 0 >"
severity warning;

assert (pop = '0')
report
"Assert 695 : < pop /= 0 >"
severity warning;

wait for 1 ns;

-- ***********************************
--------------------------

I <= "0111";
CCEN_BAR <= '0';
CC_BAR <= '0';
Rzero_bar <= '1';

wait for 1 ns; --	 Cycle No: 58

assert (PL_BAR = '0')
report
"Assert 696 : < PL_BAR /= 0 >" -- 	Vector No: 58
severity warning;

assert (VECT_BAR = '1')
report
"Assert 697 : < VECT_BAR /= 1 >"
severity warning;

assert (MAP_BAR = '1')
report
"Assert 698 : < MAP_BAR /= 1 >"
severity warning;

assert (R_sel = '0')
report
"Assert 699 : < R_sel /= 0 >"
severity warning;

assert (D_sel = '1')
report
"Assert 700 : < D_sel /= 1 >"
severity warning;

assert (uPC_sel = '0')
report
"Assert 701 : < uPC_sel /= 0 >"
severity warning;

assert (stack_sel = '0')
report
"Assert 702 : < stack_sel /= 0 >"
severity warning;

assert (decr = '0')
report
"Assert 703 : < decr /= 0 >"
severity warning;

assert (load = '0')
report
"Assert 704 : < load /= 0 >"
severity warning;

assert (clear = '0')
report
"Assert 705 : < clear /= 0 >"
severity warning;

assert (push = '0')
report
"Assert 706 : < push /= 0 >"
severity warning;

assert (pop = '0')
report
"Assert 707 : < pop /= 0 >"
severity warning;

wait for 1 ns;

--------------------------

I <= "0111";
CCEN_BAR <= '0';
CC_BAR <= '0';
Rzero_bar <= '0';

wait for 1 ns; --	 Cycle No: 59

assert (PL_BAR = '0')
report
"Assert 708 : < PL_BAR /= 0 >" -- 	Vector No: 59
severity warning;

assert (VECT_BAR = '1')
report
"Assert 709 : < VECT_BAR /= 1 >"
severity warning;

assert (MAP_BAR = '1')
report
"Assert 710 : < MAP_BAR /= 1 >"
severity warning;

assert (R_sel = '0')
report
"Assert 711 : < R_sel /= 0 >"
severity warning;

assert (D_sel = '1')
report
"Assert 712 : < D_sel /= 1 >"
severity warning;

assert (uPC_sel = '0')
report
"Assert 713 : < uPC_sel /= 0 >"
severity warning;

assert (stack_sel = '0')
report
"Assert 714 : < stack_sel /= 0 >"
severity warning;

assert (decr = '0')
report
"Assert 715 : < decr /= 0 >"
severity warning;

assert (load = '0')
report
"Assert 716 : < load /= 0 >"
severity warning;

assert (clear = '0')
report
"Assert 717 : < clear /= 0 >"
severity warning;

assert (push = '0')
report
"Assert 718 : < push /= 0 >"
severity warning;

assert (pop = '0')
report
"Assert 719 : < pop /= 0 >"
severity warning;

wait for 1 ns;

-- ***********************************
--------------------------

I <= "0111";
CCEN_BAR <= '1';
CC_BAR <= '1';
Rzero_bar <= '1';

wait for 1 ns; --	 Cycle No: 60

assert (PL_BAR = '0')
report
"Assert 720 : < PL_BAR /= 0 >" -- 	Vector No: 60
severity warning;

assert (VECT_BAR = '1')
report
"Assert 721 : < VECT_BAR /= 1 >"
severity warning;

assert (MAP_BAR = '1')
report
"Assert 722 : < MAP_BAR /= 1 >"
severity warning;

assert (R_sel = '0')
report
"Assert 723 : < R_sel /= 0 >"
severity warning;

assert (D_sel = '1')
report
"Assert 724 : < D_sel /= 1 >"
severity warning;

assert (uPC_sel = '0')
report
"Assert 725 : < uPC_sel /= 0 >"
severity warning;

assert (stack_sel = '0')
report
"Assert 726 : < stack_sel /= 0 >"
severity warning;

assert (decr = '0')
report
"Assert 727 : < decr /= 0 >"
severity warning;

assert (load = '0')
report
"Assert 728 : < load /= 0 >"
severity warning;

assert (clear = '0')
report
"Assert 729 : < clear /= 0 >"
severity warning;

assert (push = '0')
report
"Assert 730 : < push /= 0 >"
severity warning;

assert (pop = '0')
report
"Assert 731 : < pop /= 0 >"
severity warning;

wait for 1 ns;

--------------------------

I <= "0111";
CCEN_BAR <= '1';
CC_BAR <= '1';
Rzero_bar <= '0';

wait for 1 ns; --	 Cycle No: 61

assert (PL_BAR = '0')
report
"Assert 732 : < PL_BAR /= 0 >" -- 	Vector No: 61
severity warning;

assert (VECT_BAR = '1')
report
"Assert 733 : < VECT_BAR /= 1 >"
severity warning;

assert (MAP_BAR = '1')
report
"Assert 734 : < MAP_BAR /= 1 >"
severity warning;

assert (R_sel = '0')
report
"Assert 735 : < R_sel /= 0 >"
severity warning;

assert (D_sel = '1')
report
"Assert 736 : < D_sel /= 1 >"
severity warning;

assert (uPC_sel = '0')
report
"Assert 737 : < uPC_sel /= 0 >"
severity warning;

assert (stack_sel = '0')
report
"Assert 738 : < stack_sel /= 0 >"
severity warning;

assert (decr = '0')
report
"Assert 739 : < decr /= 0 >"
severity warning;

assert (load = '0')
report
"Assert 740 : < load /= 0 >"
severity warning;

assert (clear = '0')
report
"Assert 741 : < clear /= 0 >"
severity warning;

assert (push = '0')
report
"Assert 742 : < push /= 0 >"
severity warning;

assert (pop = '0')
report
"Assert 743 : < pop /= 0 >"
severity warning;

wait for 1 ns;

-- ***********************************
--------------------------

I <= "0111";
CCEN_BAR <= '1';
CC_BAR <= '0';
Rzero_bar <= '1';

wait for 1 ns; --	 Cycle No: 62

assert (PL_BAR = '0')
report
"Assert 744 : < PL_BAR /= 0 >" -- 	Vector No: 62
severity warning;

assert (VECT_BAR = '1')
report
"Assert 745 : < VECT_BAR /= 1 >"
severity warning;

assert (MAP_BAR = '1')
report
"Assert 746 : < MAP_BAR /= 1 >"
severity warning;

assert (R_sel = '0')
report
"Assert 747 : < R_sel /= 0 >"
severity warning;

assert (D_sel = '1')
report
"Assert 748 : < D_sel /= 1 >"
severity warning;

assert (uPC_sel = '0')
report
"Assert 749 : < uPC_sel /= 0 >"
severity warning;

assert (stack_sel = '0')
report
"Assert 750 : < stack_sel /= 0 >"
severity warning;

assert (decr = '0')
report
"Assert 751 : < decr /= 0 >"
severity warning;

assert (load = '0')
report
"Assert 752 : < load /= 0 >"
severity warning;

assert (clear = '0')
report
"Assert 753 : < clear /= 0 >"
severity warning;

assert (push = '0')
report
"Assert 754 : < push /= 0 >"
severity warning;

assert (pop = '0')
report
"Assert 755 : < pop /= 0 >"
severity warning;

wait for 1 ns;

--------------------------

I <= "0111";
CCEN_BAR <= '1';
CC_BAR <= '0';
Rzero_bar <= '0';

wait for 1 ns; --	 Cycle No: 63

assert (PL_BAR = '0')
report
"Assert 756 : < PL_BAR /= 0 >" -- 	Vector No: 63
severity warning;

assert (VECT_BAR = '1')
report
"Assert 757 : < VECT_BAR /= 1 >"
severity warning;

assert (MAP_BAR = '1')
report
"Assert 758 : < MAP_BAR /= 1 >"
severity warning;

assert (R_sel = '0')
report
"Assert 759 : < R_sel /= 0 >"
severity warning;

assert (D_sel = '1')
report
"Assert 760 : < D_sel /= 1 >"
severity warning;

assert (uPC_sel = '0')
report
"Assert 761 : < uPC_sel /= 0 >"
severity warning;

assert (stack_sel = '0')
report
"Assert 762 : < stack_sel /= 0 >"
severity warning;

assert (decr = '0')
report
"Assert 763 : < decr /= 0 >"
severity warning;

assert (load = '0')
report
"Assert 764 : < load /= 0 >"
severity warning;

assert (clear = '0')
report
"Assert 765 : < clear /= 0 >"
severity warning;

assert (push = '0')
report
"Assert 766 : < push /= 0 >"
severity warning;

assert (pop = '0')
report
"Assert 767 : < pop /= 0 >"
severity warning;

wait for 1 ns;

-- ***********************************
-- *********
-- * I = 8 *
-- *********
--------------------------

I <= "1000";
CCEN_BAR <= '0';
CC_BAR <= '1';
Rzero_bar <= '1';

wait for 1 ns; --	 Cycle No: 64

assert (PL_BAR = '0')
report
"Assert 768 : < PL_BAR /= 0 >" -- 	Vector No: 64
severity warning;

assert (VECT_BAR = '1')
report
"Assert 769 : < VECT_BAR /= 1 >"
severity warning;

assert (MAP_BAR = '1')
report
"Assert 770 : < MAP_BAR /= 1 >"
severity warning;

assert (R_sel = '0')
report
"Assert 771 : < R_sel /= 0 >"
severity warning;

assert (D_sel = '0')
report
"Assert 772 : < D_sel /= 0 >"
severity warning;

assert (uPC_sel = '0')
report
"Assert 773 : < uPC_sel /= 0 >"
severity warning;

assert (stack_sel = '1')
report
"Assert 774 : < stack_sel /= 1 >"
severity warning;

assert (decr = '1')
report
"Assert 775 : < decr /= 1 >"
severity warning;

assert (load = '0')
report
"Assert 776 : < load /= 0 >"
severity warning;

assert (clear = '0')
report
"Assert 777 : < clear /= 0 >"
severity warning;

assert (push = '0')
report
"Assert 778 : < push /= 0 >"
severity warning;

assert (pop = '0')
report
"Assert 779 : < pop /= 0 >"
severity warning;

wait for 1 ns;

--------------------------

I <= "1000";
CCEN_BAR <= '0';
CC_BAR <= '1';
Rzero_bar <= '0';

wait for 1 ns; --	 Cycle No: 65

assert (PL_BAR = '0')
report
"Assert 780 : < PL_BAR /= 0 >" -- 	Vector No: 65
severity warning;

assert (VECT_BAR = '1')
report
"Assert 781 : < VECT_BAR /= 1 >"
severity warning;

assert (MAP_BAR = '1')
report
"Assert 782 : < MAP_BAR /= 1 >"
severity warning;

assert (R_sel = '0')
report
"Assert 783 : < R_sel /= 0 >"
severity warning;

assert (D_sel = '0')
report
"Assert 784 : < D_sel /= 0 >"
severity warning;

assert (uPC_sel = '1')
report
"Assert 785 : < uPC_sel /= 1 >"
severity warning;

assert (stack_sel = '0')
report
"Assert 786 : < stack_sel /= 0 >"
severity warning;

assert (decr = '0')
report
"Assert 787 : < decr /= 0 >"
severity warning;

assert (load = '0')
report
"Assert 788 : < load /= 0 >"
severity warning;

assert (clear = '0')
report
"Assert 789 : < clear /= 0 >"
severity warning;

assert (push = '0')
report
"Assert 790 : < push /= 0 >"
severity warning;

assert (pop = '1')
report
"Assert 791 : < pop /= 1 >"
severity warning;

wait for 1 ns;

-- ***********************************
--------------------------

I <= "1000";
CCEN_BAR <= '0';
CC_BAR <= '0';
Rzero_bar <= '1';

wait for 1 ns; --	 Cycle No: 66

assert (PL_BAR = '0')
report
"Assert 792 : < PL_BAR /= 0 >" -- 	Vector No: 66
severity warning;

assert (VECT_BAR = '1')
report
"Assert 793 : < VECT_BAR /= 1 >"
severity warning;

assert (MAP_BAR = '1')
report
"Assert 794 : < MAP_BAR /= 1 >"
severity warning;

assert (R_sel = '0')
report
"Assert 795 : < R_sel /= 0 >"
severity warning;

assert (D_sel = '0')
report
"Assert 796 : < D_sel /= 0 >"
severity warning;

assert (uPC_sel = '0')
report
"Assert 797 : < uPC_sel /= 0 >"
severity warning;

assert (stack_sel = '1')
report
"Assert 798 : < stack_sel /= 1 >"
severity warning;

assert (decr = '1')
report
"Assert 799 : < decr /= 1 >"
severity warning;

assert (load = '0')
report
"Assert 800 : < load /= 0 >"
severity warning;

assert (clear = '0')
report
"Assert 801 : < clear /= 0 >"
severity warning;

assert (push = '0')
report
"Assert 802 : < push /= 0 >"
severity warning;

assert (pop = '0')
report
"Assert 803 : < pop /= 0 >"
severity warning;

wait for 1 ns;

--------------------------

I <= "1000";
CCEN_BAR <= '0';
CC_BAR <= '0';
Rzero_bar <= '0';

wait for 1 ns; --	 Cycle No: 67

assert (PL_BAR = '0')
report
"Assert 804 : < PL_BAR /= 0 >" -- 	Vector No: 67
severity warning;

assert (VECT_BAR = '1')
report
"Assert 805 : < VECT_BAR /= 1 >"
severity warning;

assert (MAP_BAR = '1')
report
"Assert 806 : < MAP_BAR /= 1 >"
severity warning;

assert (R_sel = '0')
report
"Assert 807 : < R_sel /= 0 >"
severity warning;

assert (D_sel = '0')
report
"Assert 808 : < D_sel /= 0 >"
severity warning;

assert (uPC_sel = '1')
report
"Assert 809 : < uPC_sel /= 1 >"
severity warning;

assert (stack_sel = '0')
report
"Assert 810 : < stack_sel /= 0 >"
severity warning;

assert (decr = '0')
report
"Assert 811 : < decr /= 0 >"
severity warning;

assert (load = '0')
report
"Assert 812 : < load /= 0 >"
severity warning;

assert (clear = '0')
report
"Assert 813 : < clear /= 0 >"
severity warning;

assert (push = '0')
report
"Assert 814 : < push /= 0 >"
severity warning;

assert (pop = '1')
report
"Assert 815 : < pop /= 1 >"
severity warning;

wait for 1 ns;

-- ***********************************
--------------------------

I <= "1000";
CCEN_BAR <= '1';
CC_BAR <= '1';
Rzero_bar <= '1';

wait for 1 ns; --	 Cycle No: 68

assert (PL_BAR = '0')
report
"Assert 816 : < PL_BAR /= 0 >" -- 	Vector No: 68
severity warning;

assert (VECT_BAR = '1')
report
"Assert 817 : < VECT_BAR /= 1 >"
severity warning;

assert (MAP_BAR = '1')
report
"Assert 818 : < MAP_BAR /= 1 >"
severity warning;

assert (R_sel = '0')
report
"Assert 819 : < R_sel /= 0 >"
severity warning;

assert (D_sel = '0')
report
"Assert 820 : < D_sel /= 0 >"
severity warning;

assert (uPC_sel = '0')
report
"Assert 821 : < uPC_sel /= 0 >"
severity warning;

assert (stack_sel = '1')
report
"Assert 822 : < stack_sel /= 1 >"
severity warning;

assert (decr = '1')
report
"Assert 823 : < decr /= 1 >"
severity warning;

assert (load = '0')
report
"Assert 824 : < load /= 0 >"
severity warning;

assert (clear = '0')
report
"Assert 825 : < clear /= 0 >"
severity warning;

assert (push = '0')
report
"Assert 826 : < push /= 0 >"
severity warning;

assert (pop = '0')
report
"Assert 827 : < pop /= 0 >"
severity warning;

wait for 1 ns;

--------------------------

I <= "1000";
CCEN_BAR <= '1';
CC_BAR <= '1';
Rzero_bar <= '0';

wait for 1 ns; --	 Cycle No: 69

assert (PL_BAR = '0')
report
"Assert 828 : < PL_BAR /= 0 >" -- 	Vector No: 69
severity warning;

assert (VECT_BAR = '1')
report
"Assert 829 : < VECT_BAR /= 1 >"
severity warning;

assert (MAP_BAR = '1')
report
"Assert 830 : < MAP_BAR /= 1 >"
severity warning;

assert (R_sel = '0')
report
"Assert 831 : < R_sel /= 0 >"
severity warning;

assert (D_sel = '0')
report
"Assert 832 : < D_sel /= 0 >"
severity warning;

assert (uPC_sel = '1')
report
"Assert 833 : < uPC_sel /= 1 >"
severity warning;

assert (stack_sel = '0')
report
"Assert 834 : < stack_sel /= 0 >"
severity warning;

assert (decr = '0')
report
"Assert 835 : < decr /= 0 >"
severity warning;

assert (load = '0')
report
"Assert 836 : < load /= 0 >"
severity warning;

assert (clear = '0')
report
"Assert 837 : < clear /= 0 >"
severity warning;

assert (push = '0')
report
"Assert 838 : < push /= 0 >"
severity warning;

assert (pop = '1')
report
"Assert 839 : < pop /= 1 >"
severity warning;

wait for 1 ns;

-- ***********************************
--------------------------

I <= "1000";
CCEN_BAR <= '1';
CC_BAR <= '0';
Rzero_bar <= '1';

wait for 1 ns; --	 Cycle No: 70

assert (PL_BAR = '0')
report
"Assert 840 : < PL_BAR /= 0 >" -- 	Vector No: 70
severity warning;

assert (VECT_BAR = '1')
report
"Assert 841 : < VECT_BAR /= 1 >"
severity warning;

assert (MAP_BAR = '1')
report
"Assert 842 : < MAP_BAR /= 1 >"
severity warning;

assert (R_sel = '0')
report
"Assert 843 : < R_sel /= 0 >"
severity warning;

assert (D_sel = '0')
report
"Assert 844 : < D_sel /= 0 >"
severity warning;

assert (uPC_sel = '0')
report
"Assert 845 : < uPC_sel /= 0 >"
severity warning;

assert (stack_sel = '1')
report
"Assert 846 : < stack_sel /= 1 >"
severity warning;

assert (decr = '1')
report
"Assert 847 : < decr /= 1 >"
severity warning;

assert (load = '0')
report
"Assert 848 : < load /= 0 >"
severity warning;

assert (clear = '0')
report
"Assert 849 : < clear /= 0 >"
severity warning;

assert (push = '0')
report
"Assert 850 : < push /= 0 >"
severity warning;

assert (pop = '0')
report
"Assert 851 : < pop /= 0 >"
severity warning;

wait for 1 ns;

--------------------------

I <= "1000";
CCEN_BAR <= '1';
CC_BAR <= '0';
Rzero_bar <= '0';

wait for 1 ns; --	 Cycle No: 71

assert (PL_BAR = '0')
report
"Assert 852 : < PL_BAR /= 0 >" -- 	Vector No: 71
severity warning;

assert (VECT_BAR = '1')
report
"Assert 853 : < VECT_BAR /= 1 >"
severity warning;

assert (MAP_BAR = '1')
report
"Assert 854 : < MAP_BAR /= 1 >"
severity warning;

assert (R_sel = '0')
report
"Assert 855 : < R_sel /= 0 >"
severity warning;

assert (D_sel = '0')
report
"Assert 856 : < D_sel /= 0 >"
severity warning;

assert (uPC_sel = '1')
report
"Assert 857 : < uPC_sel /= 1 >"
severity warning;

assert (stack_sel = '0')
report
"Assert 858 : < stack_sel /= 0 >"
severity warning;

assert (decr = '0')
report
"Assert 859 : < decr /= 0 >"
severity warning;

assert (load = '0')
report
"Assert 860 : < load /= 0 >"
severity warning;

assert (clear = '0')
report
"Assert 861 : < clear /= 0 >"
severity warning;

assert (push = '0')
report
"Assert 862 : < push /= 0 >"
severity warning;

assert (pop = '1')
report
"Assert 863 : < pop /= 1 >"
severity warning;

wait for 1 ns;

-- ***********************************
-- *********
-- * I = 9 *
-- *********
--------------------------

I <= "1001";
CCEN_BAR <= '0';
CC_BAR <= '1';
Rzero_bar <= '1';

wait for 1 ns; --	 Cycle No: 72

assert (PL_BAR = '0')
report
"Assert 864 : < PL_BAR /= 0 >" -- 	Vector No: 72
severity warning;

assert (VECT_BAR = '1')
report
"Assert 865 : < VECT_BAR /= 1 >"
severity warning;

assert (MAP_BAR = '1')
report
"Assert 866 : < MAP_BAR /= 1 >"
severity warning;

assert (R_sel = '0')
report
"Assert 867 : < R_sel /= 0 >"
severity warning;

assert (D_sel = '1')
report
"Assert 868 : < D_sel /= 1 >"
severity warning;

assert (uPC_sel = '0')
report
"Assert 869 : < uPC_sel /= 0 >"
severity warning;

assert (stack_sel = '0')
report
"Assert 870 : < stack_sel /= 0 >"
severity warning;

assert (decr = '1')
report
"Assert 871 : < decr /= 1 >"
severity warning;

assert (load = '0')
report
"Assert 872 : < load /= 0 >"
severity warning;

assert (clear = '0')
report
"Assert 873 : < clear /= 0 >"
severity warning;

assert (push = '0')
report
"Assert 874 : < push /= 0 >"
severity warning;

assert (pop = '0')
report
"Assert 875 : < pop /= 0 >"
severity warning;

wait for 1 ns;

--------------------------

I <= "1001";
CCEN_BAR <= '0';
CC_BAR <= '1';
Rzero_bar <= '0';

wait for 1 ns; --	 Cycle No: 73

assert (PL_BAR = '0')
report
"Assert 876 : < PL_BAR /= 0 >" -- 	Vector No: 73
severity warning;

assert (VECT_BAR = '1')
report
"Assert 877 : < VECT_BAR /= 1 >"
severity warning;

assert (MAP_BAR = '1')
report
"Assert 878 : < MAP_BAR /= 1 >"
severity warning;

assert (R_sel = '0')
report
"Assert 879 : < R_sel /= 0 >"
severity warning;

assert (D_sel = '0')
report
"Assert 880 : < D_sel /= 0 >"
severity warning;

assert (uPC_sel = '1')
report
"Assert 881 : < uPC_sel /= 1 >"
severity warning;

assert (stack_sel = '0')
report
"Assert 882 : < stack_sel /= 0 >"
severity warning;

assert (decr = '0')
report
"Assert 883 : < decr /= 0 >"
severity warning;

assert (load = '0')
report
"Assert 884 : < load /= 0 >"
severity warning;

assert (clear = '0')
report
"Assert 885 : < clear /= 0 >"
severity warning;

assert (push = '0')
report
"Assert 886 : < push /= 0 >"
severity warning;

assert (pop = '0')
report
"Assert 887 : < pop /= 0 >"
severity warning;

wait for 1 ns;

-- ***********************************
--------------------------

I <= "1001";
CCEN_BAR <= '0';
CC_BAR <= '0';
Rzero_bar <= '1';

wait for 1 ns; --	 Cycle No: 74

assert (PL_BAR = '0')
report
"Assert 888 : < PL_BAR /= 0 >" -- 	Vector No: 74
severity warning;

assert (VECT_BAR = '1')
report
"Assert 889 : < VECT_BAR /= 1 >"
severity warning;

assert (MAP_BAR = '1')
report
"Assert 890 : < MAP_BAR /= 1 >"
severity warning;

assert (R_sel = '0')
report
"Assert 891 : < R_sel /= 0 >"
severity warning;

assert (D_sel = '1')
report
"Assert 892 : < D_sel /= 1 >"
severity warning;

assert (uPC_sel = '0')
report
"Assert 893 : < uPC_sel /= 0 >"
severity warning;

assert (stack_sel = '0')
report
"Assert 894 : < stack_sel /= 0 >"
severity warning;

assert (decr = '1')
report
"Assert 895 : < decr /= 1 >"
severity warning;

assert (load = '0')
report
"Assert 896 : < load /= 0 >"
severity warning;

assert (clear = '0')
report
"Assert 897 : < clear /= 0 >"
severity warning;

assert (push = '0')
report
"Assert 898 : < push /= 0 >"
severity warning;

assert (pop = '0')
report
"Assert 899 : < pop /= 0 >"
severity warning;

wait for 1 ns;

--------------------------

I <= "1001";
CCEN_BAR <= '0';
CC_BAR <= '0';
Rzero_bar <= '0';

wait for 1 ns; --	 Cycle No: 75

assert (PL_BAR = '0')
report
"Assert 900 : < PL_BAR /= 0 >" -- 	Vector No: 75
severity warning;

assert (VECT_BAR = '1')
report
"Assert 901 : < VECT_BAR /= 1 >"
severity warning;

assert (MAP_BAR = '1')
report
"Assert 902 : < MAP_BAR /= 1 >"
severity warning;

assert (R_sel = '0')
report
"Assert 903 : < R_sel /= 0 >"
severity warning;

assert (D_sel = '0')
report
"Assert 904 : < D_sel /= 0 >"
severity warning;

assert (uPC_sel = '1')
report
"Assert 905 : < uPC_sel /= 1 >"
severity warning;

assert (stack_sel = '0')
report
"Assert 906 : < stack_sel /= 0 >"
severity warning;

assert (decr = '0')
report
"Assert 907 : < decr /= 0 >"
severity warning;

assert (load = '0')
report
"Assert 908 : < load /= 0 >"
severity warning;

assert (clear = '0')
report
"Assert 909 : < clear /= 0 >"
severity warning;

assert (push = '0')
report
"Assert 910 : < push /= 0 >"
severity warning;

assert (pop = '0')
report
"Assert 911 : < pop /= 0 >"
severity warning;

wait for 1 ns;

-- ***********************************
--------------------------

I <= "1001";
CCEN_BAR <= '1';
CC_BAR <= '1';
Rzero_bar <= '1';

wait for 1 ns; --	 Cycle No: 76

assert (PL_BAR = '0')
report
"Assert 912 : < PL_BAR /= 0 >" -- 	Vector No: 76
severity warning;

assert (VECT_BAR = '1')
report
"Assert 913 : < VECT_BAR /= 1 >"
severity warning;

assert (MAP_BAR = '1')
report
"Assert 914 : < MAP_BAR /= 1 >"
severity warning;

assert (R_sel = '0')
report
"Assert 915 : < R_sel /= 0 >"
severity warning;

assert (D_sel = '1')
report
"Assert 916 : < D_sel /= 1 >"
severity warning;

assert (uPC_sel = '0')
report
"Assert 917 : < uPC_sel /= 0 >"
severity warning;

assert (stack_sel = '0')
report
"Assert 918 : < stack_sel /= 0 >"
severity warning;

assert (decr = '1')
report
"Assert 919 : < decr /= 1 >"
severity warning;

assert (load = '0')
report
"Assert 920 : < load /= 0 >"
severity warning;

assert (clear = '0')
report
"Assert 921 : < clear /= 0 >"
severity warning;

assert (push = '0')
report
"Assert 922 : < push /= 0 >"
severity warning;

assert (pop = '0')
report
"Assert 923 : < pop /= 0 >"
severity warning;

wait for 1 ns;

--------------------------

I <= "1001";
CCEN_BAR <= '1';
CC_BAR <= '1';
Rzero_bar <= '0';

wait for 1 ns; --	 Cycle No: 77

assert (PL_BAR = '0')
report
"Assert 924 : < PL_BAR /= 0 >" -- 	Vector No: 77
severity warning;

assert (VECT_BAR = '1')
report
"Assert 925 : < VECT_BAR /= 1 >"
severity warning;

assert (MAP_BAR = '1')
report
"Assert 926 : < MAP_BAR /= 1 >"
severity warning;

assert (R_sel = '0')
report
"Assert 927 : < R_sel /= 0 >"
severity warning;

assert (D_sel = '0')
report
"Assert 928 : < D_sel /= 0 >"
severity warning;

assert (uPC_sel = '1')
report
"Assert 929 : < uPC_sel /= 1 >"
severity warning;

assert (stack_sel = '0')
report
"Assert 930 : < stack_sel /= 0 >"
severity warning;

assert (decr = '0')
report
"Assert 931 : < decr /= 0 >"
severity warning;

assert (load = '0')
report
"Assert 932 : < load /= 0 >"
severity warning;

assert (clear = '0')
report
"Assert 933 : < clear /= 0 >"
severity warning;

assert (push = '0')
report
"Assert 934 : < push /= 0 >"
severity warning;

assert (pop = '0')
report
"Assert 935 : < pop /= 0 >"
severity warning;

wait for 1 ns;

-- ***********************************
--------------------------

I <= "1001";
CCEN_BAR <= '1';
CC_BAR <= '0';
Rzero_bar <= '1';

wait for 1 ns; --	 Cycle No: 78

assert (PL_BAR = '0')
report
"Assert 936 : < PL_BAR /= 0 >" -- 	Vector No: 78
severity warning;

assert (VECT_BAR = '1')
report
"Assert 937 : < VECT_BAR /= 1 >"
severity warning;

assert (MAP_BAR = '1')
report
"Assert 938 : < MAP_BAR /= 1 >"
severity warning;

assert (R_sel = '0')
report
"Assert 939 : < R_sel /= 0 >"
severity warning;

assert (D_sel = '1')
report
"Assert 940 : < D_sel /= 1 >"
severity warning;

assert (uPC_sel = '0')
report
"Assert 941 : < uPC_sel /= 0 >"
severity warning;

assert (stack_sel = '0')
report
"Assert 942 : < stack_sel /= 0 >"
severity warning;

assert (decr = '1')
report
"Assert 943 : < decr /= 1 >"
severity warning;

assert (load = '0')
report
"Assert 944 : < load /= 0 >"
severity warning;

assert (clear = '0')
report
"Assert 945 : < clear /= 0 >"
severity warning;

assert (push = '0')
report
"Assert 946 : < push /= 0 >"
severity warning;

assert (pop = '0')
report
"Assert 947 : < pop /= 0 >"
severity warning;

wait for 1 ns;

--------------------------

I <= "1001";
CCEN_BAR <= '1';
CC_BAR <= '0';
Rzero_bar <= '0';

wait for 1 ns; --	 Cycle No: 79

assert (PL_BAR = '0')
report
"Assert 948 : < PL_BAR /= 0 >" -- 	Vector No: 79
severity warning;

assert (VECT_BAR = '1')
report
"Assert 949 : < VECT_BAR /= 1 >"
severity warning;

assert (MAP_BAR = '1')
report
"Assert 950 : < MAP_BAR /= 1 >"
severity warning;

assert (R_sel = '0')
report
"Assert 951 : < R_sel /= 0 >"
severity warning;

assert (D_sel = '0')
report
"Assert 952 : < D_sel /= 0 >"
severity warning;

assert (uPC_sel = '1')
report
"Assert 953 : < uPC_sel /= 1 >"
severity warning;

assert (stack_sel = '0')
report
"Assert 954 : < stack_sel /= 0 >"
severity warning;

assert (decr = '0')
report
"Assert 955 : < decr /= 0 >"
severity warning;

assert (load = '0')
report
"Assert 956 : < load /= 0 >"
severity warning;

assert (clear = '0')
report
"Assert 957 : < clear /= 0 >"
severity warning;

assert (push = '0')
report
"Assert 958 : < push /= 0 >"
severity warning;

assert (pop = '0')
report
"Assert 959 : < pop /= 0 >"
severity warning;

wait for 1 ns;

-- ***********************************
-- **********
-- * I = 10 *
-- **********
--------------------------

I <= "1010";
CCEN_BAR <= '0';
CC_BAR <= '1';
Rzero_bar <= '1';

wait for 1 ns; --	 Cycle No: 80

assert (PL_BAR = '0')
report
"Assert 960 : < PL_BAR /= 0 >" -- 	Vector No: 80
severity warning;

assert (VECT_BAR = '1')
report
"Assert 961 : < VECT_BAR /= 1 >"
severity warning;

assert (MAP_BAR = '1')
report
"Assert 962 : < MAP_BAR /= 1 >"
severity warning;

assert (R_sel = '0')
report
"Assert 963 : < R_sel /= 0 >"
severity warning;

assert (D_sel = '0')
report
"Assert 964 : < D_sel /= 0 >"
severity warning;

assert (uPC_sel = '1')
report
"Assert 965 : < uPC_sel /= 1 >"
severity warning;

assert (stack_sel = '0')
report
"Assert 966 : < stack_sel /= 0 >"
severity warning;

assert (decr = '0')
report
"Assert 967 : < decr /= 0 >"
severity warning;

assert (load = '0')
report
"Assert 968 : < load /= 0 >"
severity warning;

assert (clear = '0')
report
"Assert 969 : < clear /= 0 >"
severity warning;

assert (push = '0')
report
"Assert 970 : < push /= 0 >"
severity warning;

assert (pop = '0')
report
"Assert 971 : < pop /= 0 >"
severity warning;

wait for 1 ns;

--------------------------

I <= "1010";
CCEN_BAR <= '0';
CC_BAR <= '1';
Rzero_bar <= '0';

wait for 1 ns; --	 Cycle No: 81

assert (PL_BAR = '0')
report
"Assert 972 : < PL_BAR /= 0 >" -- 	Vector No: 81
severity warning;

assert (VECT_BAR = '1')
report
"Assert 973 : < VECT_BAR /= 1 >"
severity warning;

assert (MAP_BAR = '1')
report
"Assert 974 : < MAP_BAR /= 1 >"
severity warning;

assert (R_sel = '0')
report
"Assert 975 : < R_sel /= 0 >"
severity warning;

assert (D_sel = '0')
report
"Assert 976 : < D_sel /= 0 >"
severity warning;

assert (uPC_sel = '1')
report
"Assert 977 : < uPC_sel /= 1 >"
severity warning;

assert (stack_sel = '0')
report
"Assert 978 : < stack_sel /= 0 >"
severity warning;

assert (decr = '0')
report
"Assert 979 : < decr /= 0 >"
severity warning;

assert (load = '0')
report
"Assert 980 : < load /= 0 >"
severity warning;

assert (clear = '0')
report
"Assert 981 : < clear /= 0 >"
severity warning;

assert (push = '0')
report
"Assert 982 : < push /= 0 >"
severity warning;

assert (pop = '0')
report
"Assert 983 : < pop /= 0 >"
severity warning;

wait for 1 ns;

-- ***********************************
--------------------------

I <= "1010";
CCEN_BAR <= '0';
CC_BAR <= '0';
Rzero_bar <= '1';

wait for 1 ns; --	 Cycle No: 82

assert (PL_BAR = '0')
report
"Assert 984 : < PL_BAR /= 0 >" -- 	Vector No: 82
severity warning;

assert (VECT_BAR = '1')
report
"Assert 985 : < VECT_BAR /= 1 >"
severity warning;

assert (MAP_BAR = '1')
report
"Assert 986 : < MAP_BAR /= 1 >"
severity warning;

assert (R_sel = '0')
report
"Assert 987 : < R_sel /= 0 >"
severity warning;

assert (D_sel = '0')
report
"Assert 988 : < D_sel /= 0 >"
severity warning;

assert (uPC_sel = '0')
report
"Assert 989 : < uPC_sel /= 0 >"
severity warning;

assert (stack_sel = '1')
report
"Assert 990 : < stack_sel /= 1 >"
severity warning;

assert (decr = '0')
report
"Assert 991 : < decr /= 0 >"
severity warning;

assert (load = '0')
report
"Assert 992 : < load /= 0 >"
severity warning;

assert (clear = '0')
report
"Assert 993 : < clear /= 0 >"
severity warning;

assert (push = '0')
report
"Assert 994 : < push /= 0 >"
severity warning;

assert (pop = '1')
report
"Assert 995 : < pop /= 1 >"
severity warning;

wait for 1 ns;

--------------------------

I <= "1010";
CCEN_BAR <= '0';
CC_BAR <= '0';
Rzero_bar <= '0';

wait for 1 ns; --	 Cycle No: 83

assert (PL_BAR = '0')
report
"Assert 996 : < PL_BAR /= 0 >" -- 	Vector No: 83
severity warning;

assert (VECT_BAR = '1')
report
"Assert 997 : < VECT_BAR /= 1 >"
severity warning;

assert (MAP_BAR = '1')
report
"Assert 998 : < MAP_BAR /= 1 >"
severity warning;

assert (R_sel = '0')
report
"Assert 999 : < R_sel /= 0 >"
severity warning;

assert (D_sel = '0')
report
"Assert 1000 : < D_sel /= 0 >"
severity warning;

assert (uPC_sel = '0')
report
"Assert 1001 : < uPC_sel /= 0 >"
severity warning;

assert (stack_sel = '1')
report
"Assert 1002 : < stack_sel /= 1 >"
severity warning;

assert (decr = '0')
report
"Assert 1003 : < decr /= 0 >"
severity warning;

assert (load = '0')
report
"Assert 1004 : < load /= 0 >"
severity warning;

assert (clear = '0')
report
"Assert 1005 : < clear /= 0 >"
severity warning;

assert (push = '0')
report
"Assert 1006 : < push /= 0 >"
severity warning;

assert (pop = '1')
report
"Assert 1007 : < pop /= 1 >"
severity warning;

wait for 1 ns;

-- ***********************************
--------------------------

I <= "1010";
CCEN_BAR <= '1';
CC_BAR <= '1';
Rzero_bar <= '1';

wait for 1 ns; --	 Cycle No: 84

assert (PL_BAR = '0')
report
"Assert 1008 : < PL_BAR /= 0 >" -- 	Vector No: 84
severity warning;

assert (VECT_BAR = '1')
report
"Assert 1009 : < VECT_BAR /= 1 >"
severity warning;

assert (MAP_BAR = '1')
report
"Assert 1010 : < MAP_BAR /= 1 >"
severity warning;

assert (R_sel = '0')
report
"Assert 1011 : < R_sel /= 0 >"
severity warning;

assert (D_sel = '0')
report
"Assert 1012 : < D_sel /= 0 >"
severity warning;

assert (uPC_sel = '0')
report
"Assert 1013 : < uPC_sel /= 0 >"
severity warning;

assert (stack_sel = '1')
report
"Assert 1014 : < stack_sel /= 1 >"
severity warning;

assert (decr = '0')
report
"Assert 1015 : < decr /= 0 >"
severity warning;

assert (load = '0')
report
"Assert 1016 : < load /= 0 >"
severity warning;

assert (clear = '0')
report
"Assert 1017 : < clear /= 0 >"
severity warning;

assert (push = '0')
report
"Assert 1018 : < push /= 0 >"
severity warning;

assert (pop = '1')
report
"Assert 1019 : < pop /= 1 >"
severity warning;

wait for 1 ns;

--------------------------

I <= "1010";
CCEN_BAR <= '1';
CC_BAR <= '1';
Rzero_bar <= '0';

wait for 1 ns; --	 Cycle No: 85

assert (PL_BAR = '0')
report
"Assert 1020 : < PL_BAR /= 0 >" -- 	Vector No: 85
severity warning;

assert (VECT_BAR = '1')
report
"Assert 1021 : < VECT_BAR /= 1 >"
severity warning;

assert (MAP_BAR = '1')
report
"Assert 1022 : < MAP_BAR /= 1 >"
severity warning;

assert (R_sel = '0')
report
"Assert 1023 : < R_sel /= 0 >"
severity warning;

assert (D_sel = '0')
report
"Assert 1024 : < D_sel /= 0 >"
severity warning;

assert (uPC_sel = '0')
report
"Assert 1025 : < uPC_sel /= 0 >"
severity warning;

assert (stack_sel = '1')
report
"Assert 1026 : < stack_sel /= 1 >"
severity warning;

assert (decr = '0')
report
"Assert 1027 : < decr /= 0 >"
severity warning;

assert (load = '0')
report
"Assert 1028 : < load /= 0 >"
severity warning;

assert (clear = '0')
report
"Assert 1029 : < clear /= 0 >"
severity warning;

assert (push = '0')
report
"Assert 1030 : < push /= 0 >"
severity warning;

assert (pop = '1')
report
"Assert 1031 : < pop /= 1 >"
severity warning;

wait for 1 ns;

-- ***********************************
--------------------------

I <= "1010";
CCEN_BAR <= '1';
CC_BAR <= '0';
Rzero_bar <= '1';

wait for 1 ns; --	 Cycle No: 86

assert (PL_BAR = '0')
report
"Assert 1032 : < PL_BAR /= 0 >" -- 	Vector No: 86
severity warning;

assert (VECT_BAR = '1')
report
"Assert 1033 : < VECT_BAR /= 1 >"
severity warning;

assert (MAP_BAR = '1')
report
"Assert 1034 : < MAP_BAR /= 1 >"
severity warning;

assert (R_sel = '0')
report
"Assert 1035 : < R_sel /= 0 >"
severity warning;

assert (D_sel = '0')
report
"Assert 1036 : < D_sel /= 0 >"
severity warning;

assert (uPC_sel = '0')
report
"Assert 1037 : < uPC_sel /= 0 >"
severity warning;

assert (stack_sel = '1')
report
"Assert 1038 : < stack_sel /= 1 >"
severity warning;

assert (decr = '0')
report
"Assert 1039 : < decr /= 0 >"
severity warning;

assert (load = '0')
report
"Assert 1040 : < load /= 0 >"
severity warning;

assert (clear = '0')
report
"Assert 1041 : < clear /= 0 >"
severity warning;

assert (push = '0')
report
"Assert 1042 : < push /= 0 >"
severity warning;

assert (pop = '1')
report
"Assert 1043 : < pop /= 1 >"
severity warning;

wait for 1 ns;

--------------------------

I <= "1010";
CCEN_BAR <= '1';
CC_BAR <= '0';
Rzero_bar <= '0';

wait for 1 ns; --	 Cycle No: 87

assert (PL_BAR = '0')
report
"Assert 1044 : < PL_BAR /= 0 >" -- 	Vector No: 87
severity warning;

assert (VECT_BAR = '1')
report
"Assert 1045 : < VECT_BAR /= 1 >"
severity warning;

assert (MAP_BAR = '1')
report
"Assert 1046 : < MAP_BAR /= 1 >"
severity warning;

assert (R_sel = '0')
report
"Assert 1047 : < R_sel /= 0 >"
severity warning;

assert (D_sel = '0')
report
"Assert 1048 : < D_sel /= 0 >"
severity warning;

assert (uPC_sel = '0')
report
"Assert 1049 : < uPC_sel /= 0 >"
severity warning;

assert (stack_sel = '1')
report
"Assert 1050 : < stack_sel /= 1 >"
severity warning;

assert (decr = '0')
report
"Assert 1051 : < decr /= 0 >"
severity warning;

assert (load = '0')
report
"Assert 1052 : < load /= 0 >"
severity warning;

assert (clear = '0')
report
"Assert 1053 : < clear /= 0 >"
severity warning;

assert (push = '0')
report
"Assert 1054 : < push /= 0 >"
severity warning;

assert (pop = '1')
report
"Assert 1055 : < pop /= 1 >"
severity warning;

wait for 1 ns;

-- ***********************************
-- **********
-- * I = 11 *
-- **********
--------------------------

I <= "1011";
CCEN_BAR <= '0';
CC_BAR <= '1';
Rzero_bar <= '1';

wait for 1 ns; --	 Cycle No: 88

assert (PL_BAR = '0')
report
"Assert 1056 : < PL_BAR /= 0 >" -- 	Vector No: 88
severity warning;

assert (VECT_BAR = '1')
report
"Assert 1057 : < VECT_BAR /= 1 >"
severity warning;

assert (MAP_BAR = '1')
report
"Assert 1058 : < MAP_BAR /= 1 >"
severity warning;

assert (R_sel = '0')
report
"Assert 1059 : < R_sel /= 0 >"
severity warning;

assert (D_sel = '0')
report
"Assert 1060 : < D_sel /= 0 >"
severity warning;

assert (uPC_sel = '1')
report
"Assert 1061 : < uPC_sel /= 1 >"
severity warning;

assert (stack_sel = '0')
report
"Assert 1062 : < stack_sel /= 0 >"
severity warning;

assert (decr = '0')
report
"Assert 1063 : < decr /= 0 >"
severity warning;

assert (load = '0')
report
"Assert 1064 : < load /= 0 >"
severity warning;

assert (clear = '0')
report
"Assert 1065 : < clear /= 0 >"
severity warning;

assert (push = '0')
report
"Assert 1066 : < push /= 0 >"
severity warning;

assert (pop = '0')
report
"Assert 1067 : < pop /= 0 >"
severity warning;

wait for 1 ns;

--------------------------

I <= "1011";
CCEN_BAR <= '0';
CC_BAR <= '1';
Rzero_bar <= '0';

wait for 1 ns; --	 Cycle No: 89

assert (PL_BAR = '0')
report
"Assert 1068 : < PL_BAR /= 0 >" -- 	Vector No: 89
severity warning;

assert (VECT_BAR = '1')
report
"Assert 1069 : < VECT_BAR /= 1 >"
severity warning;

assert (MAP_BAR = '1')
report
"Assert 1070 : < MAP_BAR /= 1 >"
severity warning;

assert (R_sel = '0')
report
"Assert 1071 : < R_sel /= 0 >"
severity warning;

assert (D_sel = '0')
report
"Assert 1072 : < D_sel /= 0 >"
severity warning;

assert (uPC_sel = '1')
report
"Assert 1073 : < uPC_sel /= 1 >"
severity warning;

assert (stack_sel = '0')
report
"Assert 1074 : < stack_sel /= 0 >"
severity warning;

assert (decr = '0')
report
"Assert 1075 : < decr /= 0 >"
severity warning;

assert (load = '0')
report
"Assert 1076 : < load /= 0 >"
severity warning;

assert (clear = '0')
report
"Assert 1077 : < clear /= 0 >"
severity warning;

assert (push = '0')
report
"Assert 1078 : < push /= 0 >"
severity warning;

assert (pop = '0')
report
"Assert 1079 : < pop /= 0 >"
severity warning;

wait for 1 ns;

-- ***********************************
--------------------------

I <= "1011";
CCEN_BAR <= '0';
CC_BAR <= '0';
Rzero_bar <= '1';

wait for 1 ns; --	 Cycle No: 90

assert (PL_BAR = '0')
report
"Assert 1080 : < PL_BAR /= 0 >" -- 	Vector No: 90
severity warning;

assert (VECT_BAR = '1')
report
"Assert 1081 : < VECT_BAR /= 1 >"
severity warning;

assert (MAP_BAR = '1')
report
"Assert 1082 : < MAP_BAR /= 1 >"
severity warning;

assert (R_sel = '0')
report
"Assert 1083 : < R_sel /= 0 >"
severity warning;

assert (D_sel = '1')
report
"Assert 1084 : < D_sel /= 1 >"
severity warning;

assert (uPC_sel = '0')
report
"Assert 1085 : < uPC_sel /= 0 >"
severity warning;

assert (stack_sel = '0')
report
"Assert 1086 : < stack_sel /= 0 >"
severity warning;

assert (decr = '0')
report
"Assert 1087 : < decr /= 0 >"
severity warning;

assert (load = '0')
report
"Assert 1088 : < load /= 0 >"
severity warning;

assert (clear = '0')
report
"Assert 1089 : < clear /= 0 >"
severity warning;

assert (push = '0')
report
"Assert 1090 : < push /= 0 >"
severity warning;

assert (pop = '1')
report
"Assert 1091 : < pop /= 1 >"
severity warning;

wait for 1 ns;

--------------------------

I <= "1011";
CCEN_BAR <= '0';
CC_BAR <= '0';
Rzero_bar <= '0';

wait for 1 ns; --	 Cycle No: 91

assert (PL_BAR = '0')
report
"Assert 1092 : < PL_BAR /= 0 >" -- 	Vector No: 91
severity warning;

assert (VECT_BAR = '1')
report
"Assert 1093 : < VECT_BAR /= 1 >"
severity warning;

assert (MAP_BAR = '1')
report
"Assert 1094 : < MAP_BAR /= 1 >"
severity warning;

assert (R_sel = '0')
report
"Assert 1095 : < R_sel /= 0 >"
severity warning;

assert (D_sel = '1')
report
"Assert 1096 : < D_sel /= 1 >"
severity warning;

assert (uPC_sel = '0')
report
"Assert 1097 : < uPC_sel /= 0 >"
severity warning;

assert (stack_sel = '0')
report
"Assert 1098 : < stack_sel /= 0 >"
severity warning;

assert (decr = '0')
report
"Assert 1099 : < decr /= 0 >"
severity warning;

assert (load = '0')
report
"Assert 1100 : < load /= 0 >"
severity warning;

assert (clear = '0')
report
"Assert 1101 : < clear /= 0 >"
severity warning;

assert (push = '0')
report
"Assert 1102 : < push /= 0 >"
severity warning;

assert (pop = '1')
report
"Assert 1103 : < pop /= 1 >"
severity warning;

wait for 1 ns;

-- ***********************************
--------------------------

I <= "1011";
CCEN_BAR <= '1';
CC_BAR <= '1';
Rzero_bar <= '1';

wait for 1 ns; --	 Cycle No: 92

assert (PL_BAR = '0')
report
"Assert 1104 : < PL_BAR /= 0 >" -- 	Vector No: 92
severity warning;

assert (VECT_BAR = '1')
report
"Assert 1105 : < VECT_BAR /= 1 >"
severity warning;

assert (MAP_BAR = '1')
report
"Assert 1106 : < MAP_BAR /= 1 >"
severity warning;

assert (R_sel = '0')
report
"Assert 1107 : < R_sel /= 0 >"
severity warning;

assert (D_sel = '1')
report
"Assert 1108 : < D_sel /= 1 >"
severity warning;

assert (uPC_sel = '0')
report
"Assert 1109 : < uPC_sel /= 0 >"
severity warning;

assert (stack_sel = '0')
report
"Assert 1110 : < stack_sel /= 0 >"
severity warning;

assert (decr = '0')
report
"Assert 1111 : < decr /= 0 >"
severity warning;

assert (load = '0')
report
"Assert 1112 : < load /= 0 >"
severity warning;

assert (clear = '0')
report
"Assert 1113 : < clear /= 0 >"
severity warning;

assert (push = '0')
report
"Assert 1114 : < push /= 0 >"
severity warning;

assert (pop = '1')
report
"Assert 1115 : < pop /= 1 >"
severity warning;

wait for 1 ns;

--------------------------

I <= "1011";
CCEN_BAR <= '1';
CC_BAR <= '1';
Rzero_bar <= '0';

wait for 1 ns; --	 Cycle No: 93

assert (PL_BAR = '0')
report
"Assert 1116 : < PL_BAR /= 0 >" -- 	Vector No: 93
severity warning;

assert (VECT_BAR = '1')
report
"Assert 1117 : < VECT_BAR /= 1 >"
severity warning;

assert (MAP_BAR = '1')
report
"Assert 1118 : < MAP_BAR /= 1 >"
severity warning;

assert (R_sel = '0')
report
"Assert 1119 : < R_sel /= 0 >"
severity warning;

assert (D_sel = '1')
report
"Assert 1120 : < D_sel /= 1 >"
severity warning;

assert (uPC_sel = '0')
report
"Assert 1121 : < uPC_sel /= 0 >"
severity warning;

assert (stack_sel = '0')
report
"Assert 1122 : < stack_sel /= 0 >"
severity warning;

assert (decr = '0')
report
"Assert 1123 : < decr /= 0 >"
severity warning;

assert (load = '0')
report
"Assert 1124 : < load /= 0 >"
severity warning;

assert (clear = '0')
report
"Assert 1125 : < clear /= 0 >"
severity warning;

assert (push = '0')
report
"Assert 1126 : < push /= 0 >"
severity warning;

assert (pop = '1')
report
"Assert 1127 : < pop /= 1 >"
severity warning;

wait for 1 ns;

-- ***********************************
--------------------------

I <= "1011";
CCEN_BAR <= '1';
CC_BAR <= '0';
Rzero_bar <= '1';

wait for 1 ns; --	 Cycle No: 94

assert (PL_BAR = '0')
report
"Assert 1128 : < PL_BAR /= 0 >" -- 	Vector No: 94
severity warning;

assert (VECT_BAR = '1')
report
"Assert 1129 : < VECT_BAR /= 1 >"
severity warning;

assert (MAP_BAR = '1')
report
"Assert 1130 : < MAP_BAR /= 1 >"
severity warning;

assert (R_sel = '0')
report
"Assert 1131 : < R_sel /= 0 >"
severity warning;

assert (D_sel = '1')
report
"Assert 1132 : < D_sel /= 1 >"
severity warning;

assert (uPC_sel = '0')
report
"Assert 1133 : < uPC_sel /= 0 >"
severity warning;

assert (stack_sel = '0')
report
"Assert 1134 : < stack_sel /= 0 >"
severity warning;

assert (decr = '0')
report
"Assert 1135 : < decr /= 0 >"
severity warning;

assert (load = '0')
report
"Assert 1136 : < load /= 0 >"
severity warning;

assert (clear = '0')
report
"Assert 1137 : < clear /= 0 >"
severity warning;

assert (push = '0')
report
"Assert 1138 : < push /= 0 >"
severity warning;

assert (pop = '1')
report
"Assert 1139 : < pop /= 1 >"
severity warning;

wait for 1 ns;

--------------------------

I <= "1011";
CCEN_BAR <= '1';
CC_BAR <= '0';
Rzero_bar <= '0';

wait for 1 ns; --	 Cycle No: 95

assert (PL_BAR = '0')
report
"Assert 1140 : < PL_BAR /= 0 >" -- 	Vector No: 95
severity warning;

assert (VECT_BAR = '1')
report
"Assert 1141 : < VECT_BAR /= 1 >"
severity warning;

assert (MAP_BAR = '1')
report
"Assert 1142 : < MAP_BAR /= 1 >"
severity warning;

assert (R_sel = '0')
report
"Assert 1143 : < R_sel /= 0 >"
severity warning;

assert (D_sel = '1')
report
"Assert 1144 : < D_sel /= 1 >"
severity warning;

assert (uPC_sel = '0')
report
"Assert 1145 : < uPC_sel /= 0 >"
severity warning;

assert (stack_sel = '0')
report
"Assert 1146 : < stack_sel /= 0 >"
severity warning;

assert (decr = '0')
report
"Assert 1147 : < decr /= 0 >"
severity warning;

assert (load = '0')
report
"Assert 1148 : < load /= 0 >"
severity warning;

assert (clear = '0')
report
"Assert 1149 : < clear /= 0 >"
severity warning;

assert (push = '0')
report
"Assert 1150 : < push /= 0 >"
severity warning;

assert (pop = '1')
report
"Assert 1151 : < pop /= 1 >"
severity warning;

wait for 1 ns;

-- ***********************************
-- **********
-- * I = 12 *
-- **********
--------------------------

I <= "1100";
CCEN_BAR <= '0';
CC_BAR <= '1';
Rzero_bar <= '1';

wait for 1 ns; --	 Cycle No: 96

assert (PL_BAR = '0')
report
"Assert 1152 : < PL_BAR /= 0 >" -- 	Vector No: 96
severity warning;

assert (VECT_BAR = '1')
report
"Assert 1153 : < VECT_BAR /= 1 >"
severity warning;

assert (MAP_BAR = '1')
report
"Assert 1154 : < MAP_BAR /= 1 >"
severity warning;

assert (R_sel = '0')
report
"Assert 1155 : < R_sel /= 0 >"
severity warning;

assert (D_sel = '0')
report
"Assert 1156 : < D_sel /= 0 >"
severity warning;

assert (uPC_sel = '1')
report
"Assert 1157 : < uPC_sel /= 1 >"
severity warning;

assert (stack_sel = '0')
report
"Assert 1158 : < stack_sel /= 0 >"
severity warning;

assert (decr = '0')
report
"Assert 1159 : < decr /= 0 >"
severity warning;

assert (load = '1')
report
"Assert 1160 : < load /= 1 >"
severity warning;

assert (clear = '0')
report
"Assert 1161 : < clear /= 0 >"
severity warning;

assert (push = '0')
report
"Assert 1162 : < push /= 0 >"
severity warning;

assert (pop = '0')
report
"Assert 1163 : < pop /= 0 >"
severity warning;

wait for 1 ns;

--------------------------

I <= "1100";
CCEN_BAR <= '0';
CC_BAR <= '1';
Rzero_bar <= '0';

wait for 1 ns; --	 Cycle No: 97

assert (PL_BAR = '0')
report
"Assert 1164 : < PL_BAR /= 0 >" -- 	Vector No: 97
severity warning;

assert (VECT_BAR = '1')
report
"Assert 1165 : < VECT_BAR /= 1 >"
severity warning;

assert (MAP_BAR = '1')
report
"Assert 1166 : < MAP_BAR /= 1 >"
severity warning;

assert (R_sel = '0')
report
"Assert 1167 : < R_sel /= 0 >"
severity warning;

assert (D_sel = '0')
report
"Assert 1168 : < D_sel /= 0 >"
severity warning;

assert (uPC_sel = '1')
report
"Assert 1169 : < uPC_sel /= 1 >"
severity warning;

assert (stack_sel = '0')
report
"Assert 1170 : < stack_sel /= 0 >"
severity warning;

assert (decr = '0')
report
"Assert 1171 : < decr /= 0 >"
severity warning;

assert (load = '1')
report
"Assert 1172 : < load /= 1 >"
severity warning;

assert (clear = '0')
report
"Assert 1173 : < clear /= 0 >"
severity warning;

assert (push = '0')
report
"Assert 1174 : < push /= 0 >"
severity warning;

assert (pop = '0')
report
"Assert 1175 : < pop /= 0 >"
severity warning;

wait for 1 ns;

-- ***********************************
--------------------------

I <= "1100";
CCEN_BAR <= '0';
CC_BAR <= '0';
Rzero_bar <= '1';

wait for 1 ns; --	 Cycle No: 98

assert (PL_BAR = '0')
report
"Assert 1176 : < PL_BAR /= 0 >" -- 	Vector No: 98
severity warning;

assert (VECT_BAR = '1')
report
"Assert 1177 : < VECT_BAR /= 1 >"
severity warning;

assert (MAP_BAR = '1')
report
"Assert 1178 : < MAP_BAR /= 1 >"
severity warning;

assert (R_sel = '0')
report
"Assert 1179 : < R_sel /= 0 >"
severity warning;

assert (D_sel = '0')
report
"Assert 1180 : < D_sel /= 0 >"
severity warning;

assert (uPC_sel = '1')
report
"Assert 1181 : < uPC_sel /= 1 >"
severity warning;

assert (stack_sel = '0')
report
"Assert 1182 : < stack_sel /= 0 >"
severity warning;

assert (decr = '0')
report
"Assert 1183 : < decr /= 0 >"
severity warning;

assert (load = '1')
report
"Assert 1184 : < load /= 1 >"
severity warning;

assert (clear = '0')
report
"Assert 1185 : < clear /= 0 >"
severity warning;

assert (push = '0')
report
"Assert 1186 : < push /= 0 >"
severity warning;

assert (pop = '0')
report
"Assert 1187 : < pop /= 0 >"
severity warning;

wait for 1 ns;

--------------------------

I <= "1100";
CCEN_BAR <= '0';
CC_BAR <= '0';
Rzero_bar <= '0';

wait for 1 ns; --	 Cycle No: 99

assert (PL_BAR = '0')
report
"Assert 1188 : < PL_BAR /= 0 >" -- 	Vector No: 99
severity warning;

assert (VECT_BAR = '1')
report
"Assert 1189 : < VECT_BAR /= 1 >"
severity warning;

assert (MAP_BAR = '1')
report
"Assert 1190 : < MAP_BAR /= 1 >"
severity warning;

assert (R_sel = '0')
report
"Assert 1191 : < R_sel /= 0 >"
severity warning;

assert (D_sel = '0')
report
"Assert 1192 : < D_sel /= 0 >"
severity warning;

assert (uPC_sel = '1')
report
"Assert 1193 : < uPC_sel /= 1 >"
severity warning;

assert (stack_sel = '0')
report
"Assert 1194 : < stack_sel /= 0 >"
severity warning;

assert (decr = '0')
report
"Assert 1195 : < decr /= 0 >"
severity warning;

assert (load = '1')
report
"Assert 1196 : < load /= 1 >"
severity warning;

assert (clear = '0')
report
"Assert 1197 : < clear /= 0 >"
severity warning;

assert (push = '0')
report
"Assert 1198 : < push /= 0 >"
severity warning;

assert (pop = '0')
report
"Assert 1199 : < pop /= 0 >"
severity warning;

wait for 1 ns;

-- ***********************************
--------------------------

I <= "1100";
CCEN_BAR <= '1';
CC_BAR <= '1';
Rzero_bar <= '1';

wait for 1 ns; --	 Cycle No: 100

assert (PL_BAR = '0')
report
"Assert 1200 : < PL_BAR /= 0 >" -- 	Vector No: 100
severity warning;

assert (VECT_BAR = '1')
report
"Assert 1201 : < VECT_BAR /= 1 >"
severity warning;

assert (MAP_BAR = '1')
report
"Assert 1202 : < MAP_BAR /= 1 >"
severity warning;

assert (R_sel = '0')
report
"Assert 1203 : < R_sel /= 0 >"
severity warning;

assert (D_sel = '0')
report
"Assert 1204 : < D_sel /= 0 >"
severity warning;

assert (uPC_sel = '1')
report
"Assert 1205 : < uPC_sel /= 1 >"
severity warning;

assert (stack_sel = '0')
report
"Assert 1206 : < stack_sel /= 0 >"
severity warning;

assert (decr = '0')
report
"Assert 1207 : < decr /= 0 >"
severity warning;

assert (load = '1')
report
"Assert 1208 : < load /= 1 >"
severity warning;

assert (clear = '0')
report
"Assert 1209 : < clear /= 0 >"
severity warning;

assert (push = '0')
report
"Assert 1210 : < push /= 0 >"
severity warning;

assert (pop = '0')
report
"Assert 1211 : < pop /= 0 >"
severity warning;

wait for 1 ns;

--------------------------

I <= "1100";
CCEN_BAR <= '1';
CC_BAR <= '1';
Rzero_bar <= '0';

wait for 1 ns; --	 Cycle No: 101

assert (PL_BAR = '0')
report
"Assert 1212 : < PL_BAR /= 0 >" -- 	Vector No: 101
severity warning;

assert (VECT_BAR = '1')
report
"Assert 1213 : < VECT_BAR /= 1 >"
severity warning;

assert (MAP_BAR = '1')
report
"Assert 1214 : < MAP_BAR /= 1 >"
severity warning;

assert (R_sel = '0')
report
"Assert 1215 : < R_sel /= 0 >"
severity warning;

assert (D_sel = '0')
report
"Assert 1216 : < D_sel /= 0 >"
severity warning;

assert (uPC_sel = '1')
report
"Assert 1217 : < uPC_sel /= 1 >"
severity warning;

assert (stack_sel = '0')
report
"Assert 1218 : < stack_sel /= 0 >"
severity warning;

assert (decr = '0')
report
"Assert 1219 : < decr /= 0 >"
severity warning;

assert (load = '1')
report
"Assert 1220 : < load /= 1 >"
severity warning;

assert (clear = '0')
report
"Assert 1221 : < clear /= 0 >"
severity warning;

assert (push = '0')
report
"Assert 1222 : < push /= 0 >"
severity warning;

assert (pop = '0')
report
"Assert 1223 : < pop /= 0 >"
severity warning;

wait for 1 ns;

-- ***********************************
--------------------------

I <= "1100";
CCEN_BAR <= '1';
CC_BAR <= '0';
Rzero_bar <= '1';

wait for 1 ns; --	 Cycle No: 102

assert (PL_BAR = '0')
report
"Assert 1224 : < PL_BAR /= 0 >" -- 	Vector No: 102
severity warning;

assert (VECT_BAR = '1')
report
"Assert 1225 : < VECT_BAR /= 1 >"
severity warning;

assert (MAP_BAR = '1')
report
"Assert 1226 : < MAP_BAR /= 1 >"
severity warning;

assert (R_sel = '0')
report
"Assert 1227 : < R_sel /= 0 >"
severity warning;

assert (D_sel = '0')
report
"Assert 1228 : < D_sel /= 0 >"
severity warning;

assert (uPC_sel = '1')
report
"Assert 1229 : < uPC_sel /= 1 >"
severity warning;

assert (stack_sel = '0')
report
"Assert 1230 : < stack_sel /= 0 >"
severity warning;

assert (decr = '0')
report
"Assert 1231 : < decr /= 0 >"
severity warning;

assert (load = '1')
report
"Assert 1232 : < load /= 1 >"
severity warning;

assert (clear = '0')
report
"Assert 1233 : < clear /= 0 >"
severity warning;

assert (push = '0')
report
"Assert 1234 : < push /= 0 >"
severity warning;

assert (pop = '0')
report
"Assert 1235 : < pop /= 0 >"
severity warning;

wait for 1 ns;

--------------------------

I <= "1100";
CCEN_BAR <= '1';
CC_BAR <= '0';
Rzero_bar <= '0';

wait for 1 ns; --	 Cycle No: 103

assert (PL_BAR = '0')
report
"Assert 1236 : < PL_BAR /= 0 >" -- 	Vector No: 103
severity warning;

assert (VECT_BAR = '1')
report
"Assert 1237 : < VECT_BAR /= 1 >"
severity warning;

assert (MAP_BAR = '1')
report
"Assert 1238 : < MAP_BAR /= 1 >"
severity warning;

assert (R_sel = '0')
report
"Assert 1239 : < R_sel /= 0 >"
severity warning;

assert (D_sel = '0')
report
"Assert 1240 : < D_sel /= 0 >"
severity warning;

assert (uPC_sel = '1')
report
"Assert 1241 : < uPC_sel /= 1 >"
severity warning;

assert (stack_sel = '0')
report
"Assert 1242 : < stack_sel /= 0 >"
severity warning;

assert (decr = '0')
report
"Assert 1243 : < decr /= 0 >"
severity warning;

assert (load = '1')
report
"Assert 1244 : < load /= 1 >"
severity warning;

assert (clear = '0')
report
"Assert 1245 : < clear /= 0 >"
severity warning;

assert (push = '0')
report
"Assert 1246 : < push /= 0 >"
severity warning;

assert (pop = '0')
report
"Assert 1247 : < pop /= 0 >"
severity warning;

wait for 1 ns;

-- ***********************************
-- **********
-- * I = 13 *
-- **********
--------------------------

I <= "1101";
CCEN_BAR <= '0';
CC_BAR <= '1';
Rzero_bar <= '1';

wait for 1 ns; --	 Cycle No: 104

assert (PL_BAR = '0')
report
"Assert 1248 : < PL_BAR /= 0 >" -- 	Vector No: 104
severity warning;

assert (VECT_BAR = '1')
report
"Assert 1249 : < VECT_BAR /= 1 >"
severity warning;

assert (MAP_BAR = '1')
report
"Assert 1250 : < MAP_BAR /= 1 >"
severity warning;

assert (R_sel = '0')
report
"Assert 1251 : < R_sel /= 0 >"
severity warning;

assert (D_sel = '0')
report
"Assert 1252 : < D_sel /= 0 >"
severity warning;

assert (uPC_sel = '0')
report
"Assert 1253 : < uPC_sel /= 0 >"
severity warning;

assert (stack_sel = '1')
report
"Assert 1254 : < stack_sel /= 1 >"
severity warning;

assert (decr = '0')
report
"Assert 1255 : < decr /= 0 >"
severity warning;

assert (load = '0')
report
"Assert 1256 : < load /= 0 >"
severity warning;

assert (clear = '0')
report
"Assert 1257 : < clear /= 0 >"
severity warning;

assert (push = '0')
report
"Assert 1258 : < push /= 0 >"
severity warning;

assert (pop = '0')
report
"Assert 1259 : < pop /= 0 >"
severity warning;

wait for 1 ns;

--------------------------

I <= "1101";
CCEN_BAR <= '0';
CC_BAR <= '1';
Rzero_bar <= '0';

wait for 1 ns; --	 Cycle No: 105

assert (PL_BAR = '0')
report
"Assert 1260 : < PL_BAR /= 0 >" -- 	Vector No: 105
severity warning;

assert (VECT_BAR = '1')
report
"Assert 1261 : < VECT_BAR /= 1 >"
severity warning;

assert (MAP_BAR = '1')
report
"Assert 1262 : < MAP_BAR /= 1 >"
severity warning;

assert (R_sel = '0')
report
"Assert 1263 : < R_sel /= 0 >"
severity warning;

assert (D_sel = '0')
report
"Assert 1264 : < D_sel /= 0 >"
severity warning;

assert (uPC_sel = '0')
report
"Assert 1265 : < uPC_sel /= 0 >"
severity warning;

assert (stack_sel = '1')
report
"Assert 1266 : < stack_sel /= 1 >"
severity warning;

assert (decr = '0')
report
"Assert 1267 : < decr /= 0 >"
severity warning;

assert (load = '0')
report
"Assert 1268 : < load /= 0 >"
severity warning;

assert (clear = '0')
report
"Assert 1269 : < clear /= 0 >"
severity warning;

assert (push = '0')
report
"Assert 1270 : < push /= 0 >"
severity warning;

assert (pop = '0')
report
"Assert 1271 : < pop /= 0 >"
severity warning;

wait for 1 ns;

-- ***********************************
--------------------------

I <= "1101";
CCEN_BAR <= '0';
CC_BAR <= '0';
Rzero_bar <= '1';

wait for 1 ns; --	 Cycle No: 106

assert (PL_BAR = '0')
report
"Assert 1272 : < PL_BAR /= 0 >" -- 	Vector No: 106
severity warning;

assert (VECT_BAR = '1')
report
"Assert 1273 : < VECT_BAR /= 1 >"
severity warning;

assert (MAP_BAR = '1')
report
"Assert 1274 : < MAP_BAR /= 1 >"
severity warning;

assert (R_sel = '0')
report
"Assert 1275 : < R_sel /= 0 >"
severity warning;

assert (D_sel = '0')
report
"Assert 1276 : < D_sel /= 0 >"
severity warning;

assert (uPC_sel = '1')
report
"Assert 1277 : < uPC_sel /= 1 >"
severity warning;

assert (stack_sel = '0')
report
"Assert 1278 : < stack_sel /= 0 >"
severity warning;

assert (decr = '0')
report
"Assert 1279 : < decr /= 0 >"
severity warning;

assert (load = '0')
report
"Assert 1280 : < load /= 0 >"
severity warning;

assert (clear = '0')
report
"Assert 1281 : < clear /= 0 >"
severity warning;

assert (push = '0')
report
"Assert 1282 : < push /= 0 >"
severity warning;

assert (pop = '1')
report
"Assert 1283 : < pop /= 1 >"
severity warning;

wait for 1 ns;

--------------------------

I <= "1101";
CCEN_BAR <= '0';
CC_BAR <= '0';
Rzero_bar <= '0';

wait for 1 ns; --	 Cycle No: 107

assert (PL_BAR = '0')
report
"Assert 1284 : < PL_BAR /= 0 >" -- 	Vector No: 107
severity warning;

assert (VECT_BAR = '1')
report
"Assert 1285 : < VECT_BAR /= 1 >"
severity warning;

assert (MAP_BAR = '1')
report
"Assert 1286 : < MAP_BAR /= 1 >"
severity warning;

assert (R_sel = '0')
report
"Assert 1287 : < R_sel /= 0 >"
severity warning;

assert (D_sel = '0')
report
"Assert 1288 : < D_sel /= 0 >"
severity warning;

assert (uPC_sel = '1')
report
"Assert 1289 : < uPC_sel /= 1 >"
severity warning;

assert (stack_sel = '0')
report
"Assert 1290 : < stack_sel /= 0 >"
severity warning;

assert (decr = '0')
report
"Assert 1291 : < decr /= 0 >"
severity warning;

assert (load = '0')
report
"Assert 1292 : < load /= 0 >"
severity warning;

assert (clear = '0')
report
"Assert 1293 : < clear /= 0 >"
severity warning;

assert (push = '0')
report
"Assert 1294 : < push /= 0 >"
severity warning;

assert (pop = '1')
report
"Assert 1295 : < pop /= 1 >"
severity warning;

wait for 1 ns;

-- ***********************************
--------------------------

I <= "1101";
CCEN_BAR <= '1';
CC_BAR <= '1';
Rzero_bar <= '1';

wait for 1 ns; --	 Cycle No: 108

assert (PL_BAR = '0')
report
"Assert 1296 : < PL_BAR /= 0 >" -- 	Vector No: 108
severity warning;

assert (VECT_BAR = '1')
report
"Assert 1297 : < VECT_BAR /= 1 >"
severity warning;

assert (MAP_BAR = '1')
report
"Assert 1298 : < MAP_BAR /= 1 >"
severity warning;

assert (R_sel = '0')
report
"Assert 1299 : < R_sel /= 0 >"
severity warning;

assert (D_sel = '0')
report
"Assert 1300 : < D_sel /= 0 >"
severity warning;

assert (uPC_sel = '1')
report
"Assert 1301 : < uPC_sel /= 1 >"
severity warning;

assert (stack_sel = '0')
report
"Assert 1302 : < stack_sel /= 0 >"
severity warning;

assert (decr = '0')
report
"Assert 1303 : < decr /= 0 >"
severity warning;

assert (load = '0')
report
"Assert 1304 : < load /= 0 >"
severity warning;

assert (clear = '0')
report
"Assert 1305 : < clear /= 0 >"
severity warning;

assert (push = '0')
report
"Assert 1306 : < push /= 0 >"
severity warning;

assert (pop = '1')
report
"Assert 1307 : < pop /= 1 >"
severity warning;

wait for 1 ns;

--------------------------

I <= "1101";
CCEN_BAR <= '1';
CC_BAR <= '1';
Rzero_bar <= '0';

wait for 1 ns; --	 Cycle No: 109

assert (PL_BAR = '0')
report
"Assert 1308 : < PL_BAR /= 0 >" -- 	Vector No: 109
severity warning;

assert (VECT_BAR = '1')
report
"Assert 1309 : < VECT_BAR /= 1 >"
severity warning;

assert (MAP_BAR = '1')
report
"Assert 1310 : < MAP_BAR /= 1 >"
severity warning;

assert (R_sel = '0')
report
"Assert 1311 : < R_sel /= 0 >"
severity warning;

assert (D_sel = '0')
report
"Assert 1312 : < D_sel /= 0 >"
severity warning;

assert (uPC_sel = '1')
report
"Assert 1313 : < uPC_sel /= 1 >"
severity warning;

assert (stack_sel = '0')
report
"Assert 1314 : < stack_sel /= 0 >"
severity warning;

assert (decr = '0')
report
"Assert 1315 : < decr /= 0 >"
severity warning;

assert (load = '0')
report
"Assert 1316 : < load /= 0 >"
severity warning;

assert (clear = '0')
report
"Assert 1317 : < clear /= 0 >"
severity warning;

assert (push = '0')
report
"Assert 1318 : < push /= 0 >"
severity warning;

assert (pop = '1')
report
"Assert 1319 : < pop /= 1 >"
severity warning;

wait for 1 ns;

-- ***********************************
--------------------------

I <= "1101";
CCEN_BAR <= '1';
CC_BAR <= '0';
Rzero_bar <= '1';

wait for 1 ns; --	 Cycle No: 110

assert (PL_BAR = '0')
report
"Assert 1320 : < PL_BAR /= 0 >" -- 	Vector No: 110
severity warning;

assert (VECT_BAR = '1')
report
"Assert 1321 : < VECT_BAR /= 1 >"
severity warning;

assert (MAP_BAR = '1')
report
"Assert 1322 : < MAP_BAR /= 1 >"
severity warning;

assert (R_sel = '0')
report
"Assert 1323 : < R_sel /= 0 >"
severity warning;

assert (D_sel = '0')
report
"Assert 1324 : < D_sel /= 0 >"
severity warning;

assert (uPC_sel = '1')
report
"Assert 1325 : < uPC_sel /= 1 >"
severity warning;

assert (stack_sel = '0')
report
"Assert 1326 : < stack_sel /= 0 >"
severity warning;

assert (decr = '0')
report
"Assert 1327 : < decr /= 0 >"
severity warning;

assert (load = '0')
report
"Assert 1328 : < load /= 0 >"
severity warning;

assert (clear = '0')
report
"Assert 1329 : < clear /= 0 >"
severity warning;

assert (push = '0')
report
"Assert 1330 : < push /= 0 >"
severity warning;

assert (pop = '1')
report
"Assert 1331 : < pop /= 1 >"
severity warning;

wait for 1 ns;

--------------------------

I <= "1101";
CCEN_BAR <= '1';
CC_BAR <= '0';
Rzero_bar <= '0';

wait for 1 ns; --	 Cycle No: 111

assert (PL_BAR = '0')
report
"Assert 1332 : < PL_BAR /= 0 >" -- 	Vector No: 111
severity warning;

assert (VECT_BAR = '1')
report
"Assert 1333 : < VECT_BAR /= 1 >"
severity warning;

assert (MAP_BAR = '1')
report
"Assert 1334 : < MAP_BAR /= 1 >"
severity warning;

assert (R_sel = '0')
report
"Assert 1335 : < R_sel /= 0 >"
severity warning;

assert (D_sel = '0')
report
"Assert 1336 : < D_sel /= 0 >"
severity warning;

assert (uPC_sel = '1')
report
"Assert 1337 : < uPC_sel /= 1 >"
severity warning;

assert (stack_sel = '0')
report
"Assert 1338 : < stack_sel /= 0 >"
severity warning;

assert (decr = '0')
report
"Assert 1339 : < decr /= 0 >"
severity warning;

assert (load = '0')
report
"Assert 1340 : < load /= 0 >"
severity warning;

assert (clear = '0')
report
"Assert 1341 : < clear /= 0 >"
severity warning;

assert (push = '0')
report
"Assert 1342 : < push /= 0 >"
severity warning;

assert (pop = '1')
report
"Assert 1343 : < pop /= 1 >"
severity warning;

wait for 1 ns;

-- ***********************************
-- **********
-- * I = 14 *
-- **********
--------------------------

I <= "1110";
CCEN_BAR <= '0';
CC_BAR <= '1';
Rzero_bar <= '1';

wait for 1 ns; --	 Cycle No: 112

assert (PL_BAR = '0')
report
"Assert 1344 : < PL_BAR /= 0 >" -- 	Vector No: 112
severity warning;

assert (VECT_BAR = '1')
report
"Assert 1345 : < VECT_BAR /= 1 >"
severity warning;

assert (MAP_BAR = '1')
report
"Assert 1346 : < MAP_BAR /= 1 >"
severity warning;

assert (R_sel = '0')
report
"Assert 1347 : < R_sel /= 0 >"
severity warning;

assert (D_sel = '0')
report
"Assert 1348 : < D_sel /= 0 >"
severity warning;

assert (uPC_sel = '1')
report
"Assert 1349 : < uPC_sel /= 1 >"
severity warning;

assert (stack_sel = '0')
report
"Assert 1350 : < stack_sel /= 0 >"
severity warning;

assert (decr = '0')
report
"Assert 1351 : < decr /= 0 >"
severity warning;

assert (load = '0')
report
"Assert 1352 : < load /= 0 >"
severity warning;

assert (clear = '0')
report
"Assert 1353 : < clear /= 0 >"
severity warning;

assert (push = '0')
report
"Assert 1354 : < push /= 0 >"
severity warning;

assert (pop = '0')
report
"Assert 1355 : < pop /= 0 >"
severity warning;

wait for 1 ns;

--------------------------

I <= "1110";
CCEN_BAR <= '0';
CC_BAR <= '1';
Rzero_bar <= '0';

wait for 1 ns; --	 Cycle No: 113

assert (PL_BAR = '0')
report
"Assert 1356 : < PL_BAR /= 0 >" -- 	Vector No: 113
severity warning;

assert (VECT_BAR = '1')
report
"Assert 1357 : < VECT_BAR /= 1 >"
severity warning;

assert (MAP_BAR = '1')
report
"Assert 1358 : < MAP_BAR /= 1 >"
severity warning;

assert (R_sel = '0')
report
"Assert 1359 : < R_sel /= 0 >"
severity warning;

assert (D_sel = '0')
report
"Assert 1360 : < D_sel /= 0 >"
severity warning;

assert (uPC_sel = '1')
report
"Assert 1361 : < uPC_sel /= 1 >"
severity warning;

assert (stack_sel = '0')
report
"Assert 1362 : < stack_sel /= 0 >"
severity warning;

assert (decr = '0')
report
"Assert 1363 : < decr /= 0 >"
severity warning;

assert (load = '0')
report
"Assert 1364 : < load /= 0 >"
severity warning;

assert (clear = '0')
report
"Assert 1365 : < clear /= 0 >"
severity warning;

assert (push = '0')
report
"Assert 1366 : < push /= 0 >"
severity warning;

assert (pop = '0')
report
"Assert 1367 : < pop /= 0 >"
severity warning;

wait for 1 ns;

-- ***********************************
--------------------------

I <= "1110";
CCEN_BAR <= '0';
CC_BAR <= '0';
Rzero_bar <= '1';

wait for 1 ns; --	 Cycle No: 114

assert (PL_BAR = '0')
report
"Assert 1368 : < PL_BAR /= 0 >" -- 	Vector No: 114
severity warning;

assert (VECT_BAR = '1')
report
"Assert 1369 : < VECT_BAR /= 1 >"
severity warning;

assert (MAP_BAR = '1')
report
"Assert 1370 : < MAP_BAR /= 1 >"
severity warning;

assert (R_sel = '0')
report
"Assert 1371 : < R_sel /= 0 >"
severity warning;

assert (D_sel = '0')
report
"Assert 1372 : < D_sel /= 0 >"
severity warning;

assert (uPC_sel = '1')
report
"Assert 1373 : < uPC_sel /= 1 >"
severity warning;

assert (stack_sel = '0')
report
"Assert 1374 : < stack_sel /= 0 >"
severity warning;

assert (decr = '0')
report
"Assert 1375 : < decr /= 0 >"
severity warning;

assert (load = '0')
report
"Assert 1376 : < load /= 0 >"
severity warning;

assert (clear = '0')
report
"Assert 1377 : < clear /= 0 >"
severity warning;

assert (push = '0')
report
"Assert 1378 : < push /= 0 >"
severity warning;

assert (pop = '0')
report
"Assert 1379 : < pop /= 0 >"
severity warning;

wait for 1 ns;

--------------------------

I <= "1110";
CCEN_BAR <= '0';
CC_BAR <= '0';
Rzero_bar <= '0';

wait for 1 ns; --	 Cycle No: 115

assert (PL_BAR = '0')
report
"Assert 1380 : < PL_BAR /= 0 >" -- 	Vector No: 115
severity warning;

assert (VECT_BAR = '1')
report
"Assert 1381 : < VECT_BAR /= 1 >"
severity warning;

assert (MAP_BAR = '1')
report
"Assert 1382 : < MAP_BAR /= 1 >"
severity warning;

assert (R_sel = '0')
report
"Assert 1383 : < R_sel /= 0 >"
severity warning;

assert (D_sel = '0')
report
"Assert 1384 : < D_sel /= 0 >"
severity warning;

assert (uPC_sel = '1')
report
"Assert 1385 : < uPC_sel /= 1 >"
severity warning;

assert (stack_sel = '0')
report
"Assert 1386 : < stack_sel /= 0 >"
severity warning;

assert (decr = '0')
report
"Assert 1387 : < decr /= 0 >"
severity warning;

assert (load = '0')
report
"Assert 1388 : < load /= 0 >"
severity warning;

assert (clear = '0')
report
"Assert 1389 : < clear /= 0 >"
severity warning;

assert (push = '0')
report
"Assert 1390 : < push /= 0 >"
severity warning;

assert (pop = '0')
report
"Assert 1391 : < pop /= 0 >"
severity warning;

wait for 1 ns;

-- ***********************************
--------------------------

I <= "1110";
CCEN_BAR <= '1';
CC_BAR <= '1';
Rzero_bar <= '1';

wait for 1 ns; --	 Cycle No: 116

assert (PL_BAR = '0')
report
"Assert 1392 : < PL_BAR /= 0 >" -- 	Vector No: 116
severity warning;

assert (VECT_BAR = '1')
report
"Assert 1393 : < VECT_BAR /= 1 >"
severity warning;

assert (MAP_BAR = '1')
report
"Assert 1394 : < MAP_BAR /= 1 >"
severity warning;

assert (R_sel = '0')
report
"Assert 1395 : < R_sel /= 0 >"
severity warning;

assert (D_sel = '0')
report
"Assert 1396 : < D_sel /= 0 >"
severity warning;

assert (uPC_sel = '1')
report
"Assert 1397 : < uPC_sel /= 1 >"
severity warning;

assert (stack_sel = '0')
report
"Assert 1398 : < stack_sel /= 0 >"
severity warning;

assert (decr = '0')
report
"Assert 1399 : < decr /= 0 >"
severity warning;

assert (load = '0')
report
"Assert 1400 : < load /= 0 >"
severity warning;

assert (clear = '0')
report
"Assert 1401 : < clear /= 0 >"
severity warning;

assert (push = '0')
report
"Assert 1402 : < push /= 0 >"
severity warning;

assert (pop = '0')
report
"Assert 1403 : < pop /= 0 >"
severity warning;

wait for 1 ns;

--------------------------

I <= "1110";
CCEN_BAR <= '1';
CC_BAR <= '1';
Rzero_bar <= '0';

wait for 1 ns; --	 Cycle No: 117

assert (PL_BAR = '0')
report
"Assert 1404 : < PL_BAR /= 0 >" -- 	Vector No: 117
severity warning;

assert (VECT_BAR = '1')
report
"Assert 1405 : < VECT_BAR /= 1 >"
severity warning;

assert (MAP_BAR = '1')
report
"Assert 1406 : < MAP_BAR /= 1 >"
severity warning;

assert (R_sel = '0')
report
"Assert 1407 : < R_sel /= 0 >"
severity warning;

assert (D_sel = '0')
report
"Assert 1408 : < D_sel /= 0 >"
severity warning;

assert (uPC_sel = '1')
report
"Assert 1409 : < uPC_sel /= 1 >"
severity warning;

assert (stack_sel = '0')
report
"Assert 1410 : < stack_sel /= 0 >"
severity warning;

assert (decr = '0')
report
"Assert 1411 : < decr /= 0 >"
severity warning;

assert (load = '0')
report
"Assert 1412 : < load /= 0 >"
severity warning;

assert (clear = '0')
report
"Assert 1413 : < clear /= 0 >"
severity warning;

assert (push = '0')
report
"Assert 1414 : < push /= 0 >"
severity warning;

assert (pop = '0')
report
"Assert 1415 : < pop /= 0 >"
severity warning;

wait for 1 ns;

-- ***********************************
--------------------------

I <= "1110";
CCEN_BAR <= '1';
CC_BAR <= '0';
Rzero_bar <= '1';

wait for 1 ns; --	 Cycle No: 118

assert (PL_BAR = '0')
report
"Assert 1416 : < PL_BAR /= 0 >" -- 	Vector No: 118
severity warning;

assert (VECT_BAR = '1')
report
"Assert 1417 : < VECT_BAR /= 1 >"
severity warning;

assert (MAP_BAR = '1')
report
"Assert 1418 : < MAP_BAR /= 1 >"
severity warning;

assert (R_sel = '0')
report
"Assert 1419 : < R_sel /= 0 >"
severity warning;

assert (D_sel = '0')
report
"Assert 1420 : < D_sel /= 0 >"
severity warning;

assert (uPC_sel = '1')
report
"Assert 1421 : < uPC_sel /= 1 >"
severity warning;

assert (stack_sel = '0')
report
"Assert 1422 : < stack_sel /= 0 >"
severity warning;

assert (decr = '0')
report
"Assert 1423 : < decr /= 0 >"
severity warning;

assert (load = '0')
report
"Assert 1424 : < load /= 0 >"
severity warning;

assert (clear = '0')
report
"Assert 1425 : < clear /= 0 >"
severity warning;

assert (push = '0')
report
"Assert 1426 : < push /= 0 >"
severity warning;

assert (pop = '0')
report
"Assert 1427 : < pop /= 0 >"
severity warning;

wait for 1 ns;

--------------------------

I <= "1110";
CCEN_BAR <= '1';
CC_BAR <= '0';
Rzero_bar <= '0';

wait for 1 ns; --	 Cycle No: 119

assert (PL_BAR = '0')
report
"Assert 1428 : < PL_BAR /= 0 >" -- 	Vector No: 119
severity warning;

assert (VECT_BAR = '1')
report
"Assert 1429 : < VECT_BAR /= 1 >"
severity warning;

assert (MAP_BAR = '1')
report
"Assert 1430 : < MAP_BAR /= 1 >"
severity warning;

assert (R_sel = '0')
report
"Assert 1431 : < R_sel /= 0 >"
severity warning;

assert (D_sel = '0')
report
"Assert 1432 : < D_sel /= 0 >"
severity warning;

assert (uPC_sel = '1')
report
"Assert 1433 : < uPC_sel /= 1 >"
severity warning;

assert (stack_sel = '0')
report
"Assert 1434 : < stack_sel /= 0 >"
severity warning;

assert (decr = '0')
report
"Assert 1435 : < decr /= 0 >"
severity warning;

assert (load = '0')
report
"Assert 1436 : < load /= 0 >"
severity warning;

assert (clear = '0')
report
"Assert 1437 : < clear /= 0 >"
severity warning;

assert (push = '0')
report
"Assert 1438 : < push /= 0 >"
severity warning;

assert (pop = '0')
report
"Assert 1439 : < pop /= 0 >"
severity warning;

wait for 1 ns;

-- ***********************************
-- **********
-- * I = 15 *
-- **********
--------------------------

I <= "1111";
CCEN_BAR <= '0';
CC_BAR <= '1';
Rzero_bar <= '1';

wait for 1 ns; --	 Cycle No: 120

assert (PL_BAR = '0')
report
"Assert 1440 : < PL_BAR /= 0 >" -- 	Vector No: 120
severity warning;

assert (VECT_BAR = '1')
report
"Assert 1441 : < VECT_BAR /= 1 >"
severity warning;

assert (MAP_BAR = '1')
report
"Assert 1442 : < MAP_BAR /= 1 >"
severity warning;

assert (R_sel = '0')
report
"Assert 1443 : < R_sel /= 0 >"
severity warning;

assert (D_sel = '0')
report
"Assert 1444 : < D_sel /= 0 >"
severity warning;

assert (uPC_sel = '0')
report
"Assert 1445 : < uPC_sel /= 0 >"
severity warning;

assert (stack_sel = '1')
report
"Assert 1446 : < stack_sel /= 1 >"
severity warning;

assert (decr = '1')
report
"Assert 1447 : < decr /= 1 >"
severity warning;

assert (load = '0')
report
"Assert 1448 : < load /= 0 >"
severity warning;

assert (clear = '0')
report
"Assert 1449 : < clear /= 0 >"
severity warning;

assert (push = '0')
report
"Assert 1450 : < push /= 0 >"
severity warning;

assert (pop = '0')
report
"Assert 1451 : < pop /= 0 >"
severity warning;

wait for 1 ns;

--------------------------

I <= "1111";
CCEN_BAR <= '0';
CC_BAR <= '1';
Rzero_bar <= '0';

wait for 1 ns; --	 Cycle No: 121

assert (PL_BAR = '0')
report
"Assert 1452 : < PL_BAR /= 0 >" -- 	Vector No: 121
severity warning;

assert (VECT_BAR = '1')
report
"Assert 1453 : < VECT_BAR /= 1 >"
severity warning;

assert (MAP_BAR = '1')
report
"Assert 1454 : < MAP_BAR /= 1 >"
severity warning;

assert (R_sel = '0')
report
"Assert 1455 : < R_sel /= 0 >"
severity warning;

assert (D_sel = '1')
report
"Assert 1456 : < D_sel /= 1 >"
severity warning;

assert (uPC_sel = '0')
report
"Assert 1457 : < uPC_sel /= 0 >"
severity warning;

assert (stack_sel = '0')
report
"Assert 1458 : < stack_sel /= 0 >"
severity warning;

assert (decr = '0')
report
"Assert 1459 : < decr /= 0 >"
severity warning;

assert (load = '0')
report
"Assert 1460 : < load /= 0 >"
severity warning;

assert (clear = '0')
report
"Assert 1461 : < clear /= 0 >"
severity warning;

assert (push = '0')
report
"Assert 1462 : < push /= 0 >"
severity warning;

assert (pop = '1')
report
"Assert 1463 : < pop /= 1 >"
severity warning;

wait for 1 ns;

-- ***********************************
--------------------------

I <= "1111";
CCEN_BAR <= '0';
CC_BAR <= '0';
Rzero_bar <= '1';

wait for 1 ns; --	 Cycle No: 122

assert (PL_BAR = '0')
report
"Assert 1464 : < PL_BAR /= 0 >" -- 	Vector No: 122
severity warning;

assert (VECT_BAR = '1')
report
"Assert 1465 : < VECT_BAR /= 1 >"
severity warning;

assert (MAP_BAR = '1')
report
"Assert 1466 : < MAP_BAR /= 1 >"
severity warning;

assert (R_sel = '0')
report
"Assert 1467 : < R_sel /= 0 >"
severity warning;

assert (D_sel = '0')
report
"Assert 1468 : < D_sel /= 0 >"
severity warning;

assert (uPC_sel = '1')
report
"Assert 1469 : < uPC_sel /= 1 >"
severity warning;

assert (stack_sel = '0')
report
"Assert 1470 : < stack_sel /= 0 >"
severity warning;

assert (decr = '1')
report
"Assert 1471 : < decr /= 1 >"
severity warning;

assert (load = '0')
report
"Assert 1472 : < load /= 0 >"
severity warning;

assert (clear = '0')
report
"Assert 1473 : < clear /= 0 >"
severity warning;

assert (push = '0')
report
"Assert 1474 : < push /= 0 >"
severity warning;

assert (pop = '1')
report
"Assert 1475 : < pop /= 1 >"
severity warning;

wait for 1 ns;

--------------------------

I <= "1111";
CCEN_BAR <= '0';
CC_BAR <= '0';
Rzero_bar <= '0';

wait for 1 ns; --	 Cycle No: 123

assert (PL_BAR = '0')
report
"Assert 1476 : < PL_BAR /= 0 >" -- 	Vector No: 123
severity warning;

assert (VECT_BAR = '1')
report
"Assert 1477 : < VECT_BAR /= 1 >"
severity warning;

assert (MAP_BAR = '1')
report
"Assert 1478 : < MAP_BAR /= 1 >"
severity warning;

assert (R_sel = '0')
report
"Assert 1479 : < R_sel /= 0 >"
severity warning;

assert (D_sel = '0')
report
"Assert 1480 : < D_sel /= 0 >"
severity warning;

assert (uPC_sel = '1')
report
"Assert 1481 : < uPC_sel /= 1 >"
severity warning;

assert (stack_sel = '0')
report
"Assert 1482 : < stack_sel /= 0 >"
severity warning;

assert (decr = '0')
report
"Assert 1483 : < decr /= 0 >"
severity warning;

assert (load = '0')
report
"Assert 1484 : < load /= 0 >"
severity warning;

assert (clear = '0')
report
"Assert 1485 : < clear /= 0 >"
severity warning;

assert (push = '0')
report
"Assert 1486 : < push /= 0 >"
severity warning;

assert (pop = '1')
report
"Assert 1487 : < pop /= 1 >"
severity warning;

wait for 1 ns;

-- ***********************************
--------------------------

I <= "1111";
CCEN_BAR <= '1';
CC_BAR <= '1';
Rzero_bar <= '1';

wait for 1 ns; --	 Cycle No: 124

assert (PL_BAR = '0')
report
"Assert 1488 : < PL_BAR /= 0 >" -- 	Vector No: 124
severity warning;

assert (VECT_BAR = '1')
report
"Assert 1489 : < VECT_BAR /= 1 >"
severity warning;

assert (MAP_BAR = '1')
report
"Assert 1490 : < MAP_BAR /= 1 >"
severity warning;

assert (R_sel = '0')
report
"Assert 1491 : < R_sel /= 0 >"
severity warning;

assert (D_sel = '0')
report
"Assert 1492 : < D_sel /= 0 >"
severity warning;

assert (uPC_sel = '1')
report
"Assert 1493 : < uPC_sel /= 1 >"
severity warning;

assert (stack_sel = '0')
report
"Assert 1494 : < stack_sel /= 0 >"
severity warning;

assert (decr = '1')
report
"Assert 1495 : < decr /= 1 >"
severity warning;

assert (load = '0')
report
"Assert 1496 : < load /= 0 >"
severity warning;

assert (clear = '0')
report
"Assert 1497 : < clear /= 0 >"
severity warning;

assert (push = '0')
report
"Assert 1498 : < push /= 0 >"
severity warning;

assert (pop = '1')
report
"Assert 1499 : < pop /= 1 >"
severity warning;

wait for 1 ns;

--------------------------

I <= "1111";
CCEN_BAR <= '1';
CC_BAR <= '1';
Rzero_bar <= '0';

wait for 1 ns; --	 Cycle No: 125

assert (PL_BAR = '0')
report
"Assert 1500 : < PL_BAR /= 0 >" -- 	Vector No: 125
severity warning;

assert (VECT_BAR = '1')
report
"Assert 1501 : < VECT_BAR /= 1 >"
severity warning;

assert (MAP_BAR = '1')
report
"Assert 1502 : < MAP_BAR /= 1 >"
severity warning;

assert (R_sel = '0')
report
"Assert 1503 : < R_sel /= 0 >"
severity warning;

assert (D_sel = '0')
report
"Assert 1504 : < D_sel /= 0 >"
severity warning;

assert (uPC_sel = '1')
report
"Assert 1505 : < uPC_sel /= 1 >"
severity warning;

assert (stack_sel = '0')
report
"Assert 1506 : < stack_sel /= 0 >"
severity warning;

assert (decr = '0')
report
"Assert 1507 : < decr /= 0 >"
severity warning;

assert (load = '0')
report
"Assert 1508 : < load /= 0 >"
severity warning;

assert (clear = '0')
report
"Assert 1509 : < clear /= 0 >"
severity warning;

assert (push = '0')
report
"Assert 1510 : < push /= 0 >"
severity warning;

assert (pop = '1')
report
"Assert 1511 : < pop /= 1 >"
severity warning;

wait for 1 ns;

-- ***********************************
--------------------------

I <= "1111";
CCEN_BAR <= '1';
CC_BAR <= '0';
Rzero_bar <= '1';

wait for 1 ns; --	 Cycle No: 126

assert (PL_BAR = '0')
report
"Assert 1512 : < PL_BAR /= 0 >" -- 	Vector No: 126
severity warning;

assert (VECT_BAR = '1')
report
"Assert 1513 : < VECT_BAR /= 1 >"
severity warning;

assert (MAP_BAR = '1')
report
"Assert 1514 : < MAP_BAR /= 1 >"
severity warning;

assert (R_sel = '0')
report
"Assert 1515 : < R_sel /= 0 >"
severity warning;

assert (D_sel = '0')
report
"Assert 1516 : < D_sel /= 0 >"
severity warning;

assert (uPC_sel = '1')
report
"Assert 1517 : < uPC_sel /= 1 >"
severity warning;

assert (stack_sel = '0')
report
"Assert 1518 : < stack_sel /= 0 >"
severity warning;

assert (decr = '1')
report
"Assert 1519 : < decr /= 1 >"
severity warning;

assert (load = '0')
report
"Assert 1520 : < load /= 0 >"
severity warning;

assert (clear = '0')
report
"Assert 1521 : < clear /= 0 >"
severity warning;

assert (push = '0')
report
"Assert 1522 : < push /= 0 >"
severity warning;

assert (pop = '1')
report
"Assert 1523 : < pop /= 1 >"
severity warning;

wait for 1 ns;

--------------------------

I <= "1111";
CCEN_BAR <= '1';
CC_BAR <= '0';
Rzero_bar <= '0';

wait for 1 ns; --	 Cycle No: 127

assert (PL_BAR = '0')
report
"Assert 1524 : < PL_BAR /= 0 >" -- 	Vector No: 127
severity warning;

assert (VECT_BAR = '1')
report
"Assert 1525 : < VECT_BAR /= 1 >"
severity warning;

assert (MAP_BAR = '1')
report
"Assert 1526 : < MAP_BAR /= 1 >"
severity warning;

assert (R_sel = '0')
report
"Assert 1527 : < R_sel /= 0 >"
severity warning;

assert (D_sel = '0')
report
"Assert 1528 : < D_sel /= 0 >"
severity warning;

assert (uPC_sel = '1')
report
"Assert 1529 : < uPC_sel /= 1 >"
severity warning;

assert (stack_sel = '0')
report
"Assert 1530 : < stack_sel /= 0 >"
severity warning;

assert (decr = '0')
report
"Assert 1531 : < decr /= 0 >"
severity warning;

assert (load = '0')
report
"Assert 1532 : < load /= 0 >"
severity warning;

assert (clear = '0')
report
"Assert 1533 : < clear /= 0 >"
severity warning;

assert (push = '0')
report
"Assert 1534 : < push /= 0 >"
severity warning;

assert (pop = '1')
report
"Assert 1535 : < pop /= 1 >"
severity warning;

wait for 1 ns;

-- ***********************************
--------------------------

end process;

end AA;
