PROG            = float
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

nlopt-example:  nlopt-example.o
		$(CC) $(LDFLAGS) nlopt-example.o -lnlopt -lm -o $@

nlopt-newuoa:   nlopt-newuoa.o
		$(CC) $(LDFLAGS) nlopt-newuoa.o -lnlopt -lm -o $@

clean:
		rm -f $(PROG) *.o *.dis *~ a.out float double bfloat16

float:          float.o
		$(CC) $(LDFLAGS) $< -o $@
		ln -s -f float double
		ln -s -f float bfloat16

float128:       float128.o
		$(CC) $(LDFLAGS) $< -lquadmath -o $@
