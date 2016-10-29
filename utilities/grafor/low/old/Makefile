#       -DMSF           use 'fortran' key instead of function_ ()
#       -DCGI           use cgi library
#       -DLASERJET      enable laserjet prinring
#       -DEPSON         enable epson prinring

SHELL   = /bin/sh

# 386/ix with Green Hills Fortran
LIBGRF  = libgrafor.a
CC      = cc
F77     = gf77
CFLAGS  = -O
FFLAGS  = -OM
LDFLAGS =
LIBS    =
INSLIB  = /usr/greenhills/BIN/386/lib

# Vax under Ultrix
# LIBGRF  = libgrafor.a
# F77     = f77
# CFLAGS  = -O
# FFLAGS  = -O
# LDFLAGS =
# LIBS    =
# INSLIB  = /lib

# Convex under BSD 4.3
# LIBGRF  = libgrafor.a
# F77     = fc
# CFLAGS  = -O
# FFLAGS  = -O2
# LDFLAGS =
# LIBS    =
# INSLIB  = /lib

# Xenix with Microsoft Fortran 286
# LIBGRF  = Llibgrafor.a
# F77     = cl
# CFLAGS  = -O -DMSF -DCGI -Ml2
# FFLAGS  = -Ox -FPi
# LDFLAGS = -i -Ml2
# LIBS    = -lccgi
# INSLIB  = /lib

../$(LIBGRF): glib.o ut.o low.o grinit.o
	ar r ../$(LIBGRF) glib.o ut.o low.o grinit.o

metaplot: metaplot.o glib.o
	$(CC) $(LDFLAGS) metaplot.o glib.o $(LIBS) -o metaplot

metaplot.o: metaplot.c

clean:
	rm -f *.b *.o

lint:   glib.c ut.c low.c
	lint -DCGI -DLASERJET -DEPSON glib.c ut.c low.c >lint

.f.o:
	$(F77) -c $(FFLAGS) $<

.SUFFIXES: .o .c .f .s
