CFLAGS  = -g -Wall
LDFLAGS =

all:    read_elf

clean:
	rm -f *.o read_elf

read_elf.o: read_elf.c elf.h read_elf32.c read_elf64.c
