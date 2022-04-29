PROG    = echo beer compr
CFLAGS  = -O
LIBS    = -lxpl

all:    $(PROG)

clean:
	rm -rf *.c *.xh *.dSYM $(PROG)

echo:   echo.xpl
	xpl $<
	cc $(CFLAGS) $(LDFLAGS) $@.c $(LIBS) -o $@

beer:   beer.xpl
	xpl $<
	cc $(CFLAGS) $(LDFLAGS) $@.c $(LIBS) -o $@

compr:  compress.xpl
	xpl $<
	cc $(CFLAGS) $(LDFLAGS) compress.c $(LIBS) -o $@
