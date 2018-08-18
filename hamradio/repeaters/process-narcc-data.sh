#!/bin/sh

#
# Process raw data from NARCC and my listenings,
# and build readable report and configurations for handhelds.
#
./narcc.py \
    -a my-listenings-santa-clara.txt \
    narcc-south-bay.txt \
    narcc-east-bay.txt \
    narcc-west-bay.txt \
    narcc-north-bay.txt \
    unknown-repeaters-santa-clara.txt \
    > santa-clara.txt

echo "See result in santa-clara.txt"
