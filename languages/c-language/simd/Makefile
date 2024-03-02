PROG            = matmul-sse
CFLAGS		= -O3 -g -Wall -Werror -msse4.1 -mavx2 -mavx

all:		$(PROG)

clean:
		rm -f $(PROG) *.o

#matmul-sse:     matmul-sse.o
#		$(CC) $(LDFLAGS) $< -o $@
