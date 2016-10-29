
SHELL   = /bin/sh
INSTALL = /usr/SVSB
AS      = $(INSTALL)/bin/as
CC      = $(INSTALL)/bin/cc
CFLAGS  =
ASFLAGS = #-x

OBJ     = tst0.o

all:    $(OBJ)

install: $(INSTALL)/lib/tst0.o

$(INSTALL)/lib/tst0.o: tst0.o
	cp tst0.o $(INSTALL)/lib

clean:
	rm -f *.o *.b a.out core
