CC		= cc17
CFLAGS		= -DMHZ=16384000 -O
OBJ		= startup.s
ALL		= chardivs.hex chardivc.hex intshift.hex intarith.hex \
		  shortari.hex charcmp.hex shortcmp.hex #longcmp.hex longarit.hex

all:		$(ALL)

clean:
		rm -f *~ *.hex *.[bis] *.core *.lst

charcmp.hex:	charcmp.c $(OBJ)
		$(CC) $(CFLAGS) $(OBJ) charcmp.c -o charcmp.hex

chardivs.hex:	chardivs.c $(OBJ)
		$(CC) $(CFLAGS) $(OBJ) chardivs.c -o chardivs.hex

chardivc.hex:	chardivc.c $(OBJ)
		$(CC) $(CFLAGS) $(OBJ) chardivc.c -o chardivc.hex

longcmp.hex:	longcmp.c $(OBJ)
		$(CC) $(CFLAGS) $(OBJ) longcmp.c -o longcmp.hex

shortcmp.hex:	shortcmp.c $(OBJ)
		$(CC) $(CFLAGS) $(OBJ) shortcmp.c -o shortcmp.hex

intarith.hex:	intarith.c $(OBJ)
		$(CC) $(CFLAGS) $(OBJ) intarith.c -o intarith.hex

shortari.hex:	shortari.c $(OBJ)
		$(CC) $(CFLAGS) $(OBJ) shortari.c -o shortari.hex

longarit.hex:	longarit.c $(OBJ)
		$(CC) $(CFLAGS) $(OBJ) longarit.c -o longarit.hex

intshift.hex:	intshift.s $(OBJ)
		$(CC) $(CFLAGS) $(OBJ) intshift.s -o intshift.hex

.SUFFIXES::	.i .c .s

.c.s:
		$(CC) $(CFLAGS) -S $<

.c.i:
		$(CPP) -I/usr/local/lib/pic17/include $(CFLAGS) $< > $*.i
