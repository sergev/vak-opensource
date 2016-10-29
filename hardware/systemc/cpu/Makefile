#SYSTEMC_HOME    := /usr/local/systemc-2.3

OBJS            := Processor.o Memory.o example.o
EXEC            := example.exe
CXXFLAGS        := -g -O -Wall -Werror -I$(SYSTEMC_HOME)/include -DPRINT_WHILE_RUN
LDFLAGS         := -g
LIBS            :=

# Linux
#LIBS            += -Wl,-Bstatic -L$(SYSTEMC_HOME)/lib-macosx386 -lsystemc -Wl,-Bdynamic -lpthread

# Mac OS X
LIBS            += -L$(SYSTEMC_HOME)/lib-macosx64 -lsystemc

all:            $(EXEC)

$(EXEC):        $(OBJS)
		g++ $(LDFLAGS) $^ $(LIBS) -o $@

clean:
		rm -f *.exe *.o

view:
		gtkwave wave.vcd &
