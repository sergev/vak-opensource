#P               = $(HOME)/.uecide/compilers/pic32-tools/bin/pic32-
#P               = /Applications/mpide.app/Contents/Resources/Java/hardware/pic32/compiler/pic32-tools/bin/pic32-
P               = /usr/local/pic32-tools/bin/pic32-
CC              = $(P)gcc -mips32r2 -g -nostdlib
OBJCOPY         = $(P)objcopy
OBJDUMP         = $(P)objdump
SIZE            = $(P)size
#GDB             = /mips/arch/overflow/codesourcery/mips-sde-elf/lite/release/2012.03-64/Linux/bin/mips-sde-elf-gdb
#GDB             = mipsel-elf32-gdb
#GDB             = /usr/local/mips/insight681/bin/mipsel-elf32-insight
#GDB             = /usr/local/mips/insight70/bin/mips-elf-insight
GDB             = /usr/local/mips-gcc-4.7.2/bin/mips-elf-gdb
CFLAGS          = -O3 -Wall -Werror
LDFLAGS         = -e _start

OBJS            = pidp8.o

#
# DIY board with pic32mx2 processor and USB port.
#
#PROG            = mx2-usb
#CFLAGS          += -DPIC32MX2
#OBJS            += mx2-usb.o usb-device.o usb-function-hid.o
#LDFLAGS         += -T mx2-ram8k.ld

#
# Numato MX board with pic32mx7 processor and USB port.
#
PROG            = mx7-numato
CFLAGS          += -DPIC32MX7
OBJS            += mx7-numato.o usb-device.o usb-function-hid.o
LDFLAGS         += -T mx7-bootloader.ld

all:            $(PROG).hex
		$(SIZE) $(PROG).elf

$(PROG).hex:    $(OBJS)
		$(CC) $(LDFLAGS) $(OBJS) -o $(PROG).elf
		$(OBJCOPY) -O ihex $(PROG).elf $@
		$(OBJDUMP) -mmips:isa32r2 -d -S $(PROG).elf > $(PROG).dis

load:           $(PROG).hex
		pic32prog $(PROG).hex

clean:
		rm -f *.o *.lst *~ *.elf $(PROG).dis

debug:          $(PROG).hex
		$(GDB) $(PROG).elf

###
mx7-numato.o: mx7-numato.c pidp8.h pic32mx.h usb-device.h usb-ch9.h usb-hal-pic32.h usb-function-hid.h
pidp8.o: pidp8.c pidp8.h
usb-device.o: usb-device.c pic32mx.h usb-device.h usb-ch9.h usb-hal-pic32.h
usb-function-hid.o: usb-function-hid.c usb-device.h usb-ch9.h usb-hal-pic32.h usb-function-hid.h
