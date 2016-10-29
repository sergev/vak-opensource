CFLAGS          = -g -O -Wall -Werror
LDFLAGS         = -g
UNAME           = $(shell uname)

# Linux
ifeq ($(UNAME),Linux)
    LIBS        += -lasound
endif

# Mac OS X
ifeq ($(UNAME),Darwin)
    LIBS        += -framework CoreMIDI -framework CoreServices
    UNIV_ARCHS  = $(shell grep '^universal_archs' /opt/local/etc/macports/macports.conf)
    ifneq ($(findstring i386,$(UNIV_ARCHS)),)
        CC      += -arch i386
    endif
    ifneq ($(findstring x86_64,$(UNIV_ARCHS)),)
        CC      += -arch x86_64
    endif
endif

all:            testin testout #midiloop sound_playback

testin:         testin.o
		$(CC) $(LDFLAGS) -o $@ testin.o $(LIBS)

testout:        testout.o
		$(CC) $(LDFLAGS) -o $@ testout.o $(LIBS)

midiloop:       midiloop.o
		$(CC) $(LDFLAGS) -o $@ midiloop.o $(LIBS)

sound_playback: sound_playback.o
		$(CC) $(LDFLAGS) -o $@ sound_playback.o $(LIBS)

clean:
		rm -f *.o midiloop sound_playback testin testout
