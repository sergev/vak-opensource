CC		= gcc -Wall
CFLAGS		= -O3

all:		dipole.txt

dipole.txt:	dipole
		dipole > dipole.txt

dipole:		dipole.o
		$(CC) -o dipole dipole.o -lm

clean:
		rm -f *.o *~ dipole
