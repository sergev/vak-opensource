#P               = /usr/local/pic32-tools/bin/pic32-
#P               = /usr/local/mips-gcc-4.7.2/bin/mips-elf-
#P               = /mips/arch/overflow/codesourcery/mips-sde-elf/lite/release/2013.11-37/Linux/bin/mips-sde-elf-
P               = /usr/local/mips-2014.11/bin/mips-sde-elf-
CC              = $(P)gcc -EL -mips32r2 -g -nostdlib
OBJCOPY         = $(P)objcopy
OBJDUMP         = $(P)objdump
#GDB             = /mips/arch/overflow/codesourcery/mips-sde-elf/lite/release/2012.03-64/Linux/bin/mips-sde-elf-gdb
#GDB             = mipsel-elf32-gdb
#GDB             = /usr/local/mips/insight681/bin/mipsel-elf32-insight
#GDB             = /usr/local/mips/insight70/bin/mips-elf-insight
GDB             = /usr/local/mips-gcc-4.7.2/bin/mips-elf-gdb
CFLAGS          = -O -Wall -Werror -DPIC32MX2
LDFLAGS         = -T mx2-ram4k.ld -e _start

PROG            = flash
OBJS            = usb_boot.o usb_device.o usb_function_hid.o

#CFLAGS          += -DCPU_KHZ=40000
CFLAGS          += -DUSB_EP0_BUFF_SIZE=8
CFLAGS          += -DUSB_MAX_EP_NUMBER=3
CFLAGS          += -DUSB_NUM_STRING_DESCRIPTORS=3
CFLAGS          += -DHID_FEATURE_REPORT_BYTES=2
CFLAGS          += -DHID_INPUT_REPORT_BYTES=2
CFLAGS          += -DHID_INT_IN_EP_SIZE=64
CFLAGS          += -DHID_INT_OUT_EP_SIZE=64
CFLAGS          += -DHID_OUTPUT_REPORT_BYTES=2
CFLAGS          += -DHID_RPT01_SIZE=29
CFLAGS          += -DBL_LED_PIN=0
CFLAGS          += -DBL_LED_PORT=TRISA

all:            $(PROG).srec

$(PROG).srec:   $(OBJS)
		$(CC) $(LDFLAGS) $(OBJS) -o $(PROG).elf
		$(OBJCOPY) -O srec $(PROG).elf $(PROG).srec
		$(OBJDUMP) -mmips:isa32r2 -d -S $(PROG).elf > $(PROG).dis

load:           $(PROG).srec
		pic32prog -p $(PROG).srec

clean:
		rm -f *.o *.lst *~ *.elf *.srec *.dis

debug:          $(PROG).srec
		$(GDB) -x gdbinit $(PROG).elf
