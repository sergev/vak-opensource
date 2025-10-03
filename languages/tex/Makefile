#
# Install depencencies:
#       brew install tectonic
#
.PHONY: all clean

all:    forest5.pdf forest8.pdf

%.pdf:  %.tex
	tectonic $<

clean:
	rm -rf *.pdf
