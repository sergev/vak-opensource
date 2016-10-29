PROG            = genhex
CFLAGS		= -O -g -Wall -Werror
LDFLAGS		= -g

all:		$(PROG)

clean:
		rm -f $(PROG) *.o *~ a.out

install:        $(PROG)
		install $(PROG) $(HOME)/bin/$(PROG)
