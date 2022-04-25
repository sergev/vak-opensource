PROG    = echo

all:    $(PROG)

echo:   echo.xpl
	xpl $<
	cc $(CFLAGS) $(LDFLAGS) -lxpl $@.c -o $@

clean:
	rm -rf *.c *.xh $(PROG)
