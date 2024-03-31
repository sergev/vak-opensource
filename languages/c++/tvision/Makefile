PROG            = nomenus tvlife
CXXFLAGS        = -O -std=c++17 -Wall -I/usr/local/include
LDFLAGS         =
LIBS            = -L/usr/local/lib -ltvision -lncurses

all:            $(PROG)

nomenus:        nomenus.o
		$(CXX) $(LDFLAGS) $< $(LIBS) -o $@

tvlife:         tvlife.o
		$(CXX) $(LDFLAGS) $< $(LIBS) -o $@

clean:
		rm -f $(PROG) *.o
