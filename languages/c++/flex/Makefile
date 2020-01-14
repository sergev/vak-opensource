LEX         = flex
CXXFLAGS    = -std=c++14 -Wall -Wno-deprecated-register
LIBS        = #-ll -lm

all:    test-scan
	./test-scan < input.txt > output.txt
	@if diff expect.txt output.txt; then echo "Test PASS"; else echo "Test FAIL!"; fi

clean:
	rm -f test-scan scan.cpp *.o

run:    test-scan

test-scan: scan.o
	$(CXX) $(LDFLAGS) $(CXXFLAGS) -o $@ $< $(LIBS)

scan.cpp: scan.ll
	$(LEX) -o $@ $<
