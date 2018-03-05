CFLAGS		= -Wall -g -O
INSTDIR		= /usr/local

all:		as12 dis12

as12:		as.o
		$(CC) $(LDFLAGS) as.o -o as12

dis12:		dis.o
		$(CC) $(LDFLAGS) dis.o -o dis12

clean:
		rm -f *~ *.o a.out *.core as12 dis12

install:	as12 dis12
		install -c as12 $(INSTDIR)/bin
		install -c dis12 $(INSTDIR)/bin

.SUFFIXES:	.hex

.s.hex:
		as12 $<
