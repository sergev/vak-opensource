#!/usr/bin/python
#
# Chebyshev filter - recursion coefficient calculation.
# From book "Digital signal processing" by Steven W.Smith.
# Python version by Serge Vakulenko.
#
import sys
from math import pi, sin, cos, tan, sqrt, log, exp

# Filter parameters
fc = 0.1			# Cutoff frequency, 0 - 0.5
filter_type = "low-pass"	# Filter type "low-pass" or "high-pass"
pr = 0.5			# Percent ripple, 0 - 29
np = 4				# Number of poles, 2 - 20

# Test data set 1
#fc = 0.1
#filter_type = "low-pass"
#pr = 0
#np = 4

# Test data set 2
#fc = 0.1
#filter_type = "high-pass"
#pr = 10
#np = 4

def compute_2pole (p):
	# Calculate the pole location on the unit sircle
	rp = - cos (pi / np / 2 + (p-1) * pi / np)
	ip =   sin (pi / np / 2 + (p-1) * pi / np)

	# Warp from a circle to an ellipse
	if pr > 0:
		es = sqrt ((100.0 / (100.0 - pr)) ** 2 - 1)
		vx = (1.0 / np) * log (1/es + sqrt (1/es**2 + 1))
		kx = (1.0 / np) * log (1/es + sqrt (1/es**2 - 1))
		kx = (exp (kx) + exp (-kx)) / 2
		rp = rp * (exp (vx) - exp (-vx)) / 2 / kx
		ip = ip * (exp (vx) + exp (-vx)) / 2 / kx

	# S-domain to z-domain conversion
	t = 2 * tan (1.0/2)
	w = 2 * pi * fc
	m = rp**2 + ip**2
	d = 4 - 4 * rp * t + m * t**2
	x0 = t**2 / d
	x1 = 2 * t**2 / d
	x2 = t**2 / d
	y1 = (8 - 2 * m * t**2) / d
	y2 = (-4 - 4 * rp * t - m * t**2) / d

	#if pr > 0:
		#print "rp, ip, es, vx, kx ="
		#print rp, ip, es, vx, kx
	#else:
		#print "rp, ip ="
		#print rp, ip
	#print
	#print "t, w, m, d ="
	#print t, w, m, d
	#print

	#print "x0, x1, x2, y1, y2 ="
	#print x0, x1, x2, y1, y2
	#print

	# LP to LP, or LP to HP transform
	if filter_type == "low-pass":
		k = sin (0.5 - w/2) / sin (0.5 + w/2)
	else:
		k = - cos (w/2 + 0.5) / cos (w/2 - 0.5)

	d = 1 + y1*k - y2 * k**2
	a0 = (x0 - x1*k + x2*k**2) / d
	a1 = (-2*x0*k + x1 + x1*k**2 - 2*x2*k) / d
	a2 = (x0*k**2 - x1*k + x2) / d
	b1 = (2*k + y1 + y1*k**2 - 2*y2*k) / d
	b2 = (-k**2 - y1*k + y2) / d

	if filter_type == "high-pass":
		a1 = -a1
		b1 = -b1

	print "Stage %d:" % p
	print "        A =", a0, a1, a2
	print "        B =", b1, b2
	return (a0, a1, a2, b1, b2)

# Test 1, expected results:
# rp, ip =
# -0.923879532511 0.382683432365
# t, w, m, d =
# 1.09260497969 0.628318530718 1.0 9.23152715305
# x0, x1, x2, y1, y2 =
# 0.129316159921 0.258632319842 0.129316159921 0.607963192186 -0.12522783187
# a0, a1, a2, b1, b2 =
# 0.0618851952998 0.1237703906 0.0618851952998 1.04859957636 -0.296140357562
#(a0, a1, a2, b1, b2) = compute_2pole (1)
#print "a0, a1, a2, b1, b2 ="
#print a0, a1, a2, b1, b2
#sys.exit (0)

# Test 2, expected results:
# rp, ip, es, vx, kx =
# -0.136178716428 0.933223449526 0.484322104838 0.368054872396 1.05780188523
# t, w, m, d =
# 1.09260497969 0.628318530718 0.889450649554 5.65697158917
# x0, x1, x2, y1, y2 =
# 0.211029103261 0.422058206523 0.211029103261 1.03878428212 -0.789584174003
# a0, a1, a2, b1, b2 =
# 0.922919811253 -1.84583962251 0.922919811253 1.4469127773 -0.836653725122
#(a0, a1, a2, b1, b2) = compute_2pole (2)
#print "a0, a1, a2, b1, b2 ="
#print a0, a1, a2, b1, b2
#sys.exit (0)

print "Chebyshev filter:", filter_type
print "  Percent ripple:", pr
print "Cutoff frequency:", fc
print " Number of poles:", np

a = {}
b = {}
for i in range (0, 23):
	a [i] = 0.0
	b [i] = 0.0

a [2] = 1.0
b [2] = 1.0

# Loop for each pole pair
for p in range (1, 1 + np/2):
	(a0, a1, a2, b1, b2) = compute_2pole (p)

	# Add coefficients to the cascade
	ta = {}
	tb = {}
	for i in range (0, 23):
		ta [i] = a [i]
		tb [i] = b [i]

	for i in range (2, 23):
		a [i] = a0 * ta [i] + a1 * ta [i-1] + a2 * ta [i-2]
		b [i] =      tb [i] - b1 * tb [i-1] - b2 * tb [i-2]

# Finish combining coefficients
b [2] = 0.0
for i in range (0, 21):
	a [i] = a [i+2]
	b [i] = -b [i+2]

# Normalize the gain
sa = 0.0
sb = 0.0
for i in range (0, 21):
	if filter_type == "low-pass":
		sa = sa + a [i]
		sb = sb + b [i]
	else:
		sa = sa + a [i] * (-1) ** i
		sb = sb + b [i] * (-1) ** i

gain = sa / (1 - sb)
for i in range (0, 21):
	a [i] = a [i] / gain

# Output results.
print "A =",
for i in range (0, np+1):
	print a [i],
print
print "B =",
for i in range (1, np+1):
	print b [i],
print

sys.exit (0)
