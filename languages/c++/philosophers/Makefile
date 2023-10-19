OBJ             = main.o
CXXFLAGS        = -std=c++17 -O -g -Wall -Werror -I.
LDFLAGS         = -g
LIBS            =

all:            philosophers

philosophers:   $(OBJ)
		$(CXX) $(LDFLAGS) $(OBJ) $(LIBS) -o $@

clean:
		rm -f *.o philosophers
###
main.o: main.cpp simulator.h
