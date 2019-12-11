CFLAGS  = -g -O -Wall -Werror

PROG    = gpio-demo devcfg-demo

all:    $(PROG)

gpio-demo: gpio-demo.o
	$(CC) $(LDFLAGS) $< -o $@

devcfg-demo: devcfg-demo.o
	$(CC) $(LDFLAGS) $< -o $@

clean:
	rm -f *.o $(PROG)
