#!/usr/bin/python
# -*- coding: utf-8 -*-
#
# Вычисление числа пи.
#
import math

# arctg(x) ~= x - x^3/3 + x^5/5 - x^7/7 + x^9/9 - x^11/11 + x^13/13 - x^15/15
#
def arctan(x):
	x2 = x * x
	x3 = x2 * x
	x5 = x3 * x2
	x7 = x5 * x2
	x9 = x7 * x2
	x11 = x9 * x2
	x13 = x11 * x2
	x15 = x13 * x2
	f = x13/13 - x15/15
	f += x9/9 - x11/11
	f += x5/5 - x7/7
	f += x - x3/3
	return f

# Формула Джона Мэчина:
# pi/4 = 4*arctan(1/5) - arctan(1/239)
#
def machin():
	x = 1./5
	y = 1./239

	x2 = x * x
	x3 = x2 * x
	x5 = x3 * x2
	x7 = x5 * x2
	x9 = x7 * x2
	x11 = x9 * x2
	x13 = x11 * x2
	x15 = x13 * x2
	atx = x13/13 - x15/15
	atx += x9/9 - x11/11
	atx += x5/5 - x7/7
	atx += x - x3/3

	y2 = y * y
	y3 = y2 * y
	y5 = y3 * y2
	y7 = y5 * y2
	y9 = y7 * y2
	y11 = y9 * y2
	y13 = y11 * y2
	y15 = y13 * y2
	aty = y13/13 - y15/15
	aty += y9/9 - y11/11
	aty += y5/5 - y7/7
	aty += y - y3/3
	return 4 * (4*atx - aty)

# Формула Гаусса:
# pi/4 = 12*arctan(1/18) + 8*arctan(1/57) - 5*arctan(1/239)
#
def gauss():
	x = 1./18
	y = 1./57
	z = 1./239

	x2 = x * x
	x3 = x2 * x
	x5 = x3 * x2
	x7 = x5 * x2
	x9 = x7 * x2
	x11 = x9 * x2
	x13 = x11 * x2
	x15 = x13 * x2
	atx = x13/13 - x15/15
	atx += x9/9 - x11/11
	atx += x5/5 - x7/7
	atx += x - x3/3

	y2 = y * y
	y3 = y2 * y
	y5 = y3 * y2
	y7 = y5 * y2
	y9 = y7 * y2
	y11 = y9 * y2
	y13 = y11 * y2
	y15 = y13 * y2
	aty = y13/13 - y15/15
	aty += y9/9 - y11/11
	aty += y5/5 - y7/7
	aty += y - y3/3

	z2 = z * z
	z3 = z2 * z
	z5 = z3 * z2
	z7 = z5 * z2
	z9 = z7 * z2
	z11 = z9 * z2
	z13 = z11 * z2
	z15 = z13 * z2
	atz = z13/13 - z15/15
	atz += z9/9 - z11/11
	atz += z5/5 - z7/7
	atz += z - z3/3
	return 4 * (12*atx + 8*aty - 5*atz)

# Алгоритм Брента-Саламина:
#	a[0] = 1
#	b[0] = sqrt (2)
#	t[0] = 1/4
#	p[0] = 1
#	a[n+1] = (a[n] + b[n]) / 2
#	b[n+1] = sqrt (a[n] * b[n])
#	t[n+1] = t[n] - p[n] * (a[n] - a[n+1])^2
#	p[n+1] = 2 * p[n]
# Результат:
#	pi = (a[n] + b[n])^2 / 4 * t[n]
#
def brent_salamin():
	a0 = 1.
	b0 = 1 / math.sqrt (2)
	t0 = 1./4
	p0 = 1.
	#print (a0 + b0)*(a0 + b0) / (4 * t0)

	a1 = (a0 + b0) / 2
	b1 = math.sqrt (a0 * b0)
	t1 = t0 - p0 * (a0 - a1)*(a0 - a1)
	p1 = p0 + p0
	#print (a1 + b1)*(a1 + b1) / (4 * t1)

	a2 = (a1 + b1) / 2
	b2 = math.sqrt (a1 * b1)
	t2 = t1 - p1 * (a1 - a2)*(a1 - a2)
	p2 = p1 + p1
	#print (a2 + b2)*(a2 + b2) / (4 * t2)

	a3 = (a2 + b2) / 2
	b3 = math.sqrt (a2 * b2)
	t3 = t2 - p2 * (a2 - a3)*(a2 - a3)
	p3 = p2 + p2
	return (a3 + b3)*(a3 + b3) / (4 * t3)

pi_machin = machin()
pi_gauss = gauss()
pi_brent = brent_salamin()

print pi_machin, pi_gauss, pi_brent
