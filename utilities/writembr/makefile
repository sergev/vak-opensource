#
# Standard master boot record "mbr0.bin" comes from:
# http://ata-atapi.com/hiwmbr.htm
#
PROG		= megaprog
DEST		= /usr/local/bin
CC		= gcc
CFLAGS		= -Wall -O
#OBJS		= prog.o avr.o huff.o main.o
OBJS		= prog.o avr.o huff.o unpack.o main.o
LDFLAGS		= -s

all:		printmbr.exe writembr.exe #boot.bin boot.dis

$(PROG):	$(OBJS)
		$(CC) $(LDFLAGS) -o $@ $(OBJS)

ChangeLog:
		cvs2cl

# Use Mingw32
printmbr.exe:	printmbr.o
		$(CC) $(LDFLAGS) -o $@ printmbr.o -liberty

# Use Mingw32
writembr.exe:	writembr.o
		$(CC) $(LDFLAGS) -o $@ writembr.o -liberty

boot.bin:	boot.s
		nasm -a -f bin -o boot.bin -l boot.lst boot.s

boot.dis:	boot.bin
		ndisasm -a -k 0x12f,0xd1 boot.bin > boot.dis

mbr0.dis:	mbr0.bin
		ndisasm -a mbr0.bin > mbr0.dis

clean:
		rm -f *~ *.out *.o *.exe *.bak

depend:
		@cp Makefile Makefile~~
		(sed '/^###/,$$d' Makefile;\
		echo '### DO NOT DELETE THIS LINE';\
		$(CC) -MM $(CFLAGS) *.c;\
		echo '# DEPENDENCIES MUST END AT END OF FILE';\
		echo '# IF YOU PUT STUFF HERE IT WILL GO AWAY';\
		echo '# see make depend above') > Makefile~ &&\
		mv Makefile~ Makefile

### DO NOT DELETE THIS LINE
