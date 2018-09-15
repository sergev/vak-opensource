CC              = gcc

UNAME           = $(shell uname)
CFLAGS          = -Wall -g -O -Ihidapi/hidapi
LDFLAGS         = -g
CCARCH          =

# Linux
ifeq ($(UNAME),Linux)
    LIBS        += -Wl,-Bstatic -lusb-1.0 -Wl,-Bdynamic -lpthread -ludev
    HIDLIB      = hidapi/libusb/.libs/libhidapi-libusb.a
endif

# Mac OS X
ifeq ($(UNAME),Darwin)
    LIBS        += -framework IOKit -framework CoreFoundation
    HIDLIB      = hidapi/mac/.libs/libhidapi.a
    UNIV_ARCHS  = $(shell grep '^universal_archs' /opt/local/etc/macports/macports.conf)
    ifneq ($(findstring i386,$(UNIV_ARCHS)),)
        CCARCH  += -arch i386
    endif
    ifneq ($(findstring x86_64,$(UNIV_ARCHS)),)
        CCARCH  += -arch x86_64
    endif
    CC          += $(CCARCH)
endif

PROG_OBJS       = probe-rd5r.o $(HIDLIB)

all:            probe-rd5r

probe-rd5r:     $(PROG_OBJS)
		$(CC) $(LDFLAGS) -o $@ $(PROG_OBJS) $(LIBS)

clean:
		rm -f *~ *.o core probe-rd5r adapter-mpsse pic32prog.po hidapi/ar-lib hidapi/compile
		if [ -f hidapi/Makefile ]; then make -C hidapi clean; fi

install:	probe-rd5r #pic32prog-ru.mo
		install -c -s probe-rd5r /usr/local/bin/probe-rd5r
#		install -c -m 444 pic32prog-ru.mo /usr/local/share/locale/ru/LC_MESSAGES/pic32prog.mo

hidapi/hidapi/hidapi.h:
		git submodule update --init

$(HIDLIB):      hidapi/hidapi/hidapi.h
		if [ ! -f hidapi/configure ]; then cd hidapi && ./bootstrap; fi
		cd hidapi && ./configure --enable-shared=no CFLAGS='$(CCARCH)'
		make -C hidapi

###
probe-rd5r.o: probe-rd5r.c hidapi/hidapi/hidapi.h
