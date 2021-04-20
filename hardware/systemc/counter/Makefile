# Use: module load systemc/2.3.1
SYSTEMC_INCLUDE ?= /usr/include
SYSTEMC_LIBDIR  ?= /usr/lib/x86_64-linux-gnu

CXX             = g++ -pthread
CXXFLAGS        = -DSC_INCLUDE_FX -I$(SYSTEMC_INCLUDE) -Wall -O3
LIBS            = -L$(SYSTEMC_LIBDIR) -lsystemc -lm

PROGS           = counter-tb
OBJS            = counter-tb.o

all:		$(PROGS)
		size $(PROGS)

clean:
		rm -f $(PROGS) *.o *.vcd

counter-tb:     $(OBJS)
		$(CXX) -o $@ $(OBJS) $(LIBS)

run:
		./counter-tb
