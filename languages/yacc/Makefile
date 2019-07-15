PROG    = pdc
CFLAGS ?= -O2 -g -Wall -DHAVE_READLINE
LIBS   += -lreadline -lncurses

all:    $(PROG)

$(PROG): y.tab.o
	$(CC) $(CFLAGS) -o $@ y.tab.o $(LIBS)

y.tab.c: pdc.y
	$(YACC) pdc.y

clean:
	$(RM) $(PROG) y.tab.c *.o
