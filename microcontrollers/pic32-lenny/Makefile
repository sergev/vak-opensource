P               = /usr/local/mips-2014.11/bin/mips-sde-elf-
CC              = $(P)gcc -mips32r2 -EL -g -nostdlib
OBJCOPY         = $(P)objcopy
OBJDUMP         = $(P)objdump
CFLAGS          = -O -Wall -Werror -DPIC32MX2
LDFLAGS         = -T using-bootloader.ld -e _start

PROG            = blink

all:            $(PROG).hex

$(PROG).hex:    $(PROG).c
		$(CC) $(CFLAGS) -c $<
		$(CC) $(LDFLAGS) $(PROG).o -o $(PROG).elf
		$(OBJCOPY) -O ihex --change-addresses=0x80000000 $(PROG).elf $(PROG).hex
		$(OBJDUMP) -mmips:isa32r2 -d -S $(PROG).elf > $(PROG).dis

load:           $(PROG).hex
		pic32prog -d /dev/ttyACM1 $(PROG).hex

clean:
		rm -f *.o *.lst *~ *.elf *.hex *.dis
