CFLAGS		= -Wall -O -g -I/usr/include/freetype2
LDFLAGS		= -L/opt/local/lib -lpng

FONTS           = digits32.c digits20.c

#FONTS           += lucidasans11.c lucidasans15.c lucidasans9.c lucidasans7.c
#FONTS           += verdana7.c
#FONTS           += 5x7.c 6x9.c

FONTS           += dejavusans8.c dejavusans10.c dejavusans12.c dejavusans14.c dejavusans18.c
#FONTS           += dejavusanscond8.c dejavusanscond10.c dejavusanscond12.c dejavusanscond14.c dejavusanscond18.c
#FONTS           += freesans8.c freesans10.c freesans12.c freesans14.c freesans18.c
#FONTS           += liberationsans8.c liberationsans10.c liberationsans12.c liberationsans14.c liberationsans18.c
#FONTS           += liberationserif8.c liberationserif10.c liberationserif12.c liberationserif14.c liberationserif18.c
#FONTS           += ubuntu8.c ubuntu10.c ubuntu12.c ubuntu14.c ubuntu18.c

# Latin
CHARSET         = 20_126 164

# Cyrillics: Ё, ё, А-я
#CHARSET         += 1025 1105 1040_1103

all:		$(FONTS)

compile:        $(FONTS)
		$(CC) -c $(FONTS)
		size *.o

otf2bdf:        otf2bdf.o remap.o
		$(CC) $(LDFLAGS) otf2bdf.o remap.o -lfreetype -o $@

#
# Get Lucida_Sans_Unicode.ttf from internet free font collection:
# http://www.webpagepublicity.com/free-fonts/l/Lucida%20Sans%20Unicode.ttf
#
digits32.c:	otf2bdf convbdf ttf/Lucida_Sans_Unicode.ttf
		./otf2bdf -p 32 -l "32 48_58" ttf/Lucida_Sans_Unicode.ttf > digits32.bdf
		./convbdf -a12 -d15 digits32.bdf
		rm -f digits32.bdf

digits20.c:	otf2bdf convbdf ttf/Lucida_Sans_Unicode.ttf
		./otf2bdf -p 20 -l "32 48_58" ttf/Lucida_Sans_Unicode.ttf > digits20.bdf
		./convbdf -a9 -d11 digits20.bdf
		rm -f digits20.bdf

lucidasans11.c:	otf2bdf convbdf ttf/Lucida_Sans_Unicode.ttf
		./otf2bdf -p 11 -l "$(CHARSET)" ttf/Lucida_Sans_Unicode.ttf > lucidasans11.bdf
		./convbdf -u164 -a2 -d3 lucidasans11.bdf
		rm -f lucidasans11.bdf

lucidasans15.c:	otf2bdf convbdf ttf/Lucida_Sans_Unicode.ttf
		./otf2bdf -p 15 -l "$(CHARSET)" ttf/Lucida_Sans_Unicode.ttf > lucidasans15.bdf
		./convbdf -u164 -a5 -d5 lucidasans15.bdf
		rm -f lucidasans15.bdf

lucidasans28.c:	otf2bdf convbdf ttf/Lucida_Sans_Unicode.ttf
		./otf2bdf -p 28 -l "$(CHARSET)" ttf/Lucida_Sans_Unicode.ttf > lucidasans28.bdf
		./convbdf -u164 -a11 -d9 lucidasans28.bdf
		rm -f lucidasans28.bdf

lucidasans9.c:	otf2bdf convbdf ttf/Lucida_Sans_Unicode.ttf
		./otf2bdf -p 9 -l "$(CHARSET)" ttf/Lucida_Sans_Unicode.ttf > lucidasans9.bdf
		./convbdf -u164 -a2 -d3 lucidasans9.bdf
		rm -f lucidasans9.bdf

lucidasans7.c:	otf2bdf convbdf ttf/Lucida_Sans_Unicode.ttf
		./otf2bdf -p 7 -l "$(CHARSET)" ttf/Lucida_Sans_Unicode.ttf > lucidasans7.bdf
		./convbdf -u164 -a2 -d3 lucidasans7.bdf
		rm -f lucidasans7.bdf

#
# Use /usr/share/fonts/truetype/msttcorefonts/Verdana.ttf
#
verdana7.c:	otf2bdf convbdf ttf/Verdana.ttf
		./otf2bdf -p 7 -l "$(CHARSET)" ttf/Verdana.ttf > verdana7.bdf
		./convbdf -a2 verdana7.bdf
		rm -f verdana7.bdf

5x7.c:		convbdf bdf/5x7.bdf
		./convbdf -u0 -l0x44f -x0xa0-0x40f bdf/5x7.bdf

6x9.c:		convbdf bdf/6x9.bdf
		./convbdf -u0 -l0x44f -x0xa0-0x40f bdf/6x9.bdf

clean:
		-rm -f *~ *.o convbdf otf2bdf $(FONTS)

#
# DejaVuSans.ttf
#
dejavusans8.c:	otf2bdf convbdf ttf/DejaVuSans.ttf
		./otf2bdf -p 8 -l "$(CHARSET)" ttf/DejaVuSans.ttf > dejavusans8.bdf
		./convbdf -a1 dejavusans8.bdf
		rm -f dejavusans8.bdf

dejavusans10.c:	otf2bdf convbdf ttf/DejaVuSans.ttf
		./otf2bdf -p 10 -l "$(CHARSET)" ttf/DejaVuSans.ttf > dejavusans10.bdf
		./convbdf -a1 dejavusans10.bdf
		rm -f dejavusans10.bdf

dejavusans12.c:	otf2bdf convbdf ttf/DejaVuSans.ttf
		./otf2bdf -p 12 -l "$(CHARSET)" ttf/DejaVuSans.ttf > dejavusans12.bdf
		./convbdf -a2 -d1 dejavusans12.bdf
		rm -f dejavusans12.bdf

dejavusans14.c:	otf2bdf convbdf ttf/DejaVuSans.ttf
		./otf2bdf -p 14 -l "$(CHARSET)" ttf/DejaVuSans.ttf > dejavusans14.bdf
		./convbdf -a3 -d1 dejavusans14.bdf
		rm -f dejavusans14.bdf

dejavusans18.c:	otf2bdf convbdf ttf/DejaVuSans.ttf
		./otf2bdf -p 18 -l "$(CHARSET)" ttf/DejaVuSans.ttf > dejavusans18.bdf
		./convbdf -a4 -d1 dejavusans18.bdf
		rm -f dejavusans18.bdf

# DejaVuSansCondensed.ttf
# FreeSans.ttf
# LiberationSans-Regular.ttf
# LiberationSerif-Regular.ttf
# Ubuntu-R.ttf
