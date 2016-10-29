CC              = /usr/local/mipsel441/bin/mipsel-elf32-gcc -mips32r2 -g -nostdlib
OBJCOPY         = /usr/local/mipsel441/bin/mipsel-elf32-objcopy
OBJDUMP         = /usr/local/mipsel441/bin/mipsel-elf32-objdump
#GDB             = /mips/arch/overflow/codesourcery/mips-sde-elf/lite/release/2012.03-64/Linux/bin/mips-sde-elf-gdb
#GDB             = mipsel-elf32-gdb
#GDB             = /usr/local/mips/insight681/bin/mipsel-elf32-insight
GDB             = /usr/local/mips/insight70/bin/mips-elf-insight
CFLAGS          = -O -Wall -Werror -DPIC32MX2
LDFLAGS         = -T using-bootloader.ld -e _start

PROG            = flash

all:            $(PROG).srec

$(PROG).srec:   $(PROG).c
		$(CC) $(CFLAGS) -c $<
		$(CC) $(LDFLAGS) $(PROG).o -o $(PROG).elf
		$(OBJCOPY) -O srec $(PROG).elf $(PROG).srec
		$(OBJDUMP) -mmips:isa32r2 -d -S $(PROG).elf > $(PROG).dis

load:           $(PROG).srec
		pic32prog -p $(PROG).srec

clean:
		rm -f *.o *.lst *~ *.elf *.srec *.dis

debug:          $(PROG).srec
		$(GDB) $(PROG).elf
