PROG    = hello hello2 factorial factorial2 cubic_root

all:    $(PROG)

clean:
	rm -f *.o $(PROG)

.S.o:
	cpp $< | as -o $@ -

hello:  hello.o
	ld -o $@ $<

hello2: hello2.o print_string.o strlen.o
	ld -o $@ hello2.o print_string.o strlen.o

factorial: factorial.o print_uns.o print_newline.o print_string.o strlen.o
	ld -o $@ factorial.o print_uns.o print_newline.o print_string.o strlen.o

factorial2: factorial2.S
	cc -o $@ factorial2.S -static

cubic_root: cubic_root.s
	cc -o $@ cubic_root.s
