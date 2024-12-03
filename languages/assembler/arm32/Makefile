PROG    = hello hello2

all:    $(PROG)

clean:
	rm -f *.o $(PROG)

.S.o:
	cpp $< | as -o $@ -

hello:  hello.o
	ld -o $@ $<

hello2: hello2.s
	cc -o $@ hello2.s -z noexecstack
