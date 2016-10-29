# Почтовый сервер новостей.
#
# Copyright (C) 1991 Сергей Вакуленко, <vak@kiae.su>.
# All rights reserved.

# Flags for general Unix
# OSFLAGS         =
# LIBS            =

# Flags for Interactive Unix
# OSFLAGS         = -DSIGVOID -DFILEMASK=31 -DSYSV
# LIBS            = -lPW -lcposix
# NETLIBS         = -lrpc -linet -lpt

# Flags for SVR4
# OSFLAGS         = -DSVR4 -DSIGVOID -DFILEMASK=31
# LIBS            =
# ALLOCA          = alloca.o
# If RPC and no GCC
# NETLIBS         = -lsocket -lnsl

# Flags for Xenix 386
# OSFLAGS         = -DXENIX -DFILEMASK=7
# LIBS            =
# ALLOCA          = alloca.o

# Flags for Xenix 286 - does not work yet
# OSFLAGS         = -M2l -DXENIX -DFILEMASK=7
# LIBS            =
# OSLDFLAGS       = -M2l
# ALLOCA          = alloca.o

# Flags for Ultrix
# OSFLAGS         = -DBSD -DSIGVOID -DFILEMASK=31
# LIBS            = -lcV

# Flags for FreeBSD
OSFLAGS         = -DBSD -DSIGVOID -DFILEMASK=31
LIBS            =

# Gnu compiler
# CC              = gcc -fpcc-struct-return -g

# Use RPC to connect to database manager
# DBOBJS          = rgroups.o rgroups_clnt.o rgroups_xdr.o ping.o
# NETALL          = rgroupd
# NETINSTALL      = $(DEST)/bin/rgroupd
# NETFLAGS        = -DREMOTEDB

# If no RPC
DBOBJS          = groups.o cdbm.o $(DBMLIB)
NETLIBS         =

SHELL           = /bin/sh
DEST            = /usr/spool/newsserv
DEBUGFLAGS      = -g
LDFLAGS         = $(DEBUGFLAGS) $(OSLDFLAGS)
CFLAGS          = $(NETFLAGS) $(DEBUGFLAGS) $(OSFLAGS) -Ibtree
DBMLIB          = btree/libbtree.a
#CFLAGS         = $(NETFLAGS) $(DEBUGFLAGS) $(OSFLAGS) -Igdbm
#DBMLIB         = gdbm/gdbm.a
LD              = $(CC)
CIFLAGS         =
CI              = ci
STRIP           = ls
#STRIP          = strip

SOBJS   = commands.o reader.o server.o ut.o configure.o lang.o submit.o\
	  groupext.o vdbm.o header.o send.o gclass.o compack.o rfcdate.o\
	  $(DBOBJS) version.o
SSRCS   = commands.c reader.c server.c ut.c configure.c lang.c submit.c\
	  groups.c vdbm.c header.c send.c gclass.c compack.c rfcdate.c\
	  version.c cdbm.c groupext.c

DOBJS   = daemon.o configure.o ut.o rfcdate.o groupext.o vdbm.o $(DBOBJS)
DSRCS   = daemon.c configure.c ut.c rfcdate.c cdbm.c

FOBJS   = newnews.o ut.o configure.o vdbm.o header.o send.o gclass.o\
	  lang.o compack.o rfcdate.o groupext.o $(DBOBJS)
FSRCS   = newnews.c ut.c configure.c groups.c vdbm.c header.c send.c gclass.c\
	  lang.c compack.c rfcdate.c cdbm.c groupext.c

NOBJS   = notify.o ut.o configure.o vdbm.o lang.o gclass.o rfcdate.o groupext.o\
	  $(DBOBJS)
NSRCS   = notify.c ut.c configure.c groups.c vdbm.c lang.c gclass.c rfcdate.c cdbm.c\
	  groupext.c

COBJS   = nsconvert.o vdbm.o configure.o ut.o rfcdate.o groupext.o $(DBOBJS)
CSRCS   = nsconvert.c groups.c vdbm.c configure.c ut.c rfcdate.c cdbm.c\
	  groupext.c

UOBJS   = nsadmin.o configure.o groupext.o ut.o rfcdate.o vdbm.o $(DBOBJS)
USRCS   = nsadmin.c groups.c vdbm.c configure.c ut.c rfcdate.c cdbm.c\
	  groupext.c

HOBJS   = nscheck.o groups.o vdbm.o configure.o ut.o rfcdate.o cdbm.o\
	  groupext.o $(DBMLIB)
HSRCS   = nscheck.c groups.c vdbm.c configure.c ut.c rfcdate.c cdbm.c\
	  groupext.c

ROBJS   = rgroups_proc.o rgroups_svc.o rgroups_xdr.o groups.o groupext.o\
	  cdbm.o vdbm.o configure.o ut.o rfcdate.o $(DBMLIB)

ALL     = $(DBMLIB) $(NETALL) nsadmin daemon newnews notify unbatch nsconvert\
	  nscheck server

ALLLINT = server.ln daemon.ln newnews.ln notify.ln unbatch.ln nsconvert.ln\
	  nsadmin.ln

all:    $(ALL)

$(DBMLIB):
#       cd gdbm; $(MAKE) CC="$(CC)" OSFLAGS="$(OSFLAGS)" DEBUGFLAGS="$(DEBUGFLAGS)" LIBS="$(LIBS)" ALLOCA="$(ALLOCA)"
	cd btree; $(MAKE) CC="$(CC)" OSFLAGS="$(OSFLAGS)" DEBUGFLAGS="$(DEBUGFLAGS)" LIBS="$(LIBS)" ALLOCA="$(ALLOCA)"

put:
	ci -u $(CIFLAGS) $(SRCS)

lint:   $(ALLLINT)

rgroupd: rgroups.h $(ROBJS)
	$(LD) $(LDFLAGS) -o rgroupd $(ROBJS) $(NETLIBS) $(LIBS)

rgroups_xdr.c: rgroups.x
	rpcgen -c rgroups.x -o rgroups_xdr.c

rgroups_clnt.c: rgroups.x
	rpcgen -l rgroups.x -o rgroups_clnt.c

rgroups.h: rgroups.x
	rpcgen -h rgroups.x -o rgroups.h

rgroups_svc.c: rgroups.x makesvc.sh mainsvc.c
	rpcgen -s tcp rgroups.x -o rgroups_svc.c
	sh makesvc.sh

server: $(DBMLIB) $(SOBJS)
	$(LD) $(LDFLAGS) $(SOBJS) $(NETLIBS) $(LIBS) -o server

daemon: $(DBMLIB) $(DOBJS)
	$(LD) $(LDFLAGS) $(DOBJS) $(NETLIBS) $(LIBS) -o daemon

newnews: $(DBMLIB) $(FOBJS)
	$(LD) $(LDFLAGS) $(FOBJS) $(NETLIBS) $(LIBS) -o newnews

notify: $(DBMLIB) $(NOBJS)
	$(LD) $(LDFLAGS) $(NOBJS) $(NETLIBS) $(LIBS) -o notify

nsadmin: $(DBMLIB) $(UOBJS)
	$(LD) $(LDFLAGS) $(UOBJS) $(NETLIBS) $(LIBS) -o nsadmin

nsconvert: $(DBMLIB) $(COBJS)
	$(LD) $(LDFLAGS) $(COBJS) $(NETLIBS) $(LIBS) -o nsconvert

nscheck: $(DBMLIB) $(HOBJS)
	$(LD) $(LDFLAGS) $(HOBJS) $(LIBS) -o nscheck

gclass: gclass.c ut.o
	$(CC) $(LDFLAGS) $(CFLAGS) -DDEBUG gclass.c ut.o $(LIBS) -o gclass

compack: compack.c
	$(CC) $(LDFLAGS) $(CFLAGS) -DTEST compack.c $(LIBS) -o compack

unbatch: unbatch.o compack.o
	$(CC) $(LDFLAGS) unbatch.o compack.o $(LIBS) -o unbatch

netclean:
	rm -f rgroupd rgroups_svc.c rgroups_xdr.c rgroups_clnt.c rgroups.h

clean:  netclean
	rm -f *.o *.b .,* core a.out lint $(ALL) $(ALLLINT)
#       cd gdbm; $(MAKE) clean
	cd btree; $(MAKE) clean

version.h: *.c
	sh mkversion.sh

test:
	(echo PWD; echo LS; echo DIR; echo QUIT) | mail newsserv

install: instdirs $(DEST)/bin/server $(DEST)/bin/daemon $(DEST)/bin/startup\
	$(DEST)/bin/killdaemon $(DEST)/bin/newnews $(DEST)/bin/notify\
	$(DEST)/groups_list $(DEST)/users $(DEST)/classes $(DEST)/help $(DEST)/config\
	$(DEST)/rhelp pub $(DEST)/bin/backup $(DEST)/bin/nsconvert\
	$(DEST)/bin/nsadmin $(DEST)/bin/nscheck $(DEST)/bin/mailmdr\
	$(NETINSTALL)
	-for i in $(DEST)/bin/nsadmin $(DEST)/bin/daemon $(DEST)/bin/notify\
	    $(DEST)/bin/newnews $(DEST)/bin/nscheck; do\
		chown newsserv $$i && chmod u+s $$i; done
	if [ -f $(DEST)/bin/rgroupd ]; then chown newsserv $(DEST)/bin/rgroupd;\
		 chmod u+s $(DEST)/bin/rgroupd; fi
	-chown newsserv $(DEST)/groups
	-chown newsserv $(DEST)/groups+
	-chown newsserv $(DEST)/queue
	-chown newsserv $(DEST)/.

instdirs: $(DEST) $(DEST)/bin $(DEST)/queue $(DEST)/pub

pub:    $(DEST)/pub/uunpack-dos $(DEST)/pub/unbatch-dos $(DEST)/pub/unbatch-386\
	$(DEST)/pub/uunpack-src $(DEST)/pub/unbatch-src\
	$(DEST)/pub/uunpack-286 $(DEST)/pub/unbatch-286

$(DEST):
	mkdir $(DEST)
	chmod 775 $(DEST)

$(DEST)/bin:
	mkdir $(DEST)/bin
	chmod 775 $(DEST)/bin

$(DEST)/queue:
	mkdir $(DEST)/queue
	chmod 775 $(DEST)/queue

$(DEST)/pub:
	mkdir $(DEST)/pub
	chmod 775 $(DEST)/pub

$(DEST)/pub/uunpack-src: uunpack-src
	cp uunpack-src $(DEST)/pub/uunpack-src

$(DEST)/pub/uunpack-dos: uunpack-dos
	cp uunpack-dos $(DEST)/pub/uunpack-dos

$(DEST)/pub/uunpack-286: uunpack-286
	cp uunpack-286 $(DEST)/pub/uunpack-286

$(DEST)/pub/unbatch-src: unbatch-src
	cp unbatch-src $(DEST)/pub/unbatch-src

$(DEST)/pub/unbatch-dos: unbatch-dos
	cp unbatch-dos $(DEST)/pub/unbatch-dos

$(DEST)/pub/unbatch-286: unbatch-286
	cp unbatch-286 $(DEST)/pub/unbatch-286

$(DEST)/pub/unbatch-386: unbatch-386
	cp unbatch-386 $(DEST)/pub/unbatch-386

$(DEST)/help: help
	cp help $(DEST)
	chmod 664 $(DEST)/help

$(DEST)/rhelp: rhelp
	cp rhelp $(DEST)
	chmod 664 $(DEST)/rhelp

$(DEST)/groups_list: groups_list
	cp groups_list $(DEST)
	chmod 664 $(DEST)/groups_list

$(DEST)/users: users
	cp users $(DEST)
	chmod 664 $(DEST)/users

$(DEST)/classes: classes
	cp classes $(DEST)
	chmod 664 $(DEST)/classes

$(DEST)/config:
	cp config $(DEST)
	chmod 664 $(DEST)/config

$(DEST)/bin/daemon: daemon
	-mv $(DEST)/bin/daemon $(DEST)/bin/daemon~
	cp daemon $(DEST)/bin
	${STRIP} $(DEST)/bin/daemon
	chmod 775 $(DEST)/bin/daemon

$(DEST)/bin/rgroupd: rgroupd
	-mv $(DEST)/bin/rgroupd $(DEST)/bin/rgroupd~
	cp rgroupd $(DEST)/bin
	${STRIP} $(DEST)/bin/rgroupd
	chmod 775 $(DEST)/bin/rgroupd

$(DEST)/bin/newnews: newnews
	-mv $(DEST)/bin/newnews $(DEST)/bin/newnews~
	cp newnews $(DEST)/bin
	${STRIP} $(DEST)/bin/newnews
	chmod 775 $(DEST)/bin/newnews

$(DEST)/bin/notify: notify
	-mv $(DEST)/bin/notify $(DEST)/bin/notify~
	cp notify $(DEST)/bin
	${STRIP} $(DEST)/bin/notify
	chmod 775 $(DEST)/bin/notify

$(DEST)/bin/nsadmin: nsadmin
	-mv $(DEST)/bin/nsadmin $(DEST)/bin/nsadmin~
	cp nsadmin $(DEST)/bin
	${STRIP} $(DEST)/bin/nsadmin
	chmod 775 $(DEST)/bin/nsadmin

$(DEST)/bin/nscheck: nscheck
	-mv $(DEST)/bin/nscheck $(DEST)/bin/nscheck~
	cp nscheck $(DEST)/bin
	${STRIP} $(DEST)/bin/nscheck
	chmod 775 $(DEST)/bin/nscheck

$(DEST)/bin/nsconvert: nsconvert
	-mv $(DEST)/bin/nsconvert $(DEST)/bin/nsconvert~
	cp nsconvert $(DEST)/bin
	${STRIP} $(DEST)/bin/nsconvert
	chmod 775 $(DEST)/bin/nsconvert

$(DEST)/bin/startup: startup.sh
	cp startup.sh $(DEST)/bin/startup
	chmod 775 $(DEST)/bin/startup

$(DEST)/bin/backup: backup.sh
	cp backup.sh $(DEST)/bin/backup
	chmod 775 $(DEST)/bin/backup

$(DEST)/bin/mailmdr: mailmdr.sh
	cp mailmdr.sh $(DEST)/bin/mailmdr
	chmod 775 $(DEST)/bin/mailmdr

$(DEST)/bin/killdaemon: killdaemon.sh
	cp killdaemon.sh $(DEST)/bin/killdaemon
	chmod 775 $(DEST)/bin/killdaemon

$(DEST)/bin/server: server
	-mv $(DEST)/bin/server $(DEST)/bin/server~
	cp server $(DEST)/bin
	${STRIP} $(DEST)/bin/server
	chmod 775 $(DEST)/bin/server

server.ln: $(SSRCS)
	lint $(CFLAGS) $(SSRCS) > server.ln

daemon.ln: $(DSRCS)
	lint $(CFLAGS) $(DSRCS) > daemon.ln

newnews.ln: $(FSRCS)
	lint $(CFLAGS) $(FSRCS) > newnews.ln

notify.ln: $(NSRCS)
	lint $(CFLAGS) $(NSRCS) > notify.ln

nsadmin.ln: $(USRCS)
	lint $(CFLAGS) $(USRCS) > nsadmin.ln

nsconvert.ln: $(CSRCS)
	lint $(CFLAGS) $(CSRCS) > nsconvert.ln

unbatch.ln: unbatch.c compack.c
	lint $(CFLAGS) unbatch.c compack.c > unbatch.ln

rgroups_clnt.o: rgroups.h
rgroups_svc.o: rgroups.h
rgroups_xdr.o: rgroups.h
###
cdbm.o: cdbm.h
commands.o: config.h server.h groups.h header.h lang.h
configure.o: config.h
daemon.o: config.h
gclass.o: config.h
groupext.o: vdbm.h groups.h config.h
groups.o: cdbm.h vdbm.h groups.h tagdefs.h config.h
header.o: header.h
lang.o: lang.h
newnews.o: config.h header.h groups.h lang.h lock.h
notify.o: config.h groups.h lang.h vdbm.h lock.h
nsadmin.o: groups.h
nscheck.o: groups.h vdbm.h tagdefs.h
nsconvert.o: groups.h
ping.o: rgroups.h
reader.o: server.h lang.h
rgroups.o: rgroups.h groups.h
rgroups_proc.o: rgroups.h groups.h config.h
send.o: config.h header.h groups.h lang.h
server.o: config.h lang.h header.h
submit.o: config.h header.h groups.h lang.h
ut.o: config.h
vdbm.o: vdbm.h
version.o: version.h
