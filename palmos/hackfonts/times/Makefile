all:		times.pdb

times.pdb:	script.pd Times-4-clie-1251.prc
		fextract Times-4-clie-1251.prc
		pdbc -o times.pdb script.pd

clean:
		rm -f *~ *.pft *.pfn
