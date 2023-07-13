CXXFLAGS        = -g -Wall -Werror -O -std=c++17
LDFLAGS         = -g
LIBS            =

.PHONY:         all clean

all:            rocket optimize

clean:
		rm -f *.o rocket

rocket:         rocket.o interactive.o
		$(CXX) $(LDFLAGS) rocket.o interactive.o $(LIBS) -o $@

optimize:       rocket.o optimize.o
		$(CXX) $(LDFLAGS) rocket.o optimize.o $(LIBS) -o $@

test:           rocket
		./rocket --echo < solution-input.txt
###
interactive.o: interactive.cpp rocket.h
optimize.o: optimize.cpp rocket.h
rocket.o: rocket.cpp rocket.h
