OBJ             = add.o catch.o
CXXFLAGS        = -std=c++11 -O -g -Wall -Werror
LDFLAGS         = -g

all:            add

add:            $(OBJ)
		$(CXX) $(LDFLAGS) $(OBJ) -o $@

clean:
		rm -f *.o add

test:           add
		./add -s -r compact
