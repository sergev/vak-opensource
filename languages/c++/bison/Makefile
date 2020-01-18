CXXFLAGS = -g -O -Wall -std=c++14 -Wno-deprecated-register
LDFLAGS = -g
OBJS    = main.o grammar.o

all:    wcount

clean:
	rm -f *.o wcount grammar.tab.* location.hh

wcount: $(OBJS)
	$(CXX) $(LDFLAGS) -o $@ $(OBJS)

grammar.tab.cc grammar.tab.hh: grammar.yy
	bison -d $<

grammar.o: grammar.tab.cc
	$(CXX) $(CXXFLAGS) -o $@ -c $<
###
grammar.tab.o: grammar.tab.cc grammar.tab.hh
main.o: main.cc grammar.tab.hh
