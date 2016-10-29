CC		= gcc -Wall -g -Werror
CFLAGS 		= -O
PROG		= geeksynth
UNAME           = $(shell uname)

OBJS		= main.o midi-input.o

# For Mac OS X
ifeq ($(UNAME),Darwin)
    LDFLAGS	= -framework CoreAudio -framework CoreMIDI -framework CoreServices
    OBJS	+= macosx.o
endif

# For Linux, FreeBSD using SDL
ifeq ($(UNAME),Linux)
    CFLAGS 	+= -I/usr/include/SDL -I/usr/local/include/SDL
    LIBS	= -lSDL
    OBJS	+= sdl.o
endif

# For Windows
ifeq ($(UNAME),MinGW)
    LIBS	= -lwinmm
    OBJS	+= win32.o
endif

all:		$(PROG)

$(PROG):	$(OBJS)
		$(CC) $(LDFLAGS) $(OBJS) $(LIBS) -o $@

clean:
		rm -f *~ *.o $(PROG) $(PROG).exe

cleanall:	clean
		rm -rf qso *.mp3 *.zip
###
macosx.o: macosx.c audio.h
main.o: main.c audio.h midi.h
midi-input.o: midi-input.c midi.h
