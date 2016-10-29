CFLAGS 		= -Wall -O

all:		linear.png log.png

linear.png:	pyth.txt linear.plt
		gnuplot linear.plt

log.png:	pyth.txt log.plt
		gnuplot log.plt

pyth.txt:	pythagor
		./pythagor > pyth.txt

clean:
		rm -f *~ pythagor
