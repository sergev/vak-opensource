GOC		= gccgo -g
GOFLAGS		= -O -Wall -fno-split-stack -fno-exceptions
LDFLAGS		= #-static-libgo -lffi -lpthread

PROGS		= args goecho sieve stream chickens helloworld

all:		$(PROGS)

clean:
		rm -f *~ $(PROGS) a.out

args:		args.go
		go build $<
#		$(GOC) $(LDFLAGS) $(GOFLAGS) $< -o $@

goecho:		goecho.go
		go build $<
#		$(GOC) $(LDFLAGS) $(GOFLAGS) $< -o $@

sieve:		sieve.go
		go build $<
#		$(GOC) $(LDFLAGS) $(GOFLAGS) $< -o $@

chickens:	chickens.go
		go build $<
#		$(GOC) $(LDFLAGS) $(GOFLAGS) $< -o $@

stream:		stream.go
		go build $<
#		$(GOC) $(LDFLAGS) $(GOFLAGS) $< -o $@

helloworld:	helloworld.go
		go build $<
