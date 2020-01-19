CXXFLAGS = -g -O -Wall -std=c++14 -Wno-deprecated-register
LDFLAGS = -g
OBJS    = main.o parser.o scan.o grammar.tab.o

all:    expr

clean:
	rm -f *.o expr grammar.tab.* location.hh scan.cc output.txt

run:    expr
	./expr < input.txt > output.txt
	@if diff expect.txt output.txt; then echo "Test PASS"; else echo "Test FAIL!"; fi

expr:   $(OBJS)
	$(CXX) $(LDFLAGS) -o $@ $(OBJS)

grammar.tab.cc grammar.tab.hh: grammar.yy
	bison -d $<

scan.cc: scan.ll grammar.tab.hh
	$(LEX) -o $@ scan.ll
###
grammar.tab.o: grammar.tab.cc grammar.tab.hh parser.hh scanner.hh
main.o: main.cc parser.hh grammar.tab.hh scanner.hh
parser.o: parser.cc parser.hh grammar.tab.hh scanner.hh
scan.o: scan.cc parser.hh grammar.tab.hh scanner.hh
