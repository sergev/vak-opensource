P               = /usr/local/mips-gcc-4.8.1/bin/mips-elf-
CC              = $(P)gcc -mips32r2 -EL -g -nostdlib
OBJCOPY         = $(P)objcopy
OBJDUMP         = $(P)objdump
GDB             = /usr/local/mips/insight70/bin/mips-elf-insight
CFLAGS          = -O -Wall -Werror -DPIC32MX7
LDFLAGS         = -T using-bootloader.ld -e _start

PROG            = uart

all:            $(PROG).hex

$(PROG).hex:    $(PROG).c
		$(CC) $(CFLAGS) -c $<
		$(CC) $(LDFLAGS) $(PROG).o -o $(PROG).elf
		$(OBJCOPY) -O ihex --change-addresses=0x80000000 $(PROG).elf $(PROG).hex
		$(OBJDUMP) -mmips:isa32r2 -d -S $(PROG).elf > $(PROG).dis

load:           $(PROG).hex
		pic32prog $(PROG).hex

clean:
		rm -f *.o *.lst *~ *.elf *.hex *.dis
