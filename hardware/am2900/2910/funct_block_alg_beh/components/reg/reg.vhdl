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

entity reg is
port (
      RLD_BAR : in MVL7;
         load : in MVL7;
         decr : in MVL7;
          clk : in clock;
            D : in MVL7_VECTOR(11 downto 0);
	   RE : inout MVL7_VECTOR(11 downto 0);
    Rzero_bar : out MVL7
      );
end reg;

architecture reg of reg is

begin

-------------------------------------------------------------------------------

reg_ctr : block ( (clk = '1') and (not clk'stable) )

begin

RE <= guarded D WHEN (( load = '1') or (RLD_BAR = '0')) ELSE    -- load
          RE - "000000000001" WHEN (decr = '1') and (RLD_BAR = '1') ELSE  -- decr
          RE ;   -- hold

Rzero_bar <= RE(0) or RE(1) or RE(2) or RE(4) or RE(5) or RE(6) or RE(7) or RE(8) or RE(9) or RE(10) or RE (11);

end block reg_ctr;

-------------------------------------------------------------------------------

end reg;







