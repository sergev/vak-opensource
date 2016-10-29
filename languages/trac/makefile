SHELL		= /bin/sh
CC		= gcc -g -Wall
CFLAGS		= -O

trac:		trac.o
		$(CC) $(LDFLAGS) trac.o -o trac

demo:		trac lib.t pi.trac e.trac
		./trac pi.trac
		./trac e.trac

lib.t:		trac lib.trac
		./trac lib.trac

clean:
		rm -f *~ *.o trac lib.t
