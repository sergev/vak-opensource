CFLAGS		+= -Wall -O3 -g
ALL		= terms poly1 poly2 crc8

all:		$(ALL)

crc8:		crc8.c
		$(CC) $(LDFLAGS) $(CFLAGS) -DDEBUG_CRC8 $< -o $@

clean:
		rm -rf $(ALL) *~ *.dSYM
