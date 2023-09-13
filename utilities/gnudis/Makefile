PROG            = gnudis
BINUTILS_SRC    = /Users/vak/Project/GCC/binutils-2.38
BINUTILS_OBJ    = /Users/vak/Project/GCC/binutils-arm
CFLAGS          = -g -Wall -Werror \
                  -I$(BINUTILS_SRC)/include \
                  -I$(BINUTILS_OBJ)/bfd
LDFLAGS         = -L$(BINUTILS_OBJ)/bfd \
                  -L$(BINUTILS_OBJ)/opcodes \
                  -L$(BINUTILS_OBJ)/libiberty \
                  -L$(BINUTILS_OBJ)/zlib

all:		$(PROG)

gnudis:         gnudis.o
		$(CC) $(LDFLAGS) $(CFLAGS) gnudis.c -lopcodes -lbfd -liberty -lz -o $@

clean:
		rm -f $(PROG) *.o *.dis *~ a.out
