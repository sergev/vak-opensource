SYSTEMC         = /usr/local/mips/systemc-2.3.0

CXX             = g++ -m32 -pthread
CXXFLAGS        = -DSC_INCLUDE_FX -I$(SYSTEMC)/include -Wall -O3
LIBS            = -static -L$(SYSTEMC)/lib-linux -lsystemc -lm

PROGS           = counter-tb
OBJS            = counter-tb.o

all:		$(PROGS)
		size $(PROGS)

clean:
		rm -f $(PROGS) *.o *.vcd

counter-tb:     $(OBJS)
		$(CXX) -o $@ $(OBJS) $(LIBS)

run:
		LD_LIBRARY_PATH=$(SYSTEMC)/lib-linux ./counter-tb
