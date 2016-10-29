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

 component alu_inputs_inst
  port (
        Aadd, Badd : in integer; 
	Q, D : in MVL7_vector(3 downto 0);
        I : in MVL7_vector(8 downto 0); 
        RE, S : out MVL7_vector(3 downto 0)
       );
 end component;

 signal Aadd, Badd : integer := 0; 
 signal Q, D : MVL7_vector(3 downto 0);
 signal I : MVL7_vector(8 downto 0); 
 signal RE, S : MVL7_vector(3 downto 0);

 for all : alu_inputs_inst use entity work.alu_inputs(alu_inputs);

begin

alu_inputs_inst1 : alu_inputs_inst  port map(
                                              Aadd, Badd, 
					      Q, D,
                                              I,
                                              RE, S
                                            );

process

begin

----------------------------------------------------------------------------

Aadd <= 0;        -- #1#
Badd <= 1;
D <= "0001";  -- Select A for RE and Q for S
Q <= "0010";
I <= "000000000";

wait for 1 ns;

assert (RE = "1000")
report
"Assert 1 : < RE /= '1000'> "
severity warning;

assert (S = "0010")
report
"Assert 2 : < S /= '0010'> "
severity warning;

wait for 1 ns;
----------------------------------------------------------------------------
Aadd <= 0;       -- #2#
Badd <= 1;
D <= "0001";-- Select A for RE and B for S
Q <= "0010";
I <= "000000001";

wait for 1 ns;

assert (RE = "1000")
report
"Assert 1a : < RE /= '1000'> "
severity warning;

assert (S = "0100")
report
"Assert 2a : < S /= '0100'> "
severity warning;

wait for 1 ns;
----------------------------------------------------------------------------
Aadd <= 0;       -- #3#
Badd <= 1;
D <= "0001";-- Select 0 for RE and Q for S
Q <= "0010";
I <= "000000010";

wait for 1 ns;

assert (RE = "0000")
report
"Assert 1b : < RE /= '0000'> "
severity warning;

assert (S = "0010")
report
"Assert 2b : < S /= '0010'> "
severity warning;

wait for 1 ns;
----------------------------------------------------------------------------
Aadd <= 0;       -- #4#
Badd <= 1;
D <= "0001";-- Select 0 for RE and B for S
Q <= "0010";
I <= "000000011";

wait for 1 ns;

assert (RE = "0000")
report
"Assert 1c : < RE /= '0000'> "
severity warning;

assert (S = "0100")
report
"Assert 2c : < S /= '0100'> "
severity warning;

wait for 1 ns;
----------------------------------------------------------------------------
Aadd <= 0;       -- #5#
Badd <= 1;
D <= "0001";-- Select 0 for RE and A for S
Q <= "0010";
I <= "000000100";

wait for 1 ns;

assert (RE = "0000")
report
"Assert 1d : < RE /= '0000'> "
severity warning;

assert (S = "1000")
report
"Assert 2d : < S /= '1000'> "
severity warning;

wait for 1 ns;
----------------------------------------------------------------------------
Aadd <= 0;       -- #6#
Badd <= 1;
D <= "0001";-- Select D for RE and A for S
Q <= "0010";
I <= "000000101";

wait for 1 ns;

assert (RE = "0001")
report
"Assert 1e : < RE /= '0001'> "
severity warning;

assert (S = "1000")
report
"Assert 2e : < S /= '1000'> "
severity warning;

wait for 1 ns;
----------------------------------------------------------------------------
Aadd <= 0;       -- #7#
Badd <= 1;
D <= "0001";-- Select D for RE and Q for S
Q <= "0010";
I <= "000000110";

wait for 1 ns;

assert (RE = "0001")
report
"Assert 1f : < RE /= '0001'> "
severity warning;

assert (S = "0010")
report
"Assert 2f : < S /= '0010'> "
severity warning;

wait for 1 ns;
----------------------------------------------------------------------------
Aadd <= 0;       -- #8#
Badd <= 1;
D <= "0001";-- Select D for RE and 0 for S
Q <= "0010";
I <= "000000111";

wait for 1 ns;

assert (RE = "0001")
report
"Assert 1g : < RE /= '0001'> "
severity warning;

assert (S = "0000")
report
"Assert 2g : < S /= '0000'> "
severity warning;

wait for 1 ns;
----------------------------------------------------------------------------

end process;

end A;
