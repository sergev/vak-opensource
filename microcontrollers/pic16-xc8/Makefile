ifeq (,$(wildcard /Applications/Arduino.app/Contents/Java))
    # Linux
    XC8 = /opt/microchip/xc8/v1.45/bin
    MDB = /opt/microchip/mplabx/v4.15/mplab_ide/bin/mdb.sh
else
    # Mac OS X
    XC8 = /Applications/microchip/xc8/v1.45/bin
    MDB = /Applications/microchip/mplabx/v4.15/mplab_ide.app/Contents/Resources/mplab_ide/bin/mdb.sh
endif
DEV		= 16f1705
CC		= $(XC8)/picc --chip=$(DEV) -Q
CFLAGS          = -DCPU_HZ=32000000 -DUART_BAUD=9600

all:		uart.hex

uart.hex:	uart.c
		$(CC) $(CFLAGS) --asmlist $<
		dis16 $@ > uart.dis

phase84.hex:	phase84.c
		$(CC) --asmlist $<
		dis16 $@ > phase84.dis

upload:         uart.hex pickit3.cfg
		$(MDB) pickit3.cfg

clean:
		rm -f *~ *.as *.lst *.obj *.rlf *.cmf *.cof *.d *.hxl *.p1 *.pre *.sdb *.sym
