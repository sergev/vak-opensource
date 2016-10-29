--------------------------------------------------------------------------------
--
--   AM2901 Benchmark -- mem component
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

entity mem is
     port ( 
	    RAM : inout Memory(15 downto 0);
            F : in MVL7_vector(3 downto 0);
            clk :  in clock; 
            I :  in MVL7_vector(8 downto 0);
            RAM0, RAM3 : in MVL7;
            Aadd, Badd : in integer range 15 downto 0
          );
end mem;

architecture mem of mem is 

begin

mem1 : block ( (clk = '1') and (not clk'stable) )

begin

-- WRITE TO RAM WITH/WITHOUT SHIFTING. RAM DESTINATIONS ARE 
-- ADDRESSED BY "Badd".
        
RAM(Badd) <= guarded F when ((not(I(8)) and I(7)) = '1') else
             RAM3 & F(3 downto 1) when ((I(8) and not(I(7))) = '1') else
             F(2 downto 0) & RAM0 when ((I(8) and I(7)) = '1') else
             RAM(Badd);

end block mem1;

end mem;