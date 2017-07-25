#!/bin/bash

mkdir -p build
cd build || exit 1

# Altera Quartus 16.1
/opt/Altera/16.1/quartus/bin/quartus_sh -t ../de10-lite-build.tcl
