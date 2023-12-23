CXX             = clang++ -std=c++20 -fmodules-ts -fprebuilt-module-path=.
CXXFLAGS        = -O -g
LDFLAGS         = -g

OBJ             = std.pcm main.o
LIBS            =

.NOTPARALLEL:
main:           $(OBJ)
		$(CXX) $(LDFLAGS) $(OBJ) $(LIBS) -o $@

std.pcm:
		$(CXX) -Xclang -emit-module-interface -c std.cpp -o std.pcm

clean:
		rm -rf *.o *.pcm *.dSYM main
