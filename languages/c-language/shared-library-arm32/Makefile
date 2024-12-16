CFLAGS = -Os

all:    foobar

clean:
	rm -f *.o *.so *.dylib foobar

foobar: foobar.o lib.so
	gcc $(CFLAGS) -o $@ foobar.o lib.so
	objdump -d $@ > $@.dis

lib.so: lib.o
	gcc -shared $(CFLAGS) -o $@ lib.o
	objdump -d $@ > $@.dis
