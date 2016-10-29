all:		palmos5.pdb

palmos5.pdb:	script.pd russian.prc
		fextract russian.prc
		pdbc -o palmos5.pdb script.pd

clean:
		rm -f *~ *.pft *.pfn
