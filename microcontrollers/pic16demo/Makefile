DEV		= 16f690
CC		= sdcc --use-non-free -mpic14 -p$(DEV)

all:		flash.hex

flash.hex:	flash.c
		$(CC) $<

load:		flash.hex
		pk2cmd -ppic$(DEV) -fflash.hex -i -m -r -t

run:
		pk2cmd -ppic$(DEV) -r -t

clean:
		rm -f *.asm *.cod *.o *.lst *~
