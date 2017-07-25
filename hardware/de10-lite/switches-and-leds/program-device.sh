#!/bin/bash

file="build/result.pof"

# Factory firmware.
#file="DE10_LITE_Default.pof"

/opt/Altera/16.1/quartus/bin/quartus_pgm -m jtag -c 1 -o "p;$file"
