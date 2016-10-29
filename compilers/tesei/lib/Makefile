INCLUDE		= -I../include
OBJS		= div16u.mic div8s.mic div8u.mic mul16s.mic mul16u.mic\
		  mul8s.mic mul8u.mic

INSTDIR		= /usr/local

all:		$(OBJS)

clean:
		rm -f *~ *.lst *.hex

install:	$(OBJS)
		install -d $(INSTDIR)/lib/tesei
		for f in $(OBJS); do install -c $$f $(INSTDIR)/lib/tesei; done
