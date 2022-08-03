CFLAGS  = -O3 -std=c++17 -Wall

all:    print_floats

print_floats: print_floats.cpp
	$(CXX) $(LDFLAGS) $(CFLAGS) $< -o $@

clean:
	rm -f print_floats *.o
