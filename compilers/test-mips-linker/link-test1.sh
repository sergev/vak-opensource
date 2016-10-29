#!/bin/sh
set -x

/local/BSD/elftoolchain/ld/ld test.o -o test1.elf

#/usr/local/mips-gcc-4.8.1/bin/mips-elf-objdump -EL -mmips:isa32r2 -S test1.elf > test1.dis
/mips/arch/overflow/codesourcery/mips-sde-elf/lite/release/2013.11-37/Linux/bin/mips-sde-elf-objdump -mmips:isa32r2 -S test1.elf > test1.dis

/local/BSD/elftoolchain/readelf/readelf -a test1.elf > test1.readelf
