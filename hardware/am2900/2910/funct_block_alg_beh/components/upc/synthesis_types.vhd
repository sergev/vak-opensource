--------------------------------------------------------------------------
	
-- SOME VHDL DATA TYPES TO FACILITATE SYNTHESIS

--  Developed on Nov 1, 1991 by :
--                                Indraneel Ghosh,
--                                CADLAB,
--                                Univ. of Calif. , Irvine.

--------------------------------------------------------------------------

use work.TYPES.all;

package SYNTHESIS_TYPES is

subtype clock is MVL7;
type Memory is array (integer range <>) of MVL7_vector(3 downto 0);
type Memory_12_bit is array (integer range <>) of MVL7_vector(11 downto 0);

end SYNTHESIS_TYPES;