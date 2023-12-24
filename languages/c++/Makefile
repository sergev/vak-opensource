CXX         = g++
CXXFLAGS    = -O -std=c++20 -Wall
PROG        = std_info ascii3d hello lol calculator prolog async streambuf fmt

all:    $(PROG)

%:      %.cpp
	$(CXX) $(LDFLAGS) $(CFLAGS) $< -o $@

hello:  hello.cpp
	$(CXX) $(LDFLAGS) $(CFLAGS) $< -o $@

calculator: calculator.cpp
	$(CXX) $(LDFLAGS) $(CFLAGS) $< -o $@

prolog: prolog.cpp
	$(CXX) $(LDFLAGS) $(CFLAGS) $< -o $@

async:  async.cpp
	$(CXX) $(LDFLAGS) $(CFLAGS) $< -o $@

streambuf: streambuf.cpp
	$(CXX) $(LDFLAGS) $(CFLAGS) $< -o $@

map: map.cpp
	$(CXX) $(LDFLAGS) $(CFLAGS) $< -o $@

lol:    lol.cpp
	clang++ -std=c++11 $< -o $@

clean:
	rm -f $(PROG) *.o
