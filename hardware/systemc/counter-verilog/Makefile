# Use: module load systemc/2.3.1

ALL             = simx

all:            $(ALL)

clean:
		-rm -rf simx obj *.log *.dmp *.vpd core *~ *.vcd

run:
		./simx

simx:           obj/Vcounter.h *.v *.cpp
		[ -d obj ] && $(MAKE) -C obj -f ../Makefile-counter ../simx

obj/Vcounter.h: counter.v
		verilator --sc -f verilator.options -Mdir obj counter.v
