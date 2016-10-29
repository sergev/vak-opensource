# Makefile for synthesis of design 's3esk_mini_top'.
# Revision D of the Spartan-3E Starter Kit.
# Xilinx ISE Webpack must be installed in the path.
# Serge Vakulenko - December 2006.

DESIGN		= or1k-mini
PINS		= s3esk_mini_top.ucf
DEVICE		= xc3s500e-fg320-4
BGFLAGS		= -g TdoPin:PULLNONE -g DonePin:PULLUP \
		  -g CRC:enable -g StartUpClk:CCLK

SRC		= s3esk_mini_top.v tc_top.v

# Debug interface
SRC		+= dbg_interface/dbg_top.v dbg_interface/dbg_sync_clk1_clk2.v \
		   dbg_interface/dbg_registers.v dbg_interface/dbg_crc8_d1.v \
		   dbg_interface/dbg_register.v

# Processor
SRC		+= or1200/or1200_top.v or1200/or1200_iwb_biu.v \
		   or1200/or1200_wb_biu.v or1200/or1200_immu_top.v \
		   or1200/or1200_ic_top.v or1200/or1200_cpu.v \
		   or1200/or1200_dmmu_top.v or1200/or1200_dc_top.v \
		   or1200/or1200_qmem_top.v or1200/or1200_sb.v \
		   or1200/or1200_du.v or1200/or1200_pic.v \
		   or1200/or1200_tt.v or1200/or1200_pm.v \
		   or1200/or1200_ic_fsm.v or1200/or1200_ic_ram.v \
		   or1200/or1200_ic_tag.v or1200/or1200_genpc.v \
		   or1200/or1200_if.v or1200/or1200_ctrl.v \
		   or1200/or1200_rf.v or1200/or1200_operandmuxes.v \
		   or1200/or1200_alu.v or1200/or1200_mult_mac.v \
		   or1200/or1200_sprs.v or1200/or1200_lsu.v \
		   or1200/or1200_wbmux.v or1200/or1200_freeze.v \
		   or1200/or1200_except.v or1200/or1200_cfgr.v \
		   or1200/or1200_dc_fsm.v or1200/or1200_dc_ram.v \
		   or1200/or1200_dc_tag.v or1200/or1200_spram_2048x32.v \
		   or1200/or1200_tpram_32x32.v or1200/or1200_gmultp2_32x32.v \
		   or1200/or1200_mem2reg.v or1200/or1200_reg2mem.v

# When cache and MMU enabled:
#SRC		+= or1200/or1200_immu_tlb.v or1200/or1200_spram_1024x32.v \
#		   or1200/or1200_spram_256x21.v or1200/or1200_dmmu_tlb.v \
#		   or1200/or1200_spram_1024x32_bw.v or1200/or1200_spram_64x14.v \
#		   or1200/or1200_spram_64x22.v or1200/or1200_spram_64x24.v

# Memory
SRC		+= mem_if/onchip_ram_top.v

# UART
SRC		+= uart16550/uart_top.v uart16550/uart_wb.v \
		   uart16550/uart_regs.v uart16550/uart_debug_if.v \
		   uart16550/uart_transmitter.v uart16550/uart_sync_flops.v \
		   uart16550/uart_receiver.v uart16550/uart_tfifo.v \
		   uart16550/uart_rfifo.v uart16550/raminfr.v

all:		bits

remake:		clean-build all

clean:
		rm -f *~ */*~ a.out *.log *.key *.edf *.ps trace.dat

clean-build:
		rm -rf build

cleanall:	clean
		rm -rf build $(DESIGN).bit

bits:		$(DESIGN).bit

#
# Synthesis
#
build/project.src:
		@[ -d build ] || mkdir build
		@rm -f $@
		for i in $(SRC); do echo verilog work ../$$i >> $@; done

build/project.xst: build/project.src
		echo "run" > $@
		echo "-top s3esk_fpga_top" >> $@
		echo "-p $(DEVICE)" >> $@
		echo "-opt_mode Speed" >> $@
		echo "-opt_level 1" >> $@
		echo "-ifn project.src" >> $@
		echo "-ifmt mixed" >> $@
		echo "-ofn project.ngc" >> $@
		echo "-ofmt NGC" >> $@

build/project.ngc: build/project.xst $(SRC)
		cd build && xst -ifn project.xst -ofn project.log

build/project.ngd: build/project.ngc $(PINS)
		cd build && ngdbuild -p $(DEVICE) project.ngc -uc ../$(PINS)

build/project.ncd: build/project.ngd
		cd build && map -pr b -p $(DEVICE) project

build/project_r.ncd: build/project.ncd
		cd build && par -w project project_r.ncd

build/project_r.twr: build/project_r.ncd
		cd build && trce -v 25 project_r.ncd project.pcf

$(DESIGN).bit:	build/project_r.ncd build/project_r.twr
		cd build && bitgen project_r.ncd -l -w $(BGFLAGS)
		@mv -f build/project_r.bit $@
