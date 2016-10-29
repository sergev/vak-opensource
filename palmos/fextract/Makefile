PROG		= fextract
OBJS		= $(PROG).o
CC		= gcc -g
CFLAGS		= -O2 -Wall $(DEFINES)

all:		$(PROG)

$(PROG):	$(OBJS)
		$(CC) $(CFLAGS) $(OBJS) -o $@

clean:
		rm -rf *~ *.o $(PROG) core *.pfn *.pft
