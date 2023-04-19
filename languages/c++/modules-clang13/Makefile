CXX             = /usr/local/Cellar/llvm@13/13.0.1/bin/clang-13 -std=c++20 -fmodules
CXXFLAGS        = -O -g -Wall -Werror -fprebuilt-module-path=.
MODFLAGS        = -O -g -Wall -Werror -Xclang -emit-module-interface
LDFLAGS         = -g

OBJ             = helloworld.pcm main.o
LIBS            = -lc++

main:           $(OBJ)
		$(CXX) $(LDFLAGS) $(OBJ) $(LIBS) -o $@

clean:
		rm -rf *.o *.pcm *.dSYM main

.cpp.pcm:
		$(CXX) $(MODFLAGS) -c $< -o $@

.SUFFIXES:      .pcm
