CXX             = g++
CXXFLAGS        = -std=c++14 -O2 -Wall -pedantic -I/usr/include/python2.7
LDFLAGS         =
LIBS            = -lboost_python -lpython2.7

all:            hello
		./hello demo multiply 21 2

clean:
		rm -f *.o *.pyc hello

hello:          hello.o
		$(CXX) $(LDFLAGS) $< $(LIBS) -o $@
