#       -DMSF           use 'fortran' key instead of function_ ()
#       -DCGI           use cgi library
#       -DLASERJET      enable laserjet prinring
#       -DEPSON         enable epson prinring

SHELL   = /bin/sh

# FreeBSD
LIBGRF  = libgrafor.a
CC      = gcc
F77     = gfortran
CFLAGS  = -O
FFLAGS  = -O
LDFLAGS =
LIBS    =

# 386/ix with Green Hills Fortran
# LIBGRF  = libgrafor.a
# CC      = gcc -fstrength-reduce -fpcc-struct-return -fwritable-strings
# F77     = gf77
# CFLAGS  = -O -DXWINDOWS
# FFLAGS  = -OM
# LDFLAGS = -s
# LIBS    = -lX11_s -linet -lpt -lc_s
# INSLIB  = /usr/greenhills/BIN/386/lib

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

# Xenix with NDP Fortran 386
# LIBGRF  = libgrafor.a
# F77     = mf77
# CFLAGS  = -O -DCGI
# FFLAGS  = -O
# LDFLAGS = -i
# LIBS    = -lccgi
# INSLIB  = /usr/microway/BIN/386/lib

# Xenix with Microsoft Fortran 286
# LIBGRF  = Llibgrafor.a
# F77     = cl
# CFLAGS  = -O -DMSF -DCGI -Ml2
# FFLAGS  = -Ox -FPi
# LDFLAGS = -i -Ml2
# LIBS    = -lccgi
# INSLIB  = /lib

all:	metaplot ../$(LIBGRF)

metaplot: metaplot.o glib.o
	$(CC) $(LDFLAGS) metaplot.o glib.o $(LIBS) -o metaplot

font:   font.o glib.o
	$(CC) $(LDFLAGS) font.o glib.o $(LIBS) -o font

../$(LIBGRF): glib.o ut.o low.o grinit.o
	ar r ../$(LIBGRF) glib.o ut.o low.o grinit.o

metaplot.o: metaplot.c

clean:
	rm -f *~ *.o metaplot

lint:   glib.c ut.c low.c
	lint -DCGI -DLASERJET -DEPSON glib.c ut.c low.c >lint

.f.o:
	$(F77) -c $(FFLAGS) $<

.SUFFIXES: .o .c .f .s
