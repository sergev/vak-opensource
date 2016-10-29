
SHELL   = /bin/sh
INSTALL = /usr/SVSB
AS      = vas
CC      = vcc
CFLAGS  =
ASFLAGS = -x

OBJ     = crt0.o

all:    $(OBJ)

install: $(INSTALL)/lib/crt0.o

$(INSTALL)/lib/crt0.o: crt0.o
	cp crt0.o $(INSTALL)/lib

clean:
	rm -f *.o *.b a.out core
