CFLAGS  = -g -O -Wall -Werror

PROG    = gpio-demo

all:    $(PROG)

gpio-demo: gpio-demo.o
	$(CC) $(LDFLAGS) $< -o $@

clean:
	rm -f *.o $(PROG)
