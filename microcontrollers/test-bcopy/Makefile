P               = /usr/local/mips-2013.11/bin/mips-sde-elf-
CC              = $(P)gcc -mips32r2 -EL -g -nostdlib
OBJCOPY         = $(P)objcopy
OBJDUMP         = $(P)objdump
BIN2HEX         = $(P)bin2hex
GDB             = /usr/local/mips/insight70/bin/mips-elf-insight
CFLAGS          = -O -Wall -Werror -DPIC32MZ
LDFLAGS         = -T using-bootloader.ld -e _start #-mdebugger

PROG            = test-bcopy
OBJS            = bcopy-litebsd.o
#OBJS            = bcopy-retrobsd.o

all:            $(PROG).hex

$(PROG).hex:    $(PROG).c $(OBJS)
		$(CC) $(CFLAGS) -c $<
		$(CC) $(LDFLAGS) $(PROG).o $(OBJS) -o $(PROG).elf
		$(OBJCOPY) -O ihex $(PROG).elf $(PROG).hex
		$(OBJDUMP) -mmips:isa32r2 -d -S $(PROG).elf > $(PROG).dis

load:           $(PROG).hex
		pic32prog -d /dev/ttyUSB0 $(PROG).hex

clean:
		rm -f *.o *.lst *~ *.elf *.hex *.dis

debug:          $(PROG).srec
		$(GDB) $(PROG).elf
