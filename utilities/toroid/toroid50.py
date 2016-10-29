#!/usr/bin/python
#
# Compute MMANA geometry for toroidal helical antenna (THA).
# Copyright (C) 2005 Serge Vakulenko <vak@cronyx.ru>
#
import sys, string, math

if len(sys.argv) != 4:
	print "Usage: toroid radius nturns klen"
	sys.exit (1)

# Parameters of antenna
MHz = 21.050		# Resonant frequency
R = 0.001		# Wire radius

radius = string.atof (sys.argv[1])
N = string.atoi (sys.argv[2])
klen = string.atof (sys.argv[3])

# Wave length with experimental correction
wave = 300/MHz * klen

H = wave / N / 4
inner = radius - H
if inner < radius/100:
	print "Number of turns is too small!"
	sys.exit (1)

ox = {}
oy = {}
ix = {}
iy = {}
for k in range (N):
	kn2pi = 2 * math.pi * k / N
	ox[k] = radius - radius * math.cos (kn2pi)
	oy[k] = - radius * math.sin (kn2pi)
	ix[k] = radius - inner * math.cos (kn2pi + math.pi / N)
	iy[k] = - inner * math.sin (kn2pi + math.pi / N)
ox[0] = 0
oy[0] = 0

print "Toroid", MHz, "MHz, diameter", 2*radius, "meters with", N, "turns"
print "*"
print MHz

# Wires
print "*"
print 4*N+4

def print_segment (x1, y1, z1, x2, y2, z2):
	print "%.3f, %.3f, %.3f, %.3f, %.3f, %.3f, %.4f, -1" % \
		(x1, y1, z1, x2, y2, z2, R)

# Feeder loop
flen = 0.16
foff = (H - flen) / 2
print_segment (     foff, 0,      foff, flen+foff, 0,      foff)
print_segment (flen+foff, 0,      foff, flen+foff, 0, flen+foff)
print_segment (flen+foff, 0, flen+foff,      foff, 0, flen+foff)
print_segment (     foff, 0, flen+foff,      foff, 0,      foff)

for k in range (N-1):
	print_segment (ox[k],   oy[k],   0, ox[k],   oy[k],   H)
	print_segment (ox[k],   oy[k],   H, ix[k],   iy[k],   H)
	print_segment (ix[k],   iy[k],   H, ix[k],   iy[k],   0)
	print_segment (ix[k],   iy[k],   0, ox[k+1], oy[k+1], 0)

print_segment (ox[N-1], oy[N-1], 0, ox[N-1], oy[N-1], H)
print_segment (ox[N-1], oy[N-1], H, ix[N-1], iy[N-1], H)
print_segment (ix[N-1], iy[N-1], H, ix[N-1], iy[N-1], 0)
print_segment (ix[N-1], iy[N-1], 0, ox[0],   oy[0],   0)

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
