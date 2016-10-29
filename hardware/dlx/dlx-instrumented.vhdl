--------------------------------------------------------------------------
--
--  Copyright (C) 1993, Peter J. Ashenden
--  Mail:       Dept. Computer Science
--              University of Adelaide, SA 5005, Australia
--  e-mail:     petera@cs.adelaide.edu.au
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
--  $RCSfile: dlx-instrumented.vhdl,v $  $Revision: 2.1 $  $Date: 1993/11/02 18:36:01 $
--
--------------------------------------------------------------------------
--
--  Instrumented behavioural architecture for DLX, that generates
--  a files of instruction execution frequencies for a program.
--


use work.dlx_instr.all,
    work.bv_arithmetic.all,
    std.textio.all;


architecture instrumented of dlx is
  
begin -- instrumented
  
  interpreter: process
  
    type reg_array is array (reg_index) of dlx_word;
  
    variable reg : reg_array;
    variable fp_reg : reg_array;
    
    variable PC : dlx_word;
    variable user_mode : boolean;
    variable overflow, div_by_zero : boolean;
    
    constant PC_incr : dlx_word := X"0000_0004";

    variable IR : dlx_word;
    alias IR_opcode : dlx_opcode is IR(0 to 5);
    alias IR_sp_func : dlx_sp_func is IR(26 to 31);
    alias IR_fp_func : dlx_fp_func is IR(27 to 31);
    alias IR_rs1 : dlx_reg_addr is IR(6 to 10);
    alias IR_rs2 : dlx_reg_addr is IR(11 to 15);
    alias IR_Itype_rd : dlx_reg_addr is IR(11 to 15);
    alias IR_Rtype_rd : dlx_reg_addr is IR(16 to 20);
    alias IR_immed16 : dlx_immed16 is IR(16 to 31);
    alias IR_immed26 : dlx_immed26 is IR(6 to 31);
    
    variable IR_opcode_num : dlx_opcode_num;
    variable IR_sp_func_num : dlx_sp_func_num;
    variable IR_fp_func_num : dlx_fp_func_num;
    variable rs1, rs2, Itype_rd, Rtype_rd : reg_index;
    variable mem_addr : dlx_address;
    variable mem_data : dlx_word;

    subtype ls_2_addr_bits is bit_vector(1 downto 0);
    
    file data : text is out "dlx_instruction_counts";

    variable L : line;
    

    ---------------------------------------------------------------------------
    --  instrumentation: array of counters, one per instruction
    ---------------------------------------------------------------------------

    type opcode_count_array is array (dlx_opcode_num) of natural;
    type sp_func_count_array is array (dlx_sp_func_num) of natural;
    type fp_func_count_array is array (dlx_fp_func_num) of natural;

    variable    op_count        : opcode_count_array := (others => 0);
    variable    sp_func_count   : sp_func_count_array := (others => 0);
    variable    fp_func_count   : fp_func_count_array := (others => 0);

    variable    instr_count     : natural := 0;

    ---------------------------------------------------------------------------
    --  instrumentation: procedure to dump counter values
    ---------------------------------------------------------------------------

    procedure instrumentation_dump is
      variable L : line;
    begin
      for op in dlx_opcode_num loop
        write(L, opcode_names(op));
        write(L, ' ');
        write(L, op_count(op));
        writeline(data, L);
      end loop;
      for sp_func in dlx_sp_func_num loop
        write(L, sp_func_names(sp_func));
        write(L, ' ');
        write(L, sp_func_count(sp_func));
        writeline(data, L);
      end loop;
      for fp_func in dlx_fp_func_num loop
        write(L, fp_func_names(fp_func));
        write(L, ' ');
        write(L, fp_func_count(fp_func));
        writeline(data, L);
      end loop;
    end instrumentation_dump;

    ---------------------------------------------------------------------------

    
    procedure write (address : in dlx_address;
                     data_width : in mem_width;
                     data : in dlx_word;
                     signal phi1, phi2 : in bit;    -- 2-phase non-overlapping clks
                     signal reset : in bit;         -- synchronous reset input
                     signal a : out dlx_address;    -- address bus output
                     signal d : inout dlx_word_bus; -- bidirectional data bus
                     signal width : out mem_width;  -- byte/halfword/word
                     signal write_enable : out bit; -- selects read/write cycle
                     signal mem_enable : out bit;   -- starts memory cycle
                     signal ifetch : out bit;       -- indicates instruction fetch
                     signal ready : in bit;         -- status from memory system
                     Tpd_clk_out : in time          -- clock to output delay
                    ) is
                  
    begin
      wait until phi1 = '1';
      if reset = '1' then
        return;
      end if;
      a <= address after Tpd_clk_out;
      width <= data_width after Tpd_clk_out;
      d <= data after Tpd_clk_out;
      write_enable <= '1' after Tpd_clk_out;
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


    procedure bus_read (address : in dlx_address;
                    data_width : in mem_width;
                    instr_fetch : in boolean;
                    data : out dlx_word;
                    signal phi1, phi2 : in bit;    -- 2-phase non-overlapping clks
                    signal reset : in bit;         -- synchronous reset input
                    signal a : out dlx_address;    -- address bus output
                    signal d : inout dlx_word_bus; -- bidirectional data bus
                    signal width : out mem_width;  -- byte/halfword/word
                    signal write_enable : out bit; -- selects read/write cycle
                    signal mem_enable : out bit;   -- starts memory cycle
                    signal ifetch : out bit;       -- indicates instruction eftch
                    signal ready : in bit;         -- status from memory system
                    Tpd_clk_out : in time          -- clock to output delay
                   ) is
                 
    begin
      wait until phi1 = '1';
      if reset = '1' then
        return;
      end if;
      a <= address after Tpd_clk_out;
      width <= data_width after Tpd_clk_out;
      mem_enable <= '1' after Tpd_clk_out;
      ifetch <= bit'val(boolean'pos(instr_fetch)) after Tpd_clk_out;
      loop 
        wait until phi2 = '0';
        exit when ready = '1' or reset = '1';
      end loop;
      data := d;
      mem_enable <= '0' after Tpd_clk_out;
    end bus_read;


  begin -- interpreter
    --
    -- reset the processor
    --
    d <= null;
    halt <= '0';
    write_enable <= '0';
    mem_enable <= '0';
    reg(0) := X"0000_0000";
    PC := X"0000_0000";
    user_mode := false;
    --
    -- fetch-decode-execute loop
    --
    loop 
      --
      -- fetch next instruction
      --
      if debug then
        write(L, tag);
        write(L, string'(": fetching instruction..."));
        writeline(output, L);
      end if;
      --
      bus_read(PC, width_word, true, IR, 
         phi1, phi2, reset, a, d, width, write_enable, mem_enable, ifetch, ready,
         Tpd_clk_out);
      exit when reset = '1';
      --
      -- increment the PC to point to the following instruction
      --
      if debug then
        write(L, tag);
        write(L, string'(": incrementing PC..."));
        writeline(output, L);
      end if;
      --
      bv_add(PC, PC_incr, PC, overflow);
      --
      -- decode the instruction
      --
      if debug then
        write(L, tag);
        write(L, string'(": decoding instruction..."));
        writeline(output, L);
      end if;
      --
      IR_opcode_num := bv_to_natural(IR_opcode);
      IR_sp_func_num := bv_to_natural(IR_sp_func);
      IR_fp_func_num := bv_to_natural(IR_fp_func);
      rs1 := bv_to_natural(IR_rs1);
      rs2 := bv_to_natural(IR_rs2);
      Itype_rd := bv_to_natural(IR_Itype_rd);
      Rtype_rd := bv_to_natural(IR_Rtype_rd);
      --
      -------------------------------------------------------------------------
      --  instrumentation: increment counter for decoded instruction
      -------------------------------------------------------------------------
      --
      op_count(IR_opcode_num) := op_count(IR_opcode_num) + 1;
      if IR_opcode = op_special then
        sp_func_count(IR_sp_func_num) := sp_func_count(IR_sp_func_num) + 1;
      elsif IR_opcode = op_fparith then
        fp_func_count(IR_fp_func_num) := fp_func_count(IR_fp_func_num) + 1;
      end if;
      instr_count := instr_count + 1;
      --  
      -------------------------------------------------------------------------
      --
      -- exectute
      --
      if debug then
        write(L, tag);
        write(L, string'(": executing instruction..."));
        writeline(output, L);
      end if;
      --
      case IR_opcode is
        when op_special => 
          case IR_sp_func is
            WHEN sp_func_nop =>
              null;
            when sp_func_sll => 
              reg(Rtype_rd) := bv_sll(reg(rs1), bv_to_natural(reg(rs2)(27 to 31)));
            when sp_func_srl => 
              reg(Rtype_rd) := bv_srl(reg(rs1), bv_to_natural(reg(rs2)(27 to 31))); 
            when sp_func_sra => 
              reg(Rtype_rd) := bv_sra(reg(rs1), bv_to_natural(reg(rs2)(27 to 31))); 
            when sp_func_sequ =>
              if reg(rs1) = reg(rs2) then
                reg(Rtype_rd) := X"0000_0001";
              else
                reg(Rtype_rd) := X"0000_0000";
              end if;  
            when sp_func_sneu => 
              if reg(rs1) /= reg(rs2) then
                reg(Rtype_rd) := X"0000_0001";
              else
                reg(Rtype_rd) := X"0000_0000";
              end if;  
            when sp_func_sltu => 
              if reg(rs1) < reg(rs2) then
                reg(Rtype_rd) := X"0000_0001";
              else
                reg(Rtype_rd) := X"0000_0000";
              end if;  
            when sp_func_sgtu => 
              if reg(rs1) > reg(rs2) then
                reg(Rtype_rd) := X"0000_0001";
              else
                reg(Rtype_rd) := X"0000_0000";
              end if;  
            when sp_func_sleu => 
              if reg(rs1) <= reg(rs2) then
                reg(Rtype_rd) := X"0000_0001";
              else
                reg(Rtype_rd) := X"0000_0000";
              end if;  
            when sp_func_sgeu => 
              if reg(rs1) >= reg(rs2) then
                reg(Rtype_rd) := X"0000_0001";
              else
                reg(Rtype_rd) := X"0000_0000";
              end if;  
            when sp_func_add => 
              bv_add(reg(rs1), reg(rs2), reg(Rtype_rd), overflow);
            when sp_func_addu => 
              bv_addu(reg(rs1), reg(rs2), reg(Rtype_rd), overflow);
            when sp_func_sub => 
              bv_sub(reg(rs1), reg(rs2), reg(Rtype_rd), overflow);
            when sp_func_subu => 
              bv_subu(reg(rs1), reg(rs2), reg(Rtype_rd), overflow);
            when sp_func_and => 
              reg(Rtype_rd) := reg(rs1) and reg(rs2);
            when sp_func_or => 
              reg(Rtype_rd) := reg(rs1) or reg(rs2);
            when sp_func_xor => 
              reg(Rtype_rd) := reg(rs1) xor reg(rs2);
            when sp_func_seq => 
              if reg(rs1) = reg(rs2) then
                reg(Rtype_rd) := X"0000_0001";
              else
                reg(Rtype_rd) := X"0000_0000";
              end if;  
            when sp_func_sne => 
              if reg(rs1) /= reg(rs2) then
                reg(Rtype_rd) := X"0000_0001";
              else
                reg(Rtype_rd) := X"0000_0000";
              end if;  
            when sp_func_slt => 
               if bv_lt(reg(rs1), reg(rs2)) then
                reg(Rtype_rd) := X"0000_0001";
              else
                reg(Rtype_rd) := X"0000_0000";
              end if;  
            when sp_func_sgt => 
              if bv_gt(reg(rs1), reg(rs2)) then
                reg(Rtype_rd) := X"0000_0001";
              else
                reg(Rtype_rd) := X"0000_0000";
              end if;  
            when sp_func_sle => 
              if bv_le(reg(rs1), reg(rs2)) then
                reg(Rtype_rd) := X"0000_0001";
              else
                reg(Rtype_rd) := X"0000_0000";
              end if;  
            when sp_func_sge => 
              if bv_ge(reg(rs1), reg(rs2)) then
                reg(Rtype_rd) := X"0000_0001";
              else
                reg(Rtype_rd) := X"0000_0000";
              end if;  
            when sp_func_movi2s => 
              assert false
                report "MOVI2S instruction not implemented" severity warning;
            when sp_func_movs2i => 
              assert false
                report "MOVS2I instruction not implemented" severity warning;
            when sp_func_movf => 
              assert false
                report "MOVF instruction not implemented" severity warning;
            when sp_func_movd => 
              assert false
                report "MOVD instruction not implemented" severity warning;
            when sp_func_movfp2i => 
              reg(Rtype_rd) := fp_reg(rs1);
            when sp_func_movi2fp => 
              fp_reg(Rtype_rd) := reg(rs1);
            when others =>
              assert false
                report "undefined special instruction function" severity error;
          end case;  
        when op_fparith => 
          case IR_fp_func is
            when fp_func_mult => 
              bv_mult(fp_reg(rs1), fp_reg(rs2), fp_reg(Rtype_rd), overflow);
            when fp_func_multu => 
              bv_multu(fp_reg(rs1), fp_reg(rs2), fp_reg(Rtype_rd), overflow);
            when fp_func_div => 
              bv_div(fp_reg(rs1), fp_reg(rs2), fp_reg(Rtype_rd), div_by_zero, overflow);
            when fp_func_divu => 
              bv_divu(fp_reg(rs1), fp_reg(rs2), fp_reg(Rtype_rd), div_by_zero);
            when fp_func_addf | fp_func_subf | fp_func_multf | fp_func_divf | 
              fp_func_addd | fp_func_subd | fp_func_multd | fp_func_divd | 
              fp_func_cvtf2d | fp_func_cvtf2i | fp_func_cvtd2f | 
              fp_func_cvtd2i | fp_func_cvti2f | fp_func_cvti2d | 
              fp_func_eqf | fp_func_nef | fp_func_ltf | fp_func_gtf | 
              fp_func_lef | fp_func_gef | fp_func_eqd | fp_func_ned | 
              fp_func_ltd | fp_func_gtd | fp_func_led | fp_func_ged => 
              assert false
                report "floating point instructions not implemented" severity warning;
            when others =>
              assert false
                report "undefined floating point instruction function" severity error;
          end case;
        when op_j  => 
          bv_add(PC, bv_sext(IR_immed26, 32), PC, overflow);
        when op_jal => 
          reg(link_reg) := PC;
          bv_add(PC, bv_sext(IR_immed26, 32), PC, overflow);
        when op_beqz => 
          if reg(rs1) = X"0000_0000" then
            bv_add(PC, bv_sext(IR_immed16, 32), PC, overflow);
          end if;
        when op_bnez => 
          if reg(rs1) /= X"0000_0000" then
            bv_add(PC, bv_sext(IR_immed16, 32), PC, overflow);
          end if;
        when op_bfpt => 
          assert false
            report "BFPT instruction not implemented" severity warning;
        when op_bfpf => 
          assert false
            report "BFPF instruction not implemented" severity warning;
        when op_addi =>
          bv_add(reg(rs1), bv_sext(IR_immed16, 32), reg(Itype_rd), overflow);
        when op_addui => 
          bv_addu(reg(rs1), bv_zext(IR_immed16, 32), reg(Itype_rd), overflow);
        when op_subi => 
          bv_sub(reg(rs1), bv_sext(IR_immed16, 32), reg(Itype_rd), overflow);
        when op_subui => 
          bv_subu(reg(rs1), bv_zext(IR_immed16, 32), reg(Itype_rd), overflow);
        when op_slli => 
          reg(Itype_rd) := bv_sll(reg(rs1), bv_to_natural(IR_immed16(11 to 15)));
        when op_srli =>
          reg(Itype_rd) := bv_srl(reg(rs1), bv_to_natural(IR_immed16(11 to 15))); 
        when op_srai => 
          reg(Itype_rd) := bv_sra(reg(rs1), bv_to_natural(IR_immed16(11 to 15))); 
        when op_andi => 
          reg(Itype_rd) := reg(rs1) and bv_zext(IR_immed16, 32);
        when op_ori => 
          reg(Itype_rd) := reg(rs1) or bv_zext(IR_immed16, 32);
        when op_xori => 
          reg(Itype_rd) := reg(rs1) xor bv_zext(IR_immed16, 32);
        when op_lhi => 
          reg(Itype_rd) := IR_immed16 & X"0000";
        when op_rfe => 
          assert false
            report "RFE instruction not implemented" severity warning;
        when op_trap =>
          assert false
            report "TRAP instruction encountered, execution halted"
            severity note;
          halt <= '1' after Tpd_clk_out;
          ---------------------------------------------------------------------
          --  instrumentation: dump counters
          ---------------------------------------------------------------------
          instrumentation_dump;
          ---------------------------------------------------------------------
          wait until reset = '1';
          exit;
        when op_jr => 
          PC := reg(rs1);
        when op_jalr => 
          reg(link_reg) := PC;
          PC := reg(rs1);
        when op_seqi => 
          if reg(rs1) = bv_sext(IR_immed16, 32) then
            reg(Itype_rd) := X"0000_0001";
          else
            reg(Itype_rd) := X"0000_0000";
          end if;  
        when op_snei => 
          if reg(rs1) /= bv_sext(IR_immed16, 32) then
            reg(Itype_rd) := X"0000_0001";
          else
            reg(Itype_rd) := X"0000_0000";
          end if;  
        when op_slti => 
          if bv_lt(reg(rs1), bv_sext(IR_immed16, 32)) then
            reg(Itype_rd) := X"0000_0001";
          else
            reg(Itype_rd) := X"0000_0000";
          end if;  
        when op_sgti => 
          if bv_gt(reg(rs1), bv_sext(IR_immed16, 32)) then
            reg(Itype_rd) := X"0000_0001";
          else
            reg(Itype_rd) := X"0000_0000";
          end if;  
        when op_slei => 
          if bv_le(reg(rs1), bv_sext(IR_immed16, 32)) then
            reg(Itype_rd) := X"0000_0001";
          else
            reg(Itype_rd) := X"0000_0000";
          end if;  
        when op_sgei => 
          if bv_ge(reg(rs1), bv_sext(IR_immed16, 32)) then
            reg(Itype_rd) := X"0000_0001";
          else
            reg(Itype_rd) := X"0000_0000";
          end if;  
        when op_lb => 
          bv_add(reg(rs1), bv_sext(IR_immed16, 32), mem_addr, overflow);
          bus_read(mem_addr, width_byte, false, mem_data, 
            phi1, phi2, reset, a, d, width, write_enable, mem_enable, ifetch, ready,
            Tpd_clk_out);
          exit when reset = '1';
          case ls_2_addr_bits'(mem_addr(1 downto 0)) is
            when B"00" =>
              reg(Itype_rd) := bv_sext(mem_data(0 to 7), 32);
            when B"01" =>
              reg(Itype_rd) := bv_sext(mem_data(8 to 15), 32);
            when B"10" =>
              reg(Itype_rd) := bv_sext(mem_data(16 to 23), 32);
            when B"11" =>
              reg(Itype_rd) := bv_sext(mem_data(24 to 31), 32);
          end case;
        when op_lh => 
          bv_add(reg(rs1), bv_sext(IR_immed16, 32), mem_addr, overflow);
          bus_read(mem_addr, width_halfword, false, mem_data, 
            phi1, phi2, reset, a, d, width, write_enable, mem_enable, ifetch, ready,
            Tpd_clk_out);
          exit when reset = '1';
          if mem_addr(1) = '0' then
            reg(Itype_rd) := bv_sext(mem_data(0 to 15), 32);
          else
            reg(Itype_rd) := bv_sext(mem_data(16 to 31), 32);
          end if;
        when op_lw => 
          bv_add(reg(rs1), bv_sext(IR_immed16, 32), mem_addr, overflow);
          bus_read(mem_addr, width_word, false, mem_data, 
            phi1, phi2, reset, a, d, width, write_enable, mem_enable, ifetch, ready,
            Tpd_clk_out);
          exit when reset = '1';
          reg(Itype_rd) := mem_data;
        when op_lbu => 
          bv_add(reg(rs1), bv_sext(IR_immed16, 32), mem_addr, overflow);
          bus_read(mem_addr, width_byte, false, mem_data, 
            phi1, phi2, reset, a, d, width, write_enable, mem_enable, ifetch, ready,
            Tpd_clk_out);
          exit when reset = '1';
          case ls_2_addr_bits'(mem_addr(1 downto 0)) is
            when B"00" =>
              reg(Itype_rd) := bv_zext(mem_data(0 to 7), 32);
            when B"01" =>
              reg(Itype_rd) := bv_zext(mem_data(8 to 15), 32);
            when B"10" =>
              reg(Itype_rd) := bv_zext(mem_data(16 to 23), 32);
            when B"11" =>
              reg(Itype_rd) := bv_zext(mem_data(24 to 31), 32);
          end case;
        when op_lhu => 
          bv_add(reg(rs1), bv_sext(IR_immed16, 32), mem_addr, overflow);
          bus_read(mem_addr, width_halfword, false, mem_data, 
            phi1, phi2, reset, a, d, width, write_enable, mem_enable, ifetch, ready,
            Tpd_clk_out);
          exit when reset = '1';
          if mem_addr(1) = '0' then
            reg(Itype_rd) := bv_zext(mem_data(0 to 15), 32);
          else
            reg(Itype_rd) := bv_zext(mem_data(16 to 31), 32);
          end if;
        when op_lf => 
          assert false
            report "LF instruction not implemented" severity warning;
        when op_ld => 
          assert false
            report "LD instruction not implemented" severity warning;
        when op_sb => 
          bv_add(reg(rs1), bv_sext(IR_immed16, 32), mem_addr, overflow);
          mem_data := X"0000_0000";
          case ls_2_addr_bits'(mem_addr(1 downto 0)) is
            when B"00" =>
              mem_data(0 to 7) := reg(Itype_rd)(0 to 7);
            when B"01" =>
              mem_data(8 to 15) := reg(Itype_rd)(0 to 7);
            when B"10" =>
              mem_data(16 to 23) := reg(Itype_rd)(0 to 7);
            when B"11" =>
              mem_data(24 to 31) := reg(Itype_rd)(0 to 7);
          end case;
          write(mem_addr, width_halfword, mem_data, 
            phi1, phi2, reset, a, d, width, write_enable, mem_enable, ifetch, ready,
            Tpd_clk_out);
          exit when reset = '1';
        when op_sh => 
          bv_add(reg(rs1), bv_sext(IR_immed16, 32), mem_addr, overflow);
          mem_data := X"0000_0000";
          if mem_addr(1) = '0' then
            mem_data(0 to 15) := reg(Itype_rd)(0 to 15);
          else
            mem_data(16 to 31) := reg(Itype_rd)(0 to 15);
          end if;
          write(mem_addr, width_halfword, mem_data, 
            phi1, phi2, reset, a, d, width, write_enable, mem_enable, ifetch, ready,
            Tpd_clk_out);
          exit when reset = '1';
        when op_sw => 
          bv_add(reg(rs1), bv_sext(IR_immed16, 32), mem_addr, overflow);
          mem_data := reg(Itype_rd);
          write(mem_addr, width_word, mem_data, 
            phi1, phi2, reset, a, d, width, write_enable, mem_enable, ifetch, ready,
            Tpd_clk_out);
          exit when reset = '1';
        when op_sf => 
          assert false
            report "SF instruction not implemented" severity warning;
        when op_sd => 
          assert false
            report "SD instruction not implemented" severity warning;
        when op_sequi => 
          if reg(rs1) = bv_zext(IR_immed16, 32) then
            reg(Itype_rd) := X"0000_0001";
          else
            reg(Itype_rd) := X"0000_0000";
          end if;  
        when op_sneui => 
          if reg(rs1) /= bv_zext(IR_immed16, 32) then
            reg(Itype_rd) := X"0000_0001";
          else
            reg(Itype_rd) := X"0000_0000";
          end if;  
        when op_sltui => 
          if reg(rs1) < bv_zext(IR_immed16, 32) then
            reg(Itype_rd) := X"0000_0001";
          else
            reg(Itype_rd) := X"0000_0000";
          end if;  
        when op_sgtui => 
          if reg(rs1) > bv_zext(IR_immed16, 32) then
            reg(Itype_rd) := X"0000_0001";
          else
            reg(Itype_rd) := X"0000_0000";
          end if;  
        when op_sleui => 
          if reg(rs1) <= bv_zext(IR_immed16, 32) then
            reg(Itype_rd) := X"0000_0001";
          else
            reg(Itype_rd) := X"0000_0000";
          end if;  
        when op_sgeui => 
          if reg(rs1) >= bv_zext(IR_immed16, 32) then
            reg(Itype_rd) := X"0000_0001";
          else
            reg(Itype_rd) := X"0000_0000";
          end if;  
        when others =>
          assert false
            report "undefined instruction" severity error;
      end case;
      --
      -- fix up R0 in case it was overwritten
      --
      reg(0) := X"0000_0000";
      --
      if debug then
        write(L, tag);
        write(L, string'(": end of execution"));
        writeline(output, L);
      end if;
      if instr_count mod 100 = 0 then
        write(L, tag);
        write(L, string'(": executed "));
        write(L, instr_count);
        write(L, string'(" instructions"));
        writeline(output, L);
      end if;
      --
    end loop;
    --
    -- loop is only exited when reset active: wait until it goes inactive
    --
    assert reset = '1'
      report "reset code reached with reset = '0'" severity error;
    wait until phi2 = '0' and reset = '0';
    --
    -- process interpreter now starts again from beginning
    --
  end process interpreter;

end instrumented;
