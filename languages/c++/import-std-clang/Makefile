CXX             = clang++ -std=c++20 -fmodules-ts -fprebuilt-module-path=.
CXXFLAGS        = -O -g
LDFLAGS         = -g

OBJ             = std.pcm main.o
LIBS            =

.NOTPARALLEL:
main:           $(OBJ)
		$(CXX) $(LDFLAGS) $(OBJ) $(LIBS) -o $@

std.pcm:        std.cppm
		$(CXX) --precompile $< -o $@

clean:
		rm -rf *.o *.pcm *.dSYM main
