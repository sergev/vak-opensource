#
#       ОС ДЕМОС СВС-Б.
#
#       Обслуга объектных файлов: загрузчик, библиотекарь, разная мелочь.
#
#       Автор: Вакуленко С. В. (МФТИ).
#

SHELL   = /bin/sh
INSTALL = /usr/SVSB
I       = ../h
LDFLAGS = -M2l
CFLAGS  = -M2l -LARGE
C       = $(CFLAGS) -DCROSS
STRIP   = /bin/strip
LN      = ln

ld      = ld
ar      = ar
as      = as
strip   = strip
nm      = nm
size    = size
dis     = dis
lorder  = lorder
ranlib  = ranlib

all:    ld ar as strip nm size dis testmaker testdumper testdis ranlib testrdr

put:
	ci -q -u RCS/*

get:
	co *.c RCS/*

diff:
	rcsdiff *.c Makefile

ld:     ld.o fgetarhdr.o fgetint.o fgetsym.o fputsym.o fgethdr.o fputhdr.o\
		fgeth.o fputh.o fgetran.o
	$(CC) $(LDFLAGS) ld.o -o ld fgetarhdr.o fgetint.o fgetsym.o fputsym.o\
		fgethdr.o fputhdr.o fgeth.o fputh.o fgetran.o

ar:     ar.o getarhdr.o putarhdr.o getint.o putint.o
	$(CC) $(LDFLAGS) ar.o -o ar getarhdr.o putarhdr.o getint.o putint.o

ranlib: ranlib.o getarhdr.o putarhdr.o fgetarhdr.o fgeth.o fgetsym.o\
		fputran.o fputh.o fgethdr.o
	$(CC) $(LDFLAGS) ranlib.o -o ranlib getarhdr.o putarhdr.o fgetarhdr.o\
		fgeth.o fgetsym.o fputran.o fputh.o fgethdr.o

as:     as.o fputsym.o fputhdr.o fgeth.o fputh.o
	$(CC) $(LDFLAGS) as.o -o as fputsym.o fputhdr.o fgeth.o fputh.o

strip:  strip.o fgethdr.o fputhdr.o fgeth.o fputh.o
	$(CC) $(LDFLAGS) strip.o -o strip fgethdr.o fputhdr.o fgeth.o fputh.o

nm:     nm.o fgetsym.o fgethdr.o fgetarhdr.o fgetint.o fgeth.o
	$(CC) $(LDFLAGS) nm.o -o nm fgetsym.o fgethdr.o fgetarhdr.o fgetint.o\
		fgeth.o

size:   size.o fgethdr.o fgeth.o
	$(CC) $(LDFLAGS) size.o -o size fgethdr.o fgeth.o

dis:    dis.o fgethdr.o fgeth.o
	$(CC) $(LDFLAGS) dis.o -o dis fgethdr.o fgeth.o

testmaker: testmaker.o
	$(CC) $(LDFLAGS) testmaker.o -o testmaker

testdumper: testdumper.o
	$(CC) $(LDFLAGS) testdumper.o -o testdumper

testdis: testdis.o
	$(CC) $(LDFLAGS) testdis.o -o testdis

testrdr: testrdr.o fputh.o
	$(CC) $(LDFLAGS) testrdr.o fputh.o -o testrdr

install: $(INSTALL)/bin/$(ld) $(INSTALL)/bin/$(ar) $(INSTALL)/bin/$(nm)\
		$(INSTALL)/bin/$(as) $(INSTALL)/bin/testmaker\
		$(INSTALL)/bin/testdumper $(INSTALL)/bin/testdis\
		$(INSTALL)/bin/$(size) $(INSTALL)/bin/$(strip)\
		$(INSTALL)/bin/$(lorder) $(INSTALL)/bin/$(dis)\
		$(INSTALL)/bin/$(ranlib) $(INSTALL)/bin/testrdr
clean:
	rm -f *.o *.b a.out core ot ld ar as strip nm size dis\
		testmaker testdumper testdis ranlib testrdr

$(INSTALL)/bin/$(ld): ld
	cp ld $(INSTALL)/bin/$(ld)
	$(STRIP) $(INSTALL)/bin/$(ld)
	rm -f $$HOME/bin/vld
	$(LN) $(INSTALL)/bin/$(ld) $$HOME/bin/vld

$(INSTALL)/bin/$(ranlib): ranlib
	cp ranlib $(INSTALL)/bin/$(ranlib)
	$(STRIP) $(INSTALL)/bin/$(ranlib)
	rm -f $$HOME/bin/vranlib
	$(LN) $(INSTALL)/bin/$(ranlib) $$HOME/bin/vranlib

$(INSTALL)/bin/$(ar): ar
	cp ar $(INSTALL)/bin/$(ar)
	$(STRIP) $(INSTALL)/bin/$(ar)
	rm -f $$HOME/bin/var
	$(LN) $(INSTALL)/bin/$(ar) $$HOME/bin/var

$(INSTALL)/bin/$(as): as
	cp as $(INSTALL)/bin/$(as)
	$(STRIP) $(INSTALL)/bin/$(as)
	rm -f $$HOME/bin/vas
	$(LN) $(INSTALL)/bin/$(as) $$HOME/bin/vas

$(INSTALL)/bin/$(nm): nm
	cp nm $(INSTALL)/bin/$(nm)
	$(STRIP) $(INSTALL)/bin/$(nm)
	rm -f $$HOME/bin/vnm
	$(LN) $(INSTALL)/bin/$(nm) $$HOME/bin/vnm

$(INSTALL)/bin/$(size): size
	cp size $(INSTALL)/bin/$(size)
	$(STRIP) $(INSTALL)/bin/$(size)
	rm -f $$HOME/bin/vsize
	$(LN) $(INSTALL)/bin/$(size) $$HOME/bin/vsize

$(INSTALL)/bin/$(dis): dis
	cp dis $(INSTALL)/bin/$(dis)
	$(STRIP) $(INSTALL)/bin/$(dis)
	rm -f $$HOME/bin/vdis
	$(LN) $(INSTALL)/bin/$(dis) $$HOME/bin/vdis

$(INSTALL)/bin/$(strip): strip
	cp strip $(INSTALL)/bin/$(strip)
	$(STRIP) $(INSTALL)/bin/$(strip)
	rm -f $$HOME/bin/vstrip
	$(LN) $(INSTALL)/bin/$(strip) $$HOME/bin/vstrip

$(INSTALL)/bin/testmaker: testmaker
	cp testmaker $(INSTALL)/bin
	$(STRIP) $(INSTALL)/bin/testmaker
	rm -f $$HOME/bin/vtestmaker
	$(LN) $(INSTALL)/bin/testmaker $$HOME/bin/vtestmaker

$(INSTALL)/bin/testdumper: testdumper
	cp testdumper $(INSTALL)/bin
	$(STRIP) $(INSTALL)/bin/testdumper
	rm -f $$HOME/bin/vtestdumper
	$(LN) $(INSTALL)/bin/testdumper $$HOME/bin/vtestdumper

$(INSTALL)/bin/testdis: testdis
	cp testdis $(INSTALL)/bin
	$(STRIP) $(INSTALL)/bin/testdis
	rm -f $$HOME/bin/vtestdis
	$(LN) $(INSTALL)/bin/testdis $$HOME/bin/vtestdis

$(INSTALL)/bin/testrdr: testrdr
	cp testrdr $(INSTALL)/bin
	$(STRIP) $(INSTALL)/bin/testrdr
	rm -f $$HOME/bin/vtestrdr
	$(LN) $(INSTALL)/bin/testrdr $$HOME/bin/vtestrdr

$(INSTALL)/bin/$(lorder): lorder.sh
	cp lorder.sh $(INSTALL)/bin/$(lorder)
	rm -f $$HOME/bin/vlorder
	$(LN) $(INSTALL)/bin/$(lorder) $$HOME/bin/vlorder

###
ar.o:           $(I)/ar.h
as.o:           $(I)/a.out.h
fgetarhdr.o:    $(I)/ar.h
fgethdr.o:      $(I)/a.out.h
fgetran.o:      $(I)/ranlib.h
fgetsym.o:      $(I)/a.out.h
fputhdr.o:      $(I)/a.out.h
fputsym.o:      $(I)/a.out.h
getarhdr.o:     $(I)/ar.h
ld.o:           $(I)/a.out.h $(I)/ar.h $(I)/ranlib.h
nm.o:           $(I)/a.out.h $(I)/ar.h
old.o:          $(I)/a.out.h $(I)/ar.h
putarhdr.o:     $(I)/ar.h
dis.o:          $(I)/a.out.h
size.o:         $(I)/a.out.h
strip.o:        $(I)/a.out.h
testmaker.o:    $(I)/a.out.h
testdis.o:      $(I)/a.out.h

.c.o:
	$(CC) $(C) -c $<
