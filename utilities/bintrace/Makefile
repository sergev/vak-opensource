PROG = bintrace hello-amd64-linux
LIBS = -lcapstone

all:    $(PROG)

clean:
	rm -f *.o $(PROG)

%.o:    %.S
	cpp $< | as -o $@ -

bintrace: bintrace.o
	$(CC) $(LDFLAGS) -o $@ $< $(LIBS)

hello-amd64-linux: hello-amd64-linux.o
	ld -o $@ $<
	objdump -D $@ > $@.dis
