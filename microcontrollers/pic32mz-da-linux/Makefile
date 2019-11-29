#
# Path to U-Boot binary.
#
U_BOOT          = ../u-boot-pic32-sergev/u-boot.bin

#
# PIC32MZ2064DAxxxx device configuration words
#
DEVCFG4		= 0xF4FFFFFF
DEVCFG3		= 0xFEF0FFFF
DEVCFG2		= 0xF7F9B11A
DEVCFG1		= 0x5F74FCF9
DEVCFG0		= 0xF7FFFFD3
JMPMAIN         = 0x0B401000    # j 0x9D004000 <main>
NOP             = 0x00000000	# nop

#
# Use srec_cat utility to generate Intel hex output.
#
IHEX_CAT        = srec_cat -output - -intel

all:            u-boot.hex

u-boot.hex:     $(U_BOOT) Makefile
		( \
	            $(IHEX_CAT) -gen 0x1FC00000 0x1FC00004 -const-l-e $(JMPMAIN) 4; \
	            $(IHEX_CAT) -gen 0x1FC00004 0x1FC00008 -const-l-e $(NOP)     4; \
	            $(IHEX_CAT) -gen 0x1FC0FFBC 0x1FC0FFC0 -const-l-e $(DEVCFG4) 4; \
	            $(IHEX_CAT) -gen 0x1FC0FFC0 0x1FC0FFC4 -const-l-e $(DEVCFG3) 4; \
	            $(IHEX_CAT) -gen 0x1FC0FFC4 0x1FC0FFC8 -const-l-e $(DEVCFG2) 4; \
	            $(IHEX_CAT) -gen 0x1FC0FFC8 0x1FC0FFCC -const-l-e $(DEVCFG1) 4; \
	            $(IHEX_CAT) -gen 0x1FC0FFCC 0x1FC0FFD0 -const-l-e $(DEVCFG0) 4  \
	        ) | grep -v ':00000001FF' > u-boot.hex
		$(IHEX_CAT) $(U_BOOT) -binary -offset 0x1D004000 >> u-boot.hex

clean:
		rm -rf *.hex
