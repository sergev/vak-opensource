#DC		= gdc -Wall -g
DC              = dmd
DFLAGS		= -O
LDFLAGS		=
INSTDIR		= /usr/local

all:		hello decho man_or_boy

man_or_boy:	man_or_boy.o
		$(DC) $(LDFLAGS) man_or_boy.o -of=$@

decho:		decho.o
		$(DC) $(LDFLAGS) decho.o -of=$@

hello:		hello.o
		$(DC) $(LDFLAGS) hello.o -of=$@

clean:
		rm -f hello decho man_or_boy *.o *~ a.out

.SUFFIXES:	.d

.d.o:
		$(DC) $(DFLAGS) -c $<
