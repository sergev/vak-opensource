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

use work.types.all;
use work.MVL7_functions.all;
use work.synthesis_types.all;

entity AM2910 is
port (
            I : in MVL7_VECTOR(3 downto 0);
     CCEN_BAR : in MVL7;
       CC_BAR : in MVL7;
      RLD_BAR : in MVL7;
           CI : in MVL7;
        OEbar : in MVL7;
          clk : in clock;
            D : in MVL7_VECTOR(11 downto 0);
            Y : out MVL7_VECTOR(11 downto 0);
       PL_BAR : out MVL7;
     VECT_BAR : out MVL7;
      MAP_BAR : out MVL7;
     FULL_BAR : out MVL7
      );
end AM2910;

architecture AM2910 of AM2910 is

signal sp : INTEGER range 0 to 5; 
signal R_sel,D_sel,uPC_sel,stack_sel,decr,load,Rzero_bar,clear,push,pop : MVL7;
signal Y_temp,RE,uPC : MVL7_VECTOR(11 downto 0);
signal reg_file : MEMORY_12_BIT(5 downto 0);

begin

-------------------------------------------------------------------------------

muxr : block

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

reg_ctr : block ( (clk = '1') and (not clk'stable) )

begin

RE <= guarded D WHEN (( load = '1') or (RLD_BAR = '0')) ELSE    -- load
          RE - "000000000001" WHEN (decr = '1') and (RLD_BAR = '1') ELSE  -- decr
          RE ;   -- hold

Rzero_bar <= RE(0) or RE(1) or RE(2) or RE(4) or RE(5) or RE(6) or RE(7) or RE(8) or RE(9) or RE(10) or RE (11);

end block reg_ctr;

-------------------------------------------------------------------------------

PC : block ( (clk = '1') and (not clk'stable) )

begin

uPC <= guarded Y_temp + ("00000000000" & CI) WHEN (clear = '0') ELSE
          "000000000000" WHEN clear = '1' ELSE
          uPC;

end block PC;

------------------------------------------------------------------------------

stack_and_sp : block ( (clk = '1') and (not clk'stable) )

signal write_address : INTEGER range 0 to 5;

begin

sp <= guarded (sp - 1) WHEN (pop = '1') and (sp /= 0) ELSE
             (sp + 1) WHEN (push = '1') and (sp /= 5) ELSE
             0 when clear = '1'ELSE
             sp;

write_address <= sp + 1 WHEN (sp /= 5) ELSE -- 
                 sp;

reg_file(write_address) <= guarded uPC WHEN (push = '1') ELSE
                           reg_file(write_address);

FULL_BAR <= '0' WHEN sp = 5 ELSE
            '1';

end block stack_and_sp;

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

end AM2910;







