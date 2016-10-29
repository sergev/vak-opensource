CC              = gcc

SVNVERS         = $(shell svnversion)
CFLAGS          = -Wall -g -O -I/opt/local/include -Ihidapi -DSVNVERSION='"$(SVNVERS)"'
LDFLAGS         = -g

# Linux
ifneq (,$(wildcard /lib/i386-linux-gnu))
    LIBS        += -lusb-1.0
    HIDSRC      = hidapi/hid-libusb.c
endif

# Mac OS X
ifneq (,$(wildcard /System/Library/Frameworks/CoreFoundation.framework))
    LIBS        += -framework IOKit -framework CoreFoundation
    HIDSRC      = hidapi/hid-mac.c
endif

PROG_OBJS       = ms2upgrade.o hid.o

all:            ms2upgrade

ms2upgrade:      $(PROG_OBJS)
		$(CC) $(LDFLAGS) -o $@ $(PROG_OBJS) $(LIBS)

hid.o:          $(HIDSRC)
		$(CC) $(CFLAGS) -c -o $@ $<

clean:
		rm -f *~ *.o core ms2upgrade

install:	ms2upgrade #ms2upgrade-ru.mo
		install -c -s ms2upgrade /usr/local/bin/ms2upgrade
###
