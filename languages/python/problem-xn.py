#!/usr/bin/python
# -*- encoding: utf-8 -*-

for n in range(2,10):
	for k in range(1,100):
		x = n * (10**k - n)
		if x % (10*n - 1) == 0:
			v = 10 * x / (10*n - 1) + n
			print "Для", n, "решение =", v
			break
