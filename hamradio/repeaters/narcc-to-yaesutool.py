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
def add_repeater(name, freq, tx, location, notes, type):
    global repeaters

    r = {'name' : name,
         'freq' : freq,
           'tx' : tx,
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
        return "- "

#
# Get location and open/closed status.
#
def get_location(r, mhz):
    global activity

    location = r['location']

    type = r['type']
    if type == "DMR Repeater":    location += ' DMR'
    if type == "Mixed Mode DMR":  location += ' DMR'
    if type == "Fusion AMS Mode": location += ' Fusion'
    if type == "Dstar Voice":     location += ' Dstar'
    if type == "Mixed Mode P25":  location += ' P25'

    if 'c' in r['notes']:
        location += " (closed)"

    try:
        active = activity[mhz]
    except:
        return location

    return location + " - " + active

#
# Get transmit offset.
#
def get_offset(r, rx_mhz):
    rx_hz = int(rx_mhz * 1000000)
    tx_hz = int(float(r['tx']) * 1000000)
    if tx_hz == rx_hz:
        return "+0"
    if tx_hz == rx_hz + 5000000:
        return "+5"
    if tx_hz == rx_hz + 600000:
        return "+0.6"
    if tx_hz == rx_hz - 600000:
        return "-0.6"
    if tx_hz == rx_hz + 2500000:
        return "+2.5"
    if tx_hz == rx_hz - 400000:
        return "-0.4"
    print "Bad TX offset:", rx_hz, tx_hz, r['name'], r['location']
    sys.exit(1)
#    return str((tx_hz - rx_hz) / 1000000.0)

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
        tx = word[1]
        name = word[2]
        location = word[3]
        notes = word[6]
        type = word[7]
        if freq[-5:-4] == "." and freq[-1:] == "0":
            # Remove last zero.
            freq = freq[:-1]

        add_repeater(name, freq, tx, location, notes, type)

#
# Print repeaters, sorted by frequency.
#
chan_index = 0
for mhz in sorted(repeaters):
    if chan_index == 0:
        print "Channel Name    Receive  Transmit R-Squel T-Squel Power Modulation Scan"
    chan_index += 1
    r = repeaters[mhz]
    print "  %3d   %-6s  %-8s %-8s    -    %5s   High  Wide       +    # %s" %\
        (chan_index, r['name'], r['freq'], get_offset(r, mhz), get_ctss(r['notes']), get_location(r, mhz))
