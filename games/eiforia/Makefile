PROG            = eiforia
CXXFLAGS	= -O -g -I. #-Wall -Werror
OBJ             = eiforia.o

all:		$(PROG)

clean:
		rm -f $(PROG) *.o

$(PROG):        $(OBJ)
		$(CC) $(LDFLAGS) $(OBJ) $< -o $@
