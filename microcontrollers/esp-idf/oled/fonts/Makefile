CFLAGS		= -Wall -O -g -I/usr/include/freetype2
LDFLAGS		= -L/opt/local/lib -lpng

#
# Linux fonts
#
FONTS           += dejavusans8.c dejavusans10.c dejavusans12.c dejavusans14.c dejavusans18.c
FONTS           += dejavusanscond8.c dejavusanscond10.c dejavusanscond12.c dejavusanscond14.c dejavusanscond18.c
FONTS           += freesans8.c freesans10.c freesans12.c freesans14.c freesans18.c
FONTS           += liberationsans8.c liberationsans10.c liberationsans12.c liberationsans14.c liberationsans18.c
FONTS           += liberationserif8.c liberationserif10.c liberationserif12.c liberationserif14.c liberationserif18.c
FONTS           += ubuntu8.c ubuntu10.c ubuntu12.c ubuntu14.c ubuntu18.c
TTF_DejaVuSans  = /usr/share/fonts/truetype/dejavu/DejaVuSans.ttf
TTF_DejaVuSansC = /usr/share/fonts/truetype/dejavu/DejaVuSansCondensed.ttf
TTF_FreeSans    = /usr/share/fonts/truetype/freefont/FreeSans.ttf
TTF_LiberSans   = /usr/share/fonts/truetype/liberation/LiberationSans-Regular.ttf
TTF_LiberSerif  = /usr/share/fonts/truetype/liberation/LiberationSerif-Regular.ttf
TTF_Ubuntu      = /usr/share/fonts/truetype/ubuntu/Ubuntu-R.ttf

#
# Windows fonts
#
FONTS           += lucidasans28.c
FONTS           += verdana8.c verdana10.c verdana12.c verdana14.c verdana18.c
FONTS           += tahoma8.c tahoma10.c tahoma12.c tahoma14.c tahoma18.c
FONTS           += georgia8.c georgia10.c georgia12.c georgia14.c georgia18.c
FONTS           += digits32.c digits20.c
TTF_Lucida      = /opt/arduino-1.8.5/java/lib/fonts/LucidaSansRegular.ttf
TTF_Verdana     = /usr/share/fonts/truetype/msttcorefonts/Verdana.ttf
TTF_Tahoma      = /usr/share/wine/fonts/tahoma.ttf
TTF_Georgia     = /usr/share/fonts/truetype/msttcorefonts/Georgia.ttf

#
# Bitmap fonts
#
#FONTS           += 5x7.c 6x9.c
FONTS           += courier8.c courier10.c courier12.c courier14.c courier18.c courier24.c
FONTS           += helvetica8.c helvetica10.c helvetica12.c helvetica14.c helvetica18.c helvetica24.c
FONTS           += lucidabright8.c lucidabright10.c lucidabright12.c lucidabright14.c lucidabright18.c lucidabright19.c lucidabright24.c
FONTS           += lucidasans8.c lucidasans10.c lucidasans12.c lucidasans14.c lucidasans18.c lucidasans19.c lucidasans24.c
FONTS           += times8.c times10.c times12.c times14.c times18.c times24.c

# Latin
CHARSET         = 20_126 164

# Cyrillics: Ё, ё, А-я
#CHARSET         += 1025 1105 1040_1103

all:		$(FONTS)

compile:        $(FONTS)
		$(CC) -c $(FONTS)
		size *.o

clean:
		-rm -f *~ *.o convbdf otf2bdf $(FONTS)

otf2bdf:        otf2bdf.o remap.o
		$(CC) $(LDFLAGS) otf2bdf.o remap.o -lfreetype -o $@

#
# BDF fonts
#
# For cyrillics, use options: -u0 -l0x44f -x0xa0-0x40f
vpath %.bdf bdf
%.c:            %.bdf convbdf
		./convbdf -u0 -s0x20 -l0x7e $<

lucidasans8.c:  lucidasans8.bdf convbdf
		./convbdf -u0 -s0x20 -l0x7e -a1 -d1 $<

lucidasans10.c: lucidasans10.bdf convbdf
		./convbdf -u0 -s0x20 -l0x7e -a2 -d0 $<

lucidasans12.c: lucidasans12.bdf convbdf
		./convbdf -u0 -s0x20 -l0x7e -a2 -d1 $<

lucidasans14.c: lucidasans14.bdf convbdf
		./convbdf -u0 -s0x20 -l0x7e -a4 -d1 $<

lucidasans18.c: lucidasans18.bdf convbdf
		./convbdf -u0 -s0x20 -l0x7e -a4 -d2 $<

lucidasans19.c: lucidasans19.bdf convbdf
		./convbdf -u0 -s0x20 -l0x7e -a4 -d2 $<

lucidasans24.c: lucidasans24.bdf convbdf
		./convbdf -u0 -s0x20 -l0x7e -a5 -d2 $<

#
# Lucida Sans Regular
#
lucidasans28.c:	otf2bdf convbdf $(TTF_Lucida)
		./otf2bdf -p 28 -l "$(CHARSET)" $(TTF_Lucida) > lucidasans28.bdf
		./convbdf -u164 -a7 -d3 lucidasans28.bdf
		rm -f lucidasans28.bdf

digits20.c:	otf2bdf convbdf $(TTF_Lucida)
		./otf2bdf -p 20 -l "32 48_58" $(TTF_Lucida) > digits20.bdf
		./convbdf -a5 -d4 digits20.bdf
		rm -f digits20.bdf

digits32.c:	otf2bdf convbdf $(TTF_Lucida)
		./otf2bdf -p 32 -l "32 48_58" $(TTF_Lucida) > digits32.bdf
		./convbdf -a7 -d8 digits32.bdf
		rm -f digits32.bdf

#
# Verdana
#
verdana8.c:	otf2bdf convbdf $(TTF_Verdana)
		./otf2bdf -p 8 -l "$(CHARSET)" $(TTF_Verdana) > verdana8.bdf
		./convbdf -a2 -d0 verdana8.bdf
		rm -f verdana8.bdf

verdana10.c:	otf2bdf convbdf $(TTF_Verdana)
		./otf2bdf -p 10 -l "$(CHARSET)" $(TTF_Verdana) > verdana10.bdf
		./convbdf -a3 -d0 verdana10.bdf
		rm -f verdana10.bdf

verdana12.c:	otf2bdf convbdf $(TTF_Verdana)
		./otf2bdf -p 12 -l "$(CHARSET)" $(TTF_Verdana) > verdana12.bdf
		./convbdf -a4 -d0 verdana12.bdf
		rm -f verdana12.bdf

verdana14.c:	otf2bdf convbdf $(TTF_Verdana)
		./otf2bdf -p 14 -l "$(CHARSET)" $(TTF_Verdana) > verdana14.bdf
		./convbdf -a4 -d0 verdana14.bdf
		rm -f verdana14.bdf

verdana18.c:	otf2bdf convbdf $(TTF_Verdana)
		./otf2bdf -p 18 -l "$(CHARSET)" $(TTF_Verdana) > verdana18.bdf
		./convbdf -a5 -d1 verdana18.bdf
		rm -f verdana18.bdf

#
# Tahoma
#
tahoma8.c:	otf2bdf convbdf $(TTF_Tahoma)
		./otf2bdf -p 8 -l "$(CHARSET)" $(TTF_Tahoma) > tahoma8.bdf
		./convbdf -a2 -d0 tahoma8.bdf
		rm -f tahoma8.bdf

tahoma10.c:	otf2bdf convbdf $(TTF_Tahoma)
		./otf2bdf -p 10 -l "$(CHARSET)" $(TTF_Tahoma) > tahoma10.bdf
		./convbdf -a3 -d0 tahoma10.bdf
		rm -f tahoma10.bdf

tahoma12.c:	otf2bdf convbdf $(TTF_Tahoma)
		./otf2bdf -p 12 -l "$(CHARSET)" $(TTF_Tahoma) > tahoma12.bdf
		./convbdf -a5 -d0 tahoma12.bdf
		rm -f tahoma12.bdf

tahoma14.c:	otf2bdf convbdf $(TTF_Tahoma)
		./otf2bdf -p 14 -l "$(CHARSET)" $(TTF_Tahoma) > tahoma14.bdf
		./convbdf -a5 -d0 tahoma14.bdf
		rm -f tahoma14.bdf

tahoma18.c:	otf2bdf convbdf $(TTF_Tahoma)
		./otf2bdf -p 18 -l "$(CHARSET)" $(TTF_Tahoma) > tahoma18.bdf
		./convbdf -a6 -d0 tahoma18.bdf
		rm -f tahoma18.bdf

#
# Georgia
#
georgia8.c:	otf2bdf convbdf $(TTF_Georgia)
		./otf2bdf -p 8 -l "$(CHARSET)" $(TTF_Georgia) > georgia8.bdf
		./convbdf -a1 -d0 georgia8.bdf
		rm -f georgia8.bdf

georgia10.c:	otf2bdf convbdf $(TTF_Georgia)
		./otf2bdf -p 10 -l "$(CHARSET)" $(TTF_Georgia) > georgia10.bdf
		./convbdf -a2 -d0 georgia10.bdf
		rm -f georgia10.bdf

georgia12.c:	otf2bdf convbdf $(TTF_Georgia)
		./otf2bdf -p 12 -l "$(CHARSET)" $(TTF_Georgia) > georgia12.bdf
		./convbdf -a2 -d0 georgia12.bdf
		rm -f georgia12.bdf

georgia14.c:	otf2bdf convbdf $(TTF_Georgia)
		./otf2bdf -p 14 -l "$(CHARSET)" $(TTF_Georgia) > georgia14.bdf
		./convbdf -a3 -d1 georgia14.bdf
		rm -f georgia14.bdf

georgia18.c:	otf2bdf convbdf $(TTF_Georgia)
		./otf2bdf -p 18 -l "$(CHARSET)" $(TTF_Georgia) > georgia18.bdf
		./convbdf -a3 -d1 georgia18.bdf
		rm -f georgia18.bdf

#
# DejaVu Sans
#
dejavusans8.c:	otf2bdf convbdf $(TTF_DejaVuSans)
		./otf2bdf -p 8 -l "$(CHARSET)" $(TTF_DejaVuSans) > dejavusans8.bdf
		./convbdf -a1 dejavusans8.bdf
		rm -f dejavusans8.bdf

dejavusans10.c:	otf2bdf convbdf $(TTF_DejaVuSans)
		./otf2bdf -p 10 -l "$(CHARSET)" $(TTF_DejaVuSans) > dejavusans10.bdf
		./convbdf -a1 dejavusans10.bdf
		rm -f dejavusans10.bdf

dejavusans12.c:	otf2bdf convbdf $(TTF_DejaVuSans)
		./otf2bdf -p 12 -l "$(CHARSET)" $(TTF_DejaVuSans) > dejavusans12.bdf
		./convbdf -a2 -d1 dejavusans12.bdf
		rm -f dejavusans12.bdf

dejavusans14.c:	otf2bdf convbdf $(TTF_DejaVuSans)
		./otf2bdf -p 14 -l "$(CHARSET)" $(TTF_DejaVuSans) > dejavusans14.bdf
		./convbdf -a3 -d1 dejavusans14.bdf
		rm -f dejavusans14.bdf

dejavusans18.c:	otf2bdf convbdf $(TTF_DejaVuSans)
		./otf2bdf -p 18 -l "$(CHARSET)" $(TTF_DejaVuSans) > dejavusans18.bdf
		./convbdf -a4 -d1 dejavusans18.bdf
		rm -f dejavusans18.bdf

#
# DejaVu Sans Condensed
#
dejavusanscond8.c: otf2bdf convbdf $(TTF_DejaVuSansC)
		./otf2bdf -p 8 -l "$(CHARSET)" $(TTF_DejaVuSansC) > dejavusanscond8.bdf
		./convbdf -a1 dejavusanscond8.bdf
		rm -f dejavusanscond8.bdf

dejavusanscond10.c: otf2bdf convbdf $(TTF_DejaVuSansC)
		./otf2bdf -p 10 -l "$(CHARSET)" $(TTF_DejaVuSansC) > dejavusanscond10.bdf
		./convbdf -a1 dejavusanscond10.bdf
		rm -f dejavusanscond10.bdf

dejavusanscond12.c: otf2bdf convbdf $(TTF_DejaVuSansC)
		./otf2bdf -p 12 -l "$(CHARSET)" $(TTF_DejaVuSansC) > dejavusanscond12.bdf
		./convbdf -a2 -d1 dejavusanscond12.bdf
		rm -f dejavusanscond12.bdf

dejavusanscond14.c: otf2bdf convbdf $(TTF_DejaVuSansC)
		./otf2bdf -p 14 -l "$(CHARSET)" $(TTF_DejaVuSansC) > dejavusanscond14.bdf
		./convbdf -a3 -d1 dejavusanscond14.bdf
		rm -f dejavusanscond14.bdf

dejavusanscond18.c: otf2bdf convbdf $(TTF_DejaVuSansC)
		./otf2bdf -p 18 -l "$(CHARSET)" $(TTF_DejaVuSansC) > dejavusanscond18.bdf
		./convbdf -a4 -d1 dejavusanscond18.bdf
		rm -f dejavusanscond18.bdf

#
# FreeSans
#
freesans8.c:	otf2bdf convbdf $(TTF_FreeSans)
		./otf2bdf -p 8 -l "$(CHARSET)" $(TTF_FreeSans) > freesans8.bdf
		./convbdf -a1 -d0 freesans8.bdf
		rm -f freesans8.bdf

freesans10.c:	otf2bdf convbdf $(TTF_FreeSans)
		./otf2bdf -p 10 -l "$(CHARSET)" $(TTF_FreeSans) > freesans10.bdf
		./convbdf -a2 -d0 freesans10.bdf
		rm -f freesans10.bdf

freesans12.c:	otf2bdf convbdf $(TTF_FreeSans)
		./otf2bdf -p 12 -l "$(CHARSET)" $(TTF_FreeSans) > freesans12.bdf
		./convbdf -a2 -d0 freesans12.bdf
		rm -f freesans12.bdf

freesans14.c:	otf2bdf convbdf $(TTF_FreeSans)
		./otf2bdf -p 14 -l "$(CHARSET)" $(TTF_FreeSans) > freesans14.bdf
		./convbdf -a3 -d0 freesans14.bdf
		rm -f freesans14.bdf

freesans18.c:	otf2bdf convbdf $(TTF_FreeSans)
		./otf2bdf -p 18 -l "$(CHARSET)" $(TTF_FreeSans) > freesans18.bdf
		./convbdf -a3 -d1 freesans18.bdf
		rm -f freesans18.bdf

#
# Liberation Sans Regular
#
liberationsans8.c: otf2bdf convbdf $(TTF_LiberSans)
		./otf2bdf -p 8 -l "$(CHARSET)" $(TTF_LiberSans) > liberationsans8.bdf
		./convbdf -a1 -d0 liberationsans8.bdf
		rm -f liberationsans8.bdf

liberationsans10.c: otf2bdf convbdf $(TTF_LiberSans)
		./otf2bdf -p 10 -l "$(CHARSET)" $(TTF_LiberSans) > liberationsans10.bdf
		./convbdf -a2 -d0 liberationsans10.bdf
		rm -f liberationsans10.bdf

liberationsans12.c: otf2bdf convbdf $(TTF_LiberSans)
		./otf2bdf -p 12 -l "$(CHARSET)" $(TTF_LiberSans) > liberationsans12.bdf
		./convbdf -a3 -d0 liberationsans12.bdf
		rm -f liberationsans12.bdf

liberationsans14.c: otf2bdf convbdf $(TTF_LiberSans)
		./otf2bdf -p 14 -l "$(CHARSET)" $(TTF_LiberSans) > liberationsans14.bdf
		./convbdf -a3 -d0 liberationsans14.bdf
		rm -f liberationsans14.bdf

liberationsans18.c: otf2bdf convbdf $(TTF_LiberSans)
		./otf2bdf -p 18 -l "$(CHARSET)" $(TTF_LiberSans) > liberationsans18.bdf
		./convbdf -a4 -d0 liberationsans18.bdf
		rm -f liberationsans18.bdf

#
# Liberation Serif Regular
#
liberationserif8.c: otf2bdf convbdf $(TTF_LiberSerif)
		./otf2bdf -p 8 -l "$(CHARSET)" $(TTF_LiberSerif) > liberationserif8.bdf
		./convbdf -a1 -d0 liberationserif8.bdf
		rm -f liberationserif8.bdf

liberationserif10.c: otf2bdf convbdf $(TTF_LiberSerif)
		./otf2bdf -p 10 -l "$(CHARSET)" $(TTF_LiberSerif) > liberationserif10.bdf
		./convbdf -a2 -d0 liberationserif10.bdf
		rm -f liberationserif10.bdf

liberationserif12.c: otf2bdf convbdf $(TTF_LiberSerif)
		./otf2bdf -p 12 -l "$(CHARSET)" $(TTF_LiberSerif) > liberationserif12.bdf
		./convbdf -a3 -d0 liberationserif12.bdf
		rm -f liberationserif12.bdf

liberationserif14.c: otf2bdf convbdf $(TTF_LiberSerif)
		./otf2bdf -p 14 -l "$(CHARSET)" $(TTF_LiberSerif) > liberationserif14.bdf
		./convbdf -a3 -d0 liberationserif14.bdf
		rm -f liberationserif14.bdf

liberationserif18.c: otf2bdf convbdf $(TTF_LiberSerif)
		./otf2bdf -p 18 -l "$(CHARSET)" $(TTF_LiberSerif) > liberationserif18.bdf
		./convbdf -a5 -d0 liberationserif18.bdf
		rm -f liberationserif18.bdf

#
# Ubuntu Regular
#
ubuntu8.c:	otf2bdf convbdf $(TTF_Ubuntu)
		./otf2bdf -p 8 -l "$(CHARSET)" $(TTF_Ubuntu) > ubuntu8.bdf
		./convbdf -a1 -d0 ubuntu8.bdf
		rm -f ubuntu8.bdf

ubuntu10.c:	otf2bdf convbdf $(TTF_Ubuntu)
		./otf2bdf -p 10 -l "$(CHARSET)" $(TTF_Ubuntu) > ubuntu10.bdf
		./convbdf -a2 -d0 ubuntu10.bdf
		rm -f ubuntu10.bdf

ubuntu12.c:	otf2bdf convbdf $(TTF_Ubuntu)
		./otf2bdf -p 12 -l "$(CHARSET)" $(TTF_Ubuntu) > ubuntu12.bdf
		./convbdf -a2 -d0 ubuntu12.bdf
		rm -f ubuntu12.bdf

ubuntu14.c:	otf2bdf convbdf $(TTF_Ubuntu)
		./otf2bdf -p 14 -l "$(CHARSET)" $(TTF_Ubuntu) > ubuntu14.bdf
		./convbdf -a2 -d0 ubuntu14.bdf
		rm -f ubuntu14.bdf

ubuntu18.c:	otf2bdf convbdf $(TTF_Ubuntu)
		./otf2bdf -p 18 -l "$(CHARSET)" $(TTF_Ubuntu) > ubuntu18.bdf
		./convbdf -a4 -d0 ubuntu18.bdf
		rm -f ubuntu18.bdf
