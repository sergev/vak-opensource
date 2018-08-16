#!/usr/bin/env python
#
# Convert data from narcc.org to a readable format.
#
import sys

if len(sys.argv) != 2:
    print "Usage: narcc filename"
    sys.exit(1)

filename = sys.argv[1]

#
# Read and process the replay.log file.
#
input = open(sys.argv[1])
chan_index = 0
for line in input.readlines():
    #word = line.split()
    word = filter(None, line.split("  "))
    if len(word) <= 1:
        continue

    word = map(str.strip, word)
    if word[0] == "OUTPUT":
        if chan_index == 0:
            print "Channel Activity                Name    Receive"
            print "------------------------------------------------------------ VHF Band 145-148"
        else:
            print "------------------------------------------------------------ UHF Band 440-447"
        continue

    freq = word[1]
    if freq[-1:] == "0":
        # Remove last zero.
        freq = freq[:-1]

    chan_index += 1
    print "  %3d                           %-6s  %-8s %s" %\
        (chan_index, word[2], freq, word[3])
