PROG    = hello hello2

all:    $(PROG)

clean:
	rm -f *.o $(PROG)

.S.o:
	cpp $< | as - -o $@

hello:  hello.o
	ld $< -o $@

hello2: hello2.o print_string.o strlen.o
	ld hello2.o print_string.o strlen.o -o $@

#strlen: strlen.o print_uns.o print_newline.o
#	ld -static -nostdlib strlen.o print_uns.o print_newline.o -o $@
