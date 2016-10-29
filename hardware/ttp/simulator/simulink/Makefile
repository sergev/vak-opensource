VPATH		= . ..
CFLAGS		= -I..

all:		.deps ctlr.mexglx bus.mexglx node0.mexglx node1.mexglx

run:
		matlab -nosplash -desktop -r "open cluster2.mdl" &
#		matlab -nosplash -desktop -r "open cpu_ctlr.mdl" &

clean:
		rm -f *~ .deps *.dis

cleanall:	clean
		rm -f *.mexglx *.mexw32

.deps:		*.c ../*.c ../*.h
		gcc -MM -I/opt/matlab/extern/include -I/opt/matlab/simulink/include -I.. \
			-DMATLAB_MEX_FILE -ansi -D_GNU_SOURCE -D_FILE_OFFSET_BITS=64 \
			-DS_FUNCTION_NAME=node0 -DMX_COMPAT_32 -DNDEBUG *.c ../*.c ../*.h> .deps

node0.mexglx:	cpu-sfun.c ../node0.c ../node_common.c ../cpu.h ../ttc-reg.h
		mex -I.. CC='gcc -Wall' -DS_FUNCTION_NAME=node0 -output $@ cpu-sfun.c ../node0.c ../node_common.c

node1.mexglx:	cpu-sfun.c ../node1.c ../node_common.c cpu.h ttc-reg.h
		mex -I.. CC='gcc -Wall' -DS_FUNCTION_NAME=node1 -output $@ cpu-sfun.c ../node1.c ../node_common.c

ctlr.mexglx:	ctlr-sfun.c ../ctlr.c ../ctlr.h ../ttc-reg.h
		mex -I.. CC='gcc -Wall' -output $@ ctlr-sfun.c ../ctlr.c
#		objdump -D $@ > ctlr.dis

bus.mexglx:	bus-sfun.c ../bus.c ../bus.h
		mex -I.. CC='gcc -Wall' -output $@ bus-sfun.c ../bus.c

ifeq (.deps, $(wildcard .deps))
-include .deps
endif
