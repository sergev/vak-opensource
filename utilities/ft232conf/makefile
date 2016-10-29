CC		= gcc

CFLAGS		= -Wall -g -I/opt/local/include/libusb-1.0 -O
LDFLAGS		=
LIBS		= -L/opt/local/lib -lusb-1.0

PROG_OBJS	= ft232conf.o ftdi.o

all:		ft232conf

ft232conf:	$(PROG_OBJS)
		$(CC) $(LDFLAGS) -o $@ $(PROG_OBJS) $(LIBS)

clean:
		rm -f *~ *.o core ft232conf

install:	ft232conf
		install -c -s ft232conf /usr/local/bin/ft232conf
###
ft232conf.o: ft232conf.c ftdi.h
ftdi.o: ftdi.c ftdi.h
