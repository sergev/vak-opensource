#P              = /usr/local/mips-gcc-4.8.1/bin/mips-elf-
#P               = /usr/local/mips-2013.11/bin/mips-sde-elf-
P               = /usr/local/mips-2014.11/bin/mips-sde-elf-
CC              = $(P)gcc -mips32r2 -EL -g -nostdlib
GDB             = mipsel-elf32-gdb
OBJCOPY         = $(P)objcopy
OBJDUMP         = $(P)objdump
#GDB             = /mips/arch/overflow/codesourcery/mips-sde-elf/lite/release/2012.03-64/Linux/bin/mips-sde-elf-gdb
#GDB             = /usr/local/mips/insight681/bin/mipsel-elf32-insight
GDB             = /usr/local/mips-gcc-4.7.2/bin/mips-elf-gdb
CFLAGS          = -O -Wall -Werror -DPIC32MZ
LDFLAGS         = -T pic32mz.ld -e _start

#PROG            = lcd
PROG            = blink
#PROG            = uart

all:            $(PROG).hex

$(PROG).hex:   $(PROG).c
		$(CC) $(CFLAGS) -c $<
		$(CC) $(LDFLAGS) $(PROG).o $(LIBS) -o $(PROG).elf
		$(OBJCOPY) -O ihex $(PROG).elf $(PROG).hex
		$(OBJDUMP) -mmips:isa32r2 -d -S $(PROG).elf > $(PROG).dis

load:           $(PROG).hex
		pic32prog $(PROG).hex

clean:
		rm -f *.o *.lst *~ *.elf *.hex *.dis

debug:          $(PROG).hex
		$(GDB) $(PROG).elf
