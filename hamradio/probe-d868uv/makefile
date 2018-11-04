CC		= gcc

CFLAGS		= -g -O -Wall -Werror
LDFLAGS		= -g -s

OBJS		= probe-d868uv.o
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

all:		probe-d868uv.exe

probe-d868uv.exe: $(OBJS)
		$(CC) $(LDFLAGS) -o $@ $(OBJS) $(LIBS)

clean:
		rm -f *~ *.o core probe-d868uv

###
