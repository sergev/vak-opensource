OBJ             = add.o gtest/gtest-all.o
CXX             = c++ -std=c++11 -fsanitize=signed-integer-overflow -g
CXXFLAGS        = -O -Wall -Werror -I.
LDFLAGS         =

all:            add

add:            $(OBJ)
		$(CXX) $(LDFLAGS) $(OBJ) -o $@

clean:
		rm -f *.o gtest/*.o add

test:           add
		./add -s -r compact
