CFLAGS		= -Wall -g -O
INSTDIR		= /usr/local
OBJS		= main.o disassembler.o
PROG		= mildis

all:		$(PROG)

$(PROG):	$(OBJS)
		$(CC) $(LDFLAGS) $(OBJS) -o $@

clean:
		rm -f *.[ob] *~ a.out *.core $(PROG)

install:	$(PROG)
		install -c -s $(PROG) $(INSTDIR)/bin/$(PROG)
