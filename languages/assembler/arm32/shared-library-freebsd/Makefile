
all:    foobar

clean:
	rm -f *.o *.so foobar

foobar: foobar.o lib.so
	ld -o $@ foobar.o lib.so
	objdump -D $@ > $@.dis
	nm -n -a $@ > $@.nm

lib.so: lib.S
	cpp $< | as -o lib.o -
	ld -fPIE -shared -o $@ lib.o
	objdump -D $@ > $@.dis
