#!/bin/sh

trace="-t bcopy.trace"
/local/BSD/pic32sim/pic32mz -m $trace /local/BSD/pic32sim/demo/wifire/boot.hex test-bcopy.hex
