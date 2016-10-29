CC              = pdp11-pcc
LD              = pdp11-ld
LDFLAGS         =

all:            test00.out test01.out

clean:
		rm -f *.o *.out *.dis

.S.out:
		$(CC) $(ASFLAGS) -c $<
		$(LD) $(LDFLAGS) -a0500 -o $@ $*.o
		pdp11-disasm -a0500 $@ > $*.dis
