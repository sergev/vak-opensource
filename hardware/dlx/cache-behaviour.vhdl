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
--  $RCSfile: cache-behaviour.vhdl,v $  $Revision: 2.1 $  $Date: 1993/11/02 22:13:32 $
--
--------------------------------------------------------------------------
--
--  Behavioural architecture for cache.
--


use work.bv_arithmetic.bv_to_natural,
    work.bv_arithmetic.natural_to_bv;
    

architecture behaviour of cache is

begin  --  behaviour 

  cache_behaviour : process

    constant words_per_line : positive := line_size / 4;
    constant number_of_sets : positive := cache_size / line_size / associativity;

    subtype word_offset_range is natural range 0 to words_per_line-1;
    subtype entry_index_range is natural range 0 to associativity-1;
    subtype set_index_range is natural range 0 to number_of_sets-1;

    type line is array (word_offset_range) of dlx_word;

    type entry is record
        tag : natural;
        valid : boolean;
        dirty : boolean;
        data : line;
      end record;

    type store_array is array (set_index_range, entry_index_range) of entry;

    variable store : store_array;
    variable    cpu_address      : natural;
    variable    word_offset      : word_offset_range;
    variable    set_index        : set_index_range;
    variable    cpu_tag          : natural;
    variable    entry_index      : entry_index_range;
    variable    hit              : boolean;
    variable    next_replacement_entry_index : entry_index_range := 0;

    
    procedure do_read_hit is
    begin
      cpu_d <= store(set_index, entry_index).data(word_offset);
      cpu_ready <= '1' after Tpd_clk_out;
      wait until phi2 = '0';
      cpu_d <= null after Tpd_clk_out;
      cpu_ready <= '0' after Tpd_clk_out;
    end do_read_hit;

    
    procedure do_write_through is
    begin
      wait until phi1 = '1';
      if reset = '1' then
        return;
      end if;
      mem_a <= cpu_a after Tpd_clk_out;
      mem_width <= cpu_width after Tpd_clk_out;
      mem_d <= cpu_d after Tpd_clk_out;
      mem_write <= '1' after Tpd_clk_out;
      mem_burst <= '0' after Tpd_clk_out;
      mem_enable <= '1' after Tpd_clk_out;
      wait until mem_ready = '1' or reset = '1';
      cpu_ready <= mem_ready after Tpd_clk_out;
      wait until phi2 = '0';
      mem_d <= null after Tpd_clk_out;
      mem_write <= '0' after Tpd_clk_out;
      mem_enable <= '0' after Tpd_clk_out;
      cpu_ready <= '0' after Tpd_clk_out;
    end do_write_through;
    

    procedure do_write_hit is
    begin
      case cpu_width is
        when width_word =>
          store(set_index, entry_index).data(word_offset) := cpu_d;
        when width_halfword =>
          if cpu_a(1) = '0' then  -- ms half word
            store(set_index, entry_index).data(word_offset)(0 to 15) := cpu_d(0 to 15);
          else  -- ls half word
            store(set_index, entry_index).data(word_offset)(16 to 23) := cpu_d(16 to 23);
          end if;
        when width_byte =>
          if cpu_a(1) = '0' then  -- ms half word
            if cpu_a(0) = '0' then  -- byte 0
              store(set_index, entry_index).data(word_offset)(0 to 7) := cpu_d(0 to 7);
            else  -- byte 1
              store(set_index, entry_index).data(word_offset)(8 to 15) := cpu_d(8 to 15);
            end if;
          else  -- ls half word
            if cpu_a(0) = '0' then  -- byte 2
              store(set_index, entry_index).data(word_offset)(16 to 23) := cpu_d(16 to 23);
            else  -- byte 3
              store(set_index, entry_index).data(word_offset)(24 to 31) := cpu_d(24 to 31);
            end if;
          end if;
      end case;
      if write_strategy = copy_back then
        store(set_index, entry_index).dirty := true;
      end if;
      --
      -- if write_through cache, also update main memory
      if write_strategy = write_through then
        do_write_through;
      else  --  copy_back cache
        cpu_ready <= '1' after Tpd_clk_out;
        wait until phi2 = '0';
        cpu_ready <= '0' after Tpd_clk_out;
      end if;
    end do_write_hit;

    
    procedure copy_back_line is
      variable next_address : natural;
      variable old_word_offset : natural;
    begin
      next_address := (store(set_index, entry_index).tag * number_of_sets
                      + set_index) * line_size;
      wait until phi1 = '1';
      if reset = '1' then
        return;
      end if;
      mem_width <= width_word after Tpd_clk_out;
      mem_write <= '1' after Tpd_clk_out;
      mem_enable <= '1' after Tpd_clk_out;
      mem_burst <= '1' after Tpd_clk_out;
      old_word_offset := 0;
      burst_loop : loop
        if old_word_offset = words_per_line-1 then
          mem_burst <= '0' after Tpd_clk_out;
        end if;
        mem_a <= natural_to_bv(next_address, mem_a'length) after Tpd_clk_out;
        mem_d <= store(set_index, entry_index).data(old_word_offset) after Tpd_clk_out;
        wait_loop : loop 
      	  wait until phi2 = '0';
          exit burst_loop when reset = '1'
                          or (mem_ready = '1' and old_word_offset = words_per_line-1);
          exit wait_loop when mem_ready = '1';
        end loop wait_loop;
        old_word_offset := old_word_offset + 1;
        next_address := next_address + 4;
      end loop burst_loop;
      store(set_index, entry_index).dirty := false;
      mem_d <= null after Tpd_clk_out;
      mem_write <= '0' after Tpd_clk_out;
      mem_enable <= '0' after Tpd_clk_out;
    end copy_back_line;


    procedure fetch_line is
      variable next_address : natural;
      variable new_word_offset : natural;
    begin
      next_address := (cpu_address / line_size) * line_size;
      wait until phi1 = '1';
      if reset = '1' then
        return;
      end if;
      mem_width <= width_word after Tpd_clk_out;
      mem_write <= '0' after Tpd_clk_out;
      mem_enable <= '1' after Tpd_clk_out;
      mem_burst <= '1' after Tpd_clk_out;
      new_word_offset := 0;
      burst_loop : loop
        if new_word_offset = words_per_line-1 then
          mem_burst <= '0' after Tpd_clk_out;
        end if;
        mem_a <= natural_to_bv(next_address, mem_a'length) after Tpd_clk_out;
        wait_loop : loop 
      	  wait until phi2 = '0';
          store(set_index, entry_index).data(new_word_offset) := mem_d;
          exit burst_loop when reset = '1'
                          or (mem_ready = '1' and new_word_offset = words_per_line-1);
          exit wait_loop when mem_ready = '1';
        end loop wait_loop;
        new_word_offset := new_word_offset + 1;
        next_address := next_address + 4;
      end loop burst_loop;
      store(set_index, entry_index).valid := true;
      store(set_index, entry_index).tag := cpu_tag;
      store(set_index, entry_index).dirty := false;
      mem_enable <= '0' after Tpd_clk_out;
    end fetch_line;
    

    procedure replace_line is
    begin
      --  first chose an entry using "random" number generator
      entry_index := next_replacement_entry_index;
      next_replacement_entry_index
        := (next_replacement_entry_index + 1) mod associativity;
      if store(set_index, entry_index).dirty then
        copy_back_line;
      end if;
      fetch_line;
    end replace_line;    


    procedure do_read_miss is
    begin
      replace_line;
      if reset = '1' then
        return;
      end if;
      do_read_hit;
    end do_read_miss;

    
    procedure do_write_miss is
    begin
      -- if write_through cache, just update main memory
      if write_strategy = write_through then
        do_write_through;
      else  --  copy_back cache
        replace_line;
        if reset = '1' then
          return;
        end if;
        do_write_hit;
      end if;
    end do_write_miss;
    

  begin  --  process cache_behaviour 
    --  reset: initialize outputs and the cache store valid bits
    cpu_ready <= '0';
    cpu_d <= null;
    mem_enable <= '0';
    mem_width <= width_word;
    mem_write <= '0';
    mem_burst <= '0';
    mem_a <= X"00000000";
    mem_d <= null;
    for init_set_index in set_index_range loop
      for init_entry_index in entry_index_range loop
        store(init_set_index, init_entry_index).valid := false;
        store(init_set_index, init_entry_index).dirty := false;
      end loop;                          --  init_entry_index
    end loop;                            --  init_set_index
    --  
    loop 
      --  wait for a cpu request
      wait until phi2 = '1' and cpu_enable = '1';
      --  decode address
      cpu_address := bv_to_natural(cpu_a);
      word_offset := (cpu_address mod line_size) / 4;
      set_index := (cpu_address / line_size) mod number_of_sets;
      cpu_tag := cpu_address / line_size / number_of_sets;
      --  check for hit
      hit := false;
      for lookup_entry_index in entry_index_range loop
        if store(set_index, lookup_entry_index).valid
            and store(set_index, lookup_entry_index).tag = cpu_tag then
          hit := true;
          entry_index := lookup_entry_index;
          exit;
        end if;
      end loop;                          --  lookup_entry
      --
      if hit then
        if cpu_write = '1' then
          do_write_hit;
        else
          do_read_hit;
        end if;
      else
        if cpu_write = '1' then
          do_write_miss;
        else
          do_read_miss;
        end if;
      end if;
      exit when reset = '1';
    end loop;
    --  loop exited on reset: wait until it goes inactive
    --  then start again
    wait until phi2 = '0' and reset = '0';
  end process cache_behaviour;

end behaviour;
