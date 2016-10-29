CC		= gcc
CFLAGS		= -Wall -g -O2 -fno-builtin -nostdinc
LDFLAGS		= -g -nostdlib -Wl,-Ttext,0x100000
OBJS		= boot.o mbinfo.o

all:		mbinfo

mbinfo:		$(OBJS)
		$(CC) $(LDFLAGS) -s $(OBJS) -o $@

clean:
		rm -f *~ *.o mbinfo
###
boot.o:		boot.S multiboot.h
mbinfo.o:	mbinfo.c multiboot.h
