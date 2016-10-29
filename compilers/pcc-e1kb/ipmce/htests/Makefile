SHELL   = /bin/sh
AS      = vas
ASFLAGS = -a

ASMS    = can.s drpm.s drpm1.s op.s pe.s pr.s rzddvs.s sp.s tm.s uu.s wkb.s
OBJS    = can.o drpm.o drpm1.o op.o pe.o pr.o rzddvs.o sp.o tm.o uu.o wkb.o
SRCS    = can.src drpm.src drpm1.src op.src pe.src pr.src rzddvs.src sp.src tm.src uu.src wkb.src
TSTS    = can.tst drpm.tst drpm1.tst op.tst pe.tst pr.tst rzddvs.tst sp.tst tm.tst uu.tst wkb.tst
CHKS    = can.ss drpm.ss drpm1.ss op.ss pe.ss pr.ss rzddvs.ss sp.ss tm.ss uu.ss wkb.ss

tests:  tld $(TSTS)

comp:   $(OBJS)

decomp: tdas $(ASMS)

check:  tdas $(CHKS)

tld:    tld.c
	$(CC) $(CFLAGS) -DCROSS tld.c -o tld

.SUFFIXES: .tst .s .c .o .out .src .ss

.src.s:
	tdas $< >$@

.tst.ss:
	tdas $< >$@

.o.tst:
	tld $< -o $@
