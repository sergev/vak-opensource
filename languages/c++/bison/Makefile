CXXFLAGS = -g -O -Wall -std=c++14
LDFLAGS = -g
OBJS    = main.o grammar.o

all:    wcount

clean:
	rm -f *.o wcount grammar.tab.* location.hh

run:    wcount
	./wcount < input.txt > output.txt
	@if diff expect.txt output.txt; then echo "Test PASS"; else echo "Test FAIL!"; fi

wcount: $(OBJS)
	$(CXX) $(LDFLAGS) -o $@ $(OBJS)

grammar.tab.cc grammar.tab.hh: grammar.yy
	bison -d $<

grammar.o: grammar.tab.cc
	$(CXX) $(CXXFLAGS) -o $@ -c $<
###
grammar.tab.o: grammar.tab.cc grammar.tab.hh
main.o: main.cc grammar.tab.hh
