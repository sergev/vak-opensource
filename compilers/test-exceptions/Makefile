BINDIR		= /usr/local/mipsel441/bin
CC		= $(BINDIR)/mipsel-elf32-gcc -mips32 -g -Wall
SIZE		= $(BINDIR)/mipsel-elf32-size
OBJDUMP		= $(BINDIR)/mipsel-elf32-objdump -mmips:isa32r2
OBJCOPY		= $(BINDIR)/mipsel-elf32-objcopy
ASFLAGS		= -I$(OS)/sources -DMIPS32 -DELVEES_MC24
LDFLAGS		= -nostdlib -T code-in-flash.ld
LIBS		= -L$(TARGET) -lgcc

CURRENT		= test-exceptions
TESTS		+= $(CURRENT).srec
OUTS		= $(TESTS:%.srec=%.out)

all:		$(OUTS) $(TESTS) $(CURRENT).dis
		$(SIZE) *.out

clean:
		rm -rf *~ *.[oasi] *.out *.hex *.srec *.lst *.dis .deps $(MODULES)

load:		$(CURRENT).srec
		mcprog $(CURRENT).srec

proxy:		../gdbproxy
		../gdbproxy elvees &

gdb:		$(CURRENT).out
		mipsel-elf32-gdb -x gdbinit test-exceptions.out

insight:	$(CURRENT).out
		mipsel-elf32-insight -x gdbinit test-exceptions.out &

.SUFFIXES:	.srec .dis .bin

.S.out:
		@[ -d .deps ] || mkdir .deps
		$(CC) $(LDFLAGS) $(ASFLAGS) $< $(LIBS) -o $@

.out.srec:
		$(OBJCOPY) -O srec $< $@
		@chmod -x $@

.out.bin:
		$(OBJCOPY) -O binary $< $@
		@chmod -x $@

.out.dis:
		$(OBJDUMP) -d -S $< > $@
###