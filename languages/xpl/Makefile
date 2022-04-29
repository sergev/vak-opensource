PROG    = echo compr
CFLAGS  = -O

all:    $(PROG)

clean:
	rm -rf *.c *.xh *.dSYM $(PROG)

echo:   echo.xpl
	xpl $<
	cc $(CFLAGS) $(LDFLAGS) -lxpl $@.c -o $@

compr:  compress.xpl
	xpl $<
	cc $(CFLAGS) $(LDFLAGS) -lxpl compress.c -o $@
