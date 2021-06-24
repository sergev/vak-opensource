#
# DESCRIPTION: Verilator Example: Makefile for inside source directory
#
# This calls the object directory makefile.  That allows the objects to
# be placed in the "current directory" which simplifies the Makefile.
#
# Copyright 2003-2011 by Wilson Snyder. This program is free software; you can
# redistribute it and/or modify it under the terms of either the GNU
# Lesser General Public License Version 3 or the Perl Artistic License
# Version 2.0.
#
ALL             = obj-datapath/simx obj-alu/simx obj-memory/simx obj-regfile/simx

all compile:    $(ALL)

clean:
		-rm -rf simx obj-* *.log *.dmp *.vpd core *~

run:
#		./simx tests/test00.out
		./simx tests/test01.out

#
# alu
#
obj-alu/simx:   obj-alu/Valu.h *.h *.cpp
		[ -d obj-alu ] && $(MAKE) -C obj-alu -f ../Makefile-alu

obj-alu/Valu.h: alu.v
		verilator --cc -f verilator.options -Mdir obj-alu alu.v

#
# regfile
#
obj-regfile/simx: obj-regfile/Vregfile.h *.h *.cpp
		[ -d obj-regfile ] && $(MAKE) -C obj-regfile -f ../Makefile-regfile

obj-regfile/Vregfile.h: regfile.v
		verilator --cc -f verilator.options -Mdir obj-regfile regfile.v

#
# memory
#
obj-memory/simx: obj-memory/Vmemory.h *.h *.cpp
		[ -d obj-memory ] && $(MAKE) -C obj-memory -f ../Makefile-memory

obj-memory/Vmemory.h: memory.v
		verilator --cc -f verilator.options -Mdir obj-memory memory.v

#
# datapath
#
obj-datapath/simx: obj-datapath/Vdatapath.h *.h *.cpp
		[ -d obj-datapath ] && $(MAKE) -C obj-datapath -f ../Makefile-datapath
		cp $@ .

obj-datapath/Vdatapath.h: *.v
		verilator --cc -f verilator.options -Mdir obj-datapath datapath.v
