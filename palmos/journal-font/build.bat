pilrc journal.rcp
fontconv NFNT03e8.bin -i NFNT03ea.bin 2a normal.bin
fontconv NFNT03e9.bin -i NFNT03eb.bin 2a bold.bin
pdbc -o JournalRus_OS_5.prc journal.pd
@del *.bin
