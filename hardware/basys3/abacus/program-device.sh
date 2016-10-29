#!/bin/bash

# Xilinx Vivado for Bash shell 64 bit environment
# Needed for modern FPGA series like Virtex-7
. /opt/Xilinx/Vivado/2016.2/settings64.sh

vivado -mode batch -nojournal -nolog -source program.tcl
