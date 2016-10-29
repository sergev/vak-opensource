#!/usr/bin/python
#
# Compute MMANA geometry for half wave dipole antenna.
# Copyright (C) 2005 Serge Vakulenko <vak@cronyx.ru>
#
import math

# Parameters of antenna
MHz = 21.050		# Resonant frequency
R = 0.001		# Wire radius

# Wave length with experimental correction
wave = 300/MHz * 0.9681

print "Dipole", MHz, "MHz, half wave"
print "*"
print MHz

# Wires
print "*"
print "1"

print "0, %.4f, 0, 0, %.4f, 0, %.4f, -1" % (- wave/4, wave/4, R)

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
