GHDL            = ghdl #--ieee=synopsys -fexplicit

all:            dlx_test_behaviour

clean:
		rm -rf *.o *.vcd work dlx_test_behaviour dlx.out

dlx_test_behaviour: *.vhdl
		mkdir -p work
		$(GHDL) -i --workdir=work *.vhdl
		$(GHDL) -m --workdir=work dlx_test_behaviour

run:            dlx_test_behaviour test_loop.out
		cp test_loop.out dlx.out
		$(GHDL) -r dlx_test_behaviour --assert-level=note
