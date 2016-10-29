
ALL             = simx

all:            $(ALL)

clean:
		-rm -rf simx obj *.log *.dmp *.vpd core *~ *.vcd

run:
		LD_LIBRARY_PATH=/usr/local/mips/systemc-2.3.0/lib-linux \
		./simx

simx:           obj/Vcounter.h *.v *.cpp
		[ -d obj ] && $(MAKE) -C obj -f ../Makefile-counter ../simx

obj/Vcounter.h: counter.v
		SYSTEMC_INCLUDE=/usr/local/mips/systemc-2.3.0/include \
                SYSTEMC_LIBDIR=/usr/local/mips/systemc-2.3.0/lib-linux \
                verilator --sc -f verilator.options -Mdir obj counter.v
