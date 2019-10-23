#!/bin/sh

#
# Set path to the Microchip MPLABX installation.
#
mplabx="/opt/microchip/mplabx/v5.25"
java="$mplabx/sys/java/jre1.8.0_181/bin/java"
ipecmd="$mplabx/mplab_platform/mplab_ipe/ipecmd.jar"

#
# Program HEX file into the microcontroller.
#
set -x
$java -jar $ipecmd -p32MZ2064DAR169 -tpPKOB -fu-boot.hex -m -ol
