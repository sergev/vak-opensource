CC              = gcc -m32
CFLAGS          = -g
PROG            = helpdeco
OBJS            = helpdeco.o helpdec1.o posix.o

all:            $(PROG)

$(PROG):        $(OBJS)
		$(CC) $(LDFLAGS) $(OBJS) -o $@

clean:
		rm -f *.o $(PROG)
