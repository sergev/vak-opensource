CFLAGS  := -O -g -Wall
PROG    = cryptotool
OBJS    = cryptotool.o
LIBS    = -lgpgme -lgpg-error

all:    $(PROG)

$(PROG): $(OBJS)
	$(CC) $(LDFLAGS) $(OBJS) $(LIBS) -o $(PROG)

clean:
	rm -f *.o $(PROG)
