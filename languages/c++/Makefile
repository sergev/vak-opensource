CFLAGS  = -O3 -std=gnu++11

all:    ascii3d

ascii3d: ascii3d.cpp
	$(CXX) $(LDFLAGS) $(CFLAGS) $< -o $@

hello:  hello.cpp
	$(CXX) $(LDFLAGS) $(CFLAGS) $< -o $@

clean:
	rm -f ascii3d hello *.o
