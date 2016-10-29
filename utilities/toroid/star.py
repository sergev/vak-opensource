#!/usr/bin/python
#
# Compute MMANA geometry for star antenna.
# Copyright (C) 2005 Serge Vakulenko <vak@cronyx.ru>
#
import math

# Parameters of antenna
MHz = 21.050
A = 0.5			# Outer radius
R = 0.001		# Wire radius

# Compute the number of star vertices
wave = 300/MHz * 0.682
N = int ((wave * 5 / A - math.pi * 6) / 8)
A = wave * 5 / (N * 8 + math.pi * 6)

ox = {}
oy = {}
ix = {}
iy = {}
oxx = {}
oyy = {}
ixx = {}
iyy = {}
for k in range (N):
	kn2pi = 2 * math.pi * k / N
	delta = math.pi / N / 2
	ox[k] = A - A * math.cos (kn2pi - delta)
	oy[k] = - A * math.sin (kn2pi - delta)
	ix[k] = A - A / 5 * math.cos (kn2pi - delta)
	iy[k] = - A / 5 * math.sin (kn2pi - delta)
	oxx[k] = A - A * math.cos (kn2pi + delta)
	oyy[k] = - A * math.sin (kn2pi + delta)
	ixx[k] = A - A / 5 * math.cos (kn2pi + delta)
	iyy[k] = - A / 5 * math.sin (kn2pi + delta)

print "Star", MHz, "MHz with", N, "vertices"
print "*"
print MHz

# Wires
print "*"
print 4*N

def print_segment (x1, y1, x2, y2):
	print "%.3f, %.3f, 0, %.3f, %.3f, 0, %.4f, -1" % \
		(x1, y1, x2, y2, R)

for k in range (N-1):
	print_segment (ox[k],   oy[k],   oxx[k],  oyy[k])
	print_segment (oxx[k],  oyy[k],  ixx[k],  iyy[k])
	print_segment (ixx[k],  iyy[k],  ix[k+1], iy[k+1])
	print_segment (ix[k+1], iy[k+1], ox[k+1], oy[k+1])

print_segment (ox[N-1],  oy[N-1],  oxx[N-1], oyy[N-1])
print_segment (oxx[N-1], oyy[N-1], ixx[N-1], iyy[N-1])
print_segment (ixx[N-1], iyy[N-1], ix[0],    iy[0])
print_segment (ix[0],    iy[0],    ox[0],    oy[0])

# Source
print "*"
print "1, 1"
print "w1c, 0.0, 1.0"
