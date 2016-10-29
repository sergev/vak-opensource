CXX     = c++ -Wall -g
CFLAGS  = -I/usr/local/include -Iscrlib
SRC     = vtcl.c
OBJ     = vtcl.o
H       =

TCLLIBS = -Lscrlib -L/usr/local/lib -lscr -ltcl -lm

all:    vtcl

vtcl:   vtcl.o scrlib/libscr.a
	$(CXX) vtcl.o -o vtcl $(TCLLIBS)

clean:
	rm -f *.[ob] vtcl

vtcl.o: scrlib/Screen.h scrlib/Menu.h scrlib/Popup.h
