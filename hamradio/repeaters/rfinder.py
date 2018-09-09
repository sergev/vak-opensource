#!/usr/bin/env python
#
# Convert a CVS file downloaded from FRinder.net to readable format.
#
import csv

with open('rfexport-chirp.csv') as csv_file:
    csv_reader = csv.reader(csv_file, delimiter=',')
    line_count = 0
    for row in csv_reader:
        line_count += 1
        if line_count == 1:
            #print 'Columns:', ", ".join(row)
# Location, Name, Frequency, Duplex, Offset, Tone, rToneFreq, cToneFreq, DtcsCode, DtcsPolarity, Mode, TStep, Comment
# 0         1     2          3       4       5     6          7          8         9             10    11     12
            print 'MHz       Duplex Offset CTSS  DCS   Name   Location'
        else:
            name = row[1]
            mhz = row[2]
            duplex = row[3]

            offset = row[4]
            if offset == "0.005":
                offset = "5"

            if row[5] == "Tone":
                ctss = row[6]
            else:
                ctss = ""

            if row[8] == "23":
                dcs = ""
            else:
                dcs = row[8] + row[9]

            location = row[12]

            if duplex != "" and offset != "0":
                print '%-9s %-6s %-6s %-5s %-5s %-6s %s' %\
                    (mhz, duplex, offset, ctss, dcs, name, location)

    #print 'Processed %d lines.' % line_count
