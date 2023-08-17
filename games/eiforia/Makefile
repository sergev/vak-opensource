PROG            = eiforia
CFLAGS          = -O -g -I. -Wall -Werror
OBJ             = eiforia.o conio.o

all:		$(PROG)

clean:
		rm -f $(PROG) *.o

$(PROG):        $(OBJ)
		$(CC) $(LDFLAGS) $(OBJ) -o $@
