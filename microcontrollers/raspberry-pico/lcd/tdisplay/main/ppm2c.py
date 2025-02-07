#!/usr/bin/python
# -*- encoding: utf-8 -*-
#
# Convert PPM image to C data.
#
import sys, string

if len(sys.argv) != 2:
    print "Usage:"
    print "    ppm2c.py image.ppm > image.c"
    sys.exit(1)

lines = open(sys.argv[1]).readlines()

if lines[0][:2] != "P3":
    print("Wrong header:", lines[0])
    sys.exit(1)

if lines[1][0] != "#":
    print("Wrong comment:", lines[1])
    sys.exit(1)

word = lines[2].split()
w = int(word[0])
h = int(word[1])

word = lines[3].split()
maxval = int(word[0])

print("""static const struct {
    unsigned int   width, height;
    unsigned short data[%u * %u];
} parrot_image = {
%u, %u, {""" % (w, h, w, h))

for i in range(4, len(lines), 3):
    r = int(lines[i]) >> 3
    g = int(lines[i+1]) >> 2
    b = int(lines[i+2]) >> 3

    rgb = r * (1 << 11) + g * (1 << 5) + b
    print("0x%04x," % rgb)

print("}};")
