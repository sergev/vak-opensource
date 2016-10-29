#!/usr/bin/python
#
# Compute MMANA geometry for toroidal helical antenna (THA).
# Copyright (C) 2005 Serge Vakulenko <vak@cronyx.ru>
#
import math

# Parameters of antenna
MHz = 21.050
A = 0.6
N = 7
R = 0.001
H = 300/MHz / 2 / N / 4

# 3-rd resonance
H = H * 3

# Experimental correction
H = H * 1

a_plus_h2 = A + H/2
a_minus_h2 = A - H/2
ox = {}
oy = {}
ix = {}
iy = {}
for k in range (N):
	kn2pi = 2 * math.pi * k / N
	ox[k] = a_plus_h2 - a_plus_h2 * math.cos (kn2pi)
	oy[k] = - a_plus_h2 * math.sin (kn2pi)
	ix[k] = a_plus_h2 - a_minus_h2 * math.cos (kn2pi + math.pi / N)
	iy[k] = - a_minus_h2 * math.sin (kn2pi + math.pi / N)
ox[0] = 0
oy[0] = 0

print "Toroid", MHz, "MHz with", N, "turns"
print "*"
print MHz

# Wires
print "*"
print 4*N

def print_segment (x1, y1, z1, x2, y2, z2):
	print "%.3f, %.3f, %.3f, %.3f, %.3f, %.3f, %.4f, -1" % \
		(x1, y1, z1, x2, y2, z2, R)

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
