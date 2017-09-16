#!/usr/bin/python
# -*- encoding: utf-8 -*-
#
# Generate a table of American Wire Gauge diameters.
#
# In the American Wire Gauge (AWG), diameters can be calculated
# by applying the formula
#
#   D(AWG) = .005 * 92^((36-AWG)/39) inch.
#
# This means that in American wire gage every 6 gauge decrease
# gives a doubling of the wire diameter, and every 3 gauge
# decrease doubles the wire cross sectional area. Similar to dB
# in signal and power levels. An approximate but accurate enough
# form of this formula contributed by Mario Rodriguez is
#
#   D = .460 * (57/64)^(awg+3)
# or
#   D = .460 * 0.890625^(awg+3)
#
# (from https://www.powerstream.com/Wire_Size.htm)
#
import sys, string, subprocess, bisect

if len(sys.argv) != 1:
    print "Usage: awg.py"
    sys.exit (1)

#
# Print the result.
#
print " AVG    D mm"
print "---------------"
for awg in range(41):
    # Diameter in mm
    d = 25.4 * .005 * 92**((36-awg)/39.)
    print "%4d%8.3f" % (awg, d)
