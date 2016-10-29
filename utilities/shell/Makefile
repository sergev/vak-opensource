SHELL           = /bin/sh
DEST            = /usr/local/bin
CFLAGS          = -g
LDFLAGS         = -g
CC              = gcc
INSTALL         = cp

OBJS          = env.o\
		main.o\
		mem.o\
		shell.o

SRCS          = env.c\
		main.c\
		mem.c\
		shell.c

all:            shell

shell:          $(OBJS) $(LIBS)
		$(CC) $(LDFLAGS) $(OBJS) $(LIBS) -o shell

clean:;         rm -f *.[ob] .,* core a.out lint shell

depend:;        mkmf -f Makefile DEST=$(DEST)

install:        shell
		$(INSTALL) shell $(DEST)

lint:           $(SRCS) Makefile
		lint $(SRCS) > lint
###
env.o: mem.h
main.o: env.h shell.h
mem.o: mem.h
shell.o: mem.h shell.h
