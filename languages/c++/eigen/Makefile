CXXFLAGS= -std=c++14 -Wall -Werror -g -I /usr/local/Cellar/eigen/3.*/include/eigen3
LDFLAGS = -g

convolve: convolve.o
	$(CXX) $(LDFLAGS) convolve.o -o $@

clean:
	rm -f convolve *.o
