CFLAGS  = -O3 -std=gnu++11 -Wall

all:    ascii3d

ascii3d: ascii3d.cpp
	$(CXX) $(LDFLAGS) $(CFLAGS) $< -o $@

hello:  hello.cpp
	$(CXX) $(LDFLAGS) $(CFLAGS) $< -o $@

calculator: calculator.cpp
	$(CXX) $(LDFLAGS) $(CFLAGS) $< -o $@

lol:    lol.cpp
	clang++ -std=c++11 $< -o $@

clean:
	rm -f ascii3d hello lol calculator *.o
