PROG		= udpping
CFLAGS		= -g -O -Wall
LDFLAGS		= -g -lpthread

all:		$(PROG) udpecho

$(PROG):	$(PROG).o
		$(CC) $(LDFLAGS) $(PROG).o -o $(PROG)

clean:
		rm -rf *.[obis] *~ $(PROG) udpecho

###
udpecho.o: udpecho.c
udpping.o: udpping.c
