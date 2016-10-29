PROG            = hello example #arc
RUSTC           = rustc

all:		$(PROG)

%:              %.rs
		$(RUSTC) $<

clean:
		rm -f $(PROG) *~
