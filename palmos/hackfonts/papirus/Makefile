all:		papirus.pdb

papirus.pdb:	script.pd p41fullsonynr701.prc
		fextract p41fullsonynr701.prc
		pdbc -o papirus.pdb script.pd

clean:
		rm -f *~ *.pft *.pfn
