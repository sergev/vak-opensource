all:		verdana.pdb

verdana.pdb:	script.pd Verdana.prc
		fextract Verdana.prc
		pdbc -o verdana.pdb script.pd

clean:
		rm -f *~ *.pfn *.pft
