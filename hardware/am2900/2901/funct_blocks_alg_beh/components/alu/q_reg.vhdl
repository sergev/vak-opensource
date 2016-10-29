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

entity Q_reg is
     port (
           F : in MVL7_vector(3 downto 0);
           clk : in clock;
           I :  in MVL7_vector(8 downto 0); 
           Q0, Q3 : in MVL7;
           Q : inout MVL7_vector(3 downto 0)             
          );
end Q_reg;

architecture Q_reg of Q_reg is

begin

Q_reg1 : block ( (clk = '1') and (not clk'stable ) )

begin

-- WRITE TO  Q REGISTER WITH/WITHOUT SHIFTING. 
        
Q <= guarded F when (I(8 downto 6) = "000")                 else
            Q3 & Q(3 downto 1) when (I(8 downto 6) = "100") else
            Q(2 downto 0) & Q0 when (I(8 downto 6) = "110") else
            Q;

end block Q_reg1;

end Q_reg;

---------------------------------------------



