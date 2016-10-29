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
--  $RCSfile: memory-behaviour.vhdl,v $  $Revision: 2.1 $  $Date: 1993/10/31 21:09:45 $
--
--------------------------------------------------------------------------
--
--  Behavioural architecture for memory model
--


use work.bv_arithmetic.bv_to_natural,
    work.bv_arithmetic.natural_to_bv,
    work.images.image_hex,
    std.textio.all;
      

architecture behaviour of memory is
  
begin 
  
  mem : process
    
    constant low_address : natural := 0;
    constant high_address : natural := mem_size - 1;
    
    subtype byte is bit_vector(0 to 7);
    subtype ls_2_bits is bit_vector(1 downto 0);
    
    type memory_array is
      array (natural range low_address to high_address) of byte;
      
    variable mem : memory_array;
    variable aligned_a : dlx_address;
    variable address : natural;
    
    variable L : line;

    
    procedure load(mem : out memory_array) is
      
      file binary_file : text is in "dlx.out";
      variable L : line;
      variable addr : natural;
      variable word : dlx_word;
      
      procedure read_hex_natural(L : inout line; addr : out natural) is
        variable result : natural := 0;
        variable ch : character;
      begin
        for i in 1 to 8 loop
          read(L, ch);
          if ('0' <= ch and ch <= '9') then
            result := result*16 + character'pos(ch) - character'pos('0');
          else
            result := result*16 + character'pos(ch) - character'pos('a') + 10;
          end if;
        end loop;
        addr := result;
      end read_hex_natural;
      
      procedure read_hex_word(L : inout line; word : out dlx_word) is
        variable result : dlx_word;
        variable digit, r : natural := 0;
        variable ch : character;
      begin
        read(L, ch);  -- the space between addr and data
        for i in 10 to 17 loop
          read(L, ch);
          if ('0' <= ch and ch <= '9') then
            digit := character'pos(ch) - character'pos('0');
          else
            digit := character'pos(ch) - character'pos('a') + 10;
          end if;
          result(r to r+3) := natural_to_bv(digit, 4);
          r := r + 4;
        end loop;
        word := result;
      end read_hex_word;
      
    begin
      while not endfile(binary_file) loop
        readline(binary_file, L);
        read_hex_natural(L, addr);
        read_hex_word(L, word);
        --
        write(L, addr);
        write(L, ' ');
        write(L, image_hex(word));
        writeline(output, L);
        --
        mem(addr)   := word(0 to 7);
        mem(addr+1) := word(8 to 15);
        mem(addr+2) := word(16 to 23);
        mem(addr+3) := word(24 to 31);
      end loop;
    end load;


    procedure do_write is
    begin
      --
      -- align address to accessed unit
      --
      aligned_a := a;
      case width is
        when width_word =>
          aligned_a(1 downto 0) := b"00";
        when width_halfword =>
          aligned_a(0) := '0';
        when width_byte =>
          null;
      end case;
      address := bv_to_natural(aligned_a);
      case width is
        when width_word =>
          mem(address) := d(0 to 7);
          mem(address+1) := d(8 to 15);
          mem(address+2) := d(16 to 23);
          mem(address+3) := d(24 to 31);
        when width_halfword =>
          if a(1) = '0' then  -- ms half word
            mem(address) := d(0 to 7);
            mem(address+1) := d(8 to 15);
          else  -- ls half word
            mem(address) := d(16 to 23);
            mem(address+1) := d(24 to 31);
          end if;
        when width_byte =>
          case ls_2_bits'(a(1 downto 0)) is
            when b"00" =>
              mem(address) := d(0 to 7);
            when b"01" =>
              mem(address) := d(8 to 15);
            when b"10" =>
              mem(address) := d(16 to 23);
            when b"11" =>
              mem(address) := d(24 to 31);
          end case;
      end case;
    end do_write;
    
    procedure do_read is
    begin
      aligned_a := a;
      aligned_a(1 downto 0) := b"00";
      address := bv_to_natural(aligned_a);
      d <= mem(address) & mem(address+1) & mem(address+2) & mem(address+3);
    end do_read;
    

  begin
    load(mem);
    -- initialize outputs
    --
    d <= null;
    ready <= '0';
    -- 
    -- process memory cycles
    --
    loop
      --
      -- wait for a command, valid on leading edge of phi2
      --
      wait until phi2 = '1' and mem_enable = '1';
      --
      -- decode address and perform command if selected
      --
      address := bv_to_natural(a);
      if address >= low_address and address <= high_address then
        if write_enable = '1' then -- write cycle
          do_write;
          wait for Tac1;  -- write access time, 1st cycle
        else -- read cycle
          wait for Tac1;  -- read access time, 1st cycle
          do_read;
        end if;
        -- ready synchronous with phi2
        wait until phi2 = '1';
        ready <= '1' after Tpd_clk_out;
        wait until phi2 = '0';
        ready <= '0' after Tpd_clk_out;
        --  do subsequent cycles in burst
        while burst = '1' loop
          wait until phi2 = '1';
          if write_enable = '1' then -- write cycle
            do_write;
            wait for Tacb;  -- write access time, burst cycle
          else -- read cycle
            wait for Tacb;  -- read access time, burst cycle
            do_read;
          end if;
          -- ready synchronous with phi2
          wait until phi2 = '1';
          ready <= '1' after Tpd_clk_out;
          wait until phi2 = '0';
          ready <= '0' after Tpd_clk_out;
        end loop;
        if write_enable = '0' then  -- was read
          d <= null after Tpd_clk_out;
        end if;
      end if;
    end loop;
  end process;

end behaviour;
