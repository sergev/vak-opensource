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

entity multiplexer is
port (
	R_sel : in MVL7;
	D_sel : in MVL7;
      uPC_sel : in MVL7;
    stack_sel : in MVL7;
       OEBAR : in MVL7;
	   sp : in INTEGER range 0 to 5;
            D : in MVL7_VECTOR(11 downto 0);
           RE : in MVL7_VECTOR(11 downto 0);
          uPC : in MVL7_VECTOR(11 downto 0);
            Y : out MVL7_VECTOR(11 downto 0)
      );
end multiplexer;

architecture multiplexer of multiplexer is

begin

-------------------------------------------------------------------------------

muxr : block

signal Y_temp : MVL7_VECTOR(11 downto 0);
signal reg_file : MEMORY_12_BIT(5 downto 0) := (
						 ("000000000000"),
						 ("111111111111"),
						 ("000000000000"),
						 ("111111111111"),
						 ("000000000000"),
						 ("111111111111")
						);

begin

Y_temp <= RE WHEN R_sel = '1' ELSE
     D WHEN D_sel = '1' ELSE
     uPC WHEN uPC_sel = '1' ELSE
     reg_file(sp) WHEN stack_sel = '1' ELSE
     "000000000000";

Y <= Y_temp when OEbar = '0' else
     "ZZZZZZZZZZZZ";

end block muxr;

-------------------------------------------------------------------------------

end multiplexer;







