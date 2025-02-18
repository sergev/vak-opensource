# GNU toolchain.
#P               = /usr/local/mips-gcc-4.9.4/bin/mips-elf-
#CC              = $(P)gcc -EL -mips32r2 -g -nostdlib
#LD              = $(P)ld -EL -mips32r2 -g
#OBJCOPY         = $(P)objcopy --change-addresses=0x80000000
#OBJDUMP         = $(P)objdump -mmips:isa32r2

# LLVM toolchain.

# Install: 
# 	sudo apt install clang-18 lld-18`
#
# P               = /usr/lib/llvm-18/bin
# CC              = $(P)/clang -target mipsel -mcpu=mips32r2 -mabi=o32 -mfloat-abi=soft
# LD              = $(P)/ld.lld -m elf32ltsmip
# OBJCOPY         = $(P)/llvm-objcopy
# OBJDUMP         = $(P)/llvm-objdump --mcpu=mips32r2

# On Ubuntu: 
# 1. [what I used to get v18] install latest: 
# 	sudo bash -c "$(wget -O - https://apt.llvm.org/llvm.sh)"
# 2. OR: install specific version: ex: 18
#	wget https://apt.llvm.org/llvm.sh
#	chmod +x llvm.sh
#	sudo ./llvm.sh 18
#
P               = /usr/bin
CC              = $(P)/clang-18 -target mipsel -mcpu=mips32r2 -mabi=o32 -mfloat-abi=soft
LD              = $(P)/ld.lld-18 -m elf32ltsmip
OBJCOPY         = $(P)/llvm-objcopy-18
OBJDUMP         = $(P)/llvm-objdump-18 --mcpu=mips32r2

PROG            = uart
CFLAGS          = -Os -g -Wall -Werror -DPIC32MX7 \
                  -nostdlib -fomit-frame-pointer -finline-hint-functions
LDFLAGS         = -T using-bootloader.ld -e _start -g

all:            $(PROG).hex

$(PROG).hex:    $(PROG).c
		$(CC) $(CFLAGS) -c $<
		$(LD) $(LDFLAGS) $(PROG).o -o $(PROG).elf
		$(OBJCOPY) -O ihex $(PROG).elf $(PROG).hex
		$(OBJDUMP) -d -S $(PROG).elf > $(PROG).dis

load:           $(PROG).hex
		pic32prog $(PROG).hex

clean:
		rm -f *.o *.lst *~ *.elf *.hex *.dis
