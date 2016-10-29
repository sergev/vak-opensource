CC		= /usr/local/avr433/bin/avr-gcc -g -mmcu=at90s2313 -Wall -Os
LD		= /usr/local/avr433/bin/avr-gcc -g -mmcu=at90s2313 -nostdlib -T $(LDSCRIPT)
LDSCRIPT	= ldscript23xx.x
STARTUP		= startup2313.o
SIZE		= /usr/local/avr433/bin/avr-size
OBJCOPY		= /usr/local/avr433/bin/avr-objcopy
OBJDUMP		= /usr/local/avr433/bin/avr-objdump

# Using avrdude programmer with STK200 LPT adapter.
#PROG           = avrdude -cstk200 -Enoreset -pat90s2313

# Using avrdude programmer with AVRISP USB adapter.
#PROG            = avrdude -cstk500v2 -P /dev/ttyUSB? -pat90s2313 -B2
PROG            = avrdude -cstk500v2 -P /dev/ttyUSB? -pattiny2313 -B2

all:		nm5101.srec cronyx.srec ipmce.srec
		$(SIZE) *.out

nm5101.srec:	$(LDSCRIPT) $(STARTUP) nm5101.o pictures.o
		$(LD) $(STARTUP) nm5101.o pictures.o -o nm5101.out
		$(OBJCOPY) -O srec nm5101.out $@
		chmod -x $@
		$(OBJDUMP) -D -S nm5101.out > nm5101.dis

cronyx.srec:	$(LDSCRIPT) $(STARTUP) phrases.o cronyx.o
		$(LD) $(STARTUP) phrases.o cronyx.o -o cronyx.out
		$(OBJCOPY) -O srec cronyx.out $@
		chmod -x $@

ipmce.srec:	$(LDSCRIPT) $(STARTUP) phrases.o ipmce.o
		$(LD) $(STARTUP) phrases.o ipmce.o -o ipmce.out
		$(OBJCOPY) -O srec ipmce.out $@
		chmod -x $@

gse.srec:	masterkit/gse.hex
		$(OBJCOPY) -I ihex -O srec $? $@
clean:
		rm -f *~ *.o *.out *.dis

probe:
		$(PROG) -v

load:		ipmce.srec
#		$(PROG) -e -u -Uflash:w:$< -Ulfuse:w:0xDF:m # -- for AT90S2313
		$(PROG) -e -u -Uflash:w:$< -Ulfuse:w:0xFD:m -Uhfuse:w:0xD9:m -Uefuse:w:0xFF:m # -- for ATtiny2313
