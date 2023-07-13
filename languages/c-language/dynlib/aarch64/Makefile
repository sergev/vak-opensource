CC      = aarch64-elf-gcc
LD      = aarch64-elf-ld
OBJDUMP = aarch64-elf-objdump

#CC      = gcc
#LD      = ld
#OBJDUMP = objdump

CFLAGS  = -g -O -Wall -I.. -fpie
LDFLAGS = -g -nostdlib -shared -fpie
LIBS    = mylib-aarch64.so

all:    $(LIBS) hello.elf

clean:
	rm -f *.o *.so *.elf

hello.elf: ../hello.c $(LIBS)
	$(CC) $(CFLAGS) -c ../hello.c -o hello.o
	$(LD) $(LDFLAGS) -e main hello.o $(LIBS) -o $@
	$(OBJDUMP) -D $@ > hello.dis

mylib-aarch64.so: ../stub.c
	$(CC) $(CFLAGS) -c $< -o stub.o
	$(LD) $(LDFLAGS) stub.o -o $@
