#!/bin/bash

# Xilinx Vivado for Bash shell 64 bit environment
# Needed for modern FPGA series like Virtex-7
. /opt/Xilinx/Vivado/2016.2/settings64.sh

mkdir -p build
cd build || exit 1

vivado -mode batch -source ../basys3-build.tcl
