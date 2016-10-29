PROG            = chapt04a chapt04c chapt04d chapt04x
COBC		= cobc -x

all:		$(PROG)

clean:
		rm -f *~ $(PROG)

%:		%.cob
		$(COBC) $< -o $@
