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

entity E is
end;

architecture A of E is

 component alu_inst
  port (
         RE, S : in MVL7_vector(3 downto 0);
         I : in MVL7_vector(8 downto 0);
         C0 : in MVL7;
         C4, OVR, F30, F3, Pbar, Gbar :  out MVL7;
         F : out MVL7_vector(3 downto 0) 
       );
 end component;

 signal RE, S :  MVL7_vector(3 downto 0);                         
 signal I :  MVL7_vector(8 downto 0);
 signal C0 :  MVL7;
 signal C4, OVR, F30, F3, Pbar, Gbar :   MVL7;
 signal F :  MVL7_vector(3 downto 0);

 for all : alu_inst use entity work.alu(alu);

begin

alu_inst1 : alu_inst port map(
         RE, S,
         I,
         C0,
         C4, OVR, F30, F3, Pbar, Gbar,
         F
                             );

process

begin

----------------------------------------------------------------------------

RE <= "0001";           --#1
S <= "0001";
C0 <= '0';         -- Compute RE + S. ( RE = 0001, S = 0001)
I <= "000000000";

wait for 1 ns;

assert (F = "0010")
report
"Assert a1 : < F /= '0010'> "
severity warning;

assert (C4 = '0')
report
"Assert a2 : < C4 /= '0'> "
severity warning;

assert (OVR = '0')
report
"Assert a3 : < OVR /= '0'> "
severity warning;

assert (F30 = '0')
report
"Assert a4 : < F30 /= '0'> "
severity warning;

assert (F3 = '0')
report
"Assert a5 : < F3 /= '0'> "
severity warning;

assert (Pbar = '1')
report
"Assert a6 : < Pbar /= '1'> "
severity warning;

assert (Gbar = '1')
report
"Assert a7 : < Gbar /= '1'> "
severity warning;

wait for 1 ns;

----------------------------------------------------------------------------

RE <= "0010";           --#2
S <= "0010";
C0 <= '1';           -- Compute RE + S + 1. ( RE = 0010, S = 0010)
I <= "000000000";

wait for 1 ns;

assert (F = "0101")
report
"Assert b1 : < F /= '0101'> "
severity warning;

assert (C4 = '0')
report
"Assert b2 : < C4 /= '0'> "
severity warning;

assert (OVR = '0')
report
"Assert b3 : < OVR /= '0'> "
severity warning;

assert (F30 = '0')
report
"Assert b4 : < F30 /= '0'> "
severity warning;

assert (F3 = '0')
report
"Assert b5 : < F3 /= '0'> "
severity warning;

assert (Pbar = '1')
report
"Assert b6 : < Pbar /= '1'> "
severity warning;

assert (Gbar = '1')
report
"Assert b7 : < Gbar /= '1'> "
severity warning;

wait for 1 ns;

----------------------------------------------------------------------------

RE <= "0100";           --#3
S <= "0100";
C0 <= '0';        -- Compute RE + S. ( RE = 0100, S = 0100)
I <= "000000000";

wait for 1 ns;

assert (F = "1000")
report
"Assert c1 : < F /= '1000'> "
severity warning;

assert (C4 = '0')
report
"Assert c2 : < C4 /= '0'> "
severity warning;

assert (OVR = '1')
report
"Assert c3 : < OVR /= '1'> "
severity warning;

assert (F30 = '0')
report
"Assert c4 : < F30 /= '0'> "
severity warning;

assert (F3 = '1')
report
"Assert c5 : < F3 /= '1'> "
severity warning;

assert (Pbar = '1')
report
"Assert c6 : < Pbar /= '1'> "
severity warning;

assert (Gbar = '1')
report
"Assert c7 : < Gbar /= '1'> "
severity warning;

wait for 1 ns;

----------------------------------------------------------------------------

RE <= "1000";           --#4
S <= "1000";
C0 <= '0'; -- Compute RE + S. ( RE = 1000, S = 1000)
I <= "000000000";

wait for 1 ns;

assert (F = "0000")
report
"Assert d1 : < F /= '0000'> "
severity warning;

assert (C4 = '1')
report
"Assert d2 : < C4 /= '1'> "
severity warning;

assert (OVR = '1')
report
"Assert d3 : < OVR /= '1'> "
severity warning;

assert (F30 = '1')
report
"Assert d4 : < F30 /= '1'> "
severity warning;

assert (F3 = '0')
report
"Assert d5 : < F3 /= '0'> "
severity warning;

assert (Pbar = '1')
report
"Assert d6 : < Pbar /= '1'> "
severity warning;

assert (Gbar = '0')
report
"Assert d7 : < Gbar /= '0'> "
severity warning;

wait for 1 ns;

----------------------------------------------------------------------------

RE <= "0001";           --#5
S <= "0010";
C0 <= '1';   -- Compute S - RE. ( RE = 0001, S = 0010)
I <= "000001000";

wait for 1 ns;

assert (F = "0001")
report
"Assert e1 : < F /= '0001'> "
severity warning;

assert (C4 = '1')
report
"Assert e2 : < C4 /= '1'> "
severity warning;

assert (OVR = '0')
report
"Assert e3 : < OVR /= '0'> "
severity warning;

assert (F30 = '0')
report
"Assert e4 : < F30 /= '0'> "
severity warning;

assert (F3 = '0')
report
"Assert e5 : < F3 /= '0'> "
severity warning;

assert (Pbar = '1')
report
"Assert e6 : < Pbar /= '1'> "
severity warning;

assert (Gbar = '0')
report
"Assert e7 : < Gbar /= '0'> "
severity warning;

wait for 1 ns;

----------------------------------------------------------------------------

RE <= "0001";           --#6
S <= "0010";
C0 <= '0';        -- Compute S - RE -1. ( RE = 0001, S = 0010)
I <= "000001000";

wait for 1 ns;

assert (F = "0000")
report
"Assert f1 : < F /= '0000'> "
severity warning;

assert (C4 = '1')
report
"Assert f2 : < C4 /= '1'> "
severity warning;

assert (OVR = '0')
report
"Assert f3 : < OVR /= '0'> "
severity warning;

assert (F30 = '1')
report
"Assert f4 : < F30 /= '1'> "
severity warning;

assert (F3 = '0')
report
"Assert f5 : < F3 /= '0'> "
severity warning;

assert (Pbar = '1')
report
"Assert f6 : < Pbar /= '1'> "
severity warning;

assert (Gbar = '0')
report
"Assert f7 : < Gbar /= '0'> "
severity warning;

wait for 1 ns;

----------------------------------------------------------------------------

RE <= "1000";           --#7
S <= "0001";
C0 <= '1'; -- Compute RE - S. ( RE = 1000, S = 0001)
I <= "000010000";

wait for 1 ns;

assert (F = "0111")
report
"Assert g1 : < F /= '0111'> "
severity warning;

assert (C4 = '1')
report
"Assert g2 : < C4 /= '1'> "
severity warning;

assert (OVR = '1')
report
"Assert g3 : < OVR /= '1'> "
severity warning;

assert (F30 = '0')
report
"Assert g4 : < F30 /= '0'> "
severity warning;

assert (F3 = '0')
report
"Assert g5 : < F3 /= '0'> "
severity warning;

assert (Pbar = '1')
report
"Assert g6 : < Pbar /= '1'> "
severity warning;

assert (Gbar = '0')
report
"Assert g7 : < Gbar /= '0'> "
severity warning;

wait for 1 ns;

----------------------------------------------------------------------------

RE <= "1000";           --#8
S <= "0001";
C0 <= '0'; -- Compute RE - S - 1. ( RE = 1000, S = 0001)
I <= "000010000";

wait for 1 ns;

assert (F = "0110")
report
"Assert h1 : < F /= '0110'> "
severity warning;

assert (C4 = '1')
report
"Assert h2 : < C4 /= '1'> "
severity warning;

assert (OVR = '1')
report
"Assert h3 : < OVR /= '1'> "
severity warning;

assert (F30 = '0')
report
"Assert h4 : < F30 /= '0'> "
severity warning;

assert (F3 = '0')
report
"Assert h5 : < F3 /= '0'> "
severity warning;

assert (Pbar = '1')
report
"Assert h6 : < Pbar /= '1'> "
severity warning;

assert (Gbar = '0')
report
"Assert h7 : < Gbar /= '0'> "
severity warning;

wait for 1 ns;

----------------------------------------------------------------------------

RE <= "1010";           --#9
 S <= "1001";
C0 <= '0';        -- Compute RE or S. ( RE = 1010, S = 1001)
I <= "000011000";

wait for 1 ns;

assert (C4 = '0')
report
"Assert i2 : < C4 /= '0'> "
severity warning;

assert (OVR = '0')
report
"Assert i3 : < OVR /= '0'> "
severity warning;

assert (F = "1011")
report
"Assert i1 : < F /= '1011'> "
severity warning;

assert (F30 = '0')
report
"Assert i4 : < F30 /= '0'> "
severity warning;

assert (F3 = '1')
report
"Assert i5 : < F3 /= '1'> "
severity warning;

assert (Pbar = '1')
report
"Assert i6 : < Pbar /= '1'> "
severity warning;

assert (Gbar = '0')
report
"Assert i7 : < Gbar /= '0'> "
severity warning;

wait for 1 ns;

----------------------------------------------------------------------------

RE <= "1010";           --#10
 S <= "1001";
C0 <= '0'; -- Compute RE and S. ( RE = 1010, S = 1001)
I <= "000100000";

wait for 1 ns;

assert (F = "1000")
report
"Assert j1 : < F /= '1000'> "
severity warning;

assert (C4 = '0')
report
"Assert j2 : < C4 /= '0'> "
severity warning;

assert (OVR = '0')
report
"Assert j3 : < OVR /= '0'> "
severity warning;

assert (F30 = '0')
report
"Assert j4 : < F30 /= '0'> "
severity warning;

assert (F3 = '1')
report
"Assert j5 : < F3 /= '1'> "
severity warning;

assert (Pbar = '1')
report
"Assert j6 : < Pbar /= '1'> "
severity warning;

assert (Gbar = '0')
report
"Assert j7 : < Gbar /= '0'> "
severity warning;

wait for 1 ns;

----------------------------------------------------------------------------

RE <= "1010";           --#11
 S <= "1001";
C0 <= '0'; -- Compute not(RE) and S. ( RE = 1010, S = 1001)
I <= "000101000";

wait for 1 ns;

assert (F = "0001")
report
"Assert k1 : < F /= '0001'> "
severity warning;

assert (C4 = '0')
report
"Assert k2 : < C4 /= '0'> "
severity warning;

assert (OVR = '1')
report
"Assert k3 : < OVR /= '1'> "
severity warning;

assert (F30 = '0')
report
"Assert k4 : < F30 /= '0'> "
severity warning;

assert (F3 = '0')
report
"Assert k5 : < F3 /= '0'> "
severity warning;

assert (Pbar = '1')
report
"Assert k6 : < Pbar /= '1'> "
severity warning;

assert (Gbar = '0')
report
"Assert k7 : < Gbar /= '0'> "
severity warning;

wait for 1 ns;

----------------------------------------------------------------------------

RE <= "1010";           --#12
 S <= "1001";
C0 <= '0'; -- Compute RE xor S. ( RE = 1010, S = 1001)
I <= "000110000";

wait for 1 ns;

assert (F = "0011")
report
"Assert l1 : < F /= '0011'> "
severity warning;

assert (C4 = '0')
report
"Assert l2 : < C4 /= '0'> "
severity warning;

assert (OVR = '1')
report
"Assert l3 : < OVR /= '1'> "
severity warning;

assert (F30 = '0')
report
"Assert l4 : < F30 /= '0'> "
severity warning;

assert (F3 = '0')
report
"Assert l5 : < F3 /= '0'> "
severity warning;

assert (Pbar = '1')
report
"Assert l6 : < Pbar /= '1'> "
severity warning;

assert (Gbar = '0')
report
"Assert l7 : < Gbar /= '0'> "
severity warning;

wait for 1 ns;

----------------------------------------------------------------------------

RE <= "0101";           --#13
 S <= "1001";
C0 <= '0'; -- Compute RE xnor S. ( RE = 0101, S = 1001)
I <= "000111000";

wait for 1 ns;

assert (F = "0011")
report
"Assert m1 : < F /= '0011'> "
severity warning;

assert (C4 = '1')
report
"Assert m2 : < C4 /= '1'> "
severity warning;

assert (OVR = '0')
report
"Assert m3 : < OVR /= '0'> "
severity warning;

assert (F30 = '0')
report
"Assert m4 : < F30 /= '0'> "
severity warning;

assert (F3 = '0')
report
"Assert m5 : < F3 /= '0'> "
severity warning;

assert (Pbar = '1')
report
"Assert m6 : < Pbar /= '1'> "
severity warning;

assert (Gbar = '1')
report
"Assert m7 : < Gbar /= '1'> "
severity warning;

wait for 1 ns;

----------------------------------------------------------------------------
end process;

end A;