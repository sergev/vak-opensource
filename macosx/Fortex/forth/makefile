DEST		= /usr/local/bin
LIBS		= -lm
CC		= gcc -g -Wall
CFLAGS		= -O

PROGRAM		= forth
OBJS		= forth.o func.o

# For Turbo C graphics
#CFLAGS		+= -DGRAPH
#OBJS		+= gr-tc.o

all:		$(PROGRAM)

$(PROGRAM):     $(OBJS) func.txt graphics.txt
		$(CC) $(LDFLAGS) $(OBJS) $(LIBS) -o $(PROGRAM)

func.txt:	func.c
		grep '^ \*+' func.c | expand | sed 's/^ \*+ *$$//' |\
		sed 's/^ \*+ //' > func.txt

graphics.txt:	gr-tc.c
		grep '^ \*+' gr-tc.c | expand | sed 's/^ \*+ *$$//' |\
		sed 's/^ \*+ //' > graphics.txt

squares:
		forth base.fth squares.fth

tri:
		forth base.fth tri.fth halt.fth

clean:
		rm -f $(PROGRAM) *.o *~ core a.out

install:	$(PROGRAM)
		install -s $(PROGRAM) $(DEST)

###
forth.o: forth.c io.h forth.h
func.o: func.c io.h forth.h
gr-tc.o: gr-tc.c forth.h
