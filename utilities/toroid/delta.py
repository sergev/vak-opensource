#!/usr/bin/python
#
# Compute MMANA geometry for vertical delta antenna.
# Copyright (C) 2005 Serge Vakulenko <vak@cronyx.ru>
#
import math

# Parameters of antenna
MHz = 21.050		# Resonant frequency
R = 0.001		# Wire radius

# Wave length with experimental correction
wave = 300/MHz * 1.0707
H = wave/3 * math.sin (math.pi / 3)

print "Vertical delta", MHz, "MHz"
print "*"
print MHz

# Wires
print "*"
print "3"

def print_segment (x1, y1, x2, y2):
	print "%.4f, %.4f, %.4f, %.4f, %.4f, %.4f, %.4f, -1" % \
		(0, x1, y1, 0, x2, y2, R)

print_segment (0, 0, - wave/6, H)
print_segment (- wave/6, H, wave/6, H)
print_segment (wave/6, H, 0, 0)

# Source
print "*"
print "1, 1"
print "w1b, 0.0, 1.0"

# Load
print "*"
print "0, 1"

# Segmentation: DM1, DM2, SC, EC
print "*"
print "400, 40, 2.0, 1"

# Ground, Height, Material, R, Azimuth, Elevation, X
print "*"
print "%d, %.1f, %d, 50.0, 120, 60, 0" % (0, 0, 1) # Free space, Cu
