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
--  $RCSfile: dlx-rtl.vhdl,v $  $Revision: 2.1 $  $Date: 1993/11/02 19:54:44 $
--
--------------------------------------------------------------------------
--
--  Register transfer level architecture of DLX processor.
--


use std.textio.all,
    work.images.image_hex,
    work.bv_arithmetic.all,
    work.dlx_instr.all,
    work.alu_types.all;


architecture rtl of dlx is

  component alu
    port (s1 : in dlx_word;
          s2 : in dlx_word;
          result : out dlx_word;
          latch_en : in bit;
          func : in alu_func;
          zero, negative, overflow : out bit);
  end component;

  component reg_file
    port (a1 : in dlx_reg_addr;    -- port1 address
          q1 : out dlx_word;       -- port1 read data
          a2 : in dlx_reg_addr;    -- port2 address
          q2 : out dlx_word;       -- port2 read data
          a3 : in dlx_reg_addr;    -- port3 address
          d3 : in dlx_word;        -- port3 write data
          write_en : in bit);      -- port3 write enable
  end component;
  
  component latch 
    port (d : in dlx_word;
          q : out dlx_word;
          latch_en : in bit);
  end component;
  
  component reg_1_out
    port (d : in dlx_word;
	  q : out dlx_word_bus bus;
          latch_en : in bit;
          out_en : in bit);
  end component;
  
  component reg_2_out
    port (d : in dlx_word;
	  q1, q2 : out dlx_word_bus bus;
          latch_en : in bit;
          out_en1, out_en2 : in bit);
  end component;

  component reg_3_out
    port (d : in dlx_word;
	  q1, q2, q3 : out dlx_word_bus bus;
          latch_en : in bit;
          out_en1, out_en2, out_en3 : in bit);
  end component;

  component reg_2_1_out
    port (d : in dlx_word;
	  q1, q2 : out dlx_word_bus bus;
          q3 : out dlx_word;
          latch_en : in bit;
          out_en1, out_en2 : in bit);
  end component;

  component mux2
    port (i0, i1 : in dlx_word;
          y : out dlx_word;
          sel : in bit);
  end component;

  component ir
    port (d : in dlx_word;                           -- instruction input from memory
	  immed_q1, immed_q2 : out dlx_word_bus bus;
          ir_out : out dlx_word;                     -- instruction output to control
          latch_en : in bit;
          immed_sel1, immed_sel2 : in immed_size;    -- select 16-bit or 26-bit immed
	  immed_unsigned1, immed_unsigned2 : in bit; -- extend immed unsigned/signed
          immed_en1, immed_en2 : in bit);            -- enable immed const outputs
  end component;

  component controller
    port (phi1, phi2 : in bit;
      	  reset : in bit;
	  halt : out bit;
          width : out mem_width;
          write_enable : out bit;
          mem_enable : out bit;
	  ifetch : out bit;
          ready : in bit;
          alu_latch_en : out bit;
          alu_function : out alu_func;
          alu_zero, alu_negative, alu_overflow : in bit;
          reg_s1_addr, reg_s2_addr, reg_dest_addr : out dlx_reg_addr;
          reg_write : out bit;
          c_latch_en : out bit;
          a_latch_en, a_out_en : out bit;
          b_latch_en, b_out_en : out bit;
          temp_latch_en, temp_out_en1, temp_out_en2 : out bit;
          iar_latch_en, iar_out_en1, iar_out_en2 : out bit;
          pc_latch_en, pc_out_en1, pc_out_en2 : out bit;
          mar_latch_en, mar_out_en1, mar_out_en2 : out bit;
          mem_addr_mux_sel : out bit;
          mdr_latch_en, mdr_out_en1, mdr_out_en2, mdr_out_en3 : out bit;
          mdr_mux_sel : out bit;
	  ir_latch_en : out bit;
          ir_immed_sel1, ir_immed_sel2 : out immed_size;
	  ir_immed_unsigned1, ir_immed_unsigned2 : out bit;
          ir_immed_en1, ir_immed_en2 : out bit;
          current_instruction : in dlx_word;
          const1, const2 : out dlx_word_bus bus);
  end component;

  signal s1_bus, s2_bus : dlx_word_bus;
  signal dest_bus : dlx_word;
  signal alu_latch_en : bit;
  signal alu_function : alu_func;
  signal alu_zero, alu_negative, alu_overflow : bit;
  signal reg_s1_addr, reg_s2_addr, reg_dest_addr : dlx_reg_addr;
  signal reg_file_out1, reg_file_out2, reg_file_in : dlx_word;
  signal reg_write : bit;
  signal a_out_en, a_latch_en : bit;
  signal b_out_en, b_latch_en : bit;
  signal c_latch_en : bit;
  signal temp_out_en1, temp_out_en2, temp_latch_en : bit;
  signal iar_out_en1, iar_out_en2, iar_latch_en : bit;
  signal pc_out_en1, pc_out_en2, pc_latch_en : bit;
  signal pc_to_mem : dlx_word;
  signal mar_out_en1, mar_out_en2, mar_latch_en : bit;
  signal mar_to_mem : dlx_word;
  signal mem_addr_mux_sel : bit;
  signal mdr_out_en1, mdr_out_en2, mdr_out_en3, mdr_latch_en : bit;
  signal mdr_in : dlx_word;
  signal mdr_mux_sel : bit;
  signal current_instruction : dlx_word;
  signal ir_latch_en : bit;
  signal ir_immed_sel1, ir_immed_sel2 : immed_size;
  signal ir_immed_unsigned1, ir_immed_unsigned2 : bit;
  signal ir_immed_en1, ir_immed_en2 : bit;
  
begin

  the_alu : alu
    port map (s1 => s1_bus, s2 => s2_bus, result => dest_bus,
              latch_en => alu_latch_en, func => alu_function,
              zero => alu_zero, negative => alu_negative, overflow => alu_overflow);

  the_reg_file : reg_file
    port map (a1 => reg_s1_addr, q1 => reg_file_out1,
              a2 => reg_s2_addr, q2 => reg_file_out2,
              a3 => reg_dest_addr, d3 => reg_file_in,
              write_en => reg_write);

  c_reg : latch
    port map (d => dest_bus, q => reg_file_in, latch_en => c_latch_en);

  a_reg : reg_1_out
    port map (d => reg_file_out1, q => s1_bus,
              latch_en => a_latch_en, out_en => a_out_en);

  b_reg : reg_1_out
    port map (d => reg_file_out2, q => s2_bus,
              latch_en => b_latch_en, out_en => b_out_en);

  temp_reg : reg_2_out
    port map (d => dest_bus, q1 => s1_bus, q2 => s2_bus,
              latch_en => temp_latch_en,
              out_en1 => temp_out_en1, out_en2 => temp_out_en2);

  iar_reg : reg_2_out
    port map (d => dest_bus, q1 => s1_bus, q2 => s2_bus,
              latch_en => iar_latch_en,
              out_en1 => iar_out_en1, out_en2 => iar_out_en2);

  pc_reg : reg_2_1_out
    port map (d => dest_bus, q1 => s1_bus, q2 => s2_bus, q3 => pc_to_mem,
              latch_en => pc_latch_en,
              out_en1 => pc_out_en1, out_en2 => pc_out_en2);

  mar_reg : reg_2_1_out
    port map (d => dest_bus, q1 => s1_bus, q2 => s2_bus, q3 => mar_to_mem,
              latch_en => mar_latch_en,
              out_en1 => mar_out_en1, out_en2 => mar_out_en2);

  mem_addr_mux : mux2
    port map (i0 => pc_to_mem, i1 => mar_to_mem, y => a,
              sel => mem_addr_mux_sel);

  mdr_reg : reg_3_out
    port map (d => mdr_in, q1 => s1_bus, q2 => s2_bus, q3 => d,
              latch_en => mdr_latch_en,
              out_en1 => mdr_out_en1, out_en2 => mdr_out_en2,
	      out_en3 => mdr_out_en3);

  mdr_mux : mux2
    port map (i0 => dest_bus, i1 => d, y => mdr_in,
              sel => mdr_mux_sel);

  instr_reg : ir
    port map (d => d, immed_q1 => s1_bus, immed_q2 => s2_bus,
              ir_out => current_instruction,
              latch_en => ir_latch_en, 
	      immed_sel1 => ir_immed_sel1, immed_sel2 => ir_immed_sel2,
	      immed_unsigned1 => ir_immed_unsigned1,
              immed_unsigned2 => ir_immed_unsigned2, 
              immed_en1 => ir_immed_en1, immed_en2 => ir_immed_en2);
  
  the_controller : controller
    port map (phi1, phi2, reset, halt, 
      	      width, write_enable, mem_enable, ifetch, ready, 
              alu_latch_en, alu_function, alu_zero, alu_negative, alu_overflow,
              reg_s1_addr, reg_s2_addr, reg_dest_addr, reg_write, 
              c_latch_en, a_latch_en, a_out_en, b_latch_en, b_out_en, 
              temp_latch_en, temp_out_en1, temp_out_en2, 
              iar_latch_en, iar_out_en1, iar_out_en2, 
              pc_latch_en, pc_out_en1, pc_out_en2, 
              mar_latch_en, mar_out_en1, mar_out_en2, mem_addr_mux_sel, 
              mdr_latch_en, mdr_out_en1, mdr_out_en2, 
	      mdr_out_en3, mdr_mux_sel, 
              ir_latch_en, ir_immed_sel1, ir_immed_sel2,
	      ir_immed_unsigned1, ir_immed_unsigned2, ir_immed_en1, ir_immed_en2,  
              current_instruction, s1_bus, s2_bus); 

  debug_s1 : if debug generate
    s1_monitor : process (s1_bus)
      variable L : line;
      begin
        write(L, tag);
        write(L, string'(" s1_monitor: "));
        write(L, image_hex(s1_bus));
        writeline(output, L);
      end process s1_monitor;
  end generate;

  debug_s2 : if debug generate
    s2_monitor : process (s2_bus)
      variable L : line;
    begin
      write(L, tag);
      write(L, string'(" s2_monitor: "));
      write(L, image_hex(s2_bus));
      writeline(output, L);
    end process s2_monitor;
  end generate;

  debug_dest : if debug generate
    dest_monitor : process (dest_bus)
      variable L : line;
    begin
      write(L, tag);
      write(L, string'(" dest_monitor: "));
      write(L, image_hex(dest_bus));
      writeline(output, L);
    end process dest_monitor;
  end generate;

end rtl;
