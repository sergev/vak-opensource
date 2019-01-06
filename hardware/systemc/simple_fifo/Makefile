# Use: module load systemc/2.3.1

CXX             = g++
CXXFLAGS        = -DSC_INCLUDE_FX -I$(SYSTEMC_INCLUDE) -Wall -O3
LIBS            = -L$(SYSTEMC_LIBDIR) -lsystemc -lm

PROGS           = simple_fifo
OBJS            = simple_fifo.o

all:		$(PROGS)
		size $(PROGS)

clean:
		rm -f $(PROGS) *.o

simple_fifo:    $(OBJS)
		$(CXX) -o $@ $(OBJS) $(LIBS)

run:
		./simple_fifo
