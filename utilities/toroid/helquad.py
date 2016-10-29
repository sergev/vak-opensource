#!/usr/bin/python
#
# Compute MMANA geometry for quad helical antenna.
# Copyright (C) 2005 Serge Vakulenko <vak@cronyx.ru>
#
import sys, string

if len(sys.argv) != 5:
	print "Usage: mhz helquad side nturns klen"
	sys.exit (1)

# Parameters of antenna
R = 0.001		# Wire radius

MHz = string.atof (sys.argv[1])
side = string.atof (sys.argv[2])
N = (string.atoi (sys.argv[3]) + 7) / 8 * 8
klen = string.atof (sys.argv[4])

# Wave length with experimental correction
wave = 300/MHz * klen

H = wave / N / 4
side2 = side / 2
inner = side2 - H
if inner < side2/100:
	print "Number of turns is too small!"
	sys.exit (1)


print "Helical quad", MHz, "MHz, side", side, "meters with", N, "turns"
print "*"
print MHz

# Wires
print "*"
print 4*N

def print_segment (x1, y1, z1, x2, y2, z2):
	print "%.4f, %.4f, %.4f, %.4f, %.4f, %.4f, %.4f, -1" % \
		(x1, y1, z1, x2, y2, z2, R)

step = inner * 8 / N
nexty = 0
for k in range (N/8):
	y = nexty
	nexty = y + step
	print_segment (H, y,        0, H, y,        H)
	print_segment (H, y,        H, 0, y+step/2, H)
	print_segment (0, y+step/2, H, 0, y+step/2, 0)
	print_segment (0, y+step/2, 0, H, nexty,    0)

nextx = H
for k in range (N/4):
	x = nextx
	nextx = x + step
	print_segment (x,        inner, 0, x,        inner, H)
	print_segment (x,        inner, H, x+step/2, side2, H)
	print_segment (x+step/2, side2, H, x+step/2, side2, 0)
	print_segment (x+step/2, side2, 0, nextx,    inner, 0)

nexty = inner
for k in range (N/4):
	y = nexty
	nexty = y - step
	print_segment (side-H, y,        0, side-H, y,        H)
	print_segment (side-H, y,        H, side,   y-step/2, H)
	print_segment (side,   y-step/2, H, side,   y-step/2, 0)
	print_segment (side,   y-step/2, 0, side-H, nexty,    0)

nextx = side - H
for k in range (N/4):
	x = nextx
	nextx = x - step
	print_segment (x,        -inner, 0, x,        -inner, H)
	print_segment (x,        -inner, H, x-step/2, -side2, H)
	print_segment (x-step/2, -side2, H, x-step/2, -side2, 0)
	print_segment (x-step/2, -side2, 0, nextx,    -inner, 0)

nexty = -inner
for k in range (N/8):
	y = nexty
	nexty = y + step
	print_segment (H, y,        0, H, y,        H)
	print_segment (H, y,        H, 0, y+step/2, H)
	print_segment (0, y+step/2, H, 0, y+step/2, 0)
	print_segment (0, y+step/2, 0, H, nexty,    0)

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
