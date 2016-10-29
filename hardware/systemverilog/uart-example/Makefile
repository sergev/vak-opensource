# Makefile for NCSIM
# By: Michael Auchter, Tom Most and Jon Klein
# Usage: type "make" to compile all source and launch console-mode NCSIM on ${TARGET}
#        type "make gui" to launch the X version of NCSIM
#
# You can export your current session to a "restore.tcl" file to restore it
# the next time you run the simulator.

# This is the file that contains your testbench module
TARGET := uart_tb.sv

# Sources of the design under testing
VSOURCE := uart_top.sv

NCVLOG = ncvlog -sv -logfile ncvlog.log -errormax 15 -update -linedebug #-status
NCELAB = ncelab -logfile ncelab.log -errormax 15 -access +wc #-status
NCSIM  = ncsim -logfile ncsim.log -errormax 15 #-status
NCSIM_GUI = ${NCSIM} -gui -input restore.tcl

default: build elaborate sim

gui:    build elaborate simgui

build:
		${NCVLOG} ${VSOURCE}
		${NCVLOG} ${TARGET}

elaborate:
		${NCELAB} testbench

sim:
		${NCSIM} testbench:module

simgui:
		${NCSIM_GUI} testbench:module

clean:
		rm -f *~ *.log hdl.var ncsim.key cds.lib
		rm -r -f INCA_libs .simvision waves.shm
