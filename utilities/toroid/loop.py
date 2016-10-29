#!/usr/bin/python
#
# Compute MMANA geometry for vertical wave loop antenna.
# Copyright (C) 2005 Serge Vakulenko <vak@cronyx.ru>
#
import math

# Parameters of antenna
MHz = 21.050		# Resonant frequency
N = 12			# Number of segments
R = 0.001		# Wire radius

# Wave length with experimental correction
wave = 300/MHz * 1.04

# Outer radius
radius = wave / 2 / N / math.sin (math.pi / N)

ox = {}
oy = {}
for k in range (N):
	kn2pi = 2 * math.pi * k / N
	ox[k] = radius * (1 - math.cos (kn2pi))
	oy[k] = - radius * math.sin (kn2pi)
ox[0] = 0
oy[0] = 0

print "Loop", MHz, "MHz with", N, "segments"
print "*"
print MHz

# Wires
print "*"
print N

def print_segment (x1, y1, x2, y2):
	print "%.3f, %.3f, %.3f, %.3f, %.3f, %.3f, %.4f, -1" % \
		(0, y1, x1, 0, y2, x2, R)

for k in range (N-1):
	print_segment (ox[k], oy[k], ox[k+1], oy[k+1])

print_segment (ox[N-1], oy[N-1], ox[0], oy[0])

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
