all:		sony.pdb

sony.pdb:	script.pd russian.prc
		fextract russian.prc
		pdbc -o sony.pdb script.pd

clean:
		rm -f *~ *.pft *.pfn
