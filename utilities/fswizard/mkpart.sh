#!/bin/sh

#dev="/dev/sdb"
dev="/dev/disk1"

SWAPSZ=8
ROOTSZ=32
ROOTSTART=`expr 2 + $SWAPSZ`
USERSTART=`expr $ROOTSTART + $ROOTSZ`
sudo fdisk -H128 -S32 -uM $dev << EOF
$ROOTSTART,$ROOTSZ,b7
2,$SWAPSZ,b8
$USERSTART,+,b7
EOF
