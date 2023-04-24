CC      = arm-none-eabi-gcc
ARCH_FLAGS = -march=armv6t2 -mthumb -msingle-pic-base -mpic-register=r9 -mno-pic-data-is-text-relative
CFLAGS  = -g -O -Wall -I.. $(ARCH_FLAGS)
LDFLAGS = -g $(ARCH_FLAGS)
LIBS    = mylib-arm.so

all:    $(LIBS) hello.elf

clean:
	rm -f *.o *.so *.elf

hello.elf: ../hello.c
	$(CC) $(CFLAGS) -c -fPIC $< -o hello.o
	$(CC) $(LDFLAGS) -shared -fPIC hello.o $(LIBS) -o $@

mylib-arm.so: ../stub.c
	$(CC) $(CFLAGS) -c -fPIC $< -o stub.o
	$(CC) $(LDFLAGS) -shared -fPIC stub.o -o $@
