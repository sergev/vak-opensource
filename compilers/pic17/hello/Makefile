CC		= cc17
AS		= as17
CFLAGS		= -DMHZ=16384000 -O
OBJ		= hello.s

all:		hello.hex

hello.hex:	hello.c
		$(CC) $(CFLAGS) hello.c -o hello.hex

clean:
		rm -f $(OBJ) *~ *.i *.core

.c.s:
		$(CC) $(CFLAGS) -S $<
