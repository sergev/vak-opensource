CXX         = c++ -g
CXXFLAGS    = -std=c++17 -O -Wall -Werror
PROG        = set map

all:    $(PROG)

%:      %.cpp
	$(CXX) $(LDFLAGS) $(CXXFLAGS) $< -o $@

clean:
	rm -f $(PROG) *.o
