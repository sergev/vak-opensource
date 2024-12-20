
all:    foobar

clean:
	rm -f *.o *.so *.dis foobar

foobar: foobar.o lib.so
	ld -o $@ foobar.o lib.so
	objdump -D $@ > $@.dis

lib.so: lib.S
	cpp $< | as -o lib.o -
	ld -shared -o $@ lib.o
	objdump -D $@ > $@.dis
