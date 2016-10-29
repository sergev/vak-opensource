CC		= /usr/local/mipsel441/bin/mipsel-elf32-gcc
SIZE		= /usr/local/mipsel441/bin/mipsel-elf32-size
OBJCOPY		= /usr/local/mipsel441/bin/mipsel-elf32-objcopy
OBJDUMP		= /usr/local/mipsel441/bin/mipsel-elf32-objdump
CFLAGS		= -O -Wall -g
LDFLAGS		= -nostdlib -T script.ld

all:		hello.bin hello.dis
		$(SIZE) *.elf

run:		hello.bin
		qemu-system-mipsel -M mips -nographic -L . -d in_asm,out_asm,op,int,exec,cpu -bios $<
		cp /tmp/qemu.log qemu.log

gdb:		hello.bin
		qemu-system-mipsel -M mips -nographic -L . -s -S -bios $<

clean:
		rm -f *~ *.o

hello.bin:	hello.elf
		$(OBJCOPY) -O binary $< $@

hello.dis:	hello.elf
		$(OBJDUMP) -d -S $< > $@

hello.elf:	startup.o uart.o hello.o
		$(CC) $(LDFLAGS) startup.o uart.o hello.o -o $@
