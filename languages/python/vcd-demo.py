#!/usr/bin/env python3
#
# Generate wavedump from Python.
# Prerequisites:
#   pip3 install pyvcd
#
import sys, vcd

with vcd.VCDWriter(sys.stdout, timescale='1 ns', date='today') as writer:
    counter = writer.register_var('top.module', 'counter', 'integer', size=8)
    for t, val in enumerate(range(10)):
        writer.change(counter, t, val)
