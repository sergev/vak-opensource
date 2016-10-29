PROG            = chapt03a chapt03b chapt03c chapt03x
COBC		= cobc -x

all:		$(PROG)

clean:
		rm -f *~ $(PROG)

%:		%.cob
		$(COBC) $< -o $@
