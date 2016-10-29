GOC		= gccgo -g
GOFLAGS		= -O -Wall -fno-split-stack -fno-exceptions
LDFLAGS		= #-static-libgo -lffi -lpthread

PROGS		= args goecho sieve stream stream.s chickens

all:		$(PROGS)

clean:
		rm -f *~ $(PROGS) a.out

args:		args.go
		$(GOC) $(LDFLAGS) $(GOFLAGS) $< -o $@

goecho:		goecho.go
		$(GOC) $(LDFLAGS) $(GOFLAGS) $< -o $@

sieve:		sieve.go
		$(GOC) $(LDFLAGS) $(GOFLAGS) $< -o $@

chickens:	chickens.go
		$(GOC) $(LDFLAGS) $(GOFLAGS) $< -o $@

stream:		stream.go
		$(GOC) $(LDFLAGS) $(GOFLAGS) $< -o $@

stream.s:	stream.go
		$(GOC) $(LDFLAGS) $(GOFLAGS) -S $<
