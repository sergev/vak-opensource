# -std=c++20 -stdlib=libc++ -fmodules -fbuiltin-module-map hello.cc

#CXX             = /usr/local/Cellar/llvm/15.0.3/bin/clang-15 -std=c++20 -fmodules
CXX             = /usr/local/Cellar/llvm/15.0.3/bin/clang-15 -std=c++20 -fmodules -fbuiltin-module-map
#CXX             = /usr/bin/clang++ -std=c++20 -fmodules -fbuiltin-module-map
CXXFLAGS        = -O -g -Wall -Werror -nostdinc -I/usr/local/Cellar/llvm/15.0.3/include -I/Applications/Xcode.app/Contents/Developer/Platforms/MacOSX.platform/Developer/SDKs/MacOSX.sdk/usr/include
LDFLAGS         = -g

OBJ             = main.o std.o
LIBS            =
HEADERS         = algorithm \
                  array \
                  bitset \
                  chrono \
                  fstream \
                  iomanip \
                  iostream \
                  istream \
                  iterator \
                  limits \
                  list \
                  map \
                  memory \
                  ostream \
                  set \
                  sstream \
                  string \
                  tuple \
                  unordered_map \
                  unordered_set \
                  vector

main:           $(OBJ)
		$(CXX) $(LDFLAGS) $(OBJ) $(LIBS) -o $@

clean:
		rm -rf *.o *.dSYM main
