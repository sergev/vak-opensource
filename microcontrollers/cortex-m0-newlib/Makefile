CC              = arm-none-eabi-gcc -mcpu=cortex-m0plus -mthumb
OBJDUMP         = arm-none-eabi-objdump
CFLAGS          = -O -Wall -Werror
LDFLAGS         =
OBJ             = hello.o syscalls.o

all:            hello.elf long.o short.o

clean:
		rm -f hello.elf *.o

hello.elf:      $(OBJ)
		$(CC) $(LDFLAGS) $(CFLAGS) $(OBJ) -o $@
		$(OBJDUMP) -d -S $@ > hello.dis
