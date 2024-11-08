PROG    = hello hello2 factorial

all:    $(PROG)

clean:
	rm -f *.o $(PROG)

.S.o:
	cpp $< | as - -o $@

hello:  hello.o
	ld $< -o $@

hello2: hello2.o print_string.o strlen.o
	ld hello2.o print_string.o strlen.o -o $@

factorial: factorial.o print_uns.o print_newline.o print_string.o strlen.o
	ld -static -nostdlib factorial.o print_uns.o print_newline.o print_string.o strlen.o -o $@
