CFLAGS		= -Wall -O -g
LDFLAGS		= -g

all:		basic renumber

clean:
		rm -f *~ *.o basic renumber

basic:		basic.o
		$(CC) $(LDFLAGS) basic.o -o $@

renumber:	renumber.o
		$(CC) $(LDFLAGS) renumber.o -o $@
