# Flags for Interactive Unix with GCC
# OSFLAGS         = -DSIGVOID
# LIBS            =
# CC              = gcc -g

# Flags for Ultrix with GCC in System V mode
OSFLAGS         = -DSIGVOID
LIBS            = -lcV
CC              = gcc -g

# Flags for Ultrix with GCC in BSD mode
OSFLAGS         = -DSIGVOID -DBSD
LIBS            =
CC              = gcc -g

# Flags for Demos/32
# OSFLAGS         = -DBSD
# LIBS            =
# CC              = cc

SHELL           = /bin/sh
DEST            = /usr/spool/mailnews
LDFLAGS         =
CFLAGS          = -O $(OSFLAGS)
LD              = $(CC)

DOBJS   = daemon.o ut.o config.o
DSRCS   = daemon.c ut.c config.c

ALL     = daemon

ALLLINT = daemon.ln

all:    $(ALL)

lint:   $(ALLLINT)

daemon: $(DOBJS)
	$(LD) $(LDFLAGS) $(DOBJS) $(LIBS) -o daemon

clean:
	rm -f *.o *.b .,* core a.out lint $(ALL) $(ALLLINT)

test:
	(echo LIST) | mail news

install: instdirs $(DEST)/bin/daemon $(DEST)/bin/startup $(DEST)/configfile

instdirs: $(DEST) $(DEST)/bin $(DEST)/queue

$(DEST):
	mkdir $(DEST)
	chmod 775 $(DEST)

$(DEST)/bin:
	mkdir $(DEST)/bin
	chmod 775 $(DEST)/bin

$(DEST)/queue:
	mkdir $(DEST)/queue
	chmod 775 $(DEST)/queue

$(DEST)/configfile: configfile
	cp configfile $(DEST)
	chmod 664 $(DEST)/configfile

$(DEST)/bin/daemon: daemon
	cp daemon $(DEST)/bin
	strip $(DEST)/bin/daemon
	chmod 775 $(DEST)/bin/daemon

$(DEST)/bin/startup: startup.sh
	cp startup.sh $(DEST)/bin/startup
	chmod 775 $(DEST)/bin/startup

daemon.ln: $(DSRCS)
	lint $(OSFLAGS) $(DSRCS) > daemon.ln

###
config.o: config.h
daemon.o: config.h
ut.o: config.h
