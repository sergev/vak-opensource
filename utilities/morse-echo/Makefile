CC		= gcc -Wall -g -Werror
CFLAGS 		= -O
PROG		= morse-echo
ENCODER		= lame -h --tg Speech --ta Morse --abr 16 --silent
UNAME           = $(shell uname)

OBJS		= main.o wav.o iir.o
MP3		= Alphabet10.mp3 Alphabet15.mp3 Alphabet20.mp3 \
		  Numbers10.mp3 Numbers15.mp3 Numbers20.mp3 \
		  Yesterday10.mp3 Yesterday15.mp3 Yesterday20.mp3 \
		  Wish10.mp3 Wish15.mp3 Wish20.mp3

# For Mac OS X
ifeq ($(UNAME),Darwin)
    LDFLAGS	= -framework CoreAudio
    OBJS	+= macosx.o
endif

# For Linux, FreeBSD using SDL
ifeq ($(UNAME),Linux)
    CFLAGS 	+= -I/usr/include/SDL -I/usr/local/include/SDL
    LIBS	= -lSDL
    OBJS	+= sdl.o
endif

# For Windows
ifeq ($(UNAME),MinGW)
    LIBS	= -lwinmm
    OBJS	+= win32.o
endif

all:		$(PROG)

mp3:		$(MP3)

zip:		mp3
		zip alphabet Alphabet10.mp3 Alphabet15.mp3 Alphabet20.mp3 Alphabet.txt
		zip numbers Numbers10.mp3 Numbers15.mp3 Numbers20.mp3 Numbers.txt
		zip yesterday Yesterday10.mp3 Yesterday15.mp3 Yesterday20.mp3 Yesterday.txt
		zip wish Wish10.mp3 Wish15.mp3 Wish20.mp3 Wish.txt

$(PROG):	$(OBJS)
		$(CC) $(LDFLAGS) $(OBJS) $(LIBS) -o $@

clean:
		rm -f *~ *.o $(PROG) $(PROG).exe

cleanall:	clean
		rm -rf qso *.mp3 *.zip

qso:		Qso1.txt Qso2.txt $(PROG) split.py
		rm -rf qso
		mkdir qso qso/10wpm qso/12wpm qso/15wpm qso/20wpm
		split.py < Qso1.txt
		split.py < Qso2.txt
		(cd qso; for i in *.txt; do \
			w=`basename $$i .txt`.mp3; \
			echo $$i "-->" $$w; \
			../$(PROG) -w20 -e10 -f $$i -o - | $(ENCODER) --tl "Qso 10 wpm" - 10wpm/$$w; \
			../$(PROG) -w20 -e12 -f $$i -o - | $(ENCODER) --tl "Qso 12 wpm" - 12wpm/$$w; \
			../$(PROG) -w20 -e15 -f $$i -o - | $(ENCODER) --tl "Qso 15 wpm" - 15wpm/$$w; \
			../$(PROG) -w20 -e20 -f $$i -o - | $(ENCODER) --tl "Qso 20 wpm" - 20wpm/$$w; \
		done)

Alphabet10.mp3:	Alphabet.txt $(PROG)
		$(PROG) -w20 -e10 -f $< -o - | $(ENCODER) --tl "Texts 10 wpm" - $@

Alphabet15.mp3:	Alphabet.txt $(PROG)
		$(PROG) -w20 -e15 -f $< -o - | $(ENCODER) --tl "Texts 15 wpm" - $@

Alphabet20.mp3:	Alphabet.txt $(PROG)
		$(PROG) -w20 -e20 -f $< -o - | $(ENCODER) --tl "Texts 20 wpm" - $@

Numbers10.mp3:	Numbers.txt $(PROG)
		$(PROG) -w20 -e10 -f $< -o - | $(ENCODER) --tl "Texts 10 wpm" - $@

Numbers15.mp3:	Numbers.txt $(PROG)
		$(PROG) -w20 -e15 -f $< -o - | $(ENCODER) --tl "Texts 15 wpm" - $@

Numbers20.mp3:	Numbers.txt $(PROG)
		$(PROG) -w20 -e20 -f $< -o - | $(ENCODER) --tl "Texts 20 wpm" - $@

Yesterday10.mp3: Yesterday.txt $(PROG)
		$(PROG) -w20 -e10 -f $< -o - | $(ENCODER) --tl "Texts 10 wpm" - $@

Yesterday15.mp3: Yesterday.txt $(PROG)
		$(PROG) -w20 -e15 -f $< -o - | $(ENCODER) --tl "Texts 15 wpm" - $@

Yesterday20.mp3: Yesterday.txt $(PROG)
		$(PROG) -w20 -e20 -f $< -o - | $(ENCODER) --tl "Texts 20 wpm" - $@

Wish10.mp3:	Wish.txt $(PROG)
		$(PROG) -w20 -e10 -f $< -o - | $(ENCODER) --tl "Texts 10 wpm" - $@

Wish15.mp3:	Wish.txt $(PROG)
		$(PROG) -w20 -e15 -f $< -o - | $(ENCODER) --tl "Texts 15 wpm" - $@

Wish20.mp3:	Wish.txt $(PROG)
		$(PROG) -w20 -e20 -f $< -o - | $(ENCODER) --tl "Texts 20 wpm" - $@
