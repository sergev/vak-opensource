CFLAGS		= -Wall -O
CC_I386		= intel-gcc412
CC_ARM		= arm-gcc412
CC_THUMB	= arm-gcc412 -mthumb
CC_MIPS		= mips-gcc412
CC_BFIN		= bfin-gcc412
CC_PIC30	= /usr/local/pic33/bin/gcc

all:		hash-i386.s hash-arm.s hash-thumb.s hash-mips.s hash-bfin.s \
		hash-pic30.s codesize.plt
		@rm -f codesize.png
		gnuplot codesize.plt

hash-i386.s:	hash.c
		$(CC_I386) $(CFLAGS) -S $< -o $@

hash-arm.s:	hash.c
		$(CC_ARM) $(CFLAGS) -S $< -o $@

hash-thumb.s:	hash.c
		$(CC_THUMB) $(CFLAGS) -S $< -o $@

hash-mips.s:	hash.c
		$(CC_MIPS) $(CFLAGS) -S $< -o $@

hash-bfin.s:	hash.c
		$(CC_BFIN) $(CFLAGS) -S $< -o $@

hash-pic30.s:	hash.c
		$(CC_PIC30) $(CFLAGS) -S $< -o $@

clean:
		rm -f *~ *.o *.s
