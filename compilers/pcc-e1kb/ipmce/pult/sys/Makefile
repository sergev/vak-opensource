SHELL         = /bin/sh
DEST          = /usr/sys
LDFLAGS       = -M2me
CFLAGS        = -M2me -K -O -I/usr/sys/conf -DM_KERNEL

MAKEFILE      = Makefile

HDRS          = pult.h
OBJS          = pult.o pultio.o
SRCS          = pult.c pultio.s

all:            $(OBJS)

clean:;         rm -f $(OBJS) *.b core a.out .,*

install:        $(DEST)/conf/pult.o $(DEST)/conf/pultio.o /usr/include/sys/pult.h

$(DEST)/conf/pult.o: pult.o
		cp pult.o $(DEST)/conf

$(DEST)/conf/pultio.o: pultio.o
		cp pultio.o $(DEST)/conf

/usr/include/sys/pult.h: pult.h
		cp pult.h /usr/include/sys

.s.o:
	$(CC) $(CFLAGS) -c $<
###
