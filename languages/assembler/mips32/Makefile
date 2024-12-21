PROG    = hello

all:    $(PROG)

clean:
	rm -f *.o $(PROG)

.S.o:
	cpp $< | as -o $@ -

hello:  hello.o
	ld -o $@ $<
