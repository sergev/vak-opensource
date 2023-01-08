OBJ             = gtest/gtest-all.o \
                  main.o \
                  1-two-sum.o

CXXFLAGS        = -std=c++11 -O -g -Wall -Werror -I.
LDFLAGS         = -g
LIBS            = -lpthread

all:            alltests

alltests:       $(OBJ)
		$(CXX) $(LDFLAGS) $(OBJ) $(LIBS) -o $@

clean:
		rm -rf *.o gtest/*.o alltests test-results

test:           alltests
		./alltests
