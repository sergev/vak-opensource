#!/usr/bin/python
#
# Compute MMANA geometry for vertical quad antenna.
# Copyright (C) 2005 Serge Vakulenko <vak@cronyx.ru>
#
import math

# Parameters of antenna
MHz = 21.050		# Resonant frequency
R = 0.001		# Wire radius

# Wave length with experimental correction
wave = 300/MHz * 1.058
H = wave/4

print "Vertical quad", MHz, "MHz"
print "*"
print MHz

# Wires
print "*"
print "4"

def print_segment (x1, y1, x2, y2):
	print "%.4f, %.4f, %.4f, %.4f, %.4f, %.4f, %.4f, -1" % \
		(0, x1, y1, 0, x2, y2, R)

print_segment (-H/2, 0,  H/2, 0)
print_segment ( H/2, 0,  H/2, H)
print_segment ( H/2, H, -H/2, H)
print_segment (-H/2, H, -H/2, 0)

# Source
print "*"
print "1, 1"
print "w1c, 0.0, 1.0"

# Load
print "*"
print "0, 1"

# Segmentation: DM1, DM2, SC, EC
print "*"
print "400, 40, 2.0, 1"

# Ground, Height, Material, R, Azimuth, Elevation, X
print "*"
print "%d, %.1f, %d, 50.0, 120, 60, 0" % (0, 0, 1) # Free space, Cu
