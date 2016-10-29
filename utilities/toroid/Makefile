all:		dipole.maa delta.maa quad.maa magloop1-12.maa \
		toroid1-16.maa toroid12-16.maa toroid15-16.maa \
		toroid12-16-50.maa helquad12-24-50.maa \
		helquad1-16.maa helquad12-16.maa helquad15-16.maa \
		helquad-40m.maa helquad-20m.maa helquad-10m.maa \
		toroid-eff-bw.png helquad-eff-bw.png

clean:
		rm -f *~ *.maa

toroid-eff-bw.png: toroid.plt
		gnuplot toroid.plt

helquad-eff-bw.png: helquad.plt
		gnuplot helquad.plt

dipole.maa:	dipole.py
		dipole.py > dipole.maa

delta.maa:	delta.py
		delta.py > delta.maa

quad.maa:	quad.py
		quad.py > quad.maa

loop-12.maa:	loop.py
		loop.py > loop-12.maa

magloop1-12.maa: magloop.py Makefile
		magloop.py 0.5  12 18.31 > magloop1-12.maa
		magloop.py 0.6  12 12.84 > magloop12-12.maa
		magloop.py 0.75 12  7.27 > magloop15-12.maa
#		magloop.py 1.0  12  1.15 > magloop2-12.maa

toroid2-12.maa:	toroid.py Makefile
		toroid.py 1.0 12 1.239 > toroid2-12.maa
		toroid.py 1.0 16 1.306 > toroid2-16.maa

toroid15-16.maa: toroid.py Makefile
#		toroid.py 0.75 12 1.287  > toroid15-12.maa
		toroid.py 0.75 16 1.375  > toroid15-16.maa
		toroid.py 0.75 32 1.624  > toroid15-32.maa

toroid12-16.maa: toroid.py Makefile
		toroid.py 0.6 16 1.382 > toroid12-16.maa
		toroid.py 0.6 32 1.665 > toroid12-32.maa

toroid12-16-50.maa: toroid50.py Makefile
		toroid50.py 0.6 16 1.382 > toroid12-16-50.maa

toroid1-16.maa:	toroid.py Makefile
#		toroid.py 0.5 12 1.248 > toroid1-12.maa
		toroid.py 0.5 16 1.361 > toroid1-16.maa
		toroid.py 0.5 32 1.661 > toroid1-32.maa
#		toroid.py 0.5 64 1.992 > toroid1-64.maa

helquad1-16.maa: helquad.py Makefile
		helquad.py 21.050 1 16 1.363 > helquad1-16.maa
		helquad.py 21.050 1 24 1.539 > helquad1-24.maa

helquad12-16.maa: helquad.py Makefile
		helquad.py 21.050 1.2 16 1.389 > helquad12-16.maa
		helquad.py 21.050 1.2 24 1.542 > helquad12-24.maa

helquad12-24-50.maa: helquad50.py Makefile
		helquad50.py 1.2 24 1.542 > helquad12-24-50.maa

helquad15-16.maa: helquad.py Makefile
		helquad.py 21.050 1.5 16 1.376 > helquad15-16.maa
		helquad.py 21.050 1.5 24 1.500 > helquad15-24.maa

# Диапазон 40 метров, полоса 54 кГц (надо 40 кГц)
helquad-40m.maa: helquad.py Makefile
		helquad.py 7.020 4 32 1.542 > helquad-40m.maa

# Диапазон 20 метров, полоса 107 кГц (надо 100 кГц)
helquad-20m.maa: helquad.py Makefile
		helquad.py 14.050 2 32 1.595 > helquad-20m.maa

# Диапазон 10 метров, полоса 203 кГц (надо 200 кГц)
helquad-10m.maa: helquad.py Makefile
		helquad.py 28.100 1 24 1.552 > helquad-10m.maa

star-7.maa:	star.py
		star.py > star-7.maa
