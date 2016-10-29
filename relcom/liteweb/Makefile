#
# Lite HTTPD - WWW Hypertext Daemon with built-in interface to file archive.
#
# Copyright (C) 1994-95 Cronyx Ltd.
# Author: Serge Vakulenko, <vak@cronyx.ru>
#
CC      = gcc -g -Wall
CFLAGS  = -O
PROG    = liteweb
ALL     = $(PROG) lindex lfind

all:    $(ALL)

$(PROG): $(PROG).o match.o map.o date.o env.o
	$(CC) $(LDFLAGS) -o $(PROG) $(PROG).o match.o map.o date.o env.o -lutil

lfind:  lfind.o match.o
	$(CC) $(LDFLAGS) -static -o lfind lfind.o match.o

lindex: lindex.o vdbm.o
	$(CC) $(LDFLAGS) -o lindex lindex.o vdbm.o

clean:
	rm -f $(ALL) *.[ob] *~

install: all #mswintab.txt dostab.txt
	-mv /usr/local/etc/$(PROG) /usr/local/etc/$(PROG)~
	install -c -s $(PROG) /usr/local/etc/$(PROG)
	install -c -s lindex /usr/local/etc/lindex
	install -c -s lfind /usr/local/bin/lfind
	rm -f /usr/local/bin/index
	(cd /usr/local/bin && ln -s lfind index)
#	install -c -s lfind /pub/bin/lfind
#	rm -f /pub/bin/index
#	(cd /pub/bin && ln -s lfind index)
	@rm -f /usr/local/etc/mswintab.txt
	install -c -m 444 coding/mswintab.txt /usr/local/etc/mswintab.txt
	@rm -f /usr/local/etc/dostab.txt
	install -c -m 444 coding/dostab.txt /usr/local/etc/dostab.txt
	@rm -f /usr/local/etc/mactab.txt
	install -c -m 444 coding/mactab.txt /usr/local/etc/mactab.txt
	[ -d /var/db/liteweb ] || (mkdir /var/db/liteweb; chown nobody /var/db/liteweb)

###
lfind.o: lfind.c reg.h
$(PROG).o: $(PROG).c reg.h map.h
lindex.o: lindex.c
map.o: map.c map.h
match.o: match.c
mktab.o: mktab.c
vdbm.o: vdbm.c vdbm.h
