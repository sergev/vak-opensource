#!/usr/bin/python

import math

print "sin(m)"
for m in range(0,60):
    print "%u, " % (math.sin(m*math.pi*6/180) * 1000000000)

print
print "cos(m)"
for m in range(0,60):
    print "%u, " % (math.cos(m*math.pi*6/180) * 1000000000)
