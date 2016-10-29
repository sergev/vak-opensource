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

# This must point to the root of the VERILATOR kit
#VERILATOR_ROOT  := /usr/share/verilator
#export VERILATOR_ROOT

compile:        obj_dir/simx

run:            obj_dir/simx
		obj_dir/simx

config:
		verilator -V

obj_dir/simx:   obj_dir/Vtop.h
		[ -d obj_dir ] && $(MAKE) -C obj_dir -f ../Makefile_obj

obj_dir/Vtop.h: input.vc *.v
		verilator --cc -f input.vc top.v

clean:
		-rm -rf obj_dir *.log *.dmp *.vpd core
