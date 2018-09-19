CC		= gcc

CFLAGS		= -g -O -Wall -Werror
LDFLAGS		= -g -s

OBJS		= probe-rd5r.o
LIBS            = -lhid -lsetupapi

# Compiling Windows binary from Linux
ifeq (/usr/bin/i586-mingw32msvc-gcc,$(wildcard /usr/bin/i586-mingw32msvc-gcc))
    CC          = i586-mingw32msvc-gcc
endif
ifeq (/usr/bin/i686-w64-mingw32-gcc,$(wildcard /usr/bin/i686-w64-mingw32-gcc))
    CC          = i686-w64-mingw32-gcc
endif

# Compiling Windows binary from Mac OS
ifeq (/usr/local/bin/i686-w64-mingw32-gcc,$(wildcard /usr/local/bin/i686-w64-mingw32-gcc))
    CC          = i686-w64-mingw32-gcc
endif

all:		probe-rd5r.exe

probe-rd5r.exe:	$(OBJS)
		$(CC) $(LDFLAGS) -o $@ $(OBJS) $(LIBS)

clean:
		rm -f *~ *.o core probe-rd5r

###
