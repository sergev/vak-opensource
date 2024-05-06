PROG            = seasons moonphase easter
CFLAGS		= -O -Wall -Werror
ASTRO           = astronomy.o astro_demo_common.o

all:		$(PROG)

clean:
		rm -f $(PROG) *.o

seasons:        seasons.o $(ASTRO)
		$(CC) $(LDFLAGS) seasons.o $(ASTRO) -o $@

moonphase:      moonphase.o $(ASTRO)
		$(CC) $(LDFLAGS) moonphase.o $(ASTRO) -o $@

easter:         easter.o $(ASTRO)
		$(CC) $(LDFLAGS) easter.o $(ASTRO) -o $@
