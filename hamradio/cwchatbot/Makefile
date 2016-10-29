#CC		= i586-mingw32msvc-gcc

PROG            = test-keyer
CFLAGS		= -O -Wall
UNAME           = $(shell uname)

# Linux
ifeq ($(UNAME),Linux)
    CFLAGS 	+= -I/usr/include/SDL -I/usr/local/include/SDL
    LDLIBS	+= -lusb -lSDL
    AOBJS	+= audio-sdl.o
endif

# Mac OS X
ifeq ($(UNAME),Darwin)
    CFLAGS	+= -I/opt/local/include
    LDFLAGS	+= -framework CoreAudio
    LDLIBS	+= -L/opt/local/lib -lusb
    AOBJS	+= audio-macosx.o
endif

# Windows
ifeq ($(UNAME),MinGW)
    LDLIBS	+= -lwinmm
    AOBJS	+= audio-win32.o
endif

all:		$(PROG)

clean:
		rm -f $(PROG) *.o *~ a.out test-paddle test-keyer

test-paddle:    test-paddle.o paddle.o
		$(CC) $(LDFLAGS) test-paddle.o paddle.o -o $@ $(LDLIBS)

test-keyer:     test-keyer.o paddle.o keyer.o $(AOBJS)
		$(CC) $(LDFLAGS) test-keyer.o paddle.o keyer.o $(AOBJS) -o $@ $(LDLIBS)
