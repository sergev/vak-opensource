#
#
#
SHELL	= /bin/sh

CROSS_COMPILE	= mips-mti-linux-gnu-
U_BOOT_DIR	= u-boot-pic32

CC	= $(CROSS_COMPILE)gcc
OBJCOPY	= $(CROSS_COMPILE)objcopy
SREC	= srec_cat

DEVCFG	= -Wl,--section-start,.devcfg=0xBFC0FFBC
RESET	= -Wl,--section-start,.reset=0x9FC00000
TEXT	= -Ttext 0x9D004000
CFLAGS	= -nostdinc -nostdlib -EL -mabi=32 -march=mips32r2 -Wa,-mips32r2 \
	  -G 0 -mabicalls -msoft-float $(DEVCFG) $(RESET) $(TEXT)

ifeq ($(MEMSIZE),32)
	CFLAGS	+= -DPIC32_MEM_32M
endif

U_BOOT	= $(U_BOOT_DIR)/u-boot.bin

all:	obj/u-boot-32m.hex obj/u-boot-128m.hex

obj/u-boot-32m.hex:
	$(MAKE) $(MFLAGS) MEMSIZE=32 u-boot-hex

obj/u-boot-128m.hex:
	$(MAKE) $(MFLAGS) MEMSIZE=128 u-boot-hex

u-boot-hex:	head.c Makefile obj/u-boot.hex
	mkdir -p obj
	$(CC) -O2 -Wall $(CFLAGS) -e dummy -o obj/head-$(MEMSIZE).elf head.c
	$(OBJCOPY) -O binary \
	    -j .devcfg obj/head-$(MEMSIZE).elf obj/devcfg-$(MEMSIZE).bin
	$(SREC) -output - -intel \
	    obj/devcfg-$(MEMSIZE).bin -binary -offset 0x1FC0FFBC | \
	    grep -v ':00000001FF' > obj/devcfg-$(MEMSIZE).hex
	$(OBJCOPY) -O binary \
	    -j .reset obj/head-$(MEMSIZE).elf obj/reset-$(MEMSIZE).bin
	$(SREC) -output - -intel \
	    obj/reset-$(MEMSIZE).bin -binary -offset 0x1FC00000 | \
	    grep -v ':00000001FF' > obj/reset-$(MEMSIZE).hex
	cat obj/devcfg-$(MEMSIZE).hex obj/reset-$(MEMSIZE).hex \
	    obj/u-boot.hex > obj/u-boot-$(MEMSIZE)m.hex

obj/u-boot.hex:	$(U_BOOT)
	mkdir -p obj
	$(SREC) -output obj/u-boot.hex -intel \
	    $(U_BOOT) -binary -offset 0x1D004000

clean:
	rm -rf obj
