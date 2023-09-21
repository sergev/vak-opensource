#!/bin/sh

#
# Set path to the Microchip MPLABX installation.
#
mplabx="/Applications/microchip/mplabx/v6.15"
java="$mplabx/sys/java/zulu8.64.0.19-ca-fx-jre8.0.345-macosx_x64/bin/java"
mdb="$mplabx/mplab_platform/lib/mdb.jar"

# Linux will ignore headless mode if DISPLAY is set
unset DISPLAY
export TERM=xterm

$java -Dfile.encoding=UTF-8 -Djava.awt.headless=true -jar $mdb $@

# Manual: https://www.microchip.com/content/dam/mchp/documents/DEV/ProductDocuments/UserGuides/50002102G.pdf
