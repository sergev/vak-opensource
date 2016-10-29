#!/usr/bin/python

import string

def digits(x):
	d = []
	for c in str (x):
		d.append (c)
	d.sort()
	d.reverse()
	return string.atoi (string.join (d, ""))

n = 9
for x in range (6*10**(n-1), 10**n, 6):
	d = digits (x)
#	print d
	x1 = x / 6
	if digits (x1) != d:
		continue
	x2 = x1 * 2
	if digits (x2) != d:
		continue
	x3 = x1 * 3
	if digits (x3) != d:
		continue
	x4 = x1 * 4
	if digits (x4) != d:
		continue
	x5 = x1 * 5
	if digits (x5) != d:
		continue
	print x1, x2, x3, x4, x5, x
