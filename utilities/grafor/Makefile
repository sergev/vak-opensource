#       -DMSF           use 'fortran' key instead of function_ ()
#       -DCGI           use cgi library

SHELL   = /bin/sh
HFLAGS  = -Thpjr12
LIBCGI  = /usr/lib/Llibccgi.a
MAKE    = make

# FreeBSD
LIBGRF  = libgrafor.a
F77     = gfortran
CFLAGS  = -O
FFLAGS  = -O
LDFLAGS =
LIBS    =
INSLIB  = /usr/lib

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
# CFLAGS  = -O -DMSF -Ml -Me -M2
# FFLAGS  = -Ox -FPi
# LDFLAGS = -Ml -i -M2
# LIBS    = -lccgi
# INSLIB  = /lib

all:
	(cd ftn; $(MAKE) F77="$(F77)" FFLAGS="$(FFLAGS)")
	(cd low; $(MAKE) F77="$(F77)" FFLAGS="$(FFLAGS)" CFLAGS="$(CFLAGS)")
	ranlib $(LIBGRF)
	(cd tst; $(MAKE) F77="$(F77)" FFLAGS="$(FFLAGS)")

install: all $(INSLIB)/$(LIBGRF)
	cp $(LIBGRF) $(INSLIB)
	ranlib $(INSLIB)/$(LIBGRF)

metaplot:
	(cd low; $(MAKE) metaplot)

clean:
	(cd ftn; make clean)
	(cd low; make clean)
	(cd tst; make clean)
	rm -f *~ *.a

.f.o:
	$(F77) -c $(FFLAGS) $<

.man.hp:
	hroff $(HFLAGS) hp.nr tmac.an $? | hpflt >$@

.man.lp:
	hroff -man $? >$@

.SUFFIXES: .me .hp .lp .man .o .c .f .s
