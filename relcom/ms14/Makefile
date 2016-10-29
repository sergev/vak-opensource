# Default flags
OSFLAGS         =
LIBS            =
CC              = gcc -g -O -Wall

# Flags for SunOS with GCC and RIS
# LIBS            = -lRISA -lresolv

LIBS            = -L/usr/local/lib -lRISA

# Flags for Ultrix with GCC in System V mode
# OSFLAGS         = -DSYSTEM_FIVE
# LIBS            = -lcV

# Flags for Demos/32
# OSFLAGS         = -DSIGTYPE=int
# CC              = cc

SHELL           = /bin/sh
DEST            = /var/spool/mailserv
LDFLAGS         =
CFLAGS          = $(OSFLAGS)
LD              = $(CC)

DOBJS           = daemon.o ut.o configure.o match.o
DSRCS           = daemon.c ut.c configure.c match.c

SOBJS           = server.o commands.o reader.o ut.o configure.o header.o match.o ris.o
SSRCS           = server.c commands.c reader.c ut.c configure.c header.c match.c ris.c

LOBJS           = logmessage.o ut.o configure.o match.o
LSRCS           = logmessage.c ut.c configure.c match.c

IOBJS           = info.o ut.o match.o
ISRCS           = info.c ut.c match.c

ALL             = daemon server info filelist filesize logmessage

ALLLINT         = daemon.ln server.ln info.ln filelist.ln\
		  filesize.ln logmessage.ln

all:    $(ALL)

lint:   $(ALLLINT)

server: $(SOBJS)
	$(LD) $(LDFLAGS) $(SOBJS) $(LIBS) -o server

risdebug: ris.c
	$(CC) $(LDFLAGS) $(CFLAGS) -DDEBUG ris.c $(LIBS) -o risdebug

logmessage: $(LOBJS)
	$(LD) $(LDFLAGS) $(LOBJS) $(LIBS) -o logmessage

daemon: $(DOBJS)
	$(LD) $(LDFLAGS) $(DOBJS) $(LIBS) -o daemon

info: $(IOBJS)
	$(LD) $(LDFLAGS) $(IOBJS) $(LIBS) -o info

filesize: filesize.o
	$(LD) $(LDFLAGS) filesize.o $(LIBS) -o filesize

filelist: filelist.o
	$(LD) $(LDFLAGS) filelist.o $(LIBS) -o filelist

clean:
	rm -f *.o *.b .,* core a.out lint risdebug $(ALL) $(ALLLINT)

test:
	(echo PWD; echo LS; echo DIR; echo QUIT) | mail mailserv

install: instdirs $(DEST)/bin/server $(DEST)/users $(DEST)/paths\
	$(DEST)/bin/mailservd $(DEST)/bin/uusplit $(DEST)/bin/logmessage\
	$(DEST)/bin/filesize $(DEST)/help $(DEST)/public/bin/info\
	$(DEST)/bin/bsplit $(DEST)/bin/arlist $(DEST)/config\
	$(DEST)/bin/info $(DEST)/bin/filelist $(DEST)/bin/startup

instdirs: $(DEST) $(DEST)/bin $(DEST)/queue $(DEST)/public

$(DEST):
	mkdir -p -m 775 $(DEST)
	chown mailserv.arch $(DEST)

$(DEST)/bin:
	mkdir -p -m 775 $(DEST)/bin
	chown mailserv.arch $(DEST)/bin

$(DEST)/queue:
	mkdir -p -m 775 $(DEST)/queue
	chown mailserv.arch $(DEST)/queue

$(DEST)/public:
#       mkdir -p -m 775 $(DEST)/public
#       chown mailserv.arch $(DEST)/public

$(DEST)/help: help
	install -c -m 664 -o mailserv -g arch help $(DEST)

$(DEST)/config:
	install -c -m 664 -o mailserv -g arch config $(DEST)

$(DEST)/users: users
	install -c -m 664 -o mailserv -g arch users $(DEST)

$(DEST)/paths: paths
	install -c -m 664 -o mailserv -g arch paths $(DEST)

$(DEST)/bin/mailservd: daemon
	-mv -f $(DEST)/bin/mailservd $(DEST)/bin/mailservd~
	install -c -s -m 4775 -o mailserv -g arch daemon $(DEST)/bin/mailservd

$(DEST)/bin/logmessage: logmessage
	-mv -f $(DEST)/bin/logmessage $(DEST)/bin/logmessage~
	install -c -s -m 775 -o mailserv -g arch logmessage $(DEST)/bin

$(DEST)/bin/info: info
	-mv -f $(DEST)/bin/info $(DEST)/bin/info~
	install -c -s -m 775 -o mailserv -g arch info $(DEST)/bin

$(DEST)/public/bin/info: info
#        -mv -f $(DEST)/public/bin/info $(DEST)/public/bin/info~
#        install -c -s -m 775 -o mailserv -g arch info $(DEST)/public/bin

$(DEST)/bin/filesize: filesize
	-mv -f $(DEST)/bin/filesize $(DEST)/bin/filesize~
	install -c -s -m 775 -o mailserv -g arch filesize $(DEST)/bin

$(DEST)/bin/filelist: filelist
	-mv -f $(DEST)/bin/filelist $(DEST)/bin/filelist~
	install -c -s -m 775 -o mailserv -g arch filelist $(DEST)/bin

$(DEST)/bin/server: server
	-mv -f $(DEST)/bin/server $(DEST)/bin/server~
	install -c -s -m 775 -o mailserv -g arch server $(DEST)/bin

$(DEST)/bin/uusplit: uusplit.sh
	install -c -m 775 -o mailserv -g arch uusplit.sh $(DEST)/bin/uusplit

$(DEST)/bin/getindex: getindex.sh
	install -c -m 775 -o mailserv -g arch getindex.sh $(DEST)/bin/getindex

$(DEST)/bin/loadinfo: loadinfo.sh
	install -c -m 775 -o mailserv -g arch loadinfo.sh $(DEST)/bin/loadinfo

$(DEST)/bin/filefind: filefind.sh
	install -c -m 775 -o mailserv -g arch filefind.sh $(DEST)/bin/filefind

$(DEST)/bin/bsplit: bsplit.sh
	install -c -m 775 -o mailserv -g arch bsplit.sh $(DEST)/bin/bsplit

$(DEST)/bin/buildindex: buildindex.sh
	install -c -m 775 -o mailserv -g arch buildindex.sh $(DEST)/bin/buildindex

$(DEST)/bin/backup: backup.sh
	install -c -m 775 -o mailserv -g arch backup.sh $(DEST)/bin/backup

$(DEST)/bin/printindex: printindex.sh
	install -c -m 775 -o mailserv -g arch printindex.sh $(DEST)/bin/printindex

$(DEST)/bin/mkfullindex: mkfullindex.sh
	install -c -m 775 -o mailserv -g arch mkfullindex.sh $(DEST)/bin/mkfullindex

$(DEST)/bin/arlist: arlist.sh
	install -c -m 775 -o mailserv -g arch arlist.sh $(DEST)/bin/arlist

$(DEST)/bin/startup: startup.sh
	install -c -m 775 -o mailserv -g arch startup.sh $(DEST)/bin/startup

daemon.ln: $(DSRCS)
	lint $(OSFLAGS) $(DSRCS) > daemon.ln

server.ln: $(SSRCS)
	lint $(OSFLAGS) $(SSRCS) > server.ln

logmessage.ln: $(LSRCS)
	lint $(OSFLAGS) $(LSRCS) > logmessage.ln

filelist.ln: filelist.c
	lint $(OSFLAGS) filelist.c > filelist.ln

###
commands.o: commands.c server.h config.h ut.h match.h
configure.o: configure.c ut.h config.h
daemon.o: daemon.c config.h ut.h
filelist.o: filelist.c 
filesize.o: filesize.c 
header.o: header.c header.h
info.o: info.c match.h ut.h
logmessage.o: logmessage.c config.h ut.h
match.o: match.c match.h
reader.o: reader.c server.h ut.h
ris.o: ris.h
server.o: server.c server.h config.h header.h ut.h
ut.o: ut.c ut.h match.h
