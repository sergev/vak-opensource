PROG            = gnudis

#BINUTILS_SRC    = /Users/vak/Project/GCC/binutils-2.38
#BINUTILS_OBJ    = /Users/vak/Project/GCC/binutils-arm
BINUTILS_SRC    = /Users/vak/Project/Lang/binutils-2.41
BINUTILS_OBJ    = /Users/vak/Project/Lang/binutils-arm

CFLAGS          = -g -Wall -Werror \
                  -I$(BINUTILS_SRC)/include \
                  -I$(BINUTILS_OBJ)/bfd

LDFLAGS         = -L$(BINUTILS_OBJ)/bfd/.libs \
                  -L$(BINUTILS_OBJ)/opcodes \
                  -L$(BINUTILS_OBJ)/libiberty \
                  -L$(BINUTILS_OBJ)/zlib \
                  -L/opt/homebrew/lib

all:		$(PROG)

gnudis:         gnudis.o
		$(CC) $(LDFLAGS) $(CFLAGS) gnudis.c -lopcodes -lbfd -liberty -lz -lzstd -o $@
#

clean:
		rm -f $(PROG) *.o *.dis *~ a.out
