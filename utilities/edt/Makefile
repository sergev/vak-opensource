#
#       Line Text Editor, Copyright (C) 1989, 1990 Demos/*, Moscow.
#

#       -DTERMIO        use termio.h instead sgtty.h
#       -DNOKEYPAD      don't use keypad
#       -DINITFILE      send 'if' file to initialize terminal
#       -DGETWD         use standard getwd () (BSD)
#       -DGETCWD        use standard getcwd () (System V)
#       -DDIRENT        use file system independent directory format
#       -DSTDDIR        use standard readdir library
#       -DSTRINIT       send is, fs when VRestore, VReopen
#       -DHOSTNAME      use gethostname
#       -DHARDKEYS      cannot differ key escape codes by two last chars
#       -DBCOPY         use bcopy, bcmp instead of memcpy, memcmp

DEST          = /usr/local/bin
LINKER	      = cc
MAKEFILE      = Makefile
PRINT	      = pr
PROGRAM       = edt

# For Labtam NS 32332
# CFLAGS        = -O -DGETCWD -DTERMIO -DSTRINIT
# LDFLAGS       = -i

# For PC AT Xenix-286
# CFLAGS        = -O -K -DTERMIO -M2l
# LDFLAGS       = -i -M2l

# For Compaq Xenix-386
#CFLAGS        = -O -K -DTERMIO
#LDFLAGS       = -i

# For IX-386 with standard compiler
# CFLAGS        = -O -DTERMIO -DDIRENT
# LDFLAGS       =

# For IX-386 with gnu compiler
CFLAGS        = -O -g -DTERMIO -DDIRENT
LDFLAGS       = -g
CC            = gcc -Wall
LINKER        = gcc

# For Bsd43 and Ultrix
# CFLAGS        = -O -DGETWD -DSTDDIR -DHOSTNAME -DBCOPY -DVFORK
# LDFLAGS       =

# For Bsd43 and Ultrix with gnu compiler
# CFLAGS        = -O -g -DGETWD -DSTDDIR -DHOSTNAME -DBCOPY -DVFORK
# LDFLAGS       = -g
# CC            = gcc
# LINKER        = gcc

HDRS          = mem.h\
		rec.h

OBJS          = edt.o\
		mem.o\
		rec.o

SRCS          = edt.c\
		mem.c\
		rec.c

all:		$(PROGRAM)

$(PROGRAM):     $(OBJS) $(LIBS)
		$(LINKER) $(LDFLAGS) $(OBJS) $(LIBS) -o $(PROGRAM)

clean:;         rm -f *.o *.b z* *~ core a.out $(PROGRAM)

depend:;        mkmf -f $(MAKEFILE) PROGRAM=$(PROGRAM) DEST=$(DEST)

index:;		@ctags -wx $(HDRS) $(SRCS)

install:	$(PROGRAM)
		cp $(PROGRAM) $(DEST)
		strip $(DEST)/$(PROGRAM)
		chmod 711 $(DEST)/$(PROGRAM)
		chgrp bin $(DEST)/$(PROGRAM)
		chown bin $(DEST)/$(PROGRAM)

print:;		@$(PRINT) $(HDRS) $(SRCS)

program:        $(PROGRAM)

tags:           $(HDRS) $(SRCS); @ctags $(HDRS) $(SRCS)

lint:           $(HDRS) $(SRCS); @lint $(SRCS) >lint

update:		$(DEST)/$(PROGRAM)

$(DEST)/$(PROGRAM): $(SRCS) $(LIBS) $(HDRS) $(EXTHDRS)
		make -f $(MAKEFILE) DEST=$(DEST) install
###
edt.o: rec.h
mem.o: mem.h
rec.o: rec.h mem.h
