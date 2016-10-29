all:		lyanguzov.pdb

lyanguzov.pdb:	script.pd Russian-4-clie-1251.prc
		fextract Russian-4-clie-1251.prc
		pdbc -o lyanguzov.pdb script.pd

clean:
		rm -f *~ *.pft *.pfn
