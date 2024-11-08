PROG    = hello strlen

all:    $(PROG)

clean:
	rm -f *.o $(PROG)

.S.o:
	cpp $< | as - -o $@

hello:  hello.o
	ld $< -o $@

strlen: strlen.o print_uns.o print_newline.o
	ld -static -nostdlib strlen.o print_uns.o print_newline.o -o $@
