DEST	      = .
EXTHDRS	      =
HDRS	      =
LDFLAGS	      =
LIBS	      =
CC            = gcc -g -O
CFLAGS        =

MAKEFILE      = Makefile

OBJS	      = langine.o
PROGRAM	      = langine

SRCS	      = langine.c

all:		$(PROGRAM)

$(PROGRAM):     $(OBJS) 
		$(CC) $(LDFLAGS) $(OBJS) $(LIBS) -o $(PROGRAM)

clean:;		rm -f *.o *.b .,* core a.out $(PROGRAM)

depend:;	mkmf -f $(MAKEFILE) PROGRAM=$(PROGRAM) DEST=$(DEST)

install:	$(PROGRAM)
		install -s $(PROGRAM) $(DEST)
###
