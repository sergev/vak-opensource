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

entity stack is
port (
          clk : in clock;
	  pop : in MVL7;
         push : in MVL7;
        clear : in MVL7;
	  uPC : in MVL7_VECTOR(11 downto 0);
           sp : inout INTEGER range 0 to 5;
     reg_file : inout MEMORY_12_BIT(5 downto 0);
     FULL_BAR : out MVL7
      );
end stack;

architecture stack of stack is

begin

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

end stack;







