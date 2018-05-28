PROG            = fputest
CFLAGS		= -O3 -Wall -Werror
#CFLAGS		= -m32 -O -Wall -Werror
#LDFLAGS		= -m32
#CC		= i586-mingw32msvc-gcc
OBJDUMP         = objdump

all:		$(PROG)

fputest:        fputest.c fputest2.c
		$(CC) $(LDFLAGS) $(CFLAGS) fputest.c fputest2.c -o $@
		$(OBJDUMP) -d -S $@ > $@.dis

distance-poly:  distance-poly.o
		$(CC) $(LDFLAGS) distance-poly.o -lm -o $@

clean:
		rm -f $(PROG) *.o *.dis *~ a.out
