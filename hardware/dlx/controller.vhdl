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
--  $RCSfile: controller.vhdl,v $  $Revision: 2.1 $  $Date: 1993/11/02 19:19:52 $
--
--------------------------------------------------------------------------
--
--  Entity declaration for DLX control section.
--


use work.dlx_types.all,
    work.dlx_instr.all,
    work.alu_types.all,
    work.mem_types.all;

entity controller is
  generic (Tpd_clk_ctrl, Tpd_clk_const : Time;
           debug : boolean := false;
           tag : string := "";
           origin_x, origin_y : real := 0.0);
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
end controller;
