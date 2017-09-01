CC      = gcc
CFLAGS  = -O
#CFLAGS  += --static
LIBS    = -lalgol -lm

all:    hello manorboy primes gps a232 loop

hello:  hello.alg
	marst $< -o $@.c
	gcc $(CFLAGS) $@.c -o $@ $(LIBS)

manorboy: manorboy.alg
	marst $< -o $@.c
	gcc $(CFLAGS) $@.c -o $@ $(LIBS)

primes: primes.alg
	marst $< -o $@.c
	gcc $(CFLAGS) $@.c -o $@ $(LIBS)

gps:    gps.alg
	marst $< -o $@.c
	gcc $(CFLAGS) $@.c -o $@ $(LIBS)

a232:   a232.alg
	marst $< -o $@.c
	gcc $(CFLAGS) $@.c -o $@ $(LIBS)

loop:   loop.alg
	marst $< -o $@.c
	gcc $(CFLAGS) $@.c -o $@ $(LIBS)

clean:
	rm -f hello.c hello manorboy.c manorboy primes.c primes gps.c gps a232.c a232 loop.c loop
