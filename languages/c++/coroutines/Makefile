CLANG   = clang++ -std=c++20 -stdlib=libc++ -fcoroutines-ts
CXX     = g++-10 -std=c++20 -fcoroutines #-stdlib=libc++ -fcoroutines-ts
CXXFLAGS = -g -O
PROG    = demo

all:    $(PROG)

clean:
	rm -f $(PROG)

demo-mac: demo-mac.cpp
	$(CLANG) $< -o $@
