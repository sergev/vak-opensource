CFLAGS		= -Wall -g -O
INSTDIR		= /usr/local

all:		as12 dis12 dis16

as12:		as12.o
		$(CC) $(LDFLAGS) as12.o -o as12

dis12:		dis12.o
		$(CC) $(LDFLAGS) dis12.o -o dis12

dis16:		dis16.o
		$(CC) $(LDFLAGS) dis16.o -o dis16

clean:
		rm -f *~ *.o a.out *.core as12 dis12 dis16

install:	as12 dis12
		install -c as12 $(INSTDIR)/bin
		install -c dis12 $(INSTDIR)/bin

.SUFFIXES:	.hex

.s.hex:
		as12 $<
