CLANG   = /opt/homebrew/opt/llvm/bin/clang++
CXX     = g++-12
CXXFLAGS = -std=c++20 -g -O

all:    demo demo-clang

clean:
	rm -rf demo demo-clang demo*.dSYM

demo-clang: demo.cpp
	$(CLANG) $(CXXFLAGS) $< -o $@
