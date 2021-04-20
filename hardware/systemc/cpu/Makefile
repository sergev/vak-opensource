# Use: module load systemc/2.3.1
SYSTEMC_INCLUDE ?= /usr/include
SYSTEMC_LIBDIR  ?= /usr/lib/x86_64-linux-gnu

OBJS            := Processor.o Memory.o example.o
EXEC            := example.exe
CXXFLAGS        := -g -O -Wall -Werror -I$(SYSTEMC_INCLUDE) -DPRINT_WHILE_RUN
LDFLAGS         := -g
LIBS            := -L$(SYSTEMC_LIBDIR) -lsystemc

all:            $(EXEC)

$(EXEC):        $(OBJS)
		g++ $(LDFLAGS) $^ $(LIBS) -o $@

clean:
		rm -f *.exe *.o

view:
		gtkwave wave.vcd &
