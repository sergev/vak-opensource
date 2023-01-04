CXX             = g++-12
#CXX             = /opt/homebrew/opt/llvm/bin/clang++
CXXFLAGS        = -std=c++20 -g -O
PROG            = demo1
OBJ1            = demo1.o simulator.o

all:            $(PROG)

clean:
		rm -f *.o $(PROG)

demo1:          $(OBJ1)
		$(CXX) $(LDFLAGS) $(OBJ1) -o $@
###
demo1.o: demo1.cpp simulator.h
simulator.o: simulator.cpp simulator.h
