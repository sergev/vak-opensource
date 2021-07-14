CC      = gcc
CFLAGS  = -O
#CFLAGS  += --static
LIBS    = -lalgol -lm
PROG    = hello manorboy primes gps a232 loop

all:    $(PROG)

%:      %.alg
	jff-algol $<

gps:    gps.alg
	marst $< -o $@.c
	gcc $(CFLAGS) $@.c -o $@ $(LIBS)

a232:   a232.alg
	marst $< -o $@.c
	gcc $(CFLAGS) $@.c -o $@ $(LIBS)

clean:
	rm -f *.c *.h $(PROG)
