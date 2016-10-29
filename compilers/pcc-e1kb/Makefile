#
# Cross-C compiler for Elbrus-1-KB.
#
# Author: Serge Vakulenko
# Based on Portable CC by Johnson.
#
#
SHELL   = /bin/sh
INSTALL = /usr/SVSB
CFLAGS  = -M2l -LARGE
LDFLAGS = -M2l

DIRS    = h cfe cpp m4 mix mk # tst/libtst tst/libtst/tst0 stand

all:
	for i in $(DIRS);\
	do (cd $$i; $(MAKE) CFLAGS='$(CFLAGS)' LDFLAGS='$(LDFLAGS)'); done

instdirs: $$(HOME)/bin $(INSTALL) $(INSTALL)/bin $(INSTALL)/lib $(INSTALL)/include

install: instdirs
	for i in $(DIRS);\
	do (cd $$i; $(MAKE) install CFLAGS='$(CFLAGS)' LDFLAGS='$(LDFLAGS)' INSTALL=$(INSTALL)); done
	cd $$HOME/bin; rm -f vcc var vas vld vnm vdis vsize vstrip vlorder vccom
	ln $(INSTALL)/bin/cc $$HOME/bin/vcc
	ln $(INSTALL)/bin/ar $$HOME/bin/var
	ln $(INSTALL)/bin/as $$HOME/bin/vas
	ln $(INSTALL)/bin/ld $$HOME/bin/vld
	ln $(INSTALL)/bin/nm $$HOME/bin/vnm
	ln $(INSTALL)/bin/dis $$HOME/bin/vdis
	ln $(INSTALL)/bin/size $$HOME/bin/vsize
	ln $(INSTALL)/bin/strip $$HOME/bin/vstrip
	ln $(INSTALL)/bin/lorder $$HOME/bin/vlorder
	ln $(INSTALL)/lib/ccom $$HOME/bin/vccom

clean:
	rm -f *.b a.out core
	for i in $(DIRS);\
	do (cd $$i; $(MAKE) clean); done

uninstall:
	rm -r /usr/SVSB
	cd $$HOME/bin; rm -f vcc var vas vld vnm vdis vsize vstrip vlorder vccom

$(INSTALL):
	-mkdir $(INSTALL)

$$HOME/bin:
	-mkdir $$HOME/bin

$(INSTALL)/bin:
	-mkdir $(INSTALL)/bin

$(INSTALL)/lib:
	-mkdir $(INSTALL)/lib

$(INSTALL)/include:
	-mkdir $(INSTALL)/include
