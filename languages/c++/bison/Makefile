CXXFLAGS = -g -O -Wall -std=c++11
LDFLAGS = -g
OBJS    = main.o parser.o grammar.tab.o

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
###
grammar.tab.o: grammar.tab.cc grammar.tab.hh parser.hh
main.o: main.cc parser.hh grammar.tab.hh
parser.o: parser.cc parser.hh grammar.tab.hh
