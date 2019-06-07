CFLAGS  = -O3 -Wall -Werror
FFLAGS  = $(CFLAGS)
F90     = gfortran
PROG    = gmp-chudnovsky bbp-bailey bbp-fortran

all:    $(PROG)

clean:
	rm -f *.o $(PROG)

gmp-chudnovsky: gmp-chudnovsky.o
	$(CC) $< -lgmp -lm -o $@

bbp-bailey: piqpr8.o
	$(CC) $< -lm -o $@

bbp-fortran: piqpr16.f90
	$(F90) $(FFLAGS) $< -o $@
