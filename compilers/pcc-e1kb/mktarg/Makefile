#-------------------------------------------------------
#
#       Makefile компилятора pcc для СВС-Б:
#       однопроходная версия.
#
#       Вакуленко С. В., ИАЭ, 196-72-12.
#
#-------------------------------------------------------

SHELL   = /bin/sh
INSTALL = /usr/SVSB
M       = ../mip
P       = ../pcc
CFLAGS  = -I$(M) -I$(P) -DFLEXNAMES $(ITM)
CC      = vcc

all:    ccom

ccom:   cgram.o xdefs.o scan.o pftn.o trees.o optim.o code.o local.o reader.o\
		local2.o order.o match.o allo.o comm1.o table.o messages.o
	$(CC) $(LDFLAGS) -o ccom cgram.o xdefs.o scan.o pftn.o trees.o\
		optim.o code.o local.o reader.o local2.o order.o match.o\
		allo.o comm1.o table.o messages.o

trees.o: $(M)/manifest $(P)/macdefs $(M)/mfile1 $(M)/trees.c
	$(CC) -c $(CFLAGS) $(M)/trees.c

optim.o: $(M)/manifest $(P)/macdefs $(M)/mfile1 $(M)/optim.c
	$(CC) -c $(CFLAGS) $(M)/optim.c

pftn.o: $(M)/manifest $(P)/macdefs $(M)/mfile1 $(M)/pftn.c
	$(CC) -c $(CFLAGS) $(M)/pftn.c

code.o: $(M)/manifest $(P)/macdefs $(M)/mfile1 $(P)/code.c
	$(CC) -c $(CFLAGS) $(P)/code.c

local.o: $(M)/manifest $(P)/macdefs $(M)/mfile1 $(P)/local.c
	$(CC) -c $(CFLAGS) $(P)/local.c

scan.o: $(M)/manifest $(P)/macdefs $(M)/mfile1 $(M)/scan.c
	$(CC) -c $(CFLAGS) $(M)/scan.c

tcopy.o: $(M)/manifest $(P)/macdefs $(M)/mfile1 $(M)/tcopy.c
	$(CC) -c $(CFLAGS) $(M)/tcopy.c

xdefs.o: $(M)/manifest $(M)/mfile1 $(P)/macdefs $(M)/xdefs.c
	$(CC) -c $(CFLAGS) $(M)/xdefs.c

cgram.o: $(M)/manifest $(M)/mfile1 $(P)/macdefs cgram.c
	$(CC) -c $(CFLAGS) cgram.c

cgram.c: $(M)/cgram.y
	yacc $(M)/cgram.y
	mv y.tab.c cgram.c

comm1.o: $(M)/manifest $(M)/mfile1 $(M)/common $(P)/macdefs $(M)/comm1.c
	$(CC) -c $(CFLAGS) $(M)/comm1.c

comm2.o: $(M)/manifest $(M)/mfile2 $(M)/common $(P)/macdefs $(P)/mac2defs $(M)/comm2.c
	$(CC) -c $(CFLAGS) $(M)/comm2.c

table.o: $(M)/manifest $(M)/mfile2 $(P)/mac2defs $(P)/macdefs $(P)/table.c
	$(CC) -c $(CFLAGS) $(P)/table.c

reader.o: $(M)/manifest $(M)/mfile2 $(P)/mac2defs $(P)/macdefs $(M)/reader.c
	$(CC) -c $(CFLAGS) $(M)/reader.c

local2.o: $(M)/manifest $(M)/mfile2 $(P)/mac2defs $(P)/macdefs $(P)/local2.c
	$(CC) -c $(CFLAGS) $(P)/local2.c

order.o: $(M)/manifest $(M)/mfile2 $(P)/mac2defs $(P)/macdefs $(P)/order.c
	$(CC) -c $(CFLAGS) $(P)/order.c

match.o: $(M)/manifest $(M)/mfile2 $(P)/mac2defs $(P)/macdefs $(M)/match.c
	$(CC) -c $(CFLAGS) $(M)/match.c

allo.o: $(M)/manifest $(M)/mfile2 $(P)/mac2defs $(P)/macdefs $(M)/allo.c
	$(CC) -c $(CFLAGS) $(M)/allo.c

messages.o: $(M)/messages.h $(M)/messages.c
	$(CC) -c $(CFLAGS) $(M)/messages.c

lint:
	lint $(CFLAGS) $(M)/xdefs.c $(M)/scan.c\
		$(M)/pftn.c $(M)/trees.c $(M)/optim.c $(P)/code.c\
		$(P)/local.c $(M)/reader.c $(P)/local2.c\
		$(P)/order.c $(M)/match.c $(M)/allo.c $(M)/comm1.c\
		$(P)/table.c $(M)/messages.c >lint

cxref:
	cxref -c -w120 -I$(P) -I$(M) $(M)/cgram.y $(M)/xdefs.c $(M)/scan.c\
		$(M)/pftn.c $(M)/trees.c $(M)/optim.c $(P)/code.c\
		$(P)/local.c $(M)/reader.c $(P)/local2.c\
		$(P)/order.c $(M)/match.c $(M)/allo.c $(M)/comm1.c\
		$(P)/table.c $(M)/messages.c> cxref

tags:
	ctags -x $(M)/manifest $(M)/mfile1 $(M)/mfile2\
		$(M)/common $(P)/macdefs $(P)/mac2defs\
		$(M)/cgram.y $(M)/xdefs.c $(M)/scan.c $(M)/pftn.c\
		$(M)/trees.c $(M)/optim.c $(P)/code.c\
		$(P)/local.c $(M)/reader.c $(P)/local2.c\
		$(P)/order.c $(M)/match.c $(M)/allo.c\
		$(M)/comm1.c $(P)/table.c $(M)/messages.c> tags

install: ccom
	cp ccom $(INSTALL)/lib/ccom

clean:
	rm -f *.o *.b a.out core ccom

#-----------------------------------------------------
#       Все, что лежит дальше, относится к F77.
#       Без правки работать не будет.
#-----------------------------------------------------

fort:   ccom fort.o freader.o fallo.o fmatch.o ftable.o forder.o flocal2.o\
		fcomm2.o
	$(CC) $(CFLAGS) fort.o freader.o fallo.o fmatch.o ftable.o forder.o\
		flocal2.o fcomm2.o -o fort

fort.o: fort.h $(M)/fort.c
	$(CC) -c $(CFLAGS) $(M)/fort.c

freader.o: reader.o
	$(CC) -c $(CFLAGS) $(M)/freader.c

fallo.o: allo.o
	$(CC) -c $(CFLAGS) $(M)/fallo.c

fmatch.o: match.o
	$(CC) -c $(CFLAGS) $(M)/fmatch.c

ftable.o: table.o
	$(CC) -c $(CFLAGS) $(M)/ftable.c

forder.o: order.o
	$(CC) -c $(CFLAGS) $(M)/forder.c

flocal2.o: local2.o
	$(CC) -c $(CFLAGS) $(M)/flocal2.c

fcomm2.o: $(M)/common
	$(CC) -c $(CFLAGS) $(M)/fcomm2.c

fort.o freader.o fallo.o fmatch.o ftable.o forder.o flocal2.o fcomm2.o:\
	$(M)/mfile2 $(M)/manifest macdefs mac2defs
