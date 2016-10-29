CFLAGS		= -Wall -g -O
INSTDIR		= /usr/local

all:		as16 dis16

as16:		as.o
		$(CC) $(LDFLAGS) as.o -o as16

dis16:		dis.o
		$(CC) $(LDFLAGS) dis.o -o dis16

clean:
		rm -f *~ *.o a.out *.core as16 dis16

install:	as16 dis16
		install -c as16 $(INSTDIR)/bin
		install -c dis16 $(INSTDIR)/bin

.SUFFIXES:	.hex

.s.hex:
		as16 $<
