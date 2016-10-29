--------------------------------------------------------------------------
--
--  Copyright (C) 1993, Peter J. Ashenden
--  Mail:	Dept. Computer Science
--		University of Adelaide, SA 5005, Australia
--  e-mail:	petera@cs.adelaide.edu.au
--
--  This program is free software; you can redistribute it and/or modify
--  it under the terms of the GNU General Public License as published by
--  the Free Software Foundation; either version 1, or (at your option)
--  any later version.
--
--  This program is distributed in the hope that it will be useful,
--  but WITHOUT ANY WARRANTY; without even the implied warranty of
--  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
--  GNU General Public License for more details.
--
--  You should have received a copy of the GNU General Public License
--  along with this program; if not, write to the Free Software
--  Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
--
--------------------------------------------------------------------------
--
--  $RCSfile: memory_test-bench.vhdl,v $  $Revision: 2.1 $  $Date: 1993/10/31 21:10:12 $
--
--------------------------------------------------------------------------
--
--  Architecture for test bench for behavioural architecture of memory
--


use std.textio.all,
    work.dlx_types.all,
    work.mem_types.all,
    work.bv_arithmetic.bv_addu,
    work.images.image_hex;

architecture bench of memory_test is

  component clock_gen
    port (phi1, phi2 : out bit;
      	  reset : out bit);
  end component;
  
  component memory
    port (phi1, phi2 : in bit;
      	  a : in dlx_address;
	  d : inout dlx_word_bus bus;
	  width : in mem_width;
	  write_enable : in bit;
          burst : in bit;
	  mem_enable : in bit;
	  ready : out bit);
  end component;
  
  for cg : clock_gen
    use entity work.clock_gen(behaviour)
      generic map (Tpw => 8 ns, Tps => 2 ns);
 
  for mem : memory
    use entity work.memory(behaviour)
      generic map (mem_size => 65536,
                   Tac1 => 95 ns, Tacb => 15 ns, Tpd_clk_out => 2 ns);
 
  signal phi1, phi2, reset : bit;
  signal a : dlx_address;
  signal d : dlx_word_bus bus;
  signal width : mem_width;
  signal write_enable, mem_enable, burst, ifetch, ready : bit;
  

begin

  cg : clock_gen
    port map (phi1, phi2, reset);
    
  mem : memory
    port map (phi1, phi2, a, d, width, write_enable, burst, mem_enable, ready);
    
  test: process
  
    variable data_word : dlx_word;
    variable L : line;
    VARIABLE blk : dlx_word_array(1 to 4);

    procedure write (address : in dlx_address;
		   data_width : in mem_width;
		   data : in dlx_word;
		   Tpd_clk_out : in time          -- clock to output delay
      	      	  ) is
		  
    begin -- write
      wait until phi1 = '1';
      if reset = '1' then
        return;
      end if;
      a <= address after Tpd_clk_out;
      width <= data_width after Tpd_clk_out;
      d <= data after Tpd_clk_out;
      write_enable <= '1' after Tpd_clk_out;
      burst <= '0' after Tpd_Clk_Out;
      mem_enable <= '1' after Tpd_clk_out;
      ifetch <= '0' after Tpd_clk_out;
      loop 
      	wait until phi2 = '0';
	exit when ready = '1' or reset = '1';
      end loop;
      d <= null after Tpd_clk_out;
      write_enable <= '0' after Tpd_clk_out;
      mem_enable <= '0' after Tpd_clk_out;
    end write;


    procedure read (address : in dlx_address;
		  data_width : in mem_width;
		  instr_fetch : in boolean;
		  data : out dlx_word;
		  Tpd_clk_out : in time          -- clock to output delay
      	      	 ) is
		 
    begin -- read
      wait until phi1 = '1';
      if reset = '1' then
        return;
      end if;
      a <= address after Tpd_clk_out;
      width <= data_width after Tpd_clk_out;
      write_enable <= '0' after Tpd_clk_out;
      burst <= '0' after Tpd_Clk_Out;
      mem_enable <= '1' after Tpd_clk_out;
      ifetch <= bit'val(boolean'pos(instr_fetch)) after Tpd_clk_out;
      loop 
      	wait until phi2 = '0';
	exit when ready = '1' or reset = '1';
      end loop;
      data := d;
      mem_enable <= '0' after Tpd_clk_out;
    end read;


    procedure write_burst (address : in dlx_address;
		   data : in dlx_word_array;
		   Tpd_clk_out : in time          -- clock to output delay
      	      	  ) is
      VARIABLE next_address : dlx_address := address;
      VARIABLE ignore_overflow : boolean;
      VARIABLE index : natural;
    begin -- write_burst
      wait until phi1 = '1';
      if reset = '1' then
        return;
      end if;
      width <= width_word after Tpd_clk_out;
      write_enable <= '1' after Tpd_clk_out;
      mem_enable <= '1' after Tpd_clk_out;
      ifetch <= '0' after Tpd_clk_out;
      burst <= '1' after Tpd_Clk_Out;
      index := data'left;
      burst_loop : LOOP
        IF (index = data'right) THEN
          burst <= '0' after Tpd_Clk_Out;
        END IF;
        a <= next_address after Tpd_clk_out;
        d <= data(index) after Tpd_clk_out;
        wait_loop : LOOP 
      	  WAIT UNTIL phi2 = '0';
          EXIT burst_loop WHEN reset = '1' OR (ready = '1' AND index = data'right);
          EXIT wait_loop WHEN ready = '1';
        END LOOP wait_loop;
        index := index + 1;
        bv_addu(next_address, X"00000004", next_address, ignore_overflow);
      END LOOP burst_loop;
      d <= null after Tpd_clk_out;
      write_enable <= '0' after Tpd_clk_out;
      mem_enable <= '0' after Tpd_clk_out;
    end write_burst;


    procedure read_burst (address : in dlx_address;
		  data : out dlx_word_array;
		  Tpd_clk_out : in time          -- clock to output delay
      	      	 ) is
      VARIABLE next_address : dlx_address := address;
      VARIABLE ignore_overflow : boolean;
      VARIABLE index : natural;
    begin -- read_burst
      wait until phi1 = '1';
      if reset = '1' then
        return;
      end if;
      width <= width_word after Tpd_clk_out;
      write_enable <= '0' after Tpd_clk_out;
      mem_enable <= '1' after Tpd_clk_out;
      ifetch <= '0' after Tpd_clk_out;
      burst <= '1' after Tpd_Clk_Out;
      index := data'left;
      burst_loop : LOOP
        IF (index = data'right) THEN
          burst <= '0' after Tpd_Clk_Out;
        END IF;
        a <= next_address after Tpd_clk_out;
        wait_loop : LOOP 
      	  WAIT UNTIL phi2 = '0';
          data(index) := d;
          EXIT burst_loop WHEN reset = '1' OR (ready = '1' AND index = data'right);
          EXIT wait_loop WHEN ready = '1';
        END LOOP wait_loop;
        index := index + 1;
        bv_addu(next_address, X"00000004", next_address, ignore_overflow);
      END LOOP burst_loop;
      mem_enable <= '0' after Tpd_clk_out;
    end read_burst;


  begin
    wait until reset = '0';
    write(L, string'("Write word X""00000004"" to 4:"));
    writeline(output, L);
    write(X"0000_0004", width_word, X"00000004", 2 ns);
    --
    write(L, string'("Read word from X""00000004"":"));
    writeline(output, L);
    data_word := X"0000_0000";
    read(X"0000_0004", width_word, false, data_word, 2 ns);
    write(L, string'("  result:"));
    write(L, image_hex(data_word));
    writeline(output, L);
    --
    write(L, string'("Write halfword X""2222"" to 0:"));
    writeline(output, L);
    write(X"0000_0000", width_halfword, X"2222_0000", 2 ns);
    --
    write(L, string'("Write halfword X""3333"" to 2:"));
    writeline(output, L);
    write(X"0000_0002", width_halfword, X"0000_3333", 2 ns);
    --
    write(L, string'("Read word from X""00000000"":"));
    writeline(output, L);
    data_word := X"0000_0000";
    read(X"0000_0000", width_word, false, data_word, 2 ns);
    write(L, string'("  result:"));
    write(L, image_hex(data_word));
    writeline(output, L);
    --
    write(L, string'("Read halfword from X""00000003"":"));
    writeline(output, L);
    data_word := X"0000_0000";
    read(X"0000_0003", width_halfword, false, data_word, 2 ns);
    write(L, string'("  result:"));
    write(L, image_hex(data_word));
    writeline(output, L);
    --
    write(L, string'("Write bytes X""44"" to 4, X""55"" to 5, X""66"" to 6, X""77"" to 7:"));
    writeline(output, L);
    write(X"0000_0004", width_byte, X"44_00_00_00", 2 ns);
    write(X"0000_0005", width_byte, X"00_55_00_00", 2 ns);
    write(X"0000_0006", width_byte, X"00_00_66_00", 2 ns);
    write(X"0000_0007", width_byte, X"00_00_00_77", 2 ns);
    --
    write(L, string'("Read word from X""00000004"":"));
    writeline(output, L);
    data_word := X"0000_0000";
    read(X"0000_0004", width_word, false, data_word, 2 ns);
    write(L, string'("  result:"));
    write(L, image_hex(data_word));
    writeline(output, L);
    --
    write(L, string'("Read byte from X""00000004"":"));
    writeline(output, L);
    data_word := X"0000_0000";
    read(X"0000_0004", width_byte, false, data_word, 2 ns);
    write(L, string'("  result:"));
    write(L, image_hex(data_word));
    writeline(output, L);
    --
    write(L, string'("Read byte from X""00000005"":"));
    writeline(output, L);
    data_word := X"0000_0000";
    read(X"0000_0005", width_byte, false, data_word, 2 ns);
    write(L, string'("  result:"));
    write(L, image_hex(data_word));
    writeline(output, L);
    --
    write(L, string'("Read byte from X""00000006"":"));
    writeline(output, L);
    data_word := X"0000_0000";
    read(X"0000_0006", width_byte, false, data_word, 2 ns);
    write(L, string'("  result:"));
    write(L, image_hex(data_word));
    writeline(output, L);
    --
    write(L, string'("Read byte from X""00000007"":"));
    writeline(output, L);
    data_word := X"0000_0000";
    read(X"0000_0007", width_byte, false, data_word, 2 ns);
    write(L, string'("  result:"));
    write(L, image_hex(data_word));
    writeline(output, L);
    --
    write(L, string'("Write burst to 8..11:"));
    writeline(output, L);
    blk := (X"88888888", X"99999999", X"AAAAAAAA", X"BBBBBBBB");
    write_burst(X"0000_0008", blk, 2 ns);
    --
    write(L, string'("Read burst from 8..11:"));
    writeline(output, L);
    blk := (OTHERS => X"0000_0000");
    read_burst(X"0000_0008", blk, 2 ns);
    write(L, string'("  result: ("));
    FOR i IN blk'range LOOP
      write(L, image_hex(blk(i)));
      IF (i /= blk'right) THEN
        write(L, string'(", "));
      END IF;
    END LOOP;                            --  i
    write(L, ')');
    writeline(output, L);
    --
    -- This should hang
    write(L, string'("Read word from X""00100000"":"));
    writeline(output, L);
    data_word := X"0000_0000";
    read(X"0010_0000", width_word, false, data_word, 2 ns);
    write(L, string'("  result:"));
    write(L, data_word);
    writeline(output, L);
    --
  end process test;

end bench;
