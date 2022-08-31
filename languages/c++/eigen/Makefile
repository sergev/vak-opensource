EIGEN3  = $(shell pkg-config --cflags eigen3)
CXXFLAGS= -std=c++14 -Wall -Werror -g -O $(EIGEN3)
LDFLAGS = -g

convolve: convolve.o
	$(CXX) $(LDFLAGS) convolve.o -o $@

clean:
	rm -f convolve *.o
