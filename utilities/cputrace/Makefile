PROG = cputrace hello-amd64-macos

all:    $(PROG)

clean:
	rm -f *.o $(PROG)

%.o:    %.S
	cpp $< | as -o $@ -

cputrace: cputrace.o
	$(CC) $(LDFLAGS) -o $@ $<

hello-amd64-macos: hello-amd64-macos.o
	ld -o $@ $<
