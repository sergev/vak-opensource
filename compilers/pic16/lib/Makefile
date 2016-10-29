INCLUDE		= -I../include
OBJS		=
INSTDIR		= /usr/local

all:		$(OBJS)

clean:
		rm -f *~ *.lib *.hex

install:	$(OBJS)
		install -d $(INSTDIR)/lib/pic16
		for f in $(OBJS); do install -c $$f $(INSTDIR)/lib/pic16; done

.SUFFIXES:	.lib

.s.lib:
		$(CPP) $(INCLUDE) $< | sed -f %clean.sed > $@
