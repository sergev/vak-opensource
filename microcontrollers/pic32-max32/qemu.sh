#!/bin/sh

set -x

qemu-pic32 -machine pic32mx7-max32 \
    -nographic \
    -monitor none \
    -serial stdio \
    -bios ../max32-demo/arduino-bootloader.X.Max32.hex \
    -kernel uart.hex

#    -d instr \
#    -D out.trace
