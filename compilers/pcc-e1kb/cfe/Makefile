
SHELL   = /bin/sh
INSTALL = /usr/SVSB
CFLAGS  =
CC      = /bin/cc
cc      = cc
cc2     = cc2

all:    $(cc)

$(cc):  cc.c
	$(CC) $(LDFLAGS) $(CFLAGS) -DONEPASS cc.c -o $(cc)

$(cc2): cc.c
	$(CC) $(LDFLAGS) $(CFLAGS) -DTWOPASS cc.c -o $(cc2)

install: $(INSTALL)/bin/$(cc)

clean:
	rm -f *.o *.b a.out core $(cc) $(cc2)

$(INSTALL)/bin/$(cc): $(cc)
	cp $(cc) $(INSTALL)/bin/$(cc)
	strip $(INSTALL)/bin/$(cc)

$(INSTALL)/bin/$(cc2): $(cc2)
	cp $(cc2) $(INSTALL)/bin/$(cc2)
	strip $(INSTALL)/bin/$(cc2)
