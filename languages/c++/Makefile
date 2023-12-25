CXX         = g++
CXXFLAGS    = -O -std=c++20 -Wall
PROG        = std_info ascii3d hello calculator prolog async streambuf #fmt

all:    $(PROG)

%:      %.cpp
	$(CXX) $(LDFLAGS) $(CXXFLAGS) $< -o $@

lol:    lol.cpp
	clang++ -std=c++11 $< -o $@

clean:
	rm -f $(PROG) *.o
