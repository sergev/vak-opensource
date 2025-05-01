OBJS    = term.o
CXXFLAGS = -O -g -Wall -Werror $(shell pkgconf SDL2_ttf --cflags)
LIBS    = $(shell pkgconf SDL2_ttf --libs)

all:    term

clean:
	rm -f *.o term

reindent: term.cpp
	clang-format -i $<

term:   $(OBJS)
	$(CXX) $(LDFLAGS) $(OBJS) $(LIBS) -o $@
