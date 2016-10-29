--------------------------------------------------------------------------------
--
--   AM2901 Benchmark output_shifter
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

entity output_and_shifter is
       port (
             I :  in MVL7_vector(8 downto 0);
             A, F, Q : in MVL7_vector(3 downto 0);
             OEbar : in MVL7;
             Y :  out MVL7_vector(3 downto 0);
             RAM0, RAM3, Q0, Q3 : out MVL7
            );
end output_and_shifter;

architecture output_and_shifter of output_and_shifter is

begin

-- GENERATE DATA OUTPUT "Y" 
   
Y <= A when (( I(8 downto 6) = "010") and ( OEbar = '0')) else
     F when (not(( I(8 downto 6) = "010")) and ( OEbar = '0')) else
     "ZZZZ";

-- GENERATE BIDIRECTIONAL SHIFTER SIGNALS.

RAM0 <= F(0) when ( I(8) = '1') and ( I(7) = '0' ) else 
       'Z';             
RAM3 <= F(3) when ( I(8) = '1') and ( I(7) = '1' ) else
         'Z';           

Q3 <= Q(3) when ( I(8) = '1') and ( I(7) = '1') else
          'Z';         
Q0 <= Q(0) when ( I(8) = '1') and ( I(7) = '0') else
          'Z';         

end  output_and_shifter;

------------------------------------------------