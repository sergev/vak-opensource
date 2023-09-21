#!/bin/sh

#
# Set path to the Microchip MPLABX installation.
#
mplabx="/Applications/microchip/mplabx/v6.15"
java="$mplabx/sys/java/zulu8.64.0.19-ca-fx-jre8.0.345-macosx_x64/bin/java"
ipecmd="$mplabx/mplab_platform/mplab_ipe/ipecmd.jar"

#
# Program HEX file into the microcontroller.
#
set -x
#$java -jar $ipecmd -p32MZ2064DAR169 -tpPKOB -fu-boot.hex -m -ol

# Create symlink:
# Looking for driver at: /Applications/microchip/mplabcomm/3_52_01/lib/libUSBAccessLink_3_39.dylib
# Not found.
# Valid path: /Applications/microchip/mplabcomm/3.53.00/lib/libUSBAccessLink_3_40.dylib

#export PATH="$mplabx/mplab_platform/bin":$PATH

$java -jar $ipecmd -t

# Available Tool List
# -------------------
# 1  Snap S.No : 021038102RYN002750

$java -jar $ipecmd -ts021038102RYN002750

# Tutorial is here: https://github.com/PinguinoIDE/pinguino-bootloaders/blob/master/p32/usb/v1.x/IPECMD.txt
