#
# Portable Forth interpreter.
#
# Copyright (C) 1990-92 Serge Vakulenko, <vak@kiae.su>.
#

DEST	      = /usr/local/bin
HDRS	      = forth.h
LDFLAGS	      =
LIBS          = -lm
CC            = gcc -g
CFLAGS        = -O

PROGRAM	      = forth
OBJS          = forth.o func.o
SRCS          = forth.c func.c

all:		$(PROGRAM)

$(PROGRAM):     $(OBJS)
		$(CC) $(LDFLAGS) $(OBJS) $(LIBS) -o $(PROGRAM)

lint:           $(SRCS)
		lint $(CFLAGS) $(SRCS) $(LIBS) > lint

forth.txt:      func.c
		grep '^ \*+' func.c | expand | sed 's/^ \*+$$//' |\
		sed 's/^ \*+     //' > forth.txt

clean:;         rm -f $(PROGRAM) $(OBJS) *.b core a.out .,* lint

depend:;        mkmf

install:	$(PROGRAM)
		install -s $(PROGRAM) $(DEST)

###
forth.o: forth.h
func.o: forth.h
