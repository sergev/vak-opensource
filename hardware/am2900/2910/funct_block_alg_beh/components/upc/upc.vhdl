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

entity upc is
port (
          clk : in clock;
	   CI : in MVL7;
	clear : in MVL7;
       Y_temp : in MVL7_VECTOR(11 downto 0);
	  uPC : inout MVL7_VECTOR(11 downto 0)
      );
end upc;

architecture upc of upc is

begin

-------------------------------------------------------------------------------

PC : block ( (clk = '1') and (not clk'stable) )

begin

uPC <= guarded Y_temp + ("00000000000" & CI) WHEN (clear = '0') ELSE
          "000000000000" WHEN clear = '1' ELSE
          uPC;

end block PC;

------------------------------------------------------------------------------

end upc;







