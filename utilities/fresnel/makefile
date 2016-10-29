PROG		= fresnel
DEST		= /usr/local/bin
CFLAGS		= -Wall -O

all:		$(PROG)

install:	$(PROG)
		install -c -s $(PROG) $(DEST)/avr-$(PROG)

clean:
		rm -f *.out *.[osi] *~

depend:
		@cp Makefile Makefile~~
		(sed '/^###/,$$d' Makefile;\
		echo '### DO NOT DELETE THIS LINE';\
		$(CC) -MM $(CFLAGS) *.c;\
		echo '# DEPENDENCIES MUST END AT END OF FILE';\
		echo '# IF YOU PUT STUFF HERE IT WILL GO AWAY';\
		echo '# see make depend above') > Makefile~ &&\
		mv Makefile~ Makefile

### DO NOT DELETE THIS LINE
