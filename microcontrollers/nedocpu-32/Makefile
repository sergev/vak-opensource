#P               = /usr/local/mipsel441/bin/mipsel-elf32-
P               = /usr/local/mips-gcc-4.7.2/bin/mips-elf-
CC              = $(P)gcc -EL -mips32r2 -g -nostdlib
OBJCOPY         = $(P)objcopy
OBJDUMP         = $(P)objdump
CFLAGS          = -O -Wall -Werror -DPIC32MX2
LDFLAGS         = -T mx2-ram4k.ld -e _start

all:            run.srec

run.srec:       run.c
		$(CC) $(CFLAGS) -c $<
		$(CC) $(LDFLAGS) run.o -o run.elf
		$(OBJCOPY) -O srec run.elf run.srec
		$(OBJDUMP) -mmips:isa32r2 -d -S run.elf > run.dis

load:           run.srec
		pic32prog -p run.srec

clean:
		rm -f *.o *.lst *~ *.elf *.srec *.dis
