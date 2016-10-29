VFLAGS          =
SRCS            = example.v

all:            example

example:        $(SRCS)
		iverilog $(VFLAGS) -o $@ $(SRCS)

clean:
		rm -f example
