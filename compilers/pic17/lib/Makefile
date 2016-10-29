INCLUDE		= -I../include
OBJS		= divmod11.lib divmod11p.lib ls1.lib ls1p.lib rs1.lib \
		  rs1p.lib lt22.lib lt21.lib gt22.lib gt21.lib divmod21.lib \
		  divmod22.lib lt41.lib gt41.lib lt44.lib gt44.lib \
		  divmod42.lib divmod44.lib eq22.lib eq44.lib ls2.lib \
		  rs2.lib ls4.lib rs4.lib mul22.lib divcorr1.lib \
		  divcorr2.lib _peek.lib _poke.lib
INSTDIR		= /usr/local
CPP		= cpp

all:		$(OBJS)

clean:
		rm -f *~ *.lib *.hex

install:	$(OBJS)
		install -d $(INSTDIR)/lib/pic17
		for f in $(OBJS); do install -c $$f $(INSTDIR)/lib/pic17; done

.SUFFIXES:	.lib

.s.lib:
		$(CPP) $(INCLUDE) $< | sed -f %clean.sed > $@
