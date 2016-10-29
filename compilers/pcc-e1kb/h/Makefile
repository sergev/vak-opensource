#
#       ОС ДЕМОС СВС-Б.
#
#       Автор: Вакуленко С. В. (МФТИ).
#

SHELL   = /bin/sh
INSTALL = /usr/SVSB

all:    a.out.h ar.h

install: $(INSTALL)/include/a.out.h $(INSTALL)/include/ar.h

clean:
	rm -f *.o *.b a.out core

$(INSTALL)/include/a.out.h: a.out.h
	cp a.out.h $(INSTALL)/include/a.out.h

$(INSTALL)/include/ar.h: ar.h
	cp ar.h $(INSTALL)/include/ar.h

a.out.h:
ar.h:
