CC      = x86_64-elf-gcc
LD      = x86_64-elf-ld
CFLAGS  = -g -O -Wall -I..
LDFLAGS = -g -nostdlib
LIBS    = mylib-x86_64.so

all:    $(LIBS) hello.elf

clean:
	rm -f *.o *.so *.elf

hello.elf: ../hello.c $(LIBS)
	$(CC) $(CFLAGS) -c -fPIC -fno-plt ../hello.c -o hello.o
	$(LD) $(LDFLAGS) -e main -shared -fPIC -fno-plt hello.o $(LIBS) -o $@

mylib-x86_64.so: ../stub.c
	$(CC) $(CFLAGS) -c -fPIC -fno-plt $< -o stub.o
	$(LD) $(LDFLAGS) -shared -fPIC -fno-plt stub.o -o $@
