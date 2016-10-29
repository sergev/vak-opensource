--------------------------------------------------------------------------------
--
--   AMD 2910 Benchmark
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
               I : in MVL7_VECTOR(3 downto 0);      -- 2910 instruction 
	CCEN_BAR : in MVL7;                  -- condition code enable input bit
          CC_BAR : in MVL7;                       -- condition code input bit
         RLD_BAR : in MVL7;                       -- R register load
	      CI : in MVL7;                       -- carry in
           OEbar : in MVL7;                       -- tri-state driver
             clk : in clock;                       -- clock
               D : in MVL7_VECTOR(11 downto 0);     -- direct inputs
       	       Y : out MVL7_VECTOR(11 downto 0);    -- output instruction word
          PL_BAR : out MVL7;                     --
        VECT_BAR : out MVL7;                     --
         MAP_BAR : out MVL7;                     --
	FULL_BAR : out MVL7                      -- stack full flag

	 );
end AM2910;

architecture AM2910 of AM2910 is
begin

process

          variable FAIL : MVL7;                        -- CC fail flag
          variable SP : INTEGER range 0 to 5;    -- stack pointer
          variable STACK : MEMORY_12_bit(5 downto 0);   -- stack register file
          variable RE : MVL7_vector(11 downto 0);
          variable uPC : MVL7_vector(11 downto 0);
          variable Y_temp : MVL7_vector(11 downto 0); 
begin

	 wait until ( (clk = '0') and (not clk'stable) );

         fail := CC_bar and ( not CCEN_bar);

         if (SP = 5) then    -- NECCESSARY FOR CORRECT SIMULATION
            FULL_BAR <= '0'; -- SINCE THIS PROCESS IS NOT TRIGERRED BY
         else                -- A RISING CLOCK EDGE
            FULL_BAR <= '1';
         end if;
 
---------------------------------------------------------------------------

      case I is

	   when "0000" =>                   -- JZ instruction

              Y_temp := "000000000000";

              if (RLD_BAR = '0') then          
                 RE := D;
              end if;

	      SP := 0;

              uPC := "000000000000";
              MAP_BAR <= '1';
              VECT_BAR <= '1';
              PL_BAR <= '0';
----------------------------------------------------------------------------
	   when "0001" =>                   -- CJS instruction

	      if (FAIL = '0') then          

                 Y_temp := D;

  	         if (SP /= 5) then         -- PUSH
     		    SP := SP + 1;
	         end if;

  	         STACK(SP) := uPC;
              else
                 Y_temp := uPC;
	      end if;

              if (RLD_BAR = '0') then
                 RE := D;
              end if;

              uPC := Y_temp + ("00000000000" & CI);

              MAP_BAR <= '1';
              VECT_BAR <= '1';
              PL_BAR <= '0'; 
-------------------------------------------------------------------------------
	   when "0010" =>                   -- JMAP instruction
              Y_temp := D;

              if (RLD_BAR = '0') then
                 RE := D;
              end if;
              
              uPC := Y_temp + ("00000000000" & CI);

              MAP_BAR <= '0';
              VECT_BAR <= '1';
              PL_BAR <= '1'; 
-------------------------------------------------------------------------------
	   when "0011" =>                   -- CJP instruction

              if (FAIL = '1') then
		 Y_temp := uPC;
	      else
		 Y_temp := D;
	      end if;

              if (RLD_BAR = '0') then
                 RE := D;
              end if;

              uPC  := Y_temp + ("00000000000" & CI);

              MAP_BAR <= '1';
              VECT_BAR <= '1';
              PL_BAR <= '0'; 
--------------------------------------------------------------------------------
	   when "0100" =>                   -- PUSH instruction
              Y_temp := uPC;

	      if ( FAIL = '0') or (RLD_BAR = '0') then
		 RE := D;
	      end if;

  	      if (SP /= 5) then         -- PUSH
		 SP := SP + 1;
              end if;

              STACK(SP) := uPC;

              uPC  := Y_temp + ("00000000000" & CI);

              MAP_BAR <= '1';
              VECT_BAR <= '1';
              PL_BAR <= '0'; 
---------------------------------------------------------------------------------
	   when "0101" =>                   -- JSRP instruction

              if (FAIL = '1') then
		 Y_temp := RE;
	      else
		 Y_temp := D;
              end if;

	      if (RLD_BAR = '0') then
		 RE := D;
	      end if;

  	      if (SP /= 5) then          -- PUSH
		 SP := SP + 1;
              end if;

              STACK(SP) := uPC;

              uPC  := Y_temp + ("00000000000" & CI);

              MAP_BAR <= '1';
              VECT_BAR <= '1';
              PL_BAR <= '0'; 
---------------------------------------------------------------------------------
	   when "0110" =>                   -- CJV instruction

              if (FAIL = '1') then
		 Y_temp := uPC;
	      else
		 Y_temp := D;
	      end if;

	      if (RLD_BAR = '0') then
		 RE := D;
	      end if;

              uPC  := Y_temp + ("00000000000" & CI);

              MAP_BAR <= '1';
              VECT_BAR <= '0';
              PL_BAR <= '1'; 
---------------------------------------------------------------------------------
	   when "0111" =>                   -- JRP instruction

              if (FAIL = '1') then
		 Y_temp := RE;
	      else
		 Y_temp := D;
	      end if;

	      if (RLD_BAR = '0') then
		 RE := D;
	      end if;

              uPC  := Y_temp + ("00000000000" & CI);

              MAP_BAR <= '1';
              VECT_BAR <= '1';
              PL_BAR <= '0'; 
---------------------------------------------------------------------------------
	   when "1000" =>                   -- RFCT instruction

	      if (RE = "000000000000") then
		 Y_temp := uPC;

		 if (SP /= 0) then                --  POP
                    SP := SP - 1;
		 end if;
	      else
		 Y_temp := STACK(SP);

                 if (RLD_BAR = '1') then
                    RE := RE - "000000000001";
                 end if;
	      end if;

	      if ( RLD_BAR = '0') then
		 RE := D;
	      end if;

              uPC  := Y_temp + ("00000000000" & CI);

              MAP_BAR <= '1';
              VECT_BAR <= '1';
              PL_BAR <= '0'; 
----------------------------------------------------------------------------------
	   when "1001" =>                   -- RPCT instruction

	      if (RE /= "000000000000") then
		 Y_temp := D;

                 if (RLD_BAR = '1') then
                    RE := RE - "000000000001";
                 end if;
              else
		 Y_temp := uPC;
	      end if;

	      if ( RLD_BAR = '0') then
		 RE := D;
	      end if;

              uPC  := Y_temp + ("00000000000" & CI);

              MAP_BAR <= '1';
              VECT_BAR <= '1';
              PL_BAR <= '0'; 
-----------------------------------------------------------------------------------
	   when "1010" =>                   -- CRTN instruction

	      if (FAIL = '0') then
                 Y_temp := STACK(SP);

		 if (SP /= 0) then
                    SP := SP - 1;            -- pop
		 end if;
              else
		 Y_temp := uPC;
	      end if;

	      if ( RLD_BAR = '0') then
		 RE := D;
	      end if;

              uPC  := Y_temp + ("00000000000" & CI);

              MAP_BAR <= '1';
              VECT_BAR <= '1';
              PL_BAR <= '0'; 
------------------------------------------------------------------------------------
	   when "1011" =>                   -- CJPP instruction

	      if (FAIL = '0') then
		 Y_temp := D;

		 if (SP /= 0) then      -- pop
                    SP := SP - 1;
		 end if;
              else
		 Y_temp := uPC;
	      end if;

	      if ( RLD_BAR = '0') then
		 RE := D;
	      end if;

              uPC  := Y_temp + ("00000000000" & CI);

              MAP_BAR <= '1';
              VECT_BAR <= '1';
              PL_BAR <= '0'; 
----------------------------------------------------------------------------------
	   when "1100" =>                   -- LDCT instruction
              Y_temp := uPC;

	      RE := D;

              uPC  := Y_temp + ("00000000000" & CI);

              MAP_BAR <= '1';
              VECT_BAR <= '1';
              PL_BAR <= '0'; 
----------------------------------------------------------------------------------
	   when "1101" =>                   -- LOOP instruction

	      if (FAIL = '0') then
                 Y_temp := uPC;

		 if (SP /= 0) then        -- pop
                    SP := SP - 1;
		 end if;
              else
                 Y_temp := STACK(SP);
	      end if;

	      if ( RLD_BAR = '0') then
		 RE := D;
	      end if;

              uPC  := Y_temp + ("00000000000" & CI);

              MAP_BAR <= '1';
              VECT_BAR <= '1';
              PL_BAR <= '0'; 
------------------------------------------------------------------------------
	   when "1110" =>                   -- CONT instruction
              Y_temp := uPC;

	      if ( RLD_BAR = '0') then
		 RE := D;
	      end if;

              uPC  := Y_temp + ("00000000000" & CI);

              MAP_BAR <= '1';
              VECT_BAR <= '1';
              PL_BAR <= '0'; 
----------------------------------------------------------------------------
	   when "1111" =>                   -- TWB instruction


	      if RE = "000000000000" then
                
                 if fail = '1' then
                    Y_temp := D;
                 else
                    Y_temp := uPC;
                 end if;

                 if (SP /= 0) then       -- pop
                    SP := SP - 1;
                 end if;

	      else

		 if (FAIL = '0') then
                    Y_temp := uPC;

		    if (SP /= 0) then   -- pop
                       SP := SP - 1;
		    end if;
                 else
                    Y_temp := stack(sp);
    		 end if;

                 if(RLD_BAR = '1') then
                    RE := RE - "000000000001";
                 end if;

	      end if;

	      if ( RLD_BAR = '0') then
		 RE := D;
	      end if;

              uPC  := Y_temp + ("00000000000" & CI);

              MAP_BAR <= '1';
              VECT_BAR <= '1';
              PL_BAR <= '0'; 
-------------------------------------------------------------------------------
         when others =>

       end case;

 -- TRI-STATE DRIVER CONTROL

if OEbar = '0' then
    Y <= Y_temp;
else
    Y <= "ZZZZZZZZZZZZ";
end if;

end process;

end AM2910;




