CC              = gcc
CFLAGS          = -Wall -g -O -DMINGW32
LDFLAGS         =

# Windows
LIBS            += -lsetupapi

PROG_OBJS       = usbfind.o

# Compiling Windows binary from Linux
ifeq (/usr/bin/i586-mingw32msvc-gcc,$(wildcard /usr/bin/i586-mingw32msvc-gcc))
    CC          = i586-mingw32msvc-gcc
endif
ifeq (/usr/bin/i686-w64-mingw32-gcc,$(wildcard /usr/bin/i686-w64-mingw32-gcc))
    CC          = i686-w64-mingw32-gcc
endif

all:            usbfind.exe

clean:
		rm -f *.o *.exe

usbfind.exe:    $(PROG_OBJS)
		$(CC) $(LDFLAGS) -o $@ $(PROG_OBJS) $(LIBS)

###
