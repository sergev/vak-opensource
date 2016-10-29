MIPS_GCC_ROOT = /mips/arch/overflow/codesourcery/mips-sde-elf/lite/release/2013.11-37/Linux

CFLAGS          = -EL -mips32r2 -Wall -Werror -O
LDFLAGS         = -EL -mips32r2 -nostdlib
CC              = $(MIPS_GCC_ROOT)/bin/mips-sde-elf-gcc
LD              = $(MIPS_GCC_ROOT)/bin/mips-sde-elf-ld
OBJDUMP         = $(MIPS_GCC_ROOT)/bin/mips-sde-elf-objdump

all:            test.elf

test.elf:       test.o elf32-mips-le.ld
		$(LD) $(LDFLAGS) -Telf32-mips-le.ld test.o -o $@
		$(OBJDUMP) -S $@ > test.dis
		/local/BSD/elftoolchain/readelf/readelf -a -r $@ > test.readelf

%.o:            %.c
		$(CC) $(CFLAGS) -c $< -o $@
		/local/BSD/elftoolchain/readelf/readelf -a -r $@ > $@.readelf

%.o:            %.s
		$(CC) $(CFLAGS) -c $< -o $@
		/local/BSD/elftoolchain/readelf/readelf -a -r $@ > $@.readelf

clean:
		rm -f *.elf *.o *.dis *.readelf
