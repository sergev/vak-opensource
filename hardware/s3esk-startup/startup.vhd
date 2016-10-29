--
-- Reference design - Initial design for Spartan-3E Starter Kit when delivered.
--
-- Ken Chapman - Xilinx Ltd - January 2006
--
-- Constantly scroll the text "SPARTAN-3E STARTER KIT" and "www.xilinx.com/s3estarter" across the LCD.
--
-- SW0 turns on LD0
-- SW1 turns on LD1                                                  Single LED is moved left or
-- SW2 turns on LD2                                                  right by rotation of control.
-- SW3 turns on LD3                           OR
-- BTN East turns on LD4               by pressing centre
-- BTN South turns on LD5            button of rotary encoder
-- BTN North turns on LD6                toggle mode
-- BTN West turns on LD7
--
-- PicoBlaze provides full control over the LCD display.
--
------------------------------------------------------------------------------------
--
-- NOTICE:
--
-- Copyright Xilinx, Inc. 2006.   This code may be contain portions patented by other
-- third parties.  By providing this core as one possible implementation of a standard,
-- Xilinx is making no representation that the provided implementation of this standard
-- is free from any claims of infringement by any third party.  Xilinx expressly
-- disclaims any warranty with respect to the adequacy of the implementation, including
-- but not limited to any warranty or representation that the implementation is free
-- from claims of any third party.  Furthermore, Xilinx is providing this core as a
-- courtesy to you and suggests that you contact all third parties to obtain the
-- necessary rights to use this implementation.
--
------------------------------------------------------------------------------------
--
-- Library declarations
--
-- Standard IEEE libraries
--
library IEEE;
use IEEE.STD_LOGIC_1164.ALL;
use IEEE.STD_LOGIC_ARITH.ALL;
use IEEE.STD_LOGIC_UNSIGNED.ALL;
--
------------------------------------------------------------------------------------
--
--
entity s3esk_startup is
    Port (             led : out std_logic_vector(7 downto 0);
            strataflash_oe : out std_logic;
            strataflash_ce : out std_logic;
            strataflash_we : out std_logic;
                    switch : in std_logic_vector(3 downto 0);
                 btn_north : in std_logic;
                  btn_east : in std_logic;
                 btn_south : in std_logic;
                  btn_west : in std_logic;
                     lcd_d : inout std_logic_vector(7 downto 4);
                    lcd_rs : out std_logic;
                    lcd_rw : out std_logic;
                     lcd_e : out std_logic;
                  rotary_a : in std_logic;
                  rotary_b : in std_logic;
              rotary_press : in std_logic;
                       clk : in std_logic);
    end s3esk_startup;
--
------------------------------------------------------------------------------------
--
-- Start of test architecture
--
architecture Behavioral of s3esk_startup is
--
------------------------------------------------------------------------------------
--
-- declaration of KCPSM3
--
  component kcpsm3
    Port (      address : out std_logic_vector(9 downto 0);
            instruction : in std_logic_vector(17 downto 0);
                port_id : out std_logic_vector(7 downto 0);
           write_strobe : out std_logic;
               out_port : out std_logic_vector(7 downto 0);
            read_strobe : out std_logic;
                in_port : in std_logic_vector(7 downto 0);
              interrupt : in std_logic;
          interrupt_ack : out std_logic;
                  reset : in std_logic;
                    clk : in std_logic);
    end component;
--
-- declaration of program ROM
--
  component control
    Port (      address : in std_logic_vector(9 downto 0);
            instruction : out std_logic_vector(17 downto 0);
             proc_reset : out std_logic;                       --JTAG Loader version
                    clk : in std_logic);
    end component;
--
------------------------------------------------------------------------------------
--
-- Signals used to connect KCPSM3 to program ROM and I/O logic
--
signal address          : std_logic_vector(9 downto 0);
signal instruction      : std_logic_vector(17 downto 0);
signal port_id          : std_logic_vector(7 downto 0);
signal out_port         : std_logic_vector(7 downto 0);
signal in_port          : std_logic_vector(7 downto 0);
signal write_strobe     : std_logic;
signal read_strobe      : std_logic;
signal interrupt        : std_logic :='0';
signal interrupt_ack    : std_logic;
signal kcpsm3_reset     : std_logic;
--
--
-- Signals for LCD operation
--
-- Tri-state output requires internal signals
-- 'lcd_drive' is used to differentiate between LCD and StrataFLASH communications
-- which share the same data bits.
--
signal   lcd_rw_control : std_logic;
signal  lcd_output_data : std_logic_vector(7 downto 4);
signal        lcd_drive : std_logic;
--
--
-- Signals used to interface to rotary encoder
--
signal      rotary_a_in : std_logic;
signal      rotary_b_in : std_logic;
signal  rotary_press_in : std_logic;
signal        rotary_in : std_logic_vector(1 downto 0);
signal        rotary_q1 : std_logic;
signal        rotary_q2 : std_logic;
signal  delay_rotary_q1 : std_logic;
signal     rotary_event : std_logic;
signal      rotary_left : std_logic;
--
--
------------------------------------------------------------------------------------------------------------------------------------------------------------------------
--
-- Start of circuit description
--
begin
  --
  ----------------------------------------------------------------------------------------------------------------------------------
  -- Disable unused components
  ----------------------------------------------------------------------------------------------------------------------------------
  --
  --StrataFLASH must be disabled to prevent it conflicting with the LCD display
  --
  strataflash_oe <= '1';
  strataflash_ce <= '1';
  strataflash_we <= '1';
  --
  --
  ----------------------------------------------------------------------------------------------------------------------------------
  -- KCPSM3 and the program memory
  ----------------------------------------------------------------------------------------------------------------------------------
  --

  processor: kcpsm3
    port map(      address => address,
               instruction => instruction,
                   port_id => port_id,
              write_strobe => write_strobe,
                  out_port => out_port,
               read_strobe => read_strobe,
                   in_port => in_port,
                 interrupt => interrupt,
             interrupt_ack => interrupt_ack,
                     reset => kcpsm3_reset,
                       clk => clk);

  program_rom: control
    port map(      address => address,
               instruction => instruction,
                proc_reset => kcpsm3_reset,                       --JTAG Loader version
                       clk => clk);

  --
  ----------------------------------------------------------------------------------------------------------------------------------
  -- Interrupt
  ----------------------------------------------------------------------------------------------------------------------------------
  --
  --
  -- Interrupt is used to detect rotation of the rotary encoder.
  -- It is anticipated that the processor will respond to interrupts at a far higher
  -- rate that the rotary control can be operated and hence events will not be missed.
  --

  interrupt_control: process(clk)
  begin
    if clk'event and clk='1' then

      -- processor interrupt waits for an acknowledgement
      if interrupt_ack='1' then
         interrupt <= '0';
        elsif rotary_event='1' then
         interrupt <= '1';
        else
         interrupt <= interrupt;
      end if;

    end if;
  end process interrupt_control;

  --
  ----------------------------------------------------------------------------------------------------------------------------------
  -- KCPSM3 input ports
  ----------------------------------------------------------------------------------------------------------------------------------
  --
  --
  -- The inputs connect via a pipelined multiplexer
  --

  input_ports: process(clk)
  begin
    if clk'event and clk='1' then

      case port_id(1 downto 0) is

        -- read simple toggle switches and buttons at address 00 hex
        when "00" =>    in_port <=  btn_west & btn_north & btn_south & btn_east & switch;

        -- read rotary control signals at address 01 hex
        when "01" =>    in_port <=  "000000" & rotary_press_in & rotary_left ;

        -- read LCD data at address 02 hex
        when "10" =>    in_port <= lcd_d & "0000";

        -- Don't care used for all other addresses to ensure minimum logic implementation
        when others =>    in_port <= "XXXXXXXX";

      end case;

     end if;

  end process input_ports;


  --
  ----------------------------------------------------------------------------------------------------------------------------------
  -- KCPSM3 output ports
  ----------------------------------------------------------------------------------------------------------------------------------
  --

  -- adding the output registers to the processor

  output_ports: process(clk)
  begin

    if clk'event and clk='1' then
      if write_strobe='1' then

        -- Write to LEDs at address 80 hex.

        if port_id(7)='1' then
          led <= out_port;
        end if;

        -- LCD data output and controls at address 40 hex.

        if port_id(6)='1' then
          lcd_output_data <= out_port(7 downto 4);
          lcd_drive <= out_port(3);
          lcd_rs <= out_port(2);
          lcd_rw_control <= out_port(1);
          lcd_e <= out_port(0);
        end if;

      end if;

    end if;

  end process output_ports;

  --
  ----------------------------------------------------------------------------------------------------------------------------------
  -- LCD interface
  ----------------------------------------------------------------------------------------------------------------------------------
  --
  -- The 4-bit data port is bidirectional.
  -- lcd_rw is '1' for read and '0' for write
  -- lcd_drive is like a master enable signal which prevents either the
  -- FPGA outputs or the LCD display driving the data lines.
  --
  --Control of read and write signal
  lcd_rw <= lcd_rw_control and lcd_drive;

  --use read/write control to enable output buffers.
  lcd_d <= lcd_output_data when (lcd_rw_control='0' and lcd_drive='1') else "ZZZZ";


  ----------------------------------------------------------------------------------------------------------------------------------
  -- Interface to rotary encoder.
  -- Detection of movement and direction.
  ----------------------------------------------------------------------------------------------------------------------------------
  --
  -- The rotary switch contacts are filtered using their offset (one-hot) style to
  -- clean them. Circuit concept by Peter Alfke.
  -- Note that the clock rate is fast compared with the switch rate.

  rotary_filter: process(clk)
  begin
    if clk'event and clk='1' then

      --Synchronise inputs to clock domain using flip-flops in input/output blocks.
      rotary_a_in <= rotary_a;
      rotary_b_in <= rotary_b;
      rotary_press_in <= rotary_press;

      --concatinate rotary input signals to form vector for case construct.
      rotary_in <= rotary_b_in & rotary_a_in;

      case rotary_in is

        when "00" => rotary_q1 <= '0';
                     rotary_q2 <= rotary_q2;

        when "01" => rotary_q1 <= rotary_q1;
                     rotary_q2 <= '0';

        when "10" => rotary_q1 <= rotary_q1;
                     rotary_q2 <= '1';

        when "11" => rotary_q1 <= '1';
                     rotary_q2 <= rotary_q2;

        when others => rotary_q1 <= rotary_q1;
                       rotary_q2 <= rotary_q2;
      end case;

    end if;
  end process rotary_filter;
  --
  -- The rising edges of 'rotary_q1' indicate that a rotation has occurred and the
  -- state of 'rotary_q2' at that time will indicate the direction.
  --
  direction: process(clk)
  begin
    if clk'event and clk='1' then

      delay_rotary_q1 <= rotary_q1;
      if rotary_q1='1' and delay_rotary_q1='0' then
        rotary_event <= '1';
        rotary_left <= rotary_q2;
       else
        rotary_event <= '0';
        rotary_left <= rotary_left;
      end if;

    end if;
  end process direction;
  --
  ----------------------------------------------------------------------------------------------------------------------------------
  --

  --
  --
  --
end Behavioral;

------------------------------------------------------------------------------------------------------------------------------------
--
-- END OF FILE s3esk_startup.vhd
--
------------------------------------------------------------------------------------------------------------------------------------
