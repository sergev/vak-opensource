# Build script for Vivado.

set ODIR .
set_param messaging.defaultLimit 10000
set_param place.sliceLegEffortLimit 2000

# STEP#1: setup design sources and constraints
# ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
read_verilog \
    ../top.v

read_xdc ../basys3-pins.xdc

set_property PART "xc7a35tcpg236-1" [current_project]
set_property TARGET_LANGUAGE "Verilog" [current_project]

# STEP#2: run synthesis, write checkpoint design
# ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
synth_design -top "top" -flatten rebuilt
write_checkpoint -force $ODIR/post_synth
write_verilog -force -quiet -mode timesim -sdf_anno true post_synth.v
write_sdf -force -quiet post_synth.sdf

# STEP#3: run placement and logic optimzation, write checkpoint design
# ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
opt_design -propconst -sweep -retarget -remap

write_checkpoint -force $ODIR/post_opt
write_verilog -force -quiet -mode timesim -sdf_anno true post_opt.v
write_sdf -force -quiet post_opt.sdf

if { [file exists $ODIR/post_route.dcp] == 1 } {
    read_checkpoint -incremental $ODIR/post_route.dcp
}

# place_design -directive Quick
# place_design -directive RuntimeOptimized
# place_design -directive Explore
place_design -directive ExtraNetDelay_high
# place_design -directive SpreadLogic_high

phys_opt_design -critical_cell_opt -critical_pin_opt -placement_opt -hold_fix -rewire -retime
power_opt_design
write_checkpoint -force $ODIR/post_place
write_verilog -force -quiet -mode timesim -sdf_anno true post_place.v
write_sdf -force -quiet post_place.sdf

# STEP#4: run router, write checkpoint design
# ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
# route_design
# route_design -directive Quick
# route_design -directive Explore
# route_design -directive RuntimeOptimized
# route_design -directive NoTimingRelaxation -free_resource_mode
route_design -directive HigherDelayCost
# route_design -directive HigherDelayCost -free_resource_mode
# route_design -directive AdvancedSkewModeling
write_checkpoint -force $ODIR/post_route
write_verilog -force -quiet -mode timesim -sdf_anno true post_route.v
write_sdf -force -quiet post_route.sdf

# STEP#5: generate a bitstream
# ~~~~~~~~~~~~~~~~~~~~~~~~~~~~
set_property BITSTREAM.GENERAL.COMPRESS True [current_design]
set_property BITSTREAM.CONFIG.USERID "DEADC0DE" [current_design]
set_property BITSTREAM.CONFIG.USR_ACCESS TIMESTAMP [current_design]
set_property BITSTREAM.READBACK.ACTIVERECONFIG Yes [current_design]

write_bitstream -force -bin_file "../result.bit"

# STEP#6: generate reports
# ~~~~~~~~~~~~~~~~~~~~~~~~
report_timing -no_header -path_type summary -max_paths 1000 -slack_lesser_than 0 -setup
report_timing -no_header -path_type summary -max_paths 1000 -slack_lesser_than 0 -hold

report_clocks

report_utilization -hierarchical -file utilization.rpt
report_clock_utilization -file utilization.rpt -append
report_datasheet -file datasheet.rpt
report_timing_summary -file timing.rpt

# highlight_objects -rgb {128 128 128}  [get_cells]
# highlight_objects -rgb {64 64 64}     [get_nets]
# highlight_objects -rgb {128 0 255}	[get_cells reg_delay_inst/*]
# highlight_objects -rgb {255 0 0}	[get_cells ser_to_par_inst/*]
# highlight_objects -rgb {255 64 0}	[get_cells par_match_inst/*]
# highlight_objects -rgb {255 128 0}	[get_cells fifo_chop_inst/*]
# start_gui
