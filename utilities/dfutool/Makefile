CXX             = g++

CXXFLAGS	= -g -O -Wall -Werror
LDFLAGS		= -g -s

#OBJS            = DfuSeCommand.o StdAfx.o
OBJS            = connect.o
LIBS            = -lsetupapi

# Compiling Windows binary from Linux
ifeq (/usr/bin/i586-mingw32msvc-gcc,$(wildcard /usr/bin/i586-mingw32msvc-gcc))
    CXX         = i586-mingw32msvc-g++
endif
ifeq (/usr/bin/i686-w64-mingw32-gcc,$(wildcard /usr/bin/i686-w64-mingw32-gcc))
    CXX         = i686-w64-mingw32-g++
endif


all:            dfuse

dfuse:          $(OBJS)
	        $(CXX) $(LDFLAGS) $(OBJS) $(LIBS) -o $@

clean:
		rm -f dfuse *.o
