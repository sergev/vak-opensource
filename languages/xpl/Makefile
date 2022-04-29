PROG    = echo compr
CFLAGS  = -g

all:    $(PROG)

clean:
	rm -rf *.c *.xh $(PROG)

echo:   echo.xpl
	xpl $<
	cc $(CFLAGS) $(LDFLAGS) -lxpl $@.c -o $@

compr:  compress.xpl
	xpl $<
	cc $(CFLAGS) $(LDFLAGS) -lxpl compress.c -o $@
