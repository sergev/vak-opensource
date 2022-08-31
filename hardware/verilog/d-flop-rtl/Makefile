VFLAGS          =
ALL             = dflop dflop3

all:            $(ALL)

dflop:          dflop.v
		iverilog $(VFLAGS) -o $@ $<

dflop3:         dflop3.v
		iverilog $(VFLAGS) -o $@ $<

clean:
		rm -f $(ALL)
