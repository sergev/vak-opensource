#!/usr/bin/python

import math

def exp10 (x):
	return math.exp (x * math.log (10))

print "Decibells  Efficiency"
for db100 in [0, 1, 2, 5]:
	db = - db100 / 100.0
	percent = 100 * exp10 (db / 10)
	print "%6.2g dB  %.1f %%" % (db, percent)

for db10 in range(1,10,1):
	db = - db10 / 10.0
	percent = 100 * exp10 (db / 10)
	print "%6.1f dB  %.1f %%" % (db, percent)

for db10 in range(10,101,5):
	db = - db10 / 10.0
	percent = 100 * exp10 (db / 10)
	print "%6g dB  %.1f %%" % (db, percent)

for db in range(11,21):
	percent = 100 * exp10 (- db / 10.0)
	print "%6g dB  %.2f %%" % (- db, percent)
