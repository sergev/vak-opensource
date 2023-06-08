PDF     = week1.pdf

all:    $(PDF)

clean:
	rm -f *.pdf *.aux *.log

%.pdf:  %.tex
	pdflatex $<
