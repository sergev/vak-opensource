#!/usr/bin/env python
#
# Convert data from narcc.org to a readable format.
#
import sys

if len(sys.argv) < 2:
    print "Usage: narcc filename..."
    sys.exit(1)

repeaters = {}
chan_index = 0
def add_repeater(name, freq, location, notes, type):
    global repeaters

    r = {'name' : name,
         'freq' : freq,
     'location' : location,
        'notes' : notes,
         'type' : type}

    mhz = float(freq)
    try:
        ex = repeaters[mhz]
    except:
        ex = None

    if ex:
        print "Conflict:"
        print "      ignore:", r['freq'], r['name'], r['location']
        print "    existing:", ex['freq'], ex['name'], ex['location']
        return
        #sys.exit(1)

    repeaters[mhz] = r

#
# Get activity from type: voice or data.
#
def get_activity(type):
    if type == "Analog Repeater": return 'voice'
    if type == "DMR Repeater":    return 'DMR'
    if type == "Mixed Mode DMR":  return 'DMR'
    if type == "Fusion AMS Mode": return 'Fusion'
    if type == "Dstar Voice":     return 'Dstar'
    if type == "Mixed Mode P25":  return 'P25'

    print "Unknown repeater type:", type
    sys.exit(1)

#
# Read and process all input files.
#
for filename in sys.argv[1:]:
    input = open(filename)
    for line in input.readlines():
        #word = line.split()
        word = filter(None, line.split("  "))
        if len(word) <= 1:
            continue

        word = map(str.strip, word)
        if word[0] == "OUTPUT":
            continue

        freq = word[0]
        name = word[2]
        location = word[3]
        notes = word[6]
        type = word[7]
        if freq[-1:] == "0":
            # Remove last zero.
            freq = freq[:-1]

        add_repeater(name, freq, location, notes, type)

#
# Print repeaters, sorted by frequency.
#
print "Channel Activity                Name    Receive  Location"
chan_index = 0
for mhz in sorted(repeaters):
    chan_index += 1
    r = repeaters[mhz]
    print "  %3d   %-10s              %-6s  %-8s %s" %\
        (chan_index, get_activity(r['type']), r['name'], r['freq'], r['location'])
    #TODO: PL, voice/DMR/Fusion/Dstar
