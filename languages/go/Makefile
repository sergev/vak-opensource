PROGS		= args goecho sieve stream chickens helloworld

all:		$(PROGS)

clean:
		rm -f *~ $(PROGS) a.out

args:		args.go
		go build $<

goecho:		goecho.go
		go build $<

sieve:		sieve.go
		go build $<

chickens:	chickens.go
		go build $<

stream:		stream.go
		go build $<

helloworld:	helloworld.go
		go build $<
