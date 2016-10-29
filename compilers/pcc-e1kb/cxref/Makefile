#	@(#)cxref.mk	1.7	83/08/05
#	3.0 SID #	1.2

SHELL   = /bin/sh
INSTALL = /usr/SVSB
FLG     =
CC      = cc
M       = ../mip
L       = ../lint
CPP     = ../cpp
INCS    = -I. -I$M -I$L
DEFS    = -DCXREF -DFLEXNAMES
LINTF   = -p
LINT    = lint
OPRL    = oprl
LFLAG1  = -i
LFLAG2  = -n
FFLAG   =
CFLAGS  = -O -c $(DEFS) $(INCS)

CPASS1  = cgram.c $(M)/comm1.c $(M)/optim.c $(M)/pftn.c $(M)/scan.c\
	$(M)/trees.c $(M)/xdefs.c xlocal.c lint.c $(M)/messages.c

OPASS1  = cgram.o comm1.o optim.o pftn.o scan.o trees.o xdefs.o xlocal.o\
	lint.o messages.o

XPASS   = $(M)/cgram.y $(M)/comm1.c $(M)/common lint.c $(L)/lmanifest macdefs\
	$(M)/manifest $(M)/mfile1 $(M)/optim.c $(M)/pftn.c $(M)/scan.c\
	$(M)/trees.c $(M)/xdefs.c xlocal.c $(M)/messages.c $(M)/messages.h

all:    cxref xpass xcpp

# CXREF

cxref:  cxr.o
	$(CC) $(FFLAG) $(LDFLAGS) cxr.o -o cxref

cxr.o:	cxr.c owner.h
	$(CC) $(CFLAGS) cxr.c

# XPASS

xpass:	$(OPASS1)
	$(CC) $(FFLAG) $(LFLAG1) $(LDFLAGS) $(OPASS1) -o xpass

$(OPASS1): $(M)/manifest macdefs $(M)/mfile1

cgram.c: $(M)/cgram.y
	sed -e 's/\/\*CXREF\(.*\)\*\//\1/' $(M)/cgram.y > gram.y
	yacc gram.y
	mv y.tab.c cgram.c
	-rm -f gram.y

cgram.o: cgram.c
	$(CC) -DBUG4 $(FFLAG) $(CFLAGS) cgram.c

comm1.o: $(M)/common
	$(CC) -DBUG4 $(FFLAG) $(CFLAGS) $(M)/comm1.c

lint.o:	$(L)/lmanifest lint.c
	$(CC) $(FFLAG) $(CFLAGS) lint.c
	
optim.o: $(M)/optim.c
	$(CC) -DBUG4 $(FFLAG) $(CFLAGS) $(M)/optim.c
	
pftn.o: $(M)/pftn.c
	$(CC) -DBUG4 $(FFLAG) $(CFLAGS) $(M)/pftn.c
	
scan.o: $(M)/scan.c
	$(CC) -DBUG4 $(FFLAG) $(CFLAGS) $(M)/scan.c

trees.o: $(M)/trees.c
	$(CC) -DBUG4 $(FFLAG) $(CFLAGS) $(M)/trees.c

xdefs.o: $(M)/xdefs.c
	$(CC) -DBUG4 $(FFLAG) $(CFLAGS) $(M)/xdefs.c
	
xlocal.o: xlocal.c $(L)/lmanifest
	$(CC) $(FFLAG) $(CFLAGS) xlocal.c

messages.o: $(M)/messages.c $(M)/messages.h
	$(CC) $(FFLAG) $(CFLAGS) $(M)/messages.c

# XCPP

xcpp:	cpp.o cpy.o
	$(CC) $(FFLAG) $(LFLAG1) $(LDFLAGS) -o xcpp cpp.o cpy.o

cpp.o:	$(CPP)/cpp.c
	$(CC) $(FFLAG) $(CFLAGS) -I$(CPP) -Dunix=1 $(CPP)/cpp.c

cpy.o:	cpy.c $(CPP)/yylex.c
	$(CC) $(FFLAG) $(CFLAGS) -I$(CPP) -Dunix=1 cpy.c

cpy.c:	$(CPP)/cpy.y
	yacc $(CPP)/cpy.y
	mv y.tab.c cpy.c

# UTILITIES

install: all
	cp cxref $(INSTALL)/bin
	strip $(INSTALL)/bin/cxref
	cp xpass $(INSTALL)/lib
	strip $(INSTALL)/lib/xpass
	cp xcpp $(INSTALL)/lib
	strip $(INSTALL)/lib/xcpp

clean:
	-rm -f *.o *.b core a.out cxref xcpp xpass

clobber: clean
	-rm -f xpass cgram.c cxref xcpp cpy.c

lint:
	$(LINT) $(LINTF) cxr.c
	$(LINT) $(LINTF) -DBUG4 $(CPASS1)
	$(LINT) $(LINTF) -Dunix=1 $(CPP)/cpp.c cpy.c
