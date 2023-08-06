CC              = gcc
CFLAGS		= -g -O -Werror -Wall

OBJS            = done.o init.o io.o main.o save.o subr.o vocab.o wizard.o

all:            adventure adventure.dat

# Data file format is compatible for all little-endian machines.
# To rebuild it, compile the program on Linux.
adventure.dat:
		./adventure glorkz

adventure:      ${OBJS}
		${CC} ${LDFLAGS} -o adventure ${OBJS} ${LIBS}

clean:
		rm -f *.o adventure *~

$(OBJS):        hdr.h
