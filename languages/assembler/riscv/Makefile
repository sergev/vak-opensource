PROG    = hello hello2 factorial

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
	ld -o $@ -static -nostdlib factorial.o print_uns.o print_newline.o print_string.o strlen.o
