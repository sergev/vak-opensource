CFLAGS		= -Wall -g -O
INSTDIR		= /usr/local

all:		as-tesei dis-tesei

as-tesei:	as.o
		$(CC) $(LDFLAGS) as.o -o as-tesei

dis-tesei:	dis.o
		$(CC) $(LDFLAGS) dis.o -o dis-tesei

clean:
		rm -f *.[ob] *~ a.out *.core as-tesei dis-tesei sim-tesei *.hex

install:	as-tesei #dis-tesei sim-tesei
		install -c as-tesei $(INSTDIR)/bin
		install -c dis-tesei $(INSTDIR)/bin

.SUFFIXES:	.mic .hex

.mic.hex:
		./as-tesei $<
