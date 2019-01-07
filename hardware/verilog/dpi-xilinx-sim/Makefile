#
# Use Xilinx Vivado Simulator:
# module load vivado/2018.2
#
all:            xsim.dir

clean:
		rm -rf xsim.dir .Xil
		rm -f *.o *.so *.vcd xvhdl.pb xvlog.pb xelab.pb xsc.pb xsim*.jou webtalk*.jou webtalk*.log run_*.log xsc.log

xsim.dir:
		xvlog --nolog --sv test.sv
		xsc -c dpi.c
		xsc --link dpi.c
		xelab --nolog testbench -sv_lib dpi

run:            xsim.dir
		xsim --R --log run.log testbench

view:
		gtkwave testbench.vcd databus.gtkw
