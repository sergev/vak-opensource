CFLAGS  := -O -g -Wall
PROG    = cryptotool
OBJS    = cryptotool.o

all:    $(PROG)

$(PROG): $(OBJS)
	$(CC) $(LDFLAGS) $(OBJS) $(LIBS) -o $(PROG)

clean:
	rm -f *.o $(PROG)
