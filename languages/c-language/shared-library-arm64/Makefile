CFLAGS = -Os

all:    foobar

clean:
	rm -f *.o *.so *.dylib foobar

foobar: foobar.o lib.so
	gcc $(CFLAGS) -o $@ foobar.o lib.so
	objdump -D $@ > $@.dis

lib.so: lib.o
	gcc -dynamiclib $(CFLAGS) -o $@ lib.o
	objdump -D $@ > $@.dis
