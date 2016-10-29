SHELL   = /bin/sh
DEST    = /usr/spool/infoserv
LDFLAGS =
LIBS    =
CC      = gcc -g -traditional
CFLAGS  = -O
LD      = $(CC)

QOBJS   = enqueue.o ut.o
QSRCS   = enqueue.c ut.c

BOBJS   = batcher.o ut.o
BSRCS   = batcher.c ut.c

SOBJS   = server.o ut.o
SSRCS   = server.c ut.c

DOBJS   = infoservd.o ut.o
DSRCS   = infoservd.c ut.c

LOBJS   = logmessage.o ut.o
LSRCS   = logmessage.c ut.c

ALL     = batcher enqueue server infoservd logmessage

all:    $(ALL)

server: $(SOBJS) $(LIBS)
	$(LD) $(LDFLAGS) $(SOBJS) $(LIBS) -o server

logmessage: $(LOBJS) $(LIBS)
	$(LD) $(LDFLAGS) $(LOBJS) $(LIBS) -o logmessage

enqueue: $(QOBJS) $(LIBS)
	$(LD) $(LDFLAGS) $(QOBJS) $(LIBS) -o enqueue

batcher: $(BOBJS) $(LIBS)
	$(LD) $(LDFLAGS) $(BOBJS) $(LIBS) -o batcher

infoservd: $(DOBJS) $(LIBS)
	$(LD) $(LDFLAGS) $(DOBJS) $(LIBS) -o infoservd

clean:
	rm -f *.o *.b .,* core a.out lint $(ALL)

install: instdirs $(DEST)/bin/batcher $(DEST)/bin/enqueue $(DEST)/bin/server\
	$(DEST)/bin/infoservd $(DEST)/bin/logmessage\
	$(DEST)/bin/startup $(DEST)/helpfile

instdirs: $(DEST) $(DEST)/bin $(DEST)/queue $(DEST)/db

$(DEST):
	mkdir $(DEST)

$(DEST)/bin:
	mkdir $(DEST)/bin

$(DEST)/queue:
	mkdir $(DEST)/queue

$(DEST)/db:
	mkdir $(DEST)/db

$(DEST)/helpfile: helpfile
	cp helpfile $(DEST)

$(DEST)/bin/infoservd: infoservd
	cp infoservd $(DEST)/bin
	strip $(DEST)/bin/infoservd

$(DEST)/bin/logmessage: logmessage
	cp logmessage $(DEST)/bin
	strip $(DEST)/bin/logmessage

$(DEST)/bin/startup: startup.sh
	cp startup.sh $(DEST)/bin/startup
	chmod +x $(DEST)/bin/startup

$(DEST)/bin/batcher: batcher
	cp batcher $(DEST)/bin
	strip $(DEST)/bin/batcher

$(DEST)/bin/enqueue: enqueue
	cp enqueue $(DEST)/bin
	strip $(DEST)/bin/enqueue

$(DEST)/bin/server: server
	cp server $(DEST)/bin
	strip $(DEST)/bin/server

###
batcher.o: config.h
commands.o: messages.h config.h
enqueue.o: config.h
logmessage.o: config.h
infoservd.o: config.h
reader.o: messages.h
server.o: messages.h config.h
ut.o: config.h
