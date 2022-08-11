VFLAGS          =
SRCS            = dflop.v

all:            dflop

dflop:          $(SRCS)
		iverilog $(VFLAGS) -o $@ $(SRCS)

clean:
		rm -f dflop
