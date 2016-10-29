--
-- Definition of a dual port ROM for KCPSM2 or KCPSM3 program defined by control.psm
-- and assmbled using KCPSM2 or KCPSM3 assembler.
--
-- Standard IEEE libraries
--
library IEEE;
use IEEE.STD_LOGIC_1164.ALL;
use IEEE.STD_LOGIC_ARITH.ALL;
use IEEE.STD_LOGIC_UNSIGNED.ALL;
--
-- The Unisim Library is used to define Xilinx primitives. It is also used during
-- simulation. The source can be viewed at %XILINX%\vhdl\src\unisims\unisim_VCOMP.vhd
--  
library unisim;
use unisim.vcomponents.all;
--
--
entity control is
    Port (      address : in std_logic_vector(9 downto 0);
            instruction : out std_logic_vector(17 downto 0);
             proc_reset : out std_logic;
                    clk : in std_logic);
    end control;
--
architecture low_level_definition of control is
--
-- Declare signals internal to this module
--
signal jaddr     : std_logic_vector(10 downto 0);
signal jparity   : std_logic_vector(0 downto 0);
signal jdata     : std_logic_vector(7 downto 0);
signal doa       : std_logic_vector(7 downto 0);
signal dopa      : std_logic_vector(0 downto 0);
signal tdo1      : std_logic;
signal tdo2      : std_logic;
signal update    : std_logic;
signal shift     : std_logic;
signal reset     : std_logic;
signal tdi       : std_logic;
signal sel1      : std_logic;
signal drck1     : std_logic;
signal drck1_buf : std_logic;
signal sel2      : std_logic;
signal drck2     : std_logic;
signal capture   : std_logic;
signal tap5      : std_logic;
signal tap11     : std_logic;
signal tap17     : std_logic;
--
-- Attributes to define ROM contents during implementation synthesis. 
-- The information is repeated in the generic map for functional simulation
--
attribute INIT_00 : string; 
attribute INIT_01 : string; 
attribute INIT_02 : string; 
attribute INIT_03 : string; 
attribute INIT_04 : string; 
attribute INIT_05 : string; 
attribute INIT_06 : string; 
attribute INIT_07 : string; 
attribute INIT_08 : string; 
attribute INIT_09 : string; 
attribute INIT_0A : string; 
attribute INIT_0B : string; 
attribute INIT_0C : string; 
attribute INIT_0D : string; 
attribute INIT_0E : string; 
attribute INIT_0F : string; 
attribute INIT_10 : string; 
attribute INIT_11 : string; 
attribute INIT_12 : string; 
attribute INIT_13 : string; 
attribute INIT_14 : string; 
attribute INIT_15 : string; 
attribute INIT_16 : string; 
attribute INIT_17 : string; 
attribute INIT_18 : string; 
attribute INIT_19 : string; 
attribute INIT_1A : string; 
attribute INIT_1B : string; 
attribute INIT_1C : string; 
attribute INIT_1D : string; 
attribute INIT_1E : string; 
attribute INIT_1F : string; 
attribute INIT_20 : string; 
attribute INIT_21 : string; 
attribute INIT_22 : string; 
attribute INIT_23 : string; 
attribute INIT_24 : string; 
attribute INIT_25 : string; 
attribute INIT_26 : string; 
attribute INIT_27 : string; 
attribute INIT_28 : string; 
attribute INIT_29 : string; 
attribute INIT_2A : string; 
attribute INIT_2B : string; 
attribute INIT_2C : string; 
attribute INIT_2D : string; 
attribute INIT_2E : string; 
attribute INIT_2F : string; 
attribute INIT_30 : string; 
attribute INIT_31 : string; 
attribute INIT_32 : string; 
attribute INIT_33 : string; 
attribute INIT_34 : string; 
attribute INIT_35 : string; 
attribute INIT_36 : string; 
attribute INIT_37 : string; 
attribute INIT_38 : string; 
attribute INIT_39 : string; 
attribute INIT_3A : string; 
attribute INIT_3B : string; 
attribute INIT_3C : string; 
attribute INIT_3D : string; 
attribute INIT_3E : string; 
attribute INIT_3F : string; 
attribute INITP_00 : string;
attribute INITP_01 : string;
attribute INITP_02 : string;
attribute INITP_03 : string;
attribute INITP_04 : string;
attribute INITP_05 : string;
attribute INITP_06 : string;
attribute INITP_07 : string;
--
-- Attributes to define ROM contents during implementation synthesis.
--
attribute INIT_00 of ram_1024_x_18 : label is  "200240010EF40F01ED030DFFE00200080065010C052E003A010C0510C00100F6";
attribute INIT_01 of ram_1024_x_18 : label is  "CE0100A4ED03EDFF400C01165C0EEF00CE0100A4102C4DFF10294D006D03541C";
attribute INIT_02 of ram_1024_x_18 : label is  "5037208060006A02A000C0804000400E541E200240010EF40F0101165C25EF00";
attribute INIT_03 of ram_1024_x_18 : label is  "00CC054100CC055000CC0553A000CA80EA020A0C40370A0250362001E000A07F";
attribute INIT_04 of ram_1024_x_18 : label is  "0553009800CC054500CC053300CC052D00CC054E00CC054100CC055400CC0552";
attribute INIT_05 of ram_1024_x_18 : label is  "00CC054B009800CC055200CC054500CC055400CC055200CC054100CC055400CC";
attribute INIT_06 of ram_1024_x_18 : label is  "056900CC057800CC052E00CC057700CC057700CC0577A00000CC055400CC0549";
attribute INIT_07 of ram_1024_x_18 : label is  "056D00CC056F00CC056300CC052E00CC057800CC056E00CC056900CC056C00CC";
attribute INIT_08 of ram_1024_x_18 : label is  "057200CC056100CC057400CC057300CC056500CC053300CC057300CC052F00CC";
attribute INIT_09 of ram_1024_x_18 : label is  "0128A000549CC001000BA00000CC0520A00000CC057200CC056500CC057400CC";
attribute INIT_0A of ram_1024_x_18 : label is  "00A90432A00054AAC30100A40314A00054A5C201009F0219A00054A0C101009B";
attribute INIT_0B of ram_1024_x_18 : label is  "C408A4F01450A00000B3C440A4F8A000C440E401009BC440E401A00054AFC401";
attribute INIT_0C of ram_1024_x_18 : label is  "C440C40CA4F01450A000C44004F0009F00B904060406040604071450009B00B9";
attribute INIT_0D of ram_1024_x_18 : label is  "E401C440040EA000C44004F0009F00B3C44004060406040704071450009B00B3";
attribute INIT_0E of ram_1024_x_18 : label is  "000E000E000EA5F0C440E4014002009BC440E401009BC440E4014502009BC440";
attribute INIT_0F of ram_1024_x_18 : label is  "00B90420009F00B900A400B900A900B9043000A9A000009FC4400404D500000E";
attribute INIT_10 of ram_1024_x_18 : label is  "C580A50F51122510A00000A400A400BD050100BD050C00BD050600BD0528009F";
attribute INIT_11 of ram_1024_x_18 : label is  "E000C0804001512040006003E001A00000BD0518A00000BDC5C0A50FA00000BD";
attribute INIT_12 of ram_1024_x_18 : label is  "0000000000000000000000000000000000000000000000000000000080016001";
attribute INIT_13 of ram_1024_x_18 : label is  "0000000000000000000000000000000000000000000000000000000000000000";
attribute INIT_14 of ram_1024_x_18 : label is  "0000000000000000000000000000000000000000000000000000000000000000";
attribute INIT_15 of ram_1024_x_18 : label is  "0000000000000000000000000000000000000000000000000000000000000000";
attribute INIT_16 of ram_1024_x_18 : label is  "0000000000000000000000000000000000000000000000000000000000000000";
attribute INIT_17 of ram_1024_x_18 : label is  "0000000000000000000000000000000000000000000000000000000000000000";
attribute INIT_18 of ram_1024_x_18 : label is  "0000000000000000000000000000000000000000000000000000000000000000";
attribute INIT_19 of ram_1024_x_18 : label is  "0000000000000000000000000000000000000000000000000000000000000000";
attribute INIT_1A of ram_1024_x_18 : label is  "0000000000000000000000000000000000000000000000000000000000000000";
attribute INIT_1B of ram_1024_x_18 : label is  "0000000000000000000000000000000000000000000000000000000000000000";
attribute INIT_1C of ram_1024_x_18 : label is  "0000000000000000000000000000000000000000000000000000000000000000";
attribute INIT_1D of ram_1024_x_18 : label is  "0000000000000000000000000000000000000000000000000000000000000000";
attribute INIT_1E of ram_1024_x_18 : label is  "0000000000000000000000000000000000000000000000000000000000000000";
attribute INIT_1F of ram_1024_x_18 : label is  "0000000000000000000000000000000000000000000000000000000000000000";
attribute INIT_20 of ram_1024_x_18 : label is  "0000000000000000000000000000000000000000000000000000000000000000";
attribute INIT_21 of ram_1024_x_18 : label is  "0000000000000000000000000000000000000000000000000000000000000000";
attribute INIT_22 of ram_1024_x_18 : label is  "0000000000000000000000000000000000000000000000000000000000000000";
attribute INIT_23 of ram_1024_x_18 : label is  "0000000000000000000000000000000000000000000000000000000000000000";
attribute INIT_24 of ram_1024_x_18 : label is  "0000000000000000000000000000000000000000000000000000000000000000";
attribute INIT_25 of ram_1024_x_18 : label is  "0000000000000000000000000000000000000000000000000000000000000000";
attribute INIT_26 of ram_1024_x_18 : label is  "0000000000000000000000000000000000000000000000000000000000000000";
attribute INIT_27 of ram_1024_x_18 : label is  "0000000000000000000000000000000000000000000000000000000000000000";
attribute INIT_28 of ram_1024_x_18 : label is  "0000000000000000000000000000000000000000000000000000000000000000";
attribute INIT_29 of ram_1024_x_18 : label is  "0000000000000000000000000000000000000000000000000000000000000000";
attribute INIT_2A of ram_1024_x_18 : label is  "0000000000000000000000000000000000000000000000000000000000000000";
attribute INIT_2B of ram_1024_x_18 : label is  "0000000000000000000000000000000000000000000000000000000000000000";
attribute INIT_2C of ram_1024_x_18 : label is  "0000000000000000000000000000000000000000000000000000000000000000";
attribute INIT_2D of ram_1024_x_18 : label is  "0000000000000000000000000000000000000000000000000000000000000000";
attribute INIT_2E of ram_1024_x_18 : label is  "0000000000000000000000000000000000000000000000000000000000000000";
attribute INIT_2F of ram_1024_x_18 : label is  "0000000000000000000000000000000000000000000000000000000000000000";
attribute INIT_30 of ram_1024_x_18 : label is  "0000000000000000000000000000000000000000000000000000000000000000";
attribute INIT_31 of ram_1024_x_18 : label is  "0000000000000000000000000000000000000000000000000000000000000000";
attribute INIT_32 of ram_1024_x_18 : label is  "0000000000000000000000000000000000000000000000000000000000000000";
attribute INIT_33 of ram_1024_x_18 : label is  "0000000000000000000000000000000000000000000000000000000000000000";
attribute INIT_34 of ram_1024_x_18 : label is  "0000000000000000000000000000000000000000000000000000000000000000";
attribute INIT_35 of ram_1024_x_18 : label is  "0000000000000000000000000000000000000000000000000000000000000000";
attribute INIT_36 of ram_1024_x_18 : label is  "0000000000000000000000000000000000000000000000000000000000000000";
attribute INIT_37 of ram_1024_x_18 : label is  "0000000000000000000000000000000000000000000000000000000000000000";
attribute INIT_38 of ram_1024_x_18 : label is  "0000000000000000000000000000000000000000000000000000000000000000";
attribute INIT_39 of ram_1024_x_18 : label is  "0000000000000000000000000000000000000000000000000000000000000000";
attribute INIT_3A of ram_1024_x_18 : label is  "0000000000000000000000000000000000000000000000000000000000000000";
attribute INIT_3B of ram_1024_x_18 : label is  "0000000000000000000000000000000000000000000000000000000000000000";
attribute INIT_3C of ram_1024_x_18 : label is  "0000000000000000000000000000000000000000000000000000000000000000";
attribute INIT_3D of ram_1024_x_18 : label is  "0000000000000000000000000000000000000000000000000000000000000000";
attribute INIT_3E of ram_1024_x_18 : label is  "0000000000000000000000000000000000000000000000000000000000000000";
attribute INIT_3F of ram_1024_x_18 : label is  "4119000000000000000000000000000000000000000000000000000000000000";
attribute INITP_00 of ram_1024_x_18 : label is "33333333333332CCCF3333333CCCCCCCCCCAAED8D0A3D03D78FD7DD34088F3CF";
attribute INITP_01 of ram_1024_x_18 : label is "CFFF3B82A8838E0E228FAA8F80A3EA8F02E28E2DCB72DCB72D2CB33333333333";
attribute INITP_02 of ram_1024_x_18 : label is "00000000000000000000000000000000000000000000000C834ACB0B0DBF3333";
attribute INITP_03 of ram_1024_x_18 : label is "0000000000000000000000000000000000000000000000000000000000000000";
attribute INITP_04 of ram_1024_x_18 : label is "0000000000000000000000000000000000000000000000000000000000000000";
attribute INITP_05 of ram_1024_x_18 : label is "0000000000000000000000000000000000000000000000000000000000000000";
attribute INITP_06 of ram_1024_x_18 : label is "0000000000000000000000000000000000000000000000000000000000000000";
attribute INITP_07 of ram_1024_x_18 : label is "C000000000000000000000000000000000000000000000000000000000000000";
--
begin
--
  --Instantiate the Xilinx primitive for a block RAM
  ram_1024_x_18: RAMB16_S9_S18
  --synthesis translate_off
  --INIT values repeated to define contents for functional simulation
  generic map (INIT_00 => X"200240010EF40F01ED030DFFE00200080065010C052E003A010C0510C00100F6",
               INIT_01 => X"CE0100A4ED03EDFF400C01165C0EEF00CE0100A4102C4DFF10294D006D03541C",
               INIT_02 => X"5037208060006A02A000C0804000400E541E200240010EF40F0101165C25EF00",
               INIT_03 => X"00CC054100CC055000CC0553A000CA80EA020A0C40370A0250362001E000A07F",
               INIT_04 => X"0553009800CC054500CC053300CC052D00CC054E00CC054100CC055400CC0552",
               INIT_05 => X"00CC054B009800CC055200CC054500CC055400CC055200CC054100CC055400CC",
               INIT_06 => X"056900CC057800CC052E00CC057700CC057700CC0577A00000CC055400CC0549",
               INIT_07 => X"056D00CC056F00CC056300CC052E00CC057800CC056E00CC056900CC056C00CC",
               INIT_08 => X"057200CC056100CC057400CC057300CC056500CC053300CC057300CC052F00CC",
               INIT_09 => X"0128A000549CC001000BA00000CC0520A00000CC057200CC056500CC057400CC",
               INIT_0A => X"00A90432A00054AAC30100A40314A00054A5C201009F0219A00054A0C101009B",
               INIT_0B => X"C408A4F01450A00000B3C440A4F8A000C440E401009BC440E401A00054AFC401",
               INIT_0C => X"C440C40CA4F01450A000C44004F0009F00B904060406040604071450009B00B9",
               INIT_0D => X"E401C440040EA000C44004F0009F00B3C44004060406040704071450009B00B3",
               INIT_0E => X"000E000E000EA5F0C440E4014002009BC440E401009BC440E4014502009BC440",
               INIT_0F => X"00B90420009F00B900A400B900A900B9043000A9A000009FC4400404D500000E",
               INIT_10 => X"C580A50F51122510A00000A400A400BD050100BD050C00BD050600BD0528009F",
               INIT_11 => X"E000C0804001512040006003E001A00000BD0518A00000BDC5C0A50FA00000BD",
               INIT_12 => X"0000000000000000000000000000000000000000000000000000000080016001",
               INIT_13 => X"0000000000000000000000000000000000000000000000000000000000000000",
               INIT_14 => X"0000000000000000000000000000000000000000000000000000000000000000",
               INIT_15 => X"0000000000000000000000000000000000000000000000000000000000000000",
               INIT_16 => X"0000000000000000000000000000000000000000000000000000000000000000",
               INIT_17 => X"0000000000000000000000000000000000000000000000000000000000000000",
               INIT_18 => X"0000000000000000000000000000000000000000000000000000000000000000",
               INIT_19 => X"0000000000000000000000000000000000000000000000000000000000000000",
               INIT_1A => X"0000000000000000000000000000000000000000000000000000000000000000",
               INIT_1B => X"0000000000000000000000000000000000000000000000000000000000000000",
               INIT_1C => X"0000000000000000000000000000000000000000000000000000000000000000",
               INIT_1D => X"0000000000000000000000000000000000000000000000000000000000000000",
               INIT_1E => X"0000000000000000000000000000000000000000000000000000000000000000",
               INIT_1F => X"0000000000000000000000000000000000000000000000000000000000000000",
               INIT_20 => X"0000000000000000000000000000000000000000000000000000000000000000",
               INIT_21 => X"0000000000000000000000000000000000000000000000000000000000000000",
               INIT_22 => X"0000000000000000000000000000000000000000000000000000000000000000",
               INIT_23 => X"0000000000000000000000000000000000000000000000000000000000000000",
               INIT_24 => X"0000000000000000000000000000000000000000000000000000000000000000",
               INIT_25 => X"0000000000000000000000000000000000000000000000000000000000000000",
               INIT_26 => X"0000000000000000000000000000000000000000000000000000000000000000",
               INIT_27 => X"0000000000000000000000000000000000000000000000000000000000000000",
               INIT_28 => X"0000000000000000000000000000000000000000000000000000000000000000",
               INIT_29 => X"0000000000000000000000000000000000000000000000000000000000000000",
               INIT_2A => X"0000000000000000000000000000000000000000000000000000000000000000",
               INIT_2B => X"0000000000000000000000000000000000000000000000000000000000000000",
               INIT_2C => X"0000000000000000000000000000000000000000000000000000000000000000",
               INIT_2D => X"0000000000000000000000000000000000000000000000000000000000000000",
               INIT_2E => X"0000000000000000000000000000000000000000000000000000000000000000",
               INIT_2F => X"0000000000000000000000000000000000000000000000000000000000000000",
               INIT_30 => X"0000000000000000000000000000000000000000000000000000000000000000",
               INIT_31 => X"0000000000000000000000000000000000000000000000000000000000000000",
               INIT_32 => X"0000000000000000000000000000000000000000000000000000000000000000",
               INIT_33 => X"0000000000000000000000000000000000000000000000000000000000000000",
               INIT_34 => X"0000000000000000000000000000000000000000000000000000000000000000",
               INIT_35 => X"0000000000000000000000000000000000000000000000000000000000000000",
               INIT_36 => X"0000000000000000000000000000000000000000000000000000000000000000",
               INIT_37 => X"0000000000000000000000000000000000000000000000000000000000000000",
               INIT_38 => X"0000000000000000000000000000000000000000000000000000000000000000",
               INIT_39 => X"0000000000000000000000000000000000000000000000000000000000000000",
               INIT_3A => X"0000000000000000000000000000000000000000000000000000000000000000",
               INIT_3B => X"0000000000000000000000000000000000000000000000000000000000000000",
               INIT_3C => X"0000000000000000000000000000000000000000000000000000000000000000",
               INIT_3D => X"0000000000000000000000000000000000000000000000000000000000000000",
               INIT_3E => X"0000000000000000000000000000000000000000000000000000000000000000",
               INIT_3F => X"4119000000000000000000000000000000000000000000000000000000000000",    
               INITP_00 => X"33333333333332CCCF3333333CCCCCCCCCCAAED8D0A3D03D78FD7DD34088F3CF",
               INITP_01 => X"CFFF3B82A8838E0E228FAA8F80A3EA8F02E28E2DCB72DCB72D2CB33333333333",
               INITP_02 => X"00000000000000000000000000000000000000000000000C834ACB0B0DBF3333",
               INITP_03 => X"0000000000000000000000000000000000000000000000000000000000000000",
               INITP_04 => X"0000000000000000000000000000000000000000000000000000000000000000",
               INITP_05 => X"0000000000000000000000000000000000000000000000000000000000000000",
               INITP_06 => X"0000000000000000000000000000000000000000000000000000000000000000",
               INITP_07 => X"C000000000000000000000000000000000000000000000000000000000000000")
  --synthesis translate_on
  port map(    DIB => "0000000000000000",
              DIPB => "00",
               ENB => '1',
               WEB => '0',
              SSRB => '0',
              CLKB => clk,
             ADDRB => address,
               DOB => instruction(15 downto 0),
              DOPB => instruction(17 downto 16),
               DIA => jdata,
              DIPA => jparity,
               ENA => sel1,
               WEA => '1',
              SSRA => '0',
              CLKA => update,
              ADDRA=> jaddr,
               DOA => doa(7 downto 0),
              DOPA => dopa); 
  v2_bscan: BSCAN_VIRTEX2 
  port map(   TDO1 => tdo1,
         TDO2 => tdo2,
            UPDATE => update,
             SHIFT => shift,
             RESET => reset,
               TDI => tdi,
              SEL1 => sel1,
             DRCK1 => drck1,
              SEL2 => sel2,
             DRCK2 => drck2,
      CAPTURE => capture);
  --buffer signal used as a clock
  upload_clock: BUFG
  port map( I => drck1,
            O => drck1_buf);
  -- Assign the reset to be active whenever the uploading subsystem is active
  proc_reset <= sel1;
  srlC1: SRLC16E
  --synthesis translate_off
  generic map (INIT => X"0000")
  --synthesis translate_on
  port map(   D => tdi,
             CE => '1',
            CLK => drck1_buf,
             A0 => '1',
             A1 => '0',
             A2 => '1',
             A3 => '1',
              Q => jaddr(10),
            Q15 => jaddr(8));
  flop1: FD
  port map ( D => jaddr(10),
             Q => jaddr(9),
             C => drck1_buf);
  srlC2: SRLC16E
  --synthesis translate_off
  generic map (INIT => X"0000")
  --synthesis translate_on
  port map(   D => jaddr(8),
             CE => '1',
            CLK => drck1_buf,
             A0 => '1',
             A1 => '0',
             A2 => '1',
             A3 => '1',
              Q => jaddr(7),
            Q15 => tap5);
  flop2: FD
  port map ( D => jaddr(7),
             Q => jaddr(6),
             C => drck1_buf);
  srlC3: SRLC16E
  --synthesis translate_off
  generic map (INIT => X"0000")
  --synthesis translate_on
  port map(   D => tap5,
             CE => '1',
            CLK => drck1_buf,
             A0 => '1',
             A1 => '0',
             A2 => '1',
             A3 => '1',
              Q => jaddr(5),
            Q15 => jaddr(3));
  flop3: FD
  port map ( D => jaddr(5),
             Q => jaddr(4),
             C => drck1_buf);
  srlC4: SRLC16E
  --synthesis translate_off
  generic map (INIT => X"0000")
  --synthesis translate_on
  port map(   D => jaddr(3),
             CE => '1',
            CLK => drck1_buf,
             A0 => '1',
             A1 => '0',
             A2 => '1',
             A3 => '1',
              Q => jaddr(2),
            Q15 => tap11);
  flop4: FD
  port map ( D => jaddr(2),
             Q => jaddr(1),
             C => drck1_buf);
  srlC5: SRLC16E
  --synthesis translate_off
  generic map (INIT => X"0000")
  --synthesis translate_on
  port map(   D => tap11,
             CE => '1',
            CLK => drck1_buf,
             A0 => '1',
             A1 => '0',
             A2 => '1',
             A3 => '1',
              Q => jaddr(0),
            Q15 => jdata(7));
  flop5: FD
  port map ( D => jaddr(0),
             Q => jparity(0),
             C => drck1_buf);
  srlC6: SRLC16E
  --synthesis translate_off
  generic map (INIT => X"0000")
  --synthesis translate_on
  port map(   D => jdata(7),
             CE => '1',
            CLK => drck1_buf,
             A0 => '1',
             A1 => '0',
             A2 => '1',
             A3 => '1',
              Q => jdata(6),
            Q15 => tap17);
  flop6: FD
  port map ( D => jdata(6),
             Q => jdata(5),
             C => drck1_buf);
  srlC7: SRLC16E
  --synthesis translate_off
  generic map (INIT => X"0000")
  --synthesis translate_on
  port map(   D => tap17,
             CE => '1',
            CLK => drck1_buf,
             A0 => '1',
             A1 => '0',
             A2 => '1',
             A3 => '1',
              Q => jdata(4),
            Q15 => jdata(2));
  flop7: FD
  port map ( D => jdata(4),
             Q => jdata(3),
             C => drck1_buf);
  srlC8: SRLC16E
  --synthesis translate_off
  generic map (INIT => X"0000")
  --synthesis translate_on
  port map(   D => jdata(2),
             CE => '1',
            CLK => drck1_buf,
             A0 => '1',
             A1 => '0',
             A2 => '1',
             A3 => '1',
              Q => jdata(1),
            Q15 => tdo1);
  flop8: FD
  port map ( D => jdata(1),
             Q => jdata(0),
             C => drck1_buf);
end low_level_definition;
--
------------------------------------------------------------------------------------
--
-- END OF FILE control.vhd
--
------------------------------------------------------------------------------------
