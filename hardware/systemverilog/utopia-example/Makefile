VSRC		= top.sv cpu_ifc.sv
ISRC		= LookupTable.sv squat.sv utopia1_atm_rx.sv utopia1_atm_tx.sv Utopia.sv test.sv
SSRC		= definitions.sv atm_cell.sv environment.sv config.sv generator.sv \
		  driver.sv monitor.sv scoreboard.sv coverage.sv cpu_driver.sv
NCVLOG		= ncvlog -sv -logfile ncvlog.log -errormax 15 -update -linedebug #-status
NCELAB		= ncelab -logfile ncelab.log -errormax 15 -access +wc #-status
NCSIM		= ncsim -logfile ncsim.log -errormax 15 #-status
NCSIM_GUI	= ${NCSIM} -gui -input restore.tcl

default:	build elaborate sim

gui:		build elaborate simgui

build:
#		${NCVLOG} ${ISRC}
		${NCVLOG} LookupTable.sv
		${NCVLOG} squat.sv
		${NCVLOG} utopia1_atm_rx.sv
		${NCVLOG} utopia1_atm_tx.sv
		${NCVLOG} Utopia.sv
		${NCVLOG} CpuInterface.sv
		${NCVLOG} test.sv
		${NCVLOG} ${VSRC}

elaborate:
		${NCELAB} top

sim:
		${NCSIM} top:module

simgui:
		${NCSIM_GUI} top:module

clean:
		rm -f *~ *.log hdl.var ncsim.key cds.lib
		rm -r -f INCA_libs .simvision waves.shm
