#
# Use: module load modelsim/18.1
#
# To make it work properly, remove all gcc-*-linux directories
# from /opt/Altera/18.1/modelsim_ase/.
#
SOURCES         = test.sv dpi.c

all:            work

clean:
		rm -rf *.o *.vcd work

work:           $(SOURCES)
		vlib work
		vlog -sv $(SOURCES)

run:            work
		vsim -c +nowarn3116 -l run.log -do 'run 100000us; quit' testbench
