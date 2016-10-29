# Почтовый сервер биржи EXNET.
#
# Copyright (C) 1991 Сергей Вакуленко, <vak@kiae.su>.
# All rights reserved.

# Flags for Interactive Unix
# OSFLAGS         = -DSIGVOID
# LIBS            =

# Flags for Ultrix
# OSFLAGS         = -DSIGVOID
# LIBS            =

# Flags for 386bsd
OSFLAGS         = -DSIGVOID
LIBS            =

SHELL           = /bin/sh
DEST            = /usr/spool/trade
DEBUGFLAGS      = -O
LDFLAGS         = $(DEBUGFLAGS)
CFLAGS          = $(DEBUGFLAGS) $(OSFLAGS) -DBASETIME=`ptime`
CC              = cc
LD              = cc

QOBJS   = enqueue.o ut.o config.o
QSRCS   = enqueue.c ut.c config.c

BOBJS   = batcher.o ut.o config.o
BSRCS   = batcher.c ut.c config.c

SOBJS   = commands.o reader.o server.o ut.o glist.o lot.o config.o lang.o
SSRCS   = commands.c reader.c server.c ut.c glist.c lot.c config.c lang.c

DOBJS   = daemon.o ut.o config.o
DSRCS   = daemon.c ut.c config.c

XOBJS   = expire.o ut.o glist.o lot.o config.o lang.o
XSRCS   = expire.c ut.c glist.c lot.c config.c lang.c

ALL     = batcher enqueue server daemon expire

ALLLINT = batcher.ln enqueue.ln server.ln daemon.ln expire.ln

all:    ptime $(ALL)

ptime:	ptime.c
	$(CC) $(LDFLAGS) ptime.c -o ptime

lint:   $(ALLLINT)

server: $(SOBJS)
	$(LD) $(LDFLAGS) $(SOBJS) $(LIBS) -o server

expire: $(XOBJS)
	$(LD) $(LDFLAGS) $(XOBJS) $(LIBS) -o expire

enqueue: $(QOBJS)
	$(LD) $(LDFLAGS) $(QOBJS) $(LIBS) -o enqueue

batcher: $(BOBJS)
	$(LD) $(LDFLAGS) $(BOBJS) $(LIBS) -o batcher

daemon: $(DOBJS)
	$(LD) $(LDFLAGS) $(DOBJS) $(LIBS) -o daemon

clean:
	rm -f *.o *.b .,* core a.out lint $(ALL) $(ALLLINT) ptime

test:
	(echo PWD; echo LS; echo DIR; echo QUIT) | mail trade

install: instdirs $(DEST)/bin/batcher $(DEST)/bin/enqueue $(DEST)/bin/server\
	$(DEST)/bin/daemon $(DEST)/bin/expire $(DEST)/bin/startup $(DEST)/users\
	$(DEST)/admins $(DEST)/helpfile $(DEST)/configfile

instdirs: $(DEST) $(DEST)/bin $(DEST)/queue $(DEST)/ar $(DEST)/backup

$(DEST):
	mkdir $(DEST)
	chmod 700 $(DEST)

$(DEST)/bin:
	mkdir $(DEST)/bin
	chmod 700 $(DEST)/bin

$(DEST)/queue:
	mkdir $(DEST)/queue
	chmod 700 $(DEST)/queue

$(DEST)/ar:
	mkdir $(DEST)/ar
	chmod 700 $(DEST)/ar

$(DEST)/backup:
	mkdir $(DEST)/backup
	chmod 700 $(DEST)/backup

$(DEST)/helpfile: 
	cp helpfile $(DEST)
	chmod 600 $(DEST)/helpfile

$(DEST)/users: 
	cp users $(DEST)
	chmod 400 $(DEST)/users

$(DEST)/admins: 
	cp admins $(DEST)
	chmod 400 $(DEST)/admins

$(DEST)/configfile: 
	cp configfile $(DEST)
	chmod 600 $(DEST)/configfile

$(DEST)/bin/daemon: daemon
	cp daemon $(DEST)/bin
	strip $(DEST)/bin/daemon
	chmod 700 $(DEST)/bin/daemon

$(DEST)/bin/expire: expire
	cp expire $(DEST)/bin
	strip $(DEST)/bin/expire
	chmod 700 $(DEST)/bin/expire

$(DEST)/bin/startup: startup.sh
	cp startup.sh $(DEST)/bin/startup
	chmod 700 $(DEST)/bin/startup

$(DEST)/bin/batcher: batcher
	cp batcher $(DEST)/bin
	strip $(DEST)/bin/batcher
	chmod 700 $(DEST)/bin/batcher

$(DEST)/bin/enqueue: enqueue
	cp enqueue $(DEST)/bin
	strip $(DEST)/bin/enqueue
	chmod 700 $(DEST)/bin/enqueue

$(DEST)/bin/server: server
	cp server $(DEST)/bin
	strip $(DEST)/bin/server
	chmod 700 $(DEST)/bin/server

enqueue.ln: $(QSRCS)
	lint $(OSFLAGS) $(QSRCS) > enqueue.ln

batcher.ln: $(BSRCS)
	lint $(OSFLAGS) $(BSRCS) > batcher.ln

server.ln: $(SSRCS)
	lint $(OSFLAGS) $(SSRCS) > server.ln

daemon.ln: $(DSRCS)
	lint $(OSFLAGS) $(DSRCS) > daemon.ln

expire.ln: $(XSRCS)
	lint $(OSFLAGS) $(XSRCS) > expire.ln

###
batcher.o: config.h
commands.o: config.h server.h lang.h
config.o: config.h
daemon.o: config.h
enqueue.o: config.h
expire.o: config.h lang.h server.h
glist.o: config.h server.h
lot.o: config.h server.h
reader.o: server.h lang.h
server.o: config.h lang.h
ut.o: config.h
