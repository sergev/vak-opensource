PROG            = hello example format #arc
RUSTC           = rustc

all:		$(PROG)

%:              %.rs
		$(RUSTC) $<

clean:
		rm -f $(PROG) *~
