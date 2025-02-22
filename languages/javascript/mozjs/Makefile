
CXX             = g++ -std=c++17
CXXFLAGS        = $(shell pkg-config --cflags mozjs-115)
LIBS            = $(shell pkg-config --libs mozjs-115)

all:            factorial

factorial:      factorial.o
		$(CXX) $(LDFLAGS) $< $(LIBS) -o $@
