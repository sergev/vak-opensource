OBJS            = rocket.o
CXXFLAGS        = -g -Wall -Werror -O -std=c++17
LDFLAGS         = -g
LIBS            =

.PHONY:         all clean

all:            rocket

clean:
		rm -f *.o rocket

rocket:         $(OBJS)
		$(CXX) $(LDFLAGS) $(OBJS) $(LIBS) -o $@

test:           rocket
		./rocket --echo < solution1-2mph.txt
