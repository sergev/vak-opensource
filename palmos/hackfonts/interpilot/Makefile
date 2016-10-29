all:		interpilot.pdb

interpilot.pdb:	script.pd rus-win-25.prc
		fextract rus-win-25.prc
		pdbc -o interpilot.pdb script.pd

clean:
		rm -f *~ *.pft *.pfn
