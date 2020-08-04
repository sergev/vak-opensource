OBJ             = add.o gtest/gtest-all.o
CXXFLAGS        = -std=c++11 -O -g -Wall -Werror -I.
LDFLAGS         = -g

all:            add

add:            $(OBJ)
		$(CXX) $(LDFLAGS) $(OBJ) -o $@

clean:
		rm -f *.o gtest/*.o add

test:           add
		./add -s -r compact
