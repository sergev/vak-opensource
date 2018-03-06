ifeq (,$(wildcard /Applications/Arduino.app/Contents/Java))
    # Linux
    XC8 = /opt/microchip/xc8/v1.45/bin
else
    # Mac OS X
    XC8 = /Applications/microchip/xc8/v1.45/bin
endif
DEV		= 16f1705
CC		= $(XC8)/picc --chip=$(DEV) -Q

all:		flash.hex phase84.hex

flash.hex:	flash.c
		$(CC) --asmlist $<
		dis16 $@ > flash.dis

phase84.hex:	phase84.c
		$(CC) --asmlist $<
		dis16 $@ > phase84.dis

load:		flash.hex
		pk2cmd -ppic$(DEV) -fflash.hex -i -m -r -t

run:
		pk2cmd -ppic$(DEV) -r -t

clean:
		rm -f *~ *.as *.lst *.obj *.rlf *.cmf *.cof *.d *.hxl *.p1 *.pre *.sdb *.sym
