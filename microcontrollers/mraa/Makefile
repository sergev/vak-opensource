PROGS           = omega2-blink-rgb

LIBS            = -lmraa -lpthread -static -ljson-c

all:            $(PROGS)

omega2-blink-rgb: omega2-blink-rgb.o
		$(CC) $< -o $@ $(LIBS)

clean:
		rm -f $(PROGS) *.o
