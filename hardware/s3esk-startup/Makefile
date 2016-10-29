DESIGN		= startup
SRC		= startup.vhd control.vhd kcpsm3.vhd
PINS		= startup.ucf
DEVICE		= xc3s500e-fg320-5
BGFLAGS		= -g TdoPin:PULLNONE -g DonePin:PULLUP \
		  -g CRC:enable -g StartUpClk:CCLK

all:		bits

clean:
		rm -f *~ a.out *.log *.key *.edf

cleanall:	clean
		rm -rf build $(DESIGN).bit

bits:		$(DESIGN).bit

#
# Synthesis
#
build/project.src:
		@[ -d build ] || mkdir build
		@rm -f $@
		for i in $(SRC); do echo vhdl work ../$$i >> $@; done

build/project.xst: build/project.src
		echo "run" > $@
		echo "-top s3esk_$(DESIGN)" >> $@
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

#build/project.ngd: project.edf
#		@[ -d build ] || mkdir build
#		@rm -f $@
#		cd build && ngdbuild -p $(DEVICE) ../project.edf -uc $(PINS)
