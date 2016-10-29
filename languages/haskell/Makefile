PROGS		= problem-22 problem-xn sieve

all:		$(PROGS)

problem-22:	problem-22.hs
		ghc $@.hs -o $@

problem-xn:	problem-xn.hs
		ghc $@.hs -o $@

sieve:		sieve.hs
		ghc $@.hs -o $@

clean:
		rm -f *~ *.o *.hi $(PROGS)
