#!/bin/bash

mkdir -p build
cd build || exit 1

# Altera Quartus 18.1
/opt/Altera/18.1/quartus/bin/quartus_sh -t ../de10-lite-build.tcl
