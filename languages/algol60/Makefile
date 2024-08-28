ALGC    = marst
CC      = gcc
CFLAGS  = -O
LIBS    = -lalgol -lm
PROG    = hello manorboy primes gps a232 loop

all:    $(PROG)

%:      %.alg
	$(ALGC) $< -o $@.c
	$(CC) $(CFLAGS) $@.c -o $@ $(LIBS)

gps:    gps.alg
	$(ALGC) $< -o $@.c
	$(CC) $(CFLAGS) $@.c -o $@ $(LIBS)

a232:   a232.alg
	$(ALGC) $< -o $@.c
	$(CC) $(CFLAGS) $@.c -o $@ $(LIBS)

clean:
	rm -f *.c *.h $(PROG)
