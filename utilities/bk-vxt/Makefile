CFLAGS		= -Wall -g
LDFLAGS		= -liconv

all:		vxt-to-utf8 txt

clean:
		rm -f *~ *.o vxt-to-utf8 irps.log

txt:		vxt-to-utf8
		./vxt-to-utf8 ANBASIC.VXT > anbasic.txt
		./vxt-to-utf8 ISM_VX.VXT  > ism_vx.txt
		./vxt-to-utf8 PRIST2.VXT  > prist2.txt
		./vxt-to-utf8 PRISTM1.VXT > pristm1.txt
		./vxt-to-utf8 SPECIF.VXT  > specif.txt
