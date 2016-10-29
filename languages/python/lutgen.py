#!/usr/bin/python
# -*- encoding: utf-8 -*-
#
# Generate the init string for Xilinx LUT.
#
import sys, string, subprocess, bisect

if len(sys.argv) != 2:
    print "Usage: lutgen input.txt"
    sys.exit (1)

#
# Get bit value: 0, 1 or ? (as -1).
#
def getbit(str):
    if str == "?":
        return -1
    return int(str)

# Resulting table: total 64 bits.
table = [-1] * 64

def process(in0, in1, in2, in3, in4, out0, out1):
    index = in0 + 2*(in1 + 2*(in2 + 2*(in3 + 2*in4)))
    #print in0, in1, in2, in3, in4, out0, out1, "-- index", index
    table[index]      = out0
    table[index + 32] = out1

def process4(in0, in1, in2, in3, in4, out0, out1):
    if in4 < 0:
        process(in0, in1, in2, in3, 0,   out0, out1)
        process(in0, in1, in2, in3, 1,   out0, out1)
    else:
        process(in0, in1, in2, in3, in4, out0, out1)

def process3(in0, in1, in2, in3, in4, out0, out1):
    if in3 < 0:
        process4(in0, in1, in2, 0,   in4, out0, out1)
        process4(in0, in1, in2, 1,   in4, out0, out1)
    else:
        process4(in0, in1, in2, in3, in4, out0, out1)

def process2(in0, in1, in2, in3, in4, out0, out1):
    if in2 < 0:
        process3(in0, in1, 0,   in3, in4, out0, out1)
        process3(in0, in1, 1,   in3, in4, out0, out1)
    else:
        process3(in0, in1, in2, in3, in4, out0, out1)

def process1(in0, in1, in2, in3, in4, out0, out1):
    if in1 < 0:
        process2(in0, 0,   in2, in3, in4, out0, out1)
        process2(in0, 1,   in2, in3, in4, out0, out1)
    else:
        process2(in0, in1, in2, in3, in4, out0, out1)

def process0(in0, in1, in2, in3, in4, out0, out1):
    if in0 < 0:
        process1(0,   in1, in2, in3, in4, out0, out1)
        process1(1,   in1, in2, in3, in4, out0, out1)
    else:
        process1(in0, in1, in2, in3, in4, out0, out1)

#
# Read the input file.
#
file = open (sys.argv[1])
pc = 0
op = ""
last_op = ""
level = 0
for line in file.readlines():
    word = line.split()
    if len(word) <= 0 or word[0] == "#":
        continue
    if len(word) != 8 or word[5] != ":":
        print "Error: incorrect input: ", line.strip()
        sys.exit(1)

    #print word
    in0 = getbit(word[0])
    in1 = getbit(word[1])
    in2 = getbit(word[2])
    in3 = getbit(word[3])
    in4 = getbit(word[4])
    out0 = getbit(word[6])
    out1 = getbit(word[7])
    process0(in0, in1, in2, in3, in4, out0, out1)

#
# Print the result.
#
#print table
low = 0
high = 0
for i in range(32):
    if table[i] < 0:
        print "Error: no value at index", i
        sys.exit(1)
    if table[i+32] < 0:
        print "Error: no value at index", i+32
        sys.exit(1)

    if table[i] == 1:
        low += 1 << i
    if table[i+32] == 1:
        high += 1 << i

print "Result: %08x_%08x" % (high, low)
