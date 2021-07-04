CXXFLAGS= -std=c++17 -Wall -Werror -g
LDFLAGS = -g
OBJ     = vtty_test.o vtty.o

vtty_test: $(OBJ)
	$(CXX) $(LDFLAGS) $(OBJ) -o $@

clean:
	rm -f vtty_test *.o
