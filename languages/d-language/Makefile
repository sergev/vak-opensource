DC		= gdc -Wall -g
DC              = dmd
DFLAGS		= -O
LDFLAGS		=
INSTDIR		= /usr/local

all:		decho sample mk_black

mk_black:	mk_black.o
		$(DC) $(LDFLAGS) mk_black.o -of$@

decho:		decho.o
		$(DC) $(LDFLAGS) decho.o -of$@

sample:		sample.o getopt.o
		$(DC) $(LDFLAGS) sample.o getopt.o -of$@

clean:
		rm -f decho sample *.o *~ a.out

.SUFFIXES:	.d

.d.o:
		$(DC) $(DFLAGS) -c $<
