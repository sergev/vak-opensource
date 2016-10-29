SHELL   =       /bin/sh
INSTALL =       /usr/SVSB
AR      =       var
LORDER  =       vlorder
LIBC    =       libc.a

all:
	cd crt; $(MAKE)
	cd sys; $(MAKE)
	cd gen; $(MAKE)
	cd stdio; $(MAKE)
	cd print; $(MAKE)
	cd csu; $(MAKE)

clean:
	cd csu; $(MAKE) clean
	cd crt; $(MAKE) clean
	cd sys; $(MAKE) clean
	cd gen; $(MAKE) clean
	cd stdio; $(MAKE) clean
	cd print; $(MAKE) clean
	rm -f *.o *.b a.out core libc.a

install: $(INSTALL)/lib/$(LIBC)
	cd csu; $(MAKE) install

libc.a: all
	rm -f libc.a
	$(AR) r libc.a `$(LORDER) */*.o | tsort`

$(INSTALL)/lib/$(LIBC): libc.a
	cp libc.a $(INSTALL)/lib/$(LIBC)
