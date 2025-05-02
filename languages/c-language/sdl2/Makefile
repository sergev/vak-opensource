CXXFLAGS = -O -Wall -Werror $(shell sdl2-config --cflags)
LDFLAGS  = $(shell sdl2-config --libs)

all:    keycode_finder

clean:
	rm -f *.o keycode_finder

keycode_finder: keycode_finder.o
	$(CXX) $(LDFLAGS) $< -o $@
