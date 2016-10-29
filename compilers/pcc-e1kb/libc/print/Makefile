SHELL   = /bin/sh
CC      = vcc
INC     = ../../h
CFLAGS  = -x -I$(INC)

OBJS    = doprnt.o fprintf.o printf.o sprintf.o vfprintf.o vprintf.o vsprintf.o

ALL     = doprnt.o fprintf.o printf.o sprintf.o vfprintf.o vprintf.o vsprintf.o

all:    $(OBJS)

clean:
	-rm -f *.o *.b a.out core

doprnt.o: print.h
doprnt.o: $(INC)/ctype.h
doprnt.o fprintf.o printf.o sprintf.o vfprintf.o vprintf.o vsprintf.o: $(INC)/stdio.h
doprnt.o sprintf.o vsprintf.o: $(INC)/values.h
doprnt.o fprintf.o printf.o sprintf.o vfprintf.o vprintf.o vsprintf.o: $(INC)/varargs.h
