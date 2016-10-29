SHELL   = /bin/sh
LIB     = /usr/SVSB/lib
AS      = vas
ASFLAGS = -b
CFLAGS  =
LDFLAGS =
CC      = vcc
LD      = vld
TMAKER  = vtestmaker
TDIS    = vtestdis

all:

uns:    uns.tst uns.dis

cptr:   cptr.tst cptr.dis

tty:    tty.tst tty.dis

tc1:    tc1.tst tc1.dis

z:      z.o
	$(LD) $(LDFLAGS) z.o -lc -o z

l:      l1.o l2.o l3.o
	$(LD) $(LDFLAGS) l1.o l2.o l3.o -o l

clean:
	rm -f *.b *.out *.tst *.o core

.c.i:
	$(CC) -P $(CFLAGS) $<

.c.s:
	$(CC) -S $(CFLAGS) $<

div.tst: div.s
	$(CC) -c $(CFLAGS) div.s
	$(LD) $(LDFLAGS) -C -T1 div.o -o div.out -ltst
	$(TMAKER) -a1 div.out -o div.tst
	-rm -f div.o div.out

uns.tst: uns.c ran64.o
	$(CC) -c $(CFLAGS) uns.c
	$(LD) $(LDFLAGS) -C -T1 $(LIB)/tst0.o uns.o ran64.o -ltst -o $*.out
	$(TMAKER) -a1 $*.out -o $*.tst
	-rm -f $*.o $*.out

uns.out: uns.c ran64.o
	$(CC) -c $(CFLAGS) uns.c
	$(LD) $(LDFLAGS) -C -T1 $(LIB)/tst0.o uns.o ran64.o -ltst -o $*.out
	-rm -f $*.o

.s.tst:
	$(AS) $(ASFLAGS) $< -o $*.o
	$(LD) $(LDFLAGS) -C -T1 $*.o -o $*.out
	$(TMAKER) -a1 $*.out -o $*.tst
	-rm -f $*.o $*.out

.c.tst:
	$(CC) -c $(CFLAGS) $<
	$(LD) $(LDFLAGS) -C -T1 $(LIB)/tst0.o $*.o -ltst -o $*.out
	$(TMAKER) -a1 $*.out -o $*.tst
	-rm -f $*.o $*.out

.c.out:
	$(CC) -c $(CFLAGS) $<
	$(LD) $(LDFLAGS) -C -T0x20001 $(LIB)/tst0.o $*.o -ltst -o $*.out
	-rm -f $*.o

.c.run:
	$(CC) -c $(CFLAGS) $<
	$(LD) $(LDFLAGS) -T0x1000 $(LIB)/srt0.o $*.o -lsa -ltst -o $*.run
	-rm -f $*.o

.tst.dis:
	$(TDIS) $< > $@

.SUFFIXES: .c .o .s .i .tst .out .dis .run
