all:		4read.pdb

4read.pdb:	script.pd 4Read-4-clie-1251.prc
		fextract 4Read-4-clie-1251.prc
		pdbc -o 4read.pdb script.pd

clean:
		rm -f *~ *.pft *.pfn
