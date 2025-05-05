PROG            = curses_wchar
CXXFLAGS        = -O -g -Wall -Werror -DNCURSES_WIDECHAR $(shell pkgconf ncurses --cflags)
LDFLAGS         = -g $(shell pkgconf ncurses --libs)

all:		$(PROG)

clean:
		rm -f $(PROG) *.o

$(PROG):        curses_wchar.o
		$(CXX) $(LDFLAGS) $< -o $@
