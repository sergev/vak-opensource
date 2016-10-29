#!/usr/bin/python

for n in range(1,60):
	x = 10**n * 2 - 4
	if x % 19 == 0:
		print "решение = %d2" % (x / 19)
