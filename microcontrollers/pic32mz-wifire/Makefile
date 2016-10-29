#P               = /usr/local/pic32-tools/bin/pic32-
#P               = /usr/local/mips-2013.11/bin/mips-sde-elf-
P               = /usr/local/mips-gcc-4.8.1/bin/mips-elf-
CC              = $(P)gcc -mips32r2 -EL -g -nostdlib
OBJCOPY         = $(P)objcopy
OBJDUMP         = $(P)objdump
BIN2HEX         = $(P)bin2hex
#GDB             = /mips/arch/overflow/codesourcery/mips-sde-elf/lite/release/2012.03-64/Linux/bin/mips-sde-elf-gdb
#GDB             = mipsel-elf32-gdb
#GDB             = /usr/local/mips/insight681/bin/mipsel-elf32-insight
GDB             = /usr/local/mips/insight70/bin/mips-elf-insight
CFLAGS          = -O -Wall -Werror -DPIC32MZ
LDFLAGS         = -T using-bootloader.ld -e _start #-mdebugger

ifneq (,$(wildcard /opt/mpide-0023-linux32-20140605-test/hardware/tools/avrdude))
    # Linux
    PROGTOOL    = /opt/mpide-0023-linux32-20140605-test/hardware/tools/avrdude \
                  -C /opt/mpide-0023-linux32-20140605-test/hardware/tools/avrdude.conf \
                  -P /dev/ttyUSB1
endif
ifneq (,$(wildcard /Applications/Mpide.app/Contents/Resources/Java/hardware/tools/avr/bin/avrdude))
    # Mac OS X
    PROGTOOL    = /Applications/Mpide.app/Contents/Resources/Java/hardware/tools/avr/bin/avrdude \
                  -C /Applications/Mpide.app/Contents/Resources/Java/hardware/tools/avr/etc/avrdude.conf \
                  -P /dev/tty.usbserial-*
endif
PROGTOOL        += -c stk500v2 -p 32MZ2048ECG100 -b 115200

#PROG            = led
PROG            = uart

all:            $(PROG).hex

$(PROG).hex:    $(PROG).c
		$(CC) $(CFLAGS) -c $<
		$(CC) $(LDFLAGS) $(PROG).o -o $(PROG).elf
		$(OBJCOPY) -O ihex --change-addresses=0x80000000 $(PROG).elf $(PROG).hex
#		$(BIN2HEX) -a $(PROG).elf
		$(OBJDUMP) -mmips:isa32r2 -d -S $(PROG).elf > $(PROG).dis

load:           $(PROG).hex
		$(PROGTOOL) -U flash:w:$(PROG).hex:i

clean:
		rm -f *.o *.lst *~ *.elf *.hex *.dis

debug:          $(PROG).srec
		$(GDB) $(PROG).elf
