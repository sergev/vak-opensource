CXXFLAGS = -g -O -Wall -std=c++14 -Wno-deprecated-register
LDFLAGS = -g
OBJS    = main.o driver.o parser.o lexer.o

all:    wcount

clean:
	rm -f *.o wcount lexer.cc parser.tab.* location.hh

parser.o: parser.yy
	bison -d $<
	$(CXX) $(CXXFLAGS) -o $@ -c parser.tab.cc

lexer.o: lexer.ll
	flex -o lexer.cc $<
	$(CXX) $(CXXFLAGS) -o $@ -c lexer.cc

wcount: $(OBJS)
	$(CXX) $(LDFLAGS) -o $@ $(OBJS)
