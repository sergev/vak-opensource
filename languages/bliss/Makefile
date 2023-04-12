PROG    = hello

all:    $(PROG)

clean:
	rm -f $(PROG) *.o

hello:  hello.o main.o
	$(CC) $(LDFLAGS) hello.o main.o -o $@

%.o:    %.bliss
	blissc $<
