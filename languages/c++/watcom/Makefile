#
# Generic makefile for Watcom C/C++ compiler.
#
PROG    = cpuid
OBJS    = cpuid.o
CC      = wcl386
CXX     = wpp386
CXXFLAGS= -zq -xs -d1
CFLAGS  = -zq -d1

all:    $(PROG)

.c.o:
	$(CC) $(CFLAGS) $*

.cpp.o:
	$(CXX) $(CXXFLAGS) $*

clean:
	rm -f *.o *.lnk *.map $(PROG)

$(PROG): $(OBJS)
	(echo "NAME    $(PROG)"; \
	 echo "OPTION  quiet, eliminate, map, show"; \
	 echo "DEBUG   all"; \
	 for file in $(OBJS); do echo "FILE    $$file"; done; \
	) > linkpgm.lnk
	wlink @linkpgm.lnk
