#!/usr/bin/python
#
# Compute MMANA geometry for magnetic loop antenna.
# Copyright (C) 2005 Serge Vakulenko <vak@cronyx.ru>
#
import sys, string, math

if len(sys.argv) != 4:
	print "Usage: magloop radius nsegments capacity"
	sys.exit (0)

# Parameters of antenna
MHz = 21.050		# Resonant frequency
R = 0.010		# Wire radius

radius = string.atof (sys.argv[1])
N = string.atoi (sys.argv[2])
capacity = string.atof (sys.argv[3])

ox = {}
oy = {}
for k in range (N):
	kn2pi = 2 * math.pi * k / N
	ox[k] = radius * (1 - math.cos (kn2pi))
	oy[k] = - radius * math.sin (kn2pi)
ox[0] = 0
oy[0] = 0

print "Magnetic loop", MHz, "MHz with", N, "segments"
print "*"
print MHz

# Wires
print "*"
print N

def print_segment (x1, y1, x2, y2):
	print "%.3f, %.3f, %.3f, %.3f, %.3f, %.3f, %.4f, -1" % \
		(x1, y1, 0, x2, y2, 0, R)

for k in range (N-1):
	print_segment (ox[k], oy[k], ox[k+1], oy[k+1])

print_segment (ox[N-1], oy[N-1], ox[0], oy[0])

# Source
print "*"
print "1, 1"
print "w1b, 0.0, 1.0"

# Load
print "*"
print "1, 1"
print "w%db, 0, 0, %.2f, %.1f" % (N/2 + 1, capacity, 2000)

# Segmentation: DM1, DM2, SC, EC
print "*"
print "400, 40, 2.0, 1"

# Ground, Height, Material, R, Azimuth, Elevation, X
print "*"
print "%d, %.1f, %d, 50.0, 120, 60, 0" % (0, 0, 1) # Free space, Cu
