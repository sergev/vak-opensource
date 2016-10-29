GHDL            = ghdl

all:            hello_world adder_tb

clean:
		rm -f *.o *.vcd work-obj93.cf hello_world adder_tb

hello_world:    hello.vhdl
		$(GHDL) -a hello.vhdl
		$(GHDL) -e hello_world

adder_tb:       adder.vhdl adder_tb.vhdl
		$(GHDL) -a adder.vhdl
		$(GHDL) -a adder_tb.vhdl
		$(GHDL) -e adder_tb
