DESIGN		= pdp11
SRC		= datapath.v control.v regfile.v alu.v memory.v
PINS		= alu.ucf
DEVICE		= xc3s500e-fg320-5
BGFLAGS		= -g TdoPin:PULLNONE -g DonePin:PULLUP \
		  -g CRC:enable -g StartUpClk:CCLK

all:		test-regfile test-alu test-memory test-datapath

clean:
		rm -f *~ a.out test-* *.log *.key *.edf memory-initial.v

cleanall:	clean
		rm -rf build $(DESIGN).bit

bits:		$(DESIGN).bit

#
#  Simulation
#
test-datapath:	datapath.v control.v regfile.v alu.v memory.v
		@rm -f memory-initial.v
		@cp asm/test1.v memory-initial.v
		iverilog -o $@ -DTEST_DATAPATH datapath.v control.v regfile.v alu.v memory.v

test-regfile:	regfile.v
		iverilog -o $@ -DTEST_REGFILE $<

test-alu:	alu.v
		iverilog -o $@ -DTEST_ALU $<

test-memory:	memory.v
		@rm -f memory-initial.v
		@cp asm/test1.v memory-initial.v
		iverilog -o $@ -DTEST_MEMORY $<

well-regfile:	regfile.v
		veriwell -DTEST_REGFILE $<

#
# Synthesis
#
build/project.src:
		@[ -d build ] || mkdir build
		@rm -f $@
		for i in $(SRC); do echo verilog work ../$$i >> $@; done

build/project.xst: build/project.src
		echo "run" > $@
		echo "-top $(DESIGN)" >> $@
		echo "-p $(DEVICE)" >> $@
		echo "-opt_mode Speed" >> $@
		echo "-opt_level 1" >> $@
		echo "-ifn project.src" >> $@
		echo "-ifmt mixed" >> $@
		echo "-ofn project.ngc" >> $@
		echo "-ofmt NGC" >> $@

build/project.ngc: build/project.xst $(SRC)
		cd build && xst -ifn project.xst -ofn project.log -intstyle silent

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

#
# Experimental Icarus syntesis - not working
#
project.edf:	$(SRC)
		iverilog -S -tfpga -o project.edf $(SRC)

#build/project.ngd: project.edf
#		@[ -d build ] || mkdir build
#		@rm -f $@
#		cd build && ngdbuild -p $(DEVICE) ../project.edf -uc $(PINS)
