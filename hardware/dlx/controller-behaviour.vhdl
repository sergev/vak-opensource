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
--  $RCSfile: controller-behaviour.vhdl,v $  $Revision: 2.1 $  $Date: 1993/11/02 21:36:38 $
--
--------------------------------------------------------------------------
--
--  Behavioural architecture of DLX control section.
--


use work.bv_arithmetic.all, std.textio.all;

architecture behaviour of controller is
  
begin -- behaviour

  sequencer : process

    alias IR_opcode : dlx_opcode is current_instruction(0 to 5);
    alias IR_sp_func : dlx_sp_func is current_instruction(26 to 31);
    alias IR_fp_func : dlx_fp_func is current_instruction(27 to 31);
    alias IR_rs1 : dlx_reg_addr is current_instruction(6 to 10);
    alias IR_rs2 : dlx_reg_addr is current_instruction(11 to 15);
    alias IR_Itype_rd : dlx_reg_addr is current_instruction(11 to 15);
    alias IR_Rtype_rd : dlx_reg_addr is current_instruction(16 to 20);
    alias IR_immed16 : dlx_immed16 is current_instruction(16 to 31);
    alias IR_immed26 : dlx_immed26 is current_instruction(6 to 31);
    
    variable IR_opcode_num : dlx_opcode_num;
    variable IR_sp_func_num : dlx_sp_func_num;
    variable IR_fp_func_num : dlx_fp_func_num;
    
    variable result_of_set_is_1, branch_taken : boolean;
    
    variable L : line;

    procedure bus_instruction_fetch is
    begin
      -- use PC as address
      mem_addr_mux_sel <= '0' after Tpd_clk_ctrl;
      -- set up memory control signals
      width <= width_word after Tpd_clk_ctrl;
      ifetch <= '1' after Tpd_clk_ctrl;
      mem_enable <= '1' after Tpd_clk_ctrl;
      -- wait until phi2, then enable IR input
      wait until phi2 = '1';
      ir_latch_en <= '1' after Tpd_clk_ctrl;
      -- wait until memory is ready at end of phi2
      loop 
        wait until phi2 = '0';
        if reset = '1' then
  	return;
        end if;
        exit when ready = '1';
      end loop;
      -- disable IR input and memory control signals
      ir_latch_en <= '0' after Tpd_clk_ctrl;
      mem_enable <= '0' after Tpd_clk_ctrl;
    end bus_instruction_fetch;
    
    procedure bus_data_read(read_width : in mem_width) is
    begin
      -- use MAR as address
      mem_addr_mux_sel <= '1' after Tpd_clk_ctrl;
      -- set up memory control signals
      width <= read_width after Tpd_clk_ctrl;
      ifetch <= '0' after Tpd_clk_ctrl;
      mem_enable <= '1' after Tpd_clk_ctrl;
      -- wait until phi2, then enable MDR input
      wait until phi2 = '1';
      mdr_mux_sel <= '1' after Tpd_clk_ctrl;
      mdr_latch_en <= '1' after Tpd_clk_ctrl;
      -- wait until memory is ready at end of phi2
      loop 
        wait until phi2 = '0';
        if reset = '1' then
  	return;
        end if;
        exit when ready = '1';
      end loop;
      -- disable MDR input and memory control signals
      mdr_latch_en <= '0' after Tpd_clk_ctrl;
      mem_enable <= '0' after Tpd_clk_ctrl;
    end bus_data_read;
    
    procedure bus_data_write(write_width : in mem_width) is
    begin
      -- use MAR as address
      mem_addr_mux_sel <= '1' after Tpd_clk_ctrl;
      -- enable MDR output
      mdr_out_en3 <= '1' after Tpd_clk_ctrl;
      -- set up memory control signals
      width <= write_width after Tpd_clk_ctrl;
      ifetch <= '0' after Tpd_clk_ctrl;
      write_enable <= '1' after Tpd_clk_ctrl;
      mem_enable <= '1' after Tpd_clk_ctrl;
      -- wait until memory is ready at end of phi2
      loop 
        wait until phi2 = '0';
        if reset = '1' then
  	return;
        end if;
        exit when ready = '1';
      end loop;
      -- disable MDR output and memory control signals
      write_enable <= '0' after Tpd_clk_ctrl;
      mem_enable <= '0' after Tpd_clk_ctrl;
      mdr_out_en3 <= '0' after Tpd_clk_ctrl;
    end bus_data_write;
  
    procedure do_set_result is
    begin
      wait until phi1 = '1';
      if result_of_set_is_1 then
	const2 <= X"0000_0001" after Tpd_clk_const;
      else
	const2 <= X"0000_0000" after Tpd_clk_const;
      end if;
      alu_latch_en <= '1' after Tpd_clk_ctrl;
      alu_function <= alu_pass_s2 after Tpd_clk_ctrl;
      --
      wait until phi1 = '0';
      alu_latch_en <= '0' after Tpd_clk_ctrl;
      const2 <= null after Tpd_clk_const;
      --
      wait until phi2 = '1';
      c_latch_en <= '1' after Tpd_clk_ctrl;
      --
      wait until phi2 = '0';
      c_latch_en <= '0' after Tpd_clk_ctrl;
    end do_set_result;    

    procedure do_EX_set_unsigned(immed : boolean) is
    begin
      wait until phi1 = '1';
      a_out_en <= '1' after Tpd_clk_ctrl;
      if immed then
        ir_immed_sel2 <= immed_size_16 after Tpd_clk_ctrl;
	ir_immed_unsigned2 <= '1' after Tpd_clk_ctrl;
	ir_immed_en2 <= '1' after Tpd_clk_ctrl;
      else
        b_out_en <= '1' after Tpd_clk_ctrl;
      end if;
      alu_latch_en <= '1' after Tpd_clk_ctrl;
      alu_function <= alu_subu after Tpd_clk_ctrl;
      --
      wait until phi1 = '0';
      alu_latch_en <= '0' after Tpd_clk_ctrl;
      a_out_en <= '0' after Tpd_clk_ctrl;
      if immed then
	ir_immed_en2 <= '0' after Tpd_clk_ctrl;
      else
        b_out_en <= '0' after Tpd_clk_ctrl;
      end if;
      --
      wait until phi2 = '0';
      if immed then
        case IR_opcode is
          when op_sequi => 
	    result_of_set_is_1 := alu_zero = '1';
     	  when op_sneui =>
            result_of_set_is_1 := alu_zero /= '1';
          when op_sltui =>
	    result_of_set_is_1 := alu_overflow = '1';
    	  when op_sgtui => 
      	    result_of_set_is_1 := alu_overflow /= '1' and alu_zero /= '1';
    	  when op_sleui => 
      	    result_of_set_is_1 := alu_overflow = '1' or alu_zero = '1';
    	  when op_sgeui => 
      	    result_of_set_is_1 := alu_overflow /= '1';
	  when others =>
	    null;
        end case;
      else
        case IR_sp_func is
          when sp_func_sequ => 
	    result_of_set_is_1 := alu_zero = '1';
     	  when sp_func_sneu =>
            result_of_set_is_1 := alu_zero /= '1';
          when sp_func_sltu =>
	    result_of_set_is_1 := alu_overflow = '1';
    	  when sp_func_sgtu => 
      	    result_of_set_is_1 := alu_overflow /= '1' and alu_zero /= '1';
    	  when sp_func_sleu => 
      	    result_of_set_is_1 := alu_overflow = '1' or alu_zero = '1';
    	  when sp_func_sgeu => 
      	    result_of_set_is_1 := alu_overflow /= '1';
	  when others =>
	    null;
        end case;
      end if;
      --
      do_set_result;
    end do_EX_set_unsigned;

    procedure do_EX_set_signed(immed : boolean) is
    begin
      wait until phi1 = '1';
      a_out_en <= '1' after Tpd_clk_ctrl;
      if immed then
        ir_immed_sel2 <= immed_size_16 after Tpd_clk_ctrl;
	ir_immed_unsigned2 <= '0' after Tpd_clk_ctrl;
	ir_immed_en2 <= '1' after Tpd_clk_ctrl;
      else
        b_out_en <= '1' after Tpd_clk_ctrl;
      end if;
      alu_latch_en <= '1' after Tpd_clk_ctrl;
      alu_function <= alu_sub after Tpd_clk_ctrl;
      --
      wait until phi1 = '0';
      alu_latch_en <= '0' after Tpd_clk_ctrl;
      a_out_en <= '0' after Tpd_clk_ctrl;
      if immed then
	ir_immed_en2 <= '0' after Tpd_clk_ctrl;
      else
        b_out_en <= '0' after Tpd_clk_ctrl;
      end if;
      --
      wait until phi2 = '0';
      if immed then
        case IR_opcode is
          when op_seqi => 
	    result_of_set_is_1 := alu_zero = '1';
     	  when op_snei =>
            result_of_set_is_1 := alu_zero /= '1';
          when op_slti =>
	    result_of_set_is_1 := alu_negative = '1';
    	  when op_sgti => 
      	    result_of_set_is_1 := alu_negative /= '1' and alu_zero /= '1';
    	  when op_slei => 
      	    result_of_set_is_1 := alu_negative = '1' or alu_zero = '1';
    	  when op_sgei => 
      	    result_of_set_is_1 := alu_negative /= '1';
	  when others =>
	    null;
        end case;
      else
        case IR_sp_func is
          when sp_func_seq => 
	    result_of_set_is_1 := alu_zero = '1';
     	  when sp_func_sne =>
            result_of_set_is_1 := alu_zero /= '1';
          when sp_func_slt =>
	    result_of_set_is_1 := alu_negative = '1';
    	  when sp_func_sgt => 
      	    result_of_set_is_1 := alu_negative /= '1' and alu_zero /= '1';
    	  when sp_func_sle => 
      	    result_of_set_is_1 := alu_negative = '1' or alu_zero = '1';
    	  when sp_func_sge => 
      	    result_of_set_is_1 := alu_negative /= '1';
	  when others =>
	    null;
        end case;
      end if;
      --
      do_set_result;
    end do_EX_set_signed;

    procedure do_EX_arith_logic is
    begin
      wait until phi1 = '1';
      a_out_en <= '1' after Tpd_clk_ctrl;
      b_out_en <= '1' after Tpd_clk_ctrl;
      alu_latch_en <= '1' after Tpd_clk_ctrl;
      case IR_sp_func is
    	when sp_func_add =>
          alu_function <= alu_add after Tpd_clk_ctrl;
        when sp_func_addu =>
          alu_function <= alu_addu after Tpd_clk_ctrl;
        when sp_func_sub =>
          alu_function <= alu_sub after Tpd_clk_ctrl;
        when sp_func_subu =>
          alu_function <= alu_subu after Tpd_clk_ctrl;
        when sp_func_and =>
          alu_function <= alu_and after Tpd_clk_ctrl;
        when sp_func_or =>
          alu_function <= alu_or after Tpd_clk_ctrl;
        when sp_func_xor =>
          alu_function <= alu_xor after Tpd_clk_ctrl;
        when sp_func_sll =>
          alu_function <= alu_sll after Tpd_clk_ctrl;
        when sp_func_srl =>
          alu_function <= alu_srl after Tpd_clk_ctrl;
        when sp_func_sra =>
          alu_function <= alu_sra after Tpd_clk_ctrl;
        when others =>
          null;
      end case;                  --  IR_sp_func
      --
      wait until phi1 = '0';
      alu_latch_en <= '0' after Tpd_clk_ctrl;
      a_out_en <= '0' after Tpd_clk_ctrl;
      b_out_en <= '0' after Tpd_clk_ctrl;
      --
      wait until phi2 = '1';
      c_latch_en <= '1' after Tpd_clk_ctrl;
      --
      wait until phi2 = '0';
      c_latch_en <= '0' after Tpd_clk_ctrl;
    end do_EX_arith_logic;

    procedure do_EX_arith_logic_immed is
    begin
      wait until phi1 = '1';
      a_out_en <= '1' after Tpd_clk_ctrl;
      ir_immed_sel2 <= immed_size_16 after Tpd_clk_ctrl;
      if IR_opcode = op_addi or IR_opcode = op_subi then
        ir_immed_unsigned2 <= '0' after Tpd_clk_ctrl;
      else
	ir_immed_unsigned2 <= '1' after Tpd_clk_ctrl;
      end if;
      ir_immed_en2 <= '1' after Tpd_clk_ctrl;
      alu_latch_en <= '1' after Tpd_clk_ctrl;
      case IR_opcode is
    	when op_addi =>
      	  alu_function <= alu_add after Tpd_clk_ctrl;
        when op_subi =>
          alu_function <= alu_sub after Tpd_clk_ctrl;
    	when op_addui =>
      	  alu_function <= alu_addu after Tpd_clk_ctrl;
        when op_subui =>
          alu_function <= alu_subu after Tpd_clk_ctrl;
        when op_andi =>
          alu_function <= alu_and after Tpd_clk_ctrl;
        when op_ori =>
          alu_function <= alu_or after Tpd_clk_ctrl;
        when op_xori =>
          alu_function <= alu_xor after Tpd_clk_ctrl;
        when op_slli =>
          alu_function <= alu_sll after Tpd_clk_ctrl;
        when op_srli =>
          alu_function <= alu_srl after Tpd_clk_ctrl;
        when op_srai =>
          alu_function <= alu_sra after Tpd_clk_ctrl;
        when others =>
          null;
      end case;                      --  IR_opcode
      --
      wait until phi1 = '0';
      alu_latch_en <= '0' after Tpd_clk_ctrl;
      a_out_en <= '0' after Tpd_clk_ctrl;
      ir_immed_en2 <= '0' after Tpd_clk_ctrl;
      --
      wait until phi2 = '1';
      c_latch_en <= '1' after Tpd_clk_ctrl;
      --
      wait until phi2 = '0';
      c_latch_en <= '0' after Tpd_clk_ctrl;
    end do_EX_arith_logic_immed;
    
    procedure do_EX_link is
    begin
      wait until phi1 = '1';
      pc_out_en1 <= '1' after Tpd_clk_ctrl;
      alu_latch_en <= '1' after Tpd_clk_ctrl;
      alu_function <= alu_pass_s1 after Tpd_clk_ctrl;
      --
      wait until phi1 = '0';
      alu_latch_en <= '0' after Tpd_clk_ctrl;
      pc_out_en1 <= '0' after Tpd_clk_ctrl;
      --
      wait until phi2 = '1';
      c_latch_en <= '1' after Tpd_clk_ctrl;
      --
      wait until phi2 = '0';
      c_latch_en <= '0' after Tpd_clk_ctrl;
    end do_EX_link;

    procedure do_EX_lhi is
    begin
      wait until phi1 = '1';
      ir_immed_sel1 <= immed_size_16 after Tpd_clk_ctrl;
      ir_immed_unsigned1 <= '1' after Tpd_clk_ctrl;
      ir_immed_en1 <= '1' after Tpd_clk_ctrl;
      const2 <= X"0000_0010" after Tpd_clk_const;	  -- shift by 16 bits
      alu_latch_en <= '1' after Tpd_clk_ctrl;
      alu_function <= alu_sll after Tpd_clk_ctrl;
      --
      wait until phi1 = '0';
      alu_latch_en <= '0' after Tpd_clk_ctrl;
      ir_immed_en1 <= '0' after Tpd_clk_ctrl;
      const2 <= null after Tpd_clk_const;
      --
      wait until phi2 = '1';
      c_latch_en <= '1' after Tpd_clk_ctrl;
      --
      wait until phi2 = '0';
      c_latch_en <= '0' after Tpd_clk_ctrl;
    end do_EX_lhi;    

    procedure do_EX_branch is
    begin
      wait until phi1 = '1';
      a_out_en <= '1' after Tpd_clk_ctrl;
      const2 <= X"0000_0000" after Tpd_clk_const;
      alu_latch_en <= '1' after Tpd_clk_ctrl;
      alu_function <= alu_sub after Tpd_clk_ctrl;
      --
      wait until phi1 = '0';
      alu_latch_en <= '0' after Tpd_clk_ctrl;
      a_out_en <= '0' after Tpd_clk_ctrl;
      const2 <= null after Tpd_clk_const;
      --
      wait until phi2 = '0';
      if IR_opcode = op_beqz then
	branch_taken := alu_zero = '1';
      else
	branch_taken := alu_zero /= '1';
      end if;
    end do_EX_branch;

    procedure do_EX_load_store is
    begin
      wait until phi1 = '1';
      a_out_en <= '1' after Tpd_clk_ctrl;
      ir_immed_sel2 <= immed_size_16 after Tpd_clk_ctrl;
      ir_immed_unsigned2 <= '0' after Tpd_clk_ctrl;
      ir_immed_en2 <= '1' after Tpd_clk_ctrl;
      alu_function <= alu_add after Tpd_clk_ctrl;
      alu_latch_en <= '1' after Tpd_clk_ctrl;
      --
      wait until phi1 = '0';
      alu_latch_en <= '0' after Tpd_clk_ctrl;
      a_out_en <= '0' after Tpd_clk_ctrl;
      ir_immed_en2 <= '0' after Tpd_clk_ctrl;
      --
      wait until phi2 = '1';
      mar_latch_en <= '1' after Tpd_clk_ctrl;
      --
      wait until phi2 = '0';
      mar_latch_en <= '0' after Tpd_clk_ctrl;
    end do_EX_load_store;
    
    procedure do_MEM_jump is
    begin
      wait until phi1 = '1';
      pc_out_en1 <= '1' after Tpd_clk_ctrl;
      ir_immed_sel2 <= immed_size_26 after Tpd_clk_ctrl;
      ir_immed_unsigned2 <= '0' after Tpd_clk_ctrl;
      ir_immed_en2 <= '1' after Tpd_clk_ctrl;
      alu_latch_en <= '1' after Tpd_clk_ctrl;
      alu_function <= alu_add after Tpd_clk_ctrl;
      --
      wait until phi1 = '0';
      alu_latch_en <= '0' after Tpd_clk_ctrl;
      pc_out_en1 <= '0' after Tpd_clk_ctrl;
      ir_immed_en2 <= '0' after Tpd_clk_ctrl;
      --
      wait until phi2 = '1';
      pc_latch_en <= '1' after Tpd_clk_ctrl;
      --
      wait until phi2 = '0';
      pc_latch_en <= '0' after Tpd_clk_ctrl;
    end do_MEM_jump;

    procedure do_MEM_jump_reg is
    begin
      wait until phi1 = '1';
      a_out_en <= '1' after Tpd_clk_ctrl;
      alu_latch_en <= '1' after Tpd_clk_ctrl;
      alu_function <= alu_pass_s1 after Tpd_clk_ctrl;
      --
      wait until phi1 = '0';
      alu_latch_en <= '0' after Tpd_clk_ctrl;
      a_out_en <= '0' after Tpd_clk_ctrl;
      --
      wait until phi2 = '1';
      pc_latch_en <= '1' after Tpd_clk_ctrl;
      --
      wait until phi2 = '0';
      pc_latch_en <= '0' after Tpd_clk_ctrl;
    end do_MEM_jump_reg;

    procedure do_MEM_branch is
    begin
      wait until phi1 = '1';
      pc_out_en1 <= '1' after Tpd_clk_ctrl;
      ir_immed_sel2 <= immed_size_16 after Tpd_clk_ctrl;
      ir_immed_unsigned2 <= '0' after Tpd_clk_ctrl;
      ir_immed_en2 <= '1' after Tpd_clk_ctrl;
      alu_latch_en <= '1' after Tpd_clk_ctrl;
      alu_function <= alu_add after Tpd_clk_ctrl;
      --
      wait until phi1 = '0';
      alu_latch_en <= '0' after Tpd_clk_ctrl;
      pc_out_en1 <= '0' after Tpd_clk_ctrl;
      ir_immed_en2 <= '0' after Tpd_clk_ctrl;
      --
      wait until phi2 = '1';
      pc_latch_en <= '1' after Tpd_clk_ctrl;
      --
      wait until phi2 = '0';
      pc_latch_en <= '0' after Tpd_clk_ctrl;
    end do_MEM_branch;

    procedure do_MEM_load is
    begin
      wait until phi1 = '1';
      bus_data_read(width_word);
      if reset = '1' then
        return;
      end if;
      --
      wait until phi1 = '1';
      mdr_out_en1 <= '1' after Tpd_clk_ctrl;
      alu_function <= alu_pass_s1 after Tpd_clk_ctrl;
      alu_latch_en <= '1' after Tpd_clk_ctrl;
      --
      wait until phi1 = '0';
      mdr_out_en1 <= '0' after Tpd_clk_ctrl;
      alu_latch_en <= '0' after Tpd_clk_ctrl;
      --
      wait until phi2 = '1';
      c_latch_en <= '1' after Tpd_clk_ctrl;
      --
      wait until phi2 = '0';
      c_latch_en <= '0' after Tpd_clk_ctrl;
    end do_MEM_load;    

    procedure do_MEM_store is
    begin
      wait until phi1 = '1';
      b_out_en <= '1' after Tpd_clk_ctrl;
      alu_function <= alu_pass_s2 after Tpd_clk_ctrl;
      alu_latch_en <= '1' after Tpd_clk_ctrl;
      --
      wait until phi1 = '0';
      b_out_en <= '0' after Tpd_clk_ctrl;
      alu_latch_en <= '0' after Tpd_clk_ctrl;
      --
      wait until phi2 = '1';
      mdr_mux_sel <= '0' after Tpd_clk_ctrl;
      mdr_latch_en <= '1' after Tpd_clk_ctrl;
      --
      wait until phi2 = '0';
      mdr_latch_en <= '0' after Tpd_clk_ctrl;
      --
      wait until phi1 = '1';
      bus_data_write(width_word);
    end do_MEM_store;
    
    procedure do_WB(Rd : dlx_reg_addr) is
    begin
      wait until phi1 = '1';
      reg_dest_addr <= Rd after Tpd_clk_ctrl;
      reg_write <= '1' after Tpd_clk_ctrl;
      --
      wait until phi2 = '0';
      reg_write <= '0' after Tpd_clk_ctrl;
    end do_WB;
    

  begin -- sequencer
    --
    ----------------------------------------------------------------
    -- initialize all control signals
    ----------------------------------------------------------------
    if debug then
      write(L, string'("controller: initializing"));
      writeline(output, L);
    end if;
    --
    halt <= '0' after Tpd_clk_ctrl;
    width <= width_word after Tpd_clk_ctrl;
    write_enable <= '0' after Tpd_clk_ctrl;
    mem_enable <= '0' after Tpd_clk_ctrl;
    ifetch <= '0' after Tpd_clk_ctrl;
    alu_latch_en <= '0' after Tpd_clk_ctrl;
    alu_function <= alu_add after Tpd_clk_ctrl;
    reg_s1_addr <= B"00000" after Tpd_clk_ctrl;
    reg_s2_addr <= B"00000" after Tpd_clk_ctrl;
    reg_dest_addr <= B"00000" after Tpd_clk_ctrl;
    reg_write <= '0' after Tpd_clk_ctrl;
    c_latch_en <= '0' after Tpd_clk_ctrl;
    a_latch_en <= '0' after Tpd_clk_ctrl;
    a_out_en <= '0' after Tpd_clk_ctrl;
    b_latch_en <= '0' after Tpd_clk_ctrl;
    b_out_en <= '0' after Tpd_clk_ctrl;
    temp_latch_en <= '0' after Tpd_clk_ctrl;
    temp_out_en1 <= '0' after Tpd_clk_ctrl;
    temp_out_en2 <= '0' after Tpd_clk_ctrl;
    iar_latch_en <= '0' after Tpd_clk_ctrl;
    iar_out_en1 <= '0' after Tpd_clk_ctrl;
    iar_out_en2 <= '0' after Tpd_clk_ctrl;
    pc_latch_en <= '0' after Tpd_clk_ctrl;
    pc_out_en1 <= '0' after Tpd_clk_ctrl;
    pc_out_en2 <= '0' after Tpd_clk_ctrl;
    mar_latch_en <= '0' after Tpd_clk_ctrl;
    mar_out_en1 <= '0' after Tpd_clk_ctrl;
    mar_out_en2 <= '0' after Tpd_clk_ctrl;
    mem_addr_mux_sel <= '0' after Tpd_clk_ctrl;
    mdr_latch_en <= '0' after Tpd_clk_ctrl;
    mdr_out_en1 <= '0' after Tpd_clk_ctrl;
    mdr_out_en2 <= '0' after Tpd_clk_ctrl;
    mdr_out_en3 <= '0' after Tpd_clk_ctrl;
    mdr_mux_sel <= '0' after Tpd_clk_ctrl;
    ir_latch_en <= '0' after Tpd_clk_ctrl;
    ir_immed_sel1 <= immed_size_16 after Tpd_clk_ctrl;
    ir_immed_sel2 <= immed_size_16 after Tpd_clk_ctrl;
    ir_immed_unsigned1 <= '0' after Tpd_clk_ctrl;
    ir_immed_unsigned2 <= '0' after Tpd_clk_ctrl;
    ir_immed_en1 <= '0' after Tpd_clk_ctrl;
    ir_immed_en2 <= '0' after Tpd_clk_ctrl;
    const1 <= null after Tpd_clk_const;
    const2 <= null after Tpd_clk_const;
    --
    wait until phi2 = '0' and reset = '0';
    --
    ----------------------------------------------------------------
    -- control loop
    ----------------------------------------------------------------
    loop
      --  
      ----------------------------------------------------------------
      -- fetch next instruction (IF)
      ----------------------------------------------------------------
      wait until phi1 = '1';
      if debug then
        write(L, string'("controller: instruction fetch"));
        writeline(output, L);
      end if;
      --
      bus_instruction_fetch;
      --
      ----------------------------------------------------------------
      -- instruction decode, source register read, and PC increment (ID)
      ----------------------------------------------------------------
      wait until phi1 = '1';
      if debug then
        write(L, string'("controller: decode, reg-read and PC incr"));
        writeline(output, L);
      end if;
      --
      IR_opcode_num := bv_to_natural(IR_opcode);
      IR_sp_func_num := bv_to_natural(IR_sp_func);
      IR_fp_func_num := bv_to_natural(IR_fp_func);
      --
      reg_s1_addr <= IR_rs1 after Tpd_clk_ctrl;
      reg_s2_addr <= IR_rs2 after Tpd_clk_ctrl;
      a_latch_en <= '1' after Tpd_clk_ctrl;
      b_latch_en <= '1' after Tpd_clk_ctrl;
      --
      pc_out_en1 <= '1' after Tpd_clk_ctrl;
      const2 <= X"0000_0004" after Tpd_clk_const;
      alu_latch_en <= '1' after Tpd_clk_ctrl;
      alu_function <= alu_addu after Tpd_clk_ctrl;
      --
      wait until phi1 = '0';
      a_latch_en <= '0' after Tpd_clk_ctrl;
      b_latch_en <= '0' after Tpd_clk_ctrl;
      alu_latch_en <= '0' after Tpd_clk_ctrl;
      pc_out_en1 <= '0' after Tpd_clk_ctrl;
      const2 <= null after Tpd_clk_const;
      --
      wait until phi2 = '1';
      pc_latch_en <= '1' after Tpd_clk_ctrl;
      --
      wait until phi2 = '0';
      pc_latch_en <= '0' after Tpd_clk_ctrl;
      --
      ----------------------------------------------------------------
      -- execute instruction, (EX, MEM, WB)
      ----------------------------------------------------------------
      if debug then
        write(L, string'("controller: execute"));
        writeline(output, L);
      end if;
      --
      case IR_opcode is
    	when op_special => 
      	  case IR_sp_func is
            when sp_func_nop =>
              null;
    	    when sp_func_sequ | sp_func_sneu | 
                 sp_func_sltu | sp_func_sgtu |
                 sp_func_sleu | sp_func_sgeu => 
              do_EX_set_unsigned(immed => false);
              do_WB(IR_Rtype_rd);
    	    when sp_func_add | sp_func_addu |
	         sp_func_sub | sp_func_subu |
    	         sp_func_and | sp_func_or | sp_func_xor |
    	         sp_func_sll | sp_func_srl | sp_func_sra =>
              do_EX_arith_logic;
              do_WB(IR_Rtype_rd);
    	    when sp_func_seq | sp_func_sne | 
                 sp_func_slt | sp_func_sgt |
                 sp_func_sle | sp_func_sge => 
              do_EX_set_signed(immed => false);
              do_WB(IR_Rtype_rd);
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
      	      assert false
	      	report "MOVFP2I instruction not implemented" severity warning;
    	    when sp_func_movi2fp => 
      	      assert false
	      	report "MOVI2FP instruction not implemented" severity warning;
      	    when others =>
	      assert false
	      	report "undefined special instruction function" severity error;
    	  end case;  
    	when op_fparith => 
      	  case IR_fp_func is
    	    when fp_func_addf | fp_func_subf | fp_func_multf | fp_func_divf | 
    	         fp_func_addd | fp_func_subd | fp_func_multd | fp_func_divd | 
                 fp_func_mult | fp_func_multu | fp_func_div | fp_func_divu |
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
        when op_j =>
          do_MEM_jump;
        when op_jr =>
          do_MEM_jump_reg;
        when op_jal =>
          do_EX_link;
          do_MEM_jump;
          do_WB(natural_to_bv(link_reg, 5));
        when op_jalr =>
          do_EX_link;
          do_MEM_jump_reg;
          do_WB(natural_to_bv(link_reg, 5));
    	when op_beqz | op_bnez => 
          do_EX_branch;
	  if branch_taken then
            do_MEM_branch;
          end if;
    	when op_bfpt => 
      	  assert false
	    report "BFPT instruction not implemented" severity warning;
    	when op_bfpf => 
      	  assert false
	    report "BFPF instruction not implemented" severity warning;
    	when op_addi | op_subi | 
     	     op_addui | op_subui | 
     	     op_andi | op_ori | op_xori |
             op_slli | op_srli | op_srai => 
          do_EX_arith_logic_immed;
          do_WB(IR_Itype_rd);
    	when op_lhi =>
          do_EX_lhi;
          do_WB(IR_Itype_rd);
    	when op_rfe => 
      	  assert false
	    report "RFE instruction not implemented" severity warning;
    	when op_trap =>
      	  assert false
	    report "TRAP instruction encountered, execution halted"
	    severity note;
          wait until phi1 = '1';
      	  halt <= '1' after Tpd_clk_ctrl;
	  wait until reset = '1';
	  exit;
    	when op_seqi | op_snei | op_slti |
     	     op_sgti | op_slei | op_sgei => 
          do_EX_set_signed(immed => true);
          do_WB(IR_Itype_rd);
        when op_lb =>
	  assert false
	    report "LB instruction not implemented" severity warning;
        when op_lh =>
	  assert false
	    report "LH instruction not implemented" severity warning;
        when op_lw => 
          do_EX_load_store;
          do_MEM_load;
          exit when reset = '1';
          do_WB(IR_Itype_rd);
        when op_sw =>
          do_EX_load_store;
          do_MEM_store;
          exit when reset = '1';
        when op_lbu =>
	  assert false
	    report "LBU instruction not implemented" severity warning;
        when op_lhu =>
	  assert false
	    report "LHU instruction not implemented" severity warning;
        when op_sb => 
	  assert false
	    report "SB instruction not implemented" severity warning;
        when op_sh =>
	  assert false
	    report "SH instruction not implemented" severity warning;
        when op_lf => 
	  assert false
	    report "LF instruction not implemented" severity warning;
    	when op_ld => 
	  assert false
	    report "LD instruction not implemented" severity warning;
        when op_sf => 
	  assert false
	    report "SF instruction not implemented" severity warning;
    	when op_sd => 
	  assert false
	    report "SD instruction not implemented" severity warning;
    	when op_sequi | op_sneui | op_sltui |
     	     op_sgtui | op_sleui | op_sgeui => 
          do_EX_set_unsigned(immed => true);
          do_WB(IR_Itype_rd);
      	when others =>
	  assert false
	    report "undefined instruction" severity error;
      end case;
      --
    end loop;
    --
    ----------------------------------------------------------------
    -- loop exited on reset
    ----------------------------------------------------------------
    assert reset = '1'
  	      	 report "Internal error: reset code reached with reset = '0'"
	   severity failure;
    --
    -- start again
    --
  end process sequencer;


end behaviour;
