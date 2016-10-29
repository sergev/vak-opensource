# @(#)Makefile	1.1 (Berkeley) 12/15/82

DESTDIR = /usr/SVSB/bin
SHELL   = /bin/sh
CC      = cc
LN      = ln

m4:	m4.o m4y.o
	$(CC) $(LDFLAGS) -o m4 m4.o m4y.o

install: m4
	cp m4 $(DESTDIR)/m4
	strip $(DESTDIR)/m4
	rm -f $$HOME/bin/vm4
	$(LN) $(DESTDIR)/m4 $$HOME/bin/vm4

clean:
	rm -f *.o *.b core a.out m4
