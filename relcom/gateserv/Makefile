SHELL           = /bin/sh
CFLAGS          = -DSIGVOID
CC              = cc -g -Wall -O

OBJS            = daemon.o ut.o configure.o match.o header.o rfcdate.o

gateservd:      $(OBJS)
		$(CC) $(LDFLAGS) $(OBJS) -o $@

clean:
		rm -f *.o *.b core a.out gateservd

install:        gateservd gateserv.conf
		-mv -f /usr/libexec/gateservd /usr/libexec/gateservd~
		install -c -s -m 4775 -o news gateservd /usr/libexec/gateservd
		install -c -m 664 gateserv.conf /etc/gateserv.conf

###
daemon.o: config.h header.h
configure.o: config.h
header.o: header.h
