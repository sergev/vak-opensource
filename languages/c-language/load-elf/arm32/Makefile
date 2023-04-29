CC      = arm-none-eabi-gcc
OBJDUMP = arm-none-eabi-objdump
ARCH_FLAGS = -mcpu=cortex-m0plus -mthumb -msingle-pic-base -mpic-register=r9 -mno-pic-data-is-text-relative
# TODO: -fpie -fno-plt
CFLAGS  = -g -O -Wall -I.. $(ARCH_FLAGS)
LDFLAGS = -g $(ARCH_FLAGS) -nostdlib
LIBS    = mylib-arm.so

all:    $(LIBS) hello.elf

clean:
	rm -f *.o *.so *.elf

hello.elf: ../hello.c
	$(CC) $(CFLAGS) -c -fPIC $< -o hello.o
	$(CC) $(LDFLAGS) -e main -shared -symbolic -fPIC hello.o $(LIBS) -o $@
	$(OBJDUMP) -D $@ > hello.dis

mylib-arm.so: ../stub.c
	$(CC) $(CFLAGS) -c -fPIC $< -o stub.o
	$(CC) $(LDFLAGS) -shared -fPIC stub.o -o $@
