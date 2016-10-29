#
# You must install these packages first:
# pilrc - from http://sourceforge.net/projects/pilrc/
# fontconv - ftom http://sourceforge.net/projects/palmfontconv/
# pdbc - from http://obermuhlner.com/public/Projects/Palm/PDBC/
#
FONT	= JournalRus_OS_5
SRC	= journal.pd journal-bold.pfn journal-normal.pfn small-bold.pfn \
	  small-normal.pfn journal.rcp
UTIL	= build.bat fontconv.exe pdbc.exe pdbdec.exe pilrc.exe
README	= README.dos README.win

all:	$(FONT).prc $(FONT).zip JournalRus_src.zip

$(FONT).prc: $(SRC)
	pilrc journal.rcp
	fontconv NFNT03e8.bin -i NFNT03ea.bin 2a normal.bin
	fontconv NFNT03e9.bin -i NFNT03eb.bin 2a bold.bin
	pdbc -o $(FONT).prc journal.pd
	-@rm -f *.bin

$(FONT).zip: $(FONT).prc $(README)
	zip -q $(FONT).zip $(FONT).prc README.dos README.win

clean:
	rm -f *~ *.bin

cleandist: clean
	rm -f *.prc *.zip

JournalRus_src.zip: $(SRC) $(UTIL) $(README)
	zip -q JournalRus_src $(SRC) $(UTIL) $(README)
