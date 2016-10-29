all:            ticket-workflow.eps

clean:
		rm -f *~ *.pdf

.dot.eps:
		@rm -f $*.pdf $*.eps
		dot $< -Tpdf -o $*.pdf
		pdftops -eps $*.pdf
#		@rm -f $*.pdf

.SUFFIXES:      .dot .eps
