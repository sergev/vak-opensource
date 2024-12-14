PROG = cputrace hello-amd64-linux

all:    $(PROG)

clean:
	rm -f *.o $(PROG)

%.o:    %.S
	cpp $< | as -o $@ -

cputrace: cputrace.o
	$(CC) $(LDFLAGS) -o $@ $<

hello-amd64-linux: hello-amd64-linux.o
	ld -o $@ $<
