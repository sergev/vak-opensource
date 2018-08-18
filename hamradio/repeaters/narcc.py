#!/usr/bin/env python
#
# Convert data from narcc.org to a readable format.
#
import sys, getopt

#
# Print usage message.
#
def usage():
    print "Usage: narcc [-a activity.txt] filename.txt..."

#
# Load activity file: result of listenings.
#
activity = {}
def load_activity(filename):
    global activity

    input = open(filename)
    for line in input.readlines():
        #word = line.split()
        word = filter(None, line.split("  "))
        if len(word) <= 1:
            continue

        word = map(str.strip, word)
        if word[0] == "Index" or word[0][0] == "#":
            continue

        mhz = float(word[2])
        type = word[3]
        activity[mhz] = type

#
# Parse command line.
#
try:
    opts, args = getopt.getopt(sys.argv[1:], "ha:", ["help", "activity="])
except getopt.GetoptError as err:
    print(err)
    usage()
    sys.exit(1)

for o, a in opts:
    if o in ("-h", "--help"):
        usage()
        sys.exit()
    elif o in ("-a", "--activity"):
        load_activity(a)
    else:
        print "Unhandled option!"
        usage()
        sys.exit(1)

if not args:
    usage()
    sys.exit()

#
# Add a repeater to the list.
#
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
        #print "Conflict:"
        #print "      ignore:", r['freq'], r['name'], r['location']
        #print "    existing:", ex['freq'], ex['name'], ex['location']
        return
        #sys.exit(1)

    repeaters[mhz] = r

#
# Get modulation from NARCC data.
#
def get_modulation(type):
    if type == "Analog Repeater": return 'FM'
    if type == "DMR Repeater":    return 'DMR'
    if type == "Mixed Mode DMR":  return 'DMR'
    if type == "Fusion AMS Mode": return 'Fusion'
    if type == "Dstar Voice":     return 'Dstar'
    if type == "Mixed Mode P25":  return 'P25'
    print "Unknown repeater type:", type
    sys.exit(1)

#
# Get activity from listening data.
#
def get_activity(mhz):
    global activity
    try:
        return activity[mhz]
    except:
        return "-"

#
# Get CTSS frequency from the notes.
#
def get_ctss(notes):
    ctss = filter(lambda ch: ch in "0123456789.", notes)
    if ctss:
        return ctss
    else:
        return "-"

#
# Get location and open/closed status.
#
def get_location(r):
    location = r['location']
    if 'c' in r['notes']:
        location += " (closed)"
    return location

#
# Read and process all input files.
#
for filename in args:
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
        if freq[-5:-4] == "." and freq[-1:] == "0":
            # Remove last zero.
            freq = freq[:-1]

        add_repeater(name, freq, location, notes, type)

#
# Print repeaters, sorted by frequency.
#
chan_index = 0
for mhz in sorted(repeaters):
    if chan_index == 0:
        print "Channel Modulation  Activity    Name    Receive   PL     Location"
    chan_index += 1
    r = repeaters[mhz]
    print "  %3d   %-10s  %-10s  %-6s  %-8s  %-5s  %s" %\
        (chan_index, get_modulation(r['type']), get_activity(mhz),
         r['name'], r['freq'], get_ctss(r['notes']), get_location(r))
    #TODO: PL, voice/DMR/Fusion/Dstar
