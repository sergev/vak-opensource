PROG            = matmul-sse

# Intel x64
#CFLAGS		= -O3 -g -Wall -Werror -msse4.1 -mavx2 -mavx

# ARM
CFLAGS		= -O3 -g -Wall -Werror -march=native -mcpu=native -mtune=native #-ftree-vectorize

all:		$(PROG)

clean:
		rm -f $(PROG) *.o

#matmul-sse:     matmul-sse.o
#		$(CC) $(LDFLAGS) $< -o $@
