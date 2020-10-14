CFLAGS  = -O3 -std=c++17 -Wall

all:    ascii3d

ascii3d: ascii3d.cpp
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

lol:    lol.cpp
	clang++ -std=c++11 $< -o $@

clean:
	rm -f ascii3d hello lol calculator prolog async streambuf *.o
