#
# From http://ioccc.org/years.html#2004
#
CFLAGS	      = -Wall -O
FILES	      = boot.b lilo.conf gavin_install.txt

all:		gavin.c $(FILES)
		${CC} ${CFLAGS} gavin.c -o gavin
		rm -f kernel
		./gavin > kernel
		${CC} ${CFLAGS} -DB=_start '-Dputchar(a)=' -c gavin.c
		rm -f sh
		${LD} -s -o sh gavin.o
		rm -f vi
		cp sh vi
		rm -f fs.tar
		tar -cf fs.tar sh vi gavin.c gavin.hint

clean:
		rm -f *~ sh vi kernel gavin.o gavin fs.tar
