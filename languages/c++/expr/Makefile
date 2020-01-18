CXXFLAGS = -g -O -Wall -std=c++14 -Wno-deprecated-register
LDFLAGS = -g
OBJS    = scan.o grammar.o

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

grammar.o: grammar.tab.cc
	$(CXX) $(CXXFLAGS) -o $@ -c $<

scan.cc: scan.ll grammar.tab.hh
	$(LEX) -o $@ scan.ll
###
grammar.tab.o: grammar.tab.cc grammar.tab.hh
expr.o: expr.cc grammar.tab.hh
