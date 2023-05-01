CC      = arm-none-eabi-gcc
OBJDUMP = arm-none-eabi-objdump
ARCH_FLAGS = -mcpu=cortex-m4 -mthumb -msingle-pic-base -mpic-register=r9 -mno-pic-data-is-text-relative
#ARCH_FLAGS = -mcpu=cortex-m0plus -mthumb -msingle-pic-base -mpic-register=r9 -mno-pic-data-is-text-relative
# TODO: -fpie -fno-plt
CFLAGS  = -g -O -Wall -I.. $(ARCH_FLAGS) -fPIC
LDFLAGS = -g $(ARCH_FLAGS) -nostdlib -shared -fPIC
LIBS    = mylib-arm.so

all:    $(LIBS) hello.elf

clean:
	rm -f *.o *.so *.elf

hello.elf: ../hello.c
	$(CC) $(CFLAGS) -c $< -o hello.o
	$(CC) $(LDFLAGS) -e main hello.o $(LIBS) -o $@ # -symbolic
	$(OBJDUMP) -D $@ > hello.dis

mylib-arm.so: ../stub.c
	$(CC) $(CFLAGS) -c $< -o stub.o
	$(CC) $(LDFLAGS) stub.o -o $@
