CFLAGS		= -Werror -Wall -O -g
LDFLAGS         =

OBJS            = hexdis.o mips-dis.o

all:            hexdis

hexdis:         $(OBJS)
		${CC} ${LDFLAGS} $(OBJS) ${LIBS} -o $@

clean:
		rm -f *.o hexdis
