SYSTEMC         = /usr/local/systemc-2.3

CXX             = g++
CXXFLAGS        = -DSC_INCLUDE_FX -I$(SYSTEMC)/include -Wall -O3

# Linux
#CXX             += -m32 -pthread
#LIBS            = -static -L$(SYSTEMC)/lib-linux -lsystemc -lm

# Mac OS X
LIBS            = -L$(SYSTEMC)/lib-macosx386 -lsystemc -lm

PROGS           = simple_fifo
OBJS            = simple_fifo.o

all:		$(PROGS)
		size $(PROGS)

clean:
		rm -f $(PROGS) *.o

simple_fifo:    $(OBJS)
		$(CXX) -o $@ $(OBJS) $(LIBS)

run:
		LD_LIBRARY_PATH=$(SYSTEMC)/lib-linux ./simple_fifo
