CC      = clang
CFLAGS  = -g -O -Wall -Werror `llvm-config --cflags`
LD      = clang++
LDFLAGS = -g `llvm-config --cxxflags --ldflags --libs core executionengine mcjit interpreter analysis native bitwriter --system-libs`

all:    sum

sum.o:  sum.c
	$(CC) $(CFLAGS) -c $<

sum:    sum.o
	$(LD) $< $(LDFLAGS) -o $@

sum.bc: sum
	./sum 0 0

sum.ll: sum.bc
	llvm-dis $<

clean:
	-rm -f sum.o sum sum.bc sum.ll
