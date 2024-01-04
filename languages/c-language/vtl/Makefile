CC=gcc
CFLAGS=-Wall -Os

all : vtl

vtl : vtl.o system.o conio.o
	$(CC) $(CFLAGS) -o $@ $+

vtl.o : vtl.c
	$(CC) $(CFLAGS) -c $<

system.o : system.c system.h
	$(CC) $(CFLAGS) -c $<

conio.o : conio.c conio.h
	$(CC) $(CFLAGS) -c $<

clean :
	rm -f vtl vtl.o system.o conio.o
