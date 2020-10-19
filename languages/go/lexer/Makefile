NEX     = $(HOME)/go/bin/nex

all: wc.nn
	./wc.nn < /etc/fstab

clean:
	rm -f wc.nn wc.nn.go

wc.nn:  wc.nn.go
	go build $<

wc.nn.go: wc.nex
	[ -f $(NEX) ] || go get github.com/blynn/nex
	$(NEX) $<
