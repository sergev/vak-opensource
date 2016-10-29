CC              = gcc
CFLAGS          = -Wall -g -O
LDFLAGS         = -g
UNAME           = $(shell uname)

# Linux
ifeq ($(UNAME),Linux)
    LIBS        += -ludev
endif

# Mac OS X
ifeq ($(UNAME),Darwin)
    LIBS        += -framework IOKit -framework CoreFoundation
    UNIV_ARCHS  = $(shell grep '^universal_archs' /opt/local/etc/macports/macports.conf)
    ifneq ($(findstring i386,$(UNIV_ARCHS)),)
        CC      += -arch i386
    endif
    ifneq ($(findstring x86_64,$(UNIV_ARCHS)),)
        CC      += -arch x86_64
    endif
endif

all:            sdwriter

sdwriter:       sdwriter.o
		$(CC) $(LDFLAGS) -o $@ $< $(LIBS)

clean:
		rm -f *~ *.o sdwriter

install:	sdwriter
		install -c -s sdwriter /usr/local/bin/sdwriter
###
