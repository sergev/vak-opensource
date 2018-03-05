DEV		= 16f1705
CC		= /opt/microchip/xc8/v1.45/bin/picc --chip=$(DEV) -Q

all:		flash.hex

flash.hex:	flash.c
		$(CC) --asmlist $<

load:		flash.hex
		pk2cmd -ppic$(DEV) -fflash.hex -i -m -r -t

run:
		pk2cmd -ppic$(DEV) -r -t

clean:
		rm -f *~ *.as *.lst *.obj *.rlf *.cmf *.cof *.d *.hxl *.p1 *.pre *.sdb *.sym
