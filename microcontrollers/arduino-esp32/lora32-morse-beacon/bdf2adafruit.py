#!/usr/bin/python2

# MIT License.

# Copyright (c) 2016 William Skellenger
# 
# Permission is hereby granted, free of charge, to any person obtaining a 
# copy of this software and associated documentation files (the "Software"), 
# to deal in the Software without restriction, including without limitation 
# the rights to use, copy, modify, merge, publish, distribute, sublicense, 
# and/or sell copies of the Software, and to permit persons to whom the 
# Software is furnished to do so, subject to the following conditions:
# 
# The above copyright notice and this permission notice shall be included 
# in all copies or substantial portions of the Software.
# 
# THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS 
# OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, 
# FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE 
# AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER 
# LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING 
# FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS 
# IN THE SOFTWARE.

# This small script is designed to mostly take a BDF file and convert it to a
# format that can largely be cut/pasted as an Adafruit-format font.
# It was written in an hour or so and did what I needed it to do.
# I used it for one file.  Maybe it bombs on other files.
# William Skellenger, Feb 2016
# (email: williamj@skellenger.net)
# (Twitter: @skelliam)
#
# Usage: bdf2adafruit.py <somefont.bdf> > out.txt
#
# Once you have out.txt you can cut/paste the contents into a new font 
# header file as part of the Adafruit GFX library.

import sys

myfile = open(sys.argv[1])

processing = 0
getting_rows = 0

chars = []

class Glyph:
    encoding = -1
    rows = []
    comment = ""
    offset = -1
    width = 0
    height = 0
    advance = 0
    xoffs = 0
    yoffs = 0 
    def __init__(self, comment):
        self.comment = comment
        self.rows = []

for line in myfile.readlines():
    if 'STARTCHAR' in line:
        processing = 1
        vals = line.split()
        g = Glyph(vals[1])
        g.width = 8  #in this example always 8 bits wide
    elif 'ENDCHAR' in line:
        chars.append(g)  #append the completed glyph into list
        processing = 0
        getting_rows = 0

    if processing:
        if 'ENCODING' in line:
            vals = line.split()
            g.encoding = int(vals[1])
        elif 'DWIDTH' in line:
            vals = line.split()
            #g.advance = int(vals[1])  #cursor advance seems to be the first number in DWIDTH
        elif 'BBX' in line:
            vals = line.split()
            g.xoffs = 0
            g.yoffs = -(int(vals[2]) + int(vals[4]))
            g.advance = (int(vals[1]) + 1)  #x bounding box + 1
        elif 'BITMAP' in line:
            getting_rows = 1
        elif getting_rows:
            g.rows.append(int(line, 16))  #append pixel rows into glyph's list of rows

print 

i=0
for char in chars:
    height = 0
    char.offset = i
    for row in char.rows:
        print "0x%02X," %(row),
        i+=1
        height+=1
    char.height = height
    print "/* 0x%02X %s */" %(char.encoding, char.comment)

for char in chars:
    # offset, bit-width, bit-height, advance cursor, x offset, y offset 
    print "{ %d, %d, %d, %d, %d, %d }, /* 0x%02X %s */" %(
            char.offset, char.width, char.height, 
            char.advance, char.xoffs, char.yoffs, 
            char.encoding, char.comment)
