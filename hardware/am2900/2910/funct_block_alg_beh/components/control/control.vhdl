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

entity control is
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
end control;

architecture control of control is

begin

------------------------------------------------------------------------------

ctrl:block

signal fail : MVL7;

begin

fail <= cc_bar and not(ccen_bar);
    
D_sel <= '1' WHEN ( I = "0010") or 
                  ( Rzero_bar = '1' and I = "1001") or
                  ( Rzero_bar = '0' and fail = '1' and I = "1111") or
                  ( ( fail = '0') and 
                                    (  ( I = "0001" ) or ( I = "0011" ) or
                                       ( I = "0101" ) or ( I = "0110" ) or
                                       ( I = "0111" ) or ( I = "1011" )
                                    )
                   ) ELSE
         '0';

uPC_sel <= '1' WHEN ( I = "0100" ) or ( I = "1100" ) or ( I = "1110") or 
                    ( ( fail = '1') and ( ( I = "0001" ) or ( I = "0011" ) or
                                          ( I = "0110" ) or ( I = "1010" ) or
                                          ( I = "1011" ) or ( I = "1110" ) 
                                        ) 
                    ) or
                    ( ( Rzero_bar = '0') and 
                      ( ( I = "1000" ) or ( I = "1001") )
                    ) or
                    ( (fail = '0') and 
                      ( ( I = "1111" ) or ( I = "1101") )
                    ) ELSE
            '0';

stack_sel <= '1' WHEN ( Rzero_bar = '1' and I = "1000") or
                      ( fail = '0' and I = "1010") or
                      ( fail = '1' and I = "1101") or
                      ( Rzero_bar = '1' and fail = '1' and I = "1111") ELSE
             '0'; 

R_sel <= '1' WHEN (( fail = '1') and (( I = "0101" ) or ( I = "0111"))) ELSE
         '0';

push <= '1' WHEN ( (fail = '0') and ( I = "0001") ) or
                 ( I = "0100" ) or 
                 ( I = "0101") ELSE
        '0';

pop <= '1' WHEN ( (fail = '0') and ( ( I = "1010" ) or ( I = "1011" ) or
                                     ( I = "1101" ) or ( I = "1111" )
                                   )
                ) or
                ( (Rzero_bar = '0') and 
                  ( (I = "1000" ) or ( I = "1111") ) 
                ) ELSE
       '0';

load <= '1' WHEN ( (I = "1100") or
                 ( I = "0100" and fail = '0')) ELSE
        '0';

decr <= '1' WHEN ( (Rzero_bar = '1') and 
                   ( (I = "1000" ) or ( I = "1001" ) or ( I = "1111") )
                 ) ELSE
        '0';

MAP_BAR <= '0' WHEN I = "0010" ELSE
           '1';

VECT_BAR <= '0' WHEN I = "0110" ELSE
            '1';

PL_BAR <= '1' WHEN ( I = "0010" ) or ( I = "0110") ELSE
          '0';

clear <= '1' WHEN I = "0000" ELSE
         '0';

end block ctrl;

------------------------------------------------------------------------------

end control;







