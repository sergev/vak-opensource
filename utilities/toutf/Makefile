OBJS		= toutf.o #detect.o
PROG		= toutf
CFLAGS		= -Wall -g #-O
LDFLAGS		= -g

all:		$(PROG)

clean:
		rm -f *~ *.o $(PROG)

$(PROG):	$(OBJS)
		$(CC) $(LDFLAGS) -o $@ $(OBJS)

install:	$(PROG)
		install -c -s $< /usr/local/bin/$(PROG)
