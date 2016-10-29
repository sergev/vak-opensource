CC              = /usr/local/mipsel441/bin/mipsel-elf32-gcc -mips32r2 -g -nostdlib
OBJCOPY         = /usr/local/mipsel441/bin/mipsel-elf32-objcopy
OBJDUMP         = /usr/local/mipsel441/bin/mipsel-elf32-objdump
CFLAGS          = -O -Wall -Werror
LDFLAGS         = -T pic32.ld -e _start

all:            flash.srec

flash.srec:     flash.c
		$(CC) $(CFLAGS) -c $<
		$(CC) $(LDFLAGS) flash.o -o flash.elf
		$(OBJCOPY) -O srec flash.elf flash.srec
		$(OBJDUMP) -mmips:isa32r2 -d -S flash.elf > flash.dis

load:           flash.srec
		pic32prog -p flash.srec

clean:
		rm -f *.o *.lst *~ *.elf *.srec *.dis
