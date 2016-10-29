CC		= gcc -Wall -g
CFLAGS 		= -O
PROG		= volapuk
OBJS		= volapuk.o

# For Linux, FreeBSD
#CFLAGS 	+=
#OBJS		+=
#LIBS		=

# For Windows
#OBJS		+=
#LIBS		=

all:		$(PROG)

$(PROG):	$(OBJS)
		$(CC) $(LDFLAGS) $(OBJS) $(LIBS) -o $@

clean:
		rm -f *~ *.o $(PROG) $(PROG).exe
