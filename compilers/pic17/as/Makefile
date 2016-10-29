CFLAGS		= -Wall -g -O
INSTDIR		= /usr/local

all:		as17 dis17 sim17 hello.hex

as17:		as.o
		$(CC) $(LDFLAGS) as.o -o as17

dis17:		dis.o
		$(CC) $(LDFLAGS) dis.o -o dis17

sim17:		sim.o
		$(CC) $(LDFLAGS) sim.o -o sim17

clean:
		rm -f *~ *.o a.out *.core as17 dis17 sim17 *.hex *.lst

install:	as17 dis17 sim17
		install -c as17 $(INSTDIR)/bin
		install -c dis17 $(INSTDIR)/bin
		install -c sim17 $(INSTDIR)/bin

.SUFFIXES:	.hex

.s.hex:
		as17 $<
